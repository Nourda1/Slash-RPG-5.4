#include "Components/InventoryComponent.h"
#include "Items/ItemDefinition.h"


UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();
    
}

FInventoryEntry* UInventoryComponent::FindEntryBySlot(int32 SlotIndex)
{
    for (FInventoryEntry& Entry : InventoryEntries)
    {
        if (Entry.SlotIndex == SlotIndex)
        {
            return &Entry;
        }
    }

    return nullptr;
}

const FInventoryEntry* UInventoryComponent::FindEntryBySlot(int32 SlotIndex) const
{
    for (const FInventoryEntry& Entry : InventoryEntries)
    {
        if (Entry.SlotIndex == SlotIndex)
        {
            return &Entry;
        }
    }

    return nullptr;
}

int32 UInventoryComponent::FindFirstEmptySlot() const
{
    for (int32 SlotIndex = 0; SlotIndex < MaxSlots; ++SlotIndex)
    {
        if (!FindEntryBySlot(SlotIndex))
        {
            return SlotIndex;
        }
    }

    return -1;
}

int32 UInventoryComponent::AddItem(UItemDefinition* ItemDefinition, int32 Quantity)
{
    if (!ItemDefinition || Quantity <= 0)
    {
        return 0;
    }

    int32 RemainingQuantity = Quantity;

    // If the item can stack, fill existing partial stacks first.
    if (ItemDefinition->bCanStack)
    {
        for (FInventoryEntry& Entry : InventoryEntries)
        {
            if (Entry.ItemDefinition != ItemDefinition)
            {
                continue;
            }

            if (Entry.Quantity >= ItemDefinition->MaxStackSize)
            {
                continue;
            }

            const int32 AvailableSpace =
                ItemDefinition->MaxStackSize - Entry.Quantity;

            const int32 AmountToAdd =
                FMath::Min(RemainingQuantity, AvailableSpace);

            Entry.Quantity += AmountToAdd;
            RemainingQuantity -= AmountToAdd;

            if (RemainingQuantity <= 0)
            {
                break;
            }
        }
    }

    // Create new stacks/entries in empty slots.
    while (RemainingQuantity > 0)
    {
        const int32 FoundSlotIndex = FindFirstEmptySlot();

        if (FoundSlotIndex == -1)
        {
            break;
        }

        const int32 AmountToAdd =
            ItemDefinition->bCanStack
                ? FMath::Min(RemainingQuantity, ItemDefinition->MaxStackSize)
                : 1;

        FInventoryEntry NewEntry;
        NewEntry.SlotIndex = FoundSlotIndex;
        NewEntry.ItemDefinition = ItemDefinition;
        NewEntry.Quantity = AmountToAdd;

        InventoryEntries.Add(NewEntry);

        RemainingQuantity -= AmountToAdd;
    }

    const int32 AddedQuantity = Quantity - RemainingQuantity;

    if (AddedQuantity > 0)
    {
        OnInventoryChanged.Broadcast();
    }

    return AddedQuantity;
}

int32 UInventoryComponent::RemoveItem(UItemDefinition* ItemDefinition, int32 Quantity)
{
    if (!ItemDefinition || Quantity <= 0)
    {
        return 0;
    }

    int32 RemainingQuantity = Quantity;

    // Search through the inventory for the requested item.
    for (int32 Index = InventoryEntries.Num() - 1; Index >= 0; --Index)
    {
        FInventoryEntry& Entry = InventoryEntries[Index];

        // This isn't the item we're looking for.
        if (Entry.ItemDefinition != ItemDefinition)
        {
            continue;
        }

        // Determine how much we can remove from this stack.
        const int32 AmountToRemove =
            FMath::Min(RemainingQuantity, Entry.Quantity);

        Entry.Quantity -= AmountToRemove;
        RemainingQuantity -= AmountToRemove;

        // Remove the entry if the stack is now empty.
        if (Entry.Quantity <= 0)
        {
            InventoryEntries.RemoveAt(Index);
        }

        // We've removed everything requested.
        if (RemainingQuantity <= 0)
        {
            break;
        }
    }

   // Determine how much was actually removed.
   const int32 RemovedQuantity = Quantity - RemainingQuantity;
   
   // Only notify listeners if the inventory actually changed.
   if (RemovedQuantity > 0)
   {
       OnInventoryChanged.Broadcast();
   }
   
   // Return the amount that was actually removed.
   return RemovedQuantity;
}

bool UInventoryComponent::SplitStack(int32 SourceSlotIndex, int32 QuantityToSplit)
{
    // Find the source entry.
    FInventoryEntry* SourceEntry =
        FindEntryBySlot(SourceSlotIndex);

    // Source slot doesn't contain an item.
    if (!SourceEntry)
    {
        return false;
    }

    // The item must be stackable.
    if (!SourceEntry->ItemDefinition ||
        !SourceEntry->ItemDefinition->bCanStack)
    {
        return false;
    }

    // Quantity must be valid.
    if (QuantityToSplit <= 0 ||
        QuantityToSplit >= SourceEntry->Quantity)
    {
        return false;
    }

    // Find an empty slot.
    const int32 FoundSlotIndex =
        FindFirstEmptySlot();

    // No empty slot available.
    if (FoundSlotIndex == -1)
    {
        return false;
    }

    // Create the new stack.
    FInventoryEntry NewEntry;
    NewEntry.SlotIndex = FoundSlotIndex;
    NewEntry.ItemDefinition = SourceEntry->ItemDefinition;
    NewEntry.Quantity = QuantityToSplit;

    InventoryEntries.Add(NewEntry);

    // Remove the split quantity from the original stack.
    SourceEntry->Quantity -= QuantityToSplit;

    OnInventoryChanged.Broadcast();

    return true;
}

bool UInventoryComponent::MergeStacks(
    int32 SourceSlotIndex,
    int32 DestinationSlotIndex)
{
    // Source and destination must be different slots.
    if (SourceSlotIndex == DestinationSlotIndex)
    {
        return false;
    }

    FInventoryEntry* SourceEntry =
        FindEntryBySlot(SourceSlotIndex);

    FInventoryEntry* DestinationEntry =
        FindEntryBySlot(DestinationSlotIndex);

    // Both slots must contain items.
    if (!SourceEntry || !DestinationEntry)
    {
        return false;
    }

    // Both entries must refer to the same item.
    if (SourceEntry->ItemDefinition !=
        DestinationEntry->ItemDefinition)
    {
        return false;
    }

    // The item must be stackable.
    if (!SourceEntry->ItemDefinition ||
        !SourceEntry->ItemDefinition->bCanStack)
    {
        return false;
    }

    // Destination is already full.
    if (DestinationEntry->Quantity >=
        SourceEntry->ItemDefinition->MaxStackSize)
    {
        return false;
    }

    // Determine how much room is available in the destination.
    const int32 AvailableSpace =
        SourceEntry->ItemDefinition->MaxStackSize -
        DestinationEntry->Quantity;

    // Determine how much we can actually move.
    const int32 AmountToMove =
        FMath::Min(SourceEntry->Quantity, AvailableSpace);

    // Move the quantity.
    SourceEntry->Quantity -= AmountToMove;
    DestinationEntry->Quantity += AmountToMove;

    // If the source is now empty, remove it.
    if (SourceEntry->Quantity <= 0)
    {
        InventoryEntries.RemoveAll(
            [SourceSlotIndex](const FInventoryEntry& Entry)
            {
                return Entry.SlotIndex == SourceSlotIndex;
            });
    }
    
    OnInventoryChanged.Broadcast();
    
    return true;
}

bool UInventoryComponent::MoveItem(
    int32 SourceSlotIndex,
    int32 DestinationSlotIndex)
{
    // Source and destination must be different slots.
    if (SourceSlotIndex == DestinationSlotIndex)
    {
        return false;
    }

    FInventoryEntry* SourceEntry =
        FindEntryBySlot(SourceSlotIndex);

    FInventoryEntry* DestinationEntry =
        FindEntryBySlot(DestinationSlotIndex);

    // Source slot must contain an item.
    if (!SourceEntry)
    {
        return false;
    }

    // If the destination is empty, simply move the source entry.
    if (!DestinationEntry)
    {
        SourceEntry->SlotIndex = DestinationSlotIndex;
        OnInventoryChanged.Broadcast();
        return true;
    }

    // If both slots contain the same item, merge the stacks.
    if (SourceEntry->ItemDefinition ==
        DestinationEntry->ItemDefinition)
    {
        // Let MergeStacks() handle the full or partial merge.
        return MergeStacks(
            SourceSlotIndex,
            DestinationSlotIndex);
    }

    // The slots contain different items, so swap them.
    SourceEntry->SlotIndex = DestinationSlotIndex;
    DestinationEntry->SlotIndex = SourceSlotIndex;

    OnInventoryChanged.Broadcast();
    return true;
}

int32 UInventoryComponent::GetMaxSlots() const
{
    return MaxSlots;
}

bool UInventoryComponent::IsSlotOccupied(int32 SlotIndex) const
{
    return FindEntryBySlot(SlotIndex) != nullptr;
}

const FInventoryEntry* UInventoryComponent::GetEntryAtSlot(int32 SlotIndex) const
{
    return FindEntryBySlot(SlotIndex);
}







