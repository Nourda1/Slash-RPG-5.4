#include "Components/InventoryComponent.h"

#include "Items/ItemDefinition.h"

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    // Create a stack of 6.
    AddItem(TestItemDefinition, 6);

    // Try to add 8 more.
    const int32 AddedQuantity = AddItem(TestItemDefinition, 8);

    UE_LOG(
        LogTemp,
        Warning,
        TEXT("Requested: 8 | Actually Added: %d"),
        AddedQuantity
    );

    // Print the current inventory contents.
    for (const FInventoryEntry& Entry : InventoryEntries)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Slot %d: Quantity %d"),
            Entry.SlotIndex,
            Entry.Quantity
        );
    }
}

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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
        int32 FoundSlotIndex = -1;

        // Find the first empty slot.
        for (int32 SlotIndex = 0; SlotIndex < MaxSlots; ++SlotIndex)
        {
            bool bSlotOccupied = false;

            for (const FInventoryEntry& Entry : InventoryEntries)
            {
                if (Entry.SlotIndex == SlotIndex)
                {
                    bSlotOccupied = true;
                    break;
                }
            }

            if (!bSlotOccupied)
            {
                FoundSlotIndex = SlotIndex;
                break;
            }
        }

        // No empty slot available.
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

    // Return the amount that was actually added.
    return Quantity - RemainingQuantity;
}

bool UInventoryComponent::SplitStack(int32 SourceSlotIndex, int32 QuantityToSplit)
{
    // Find the source entry.
    FInventoryEntry* SourceEntry = nullptr;

    for (FInventoryEntry& Entry : InventoryEntries)
    {
        if (Entry.SlotIndex == SourceSlotIndex)
        {
            SourceEntry = &Entry;
            break;
        }
    }

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
    int32 FoundSlotIndex = -1;

    for (int32 SlotIndex = 0; SlotIndex < MaxSlots; ++SlotIndex)
    {
        bool bSlotOccupied = false;

        for (const FInventoryEntry& Entry : InventoryEntries)
        {
            if (Entry.SlotIndex == SlotIndex)
            {
                bSlotOccupied = true;
                break;
            }
        }

        if (!bSlotOccupied)
        {
            FoundSlotIndex = SlotIndex;
            break;
        }
    }

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

    return true;
}

bool UInventoryComponent::MergeStacks(int32 SourceSlotIndex, int32 DestinationSlotIndex)
{
     // Source and destination must be different slots.
    if (SourceSlotIndex == DestinationSlotIndex)
    {
        return false;
    }

    FInventoryEntry* SourceEntry = nullptr;
    FInventoryEntry* DestinationEntry = nullptr;

    // Find the source and destination entries.
    for (FInventoryEntry& Entry : InventoryEntries)
    {
        if (Entry.SlotIndex == SourceSlotIndex)
        {
            SourceEntry = &Entry;
        }

        if (Entry.SlotIndex == DestinationSlotIndex)
        {
            DestinationEntry = &Entry;
        }

        // We found both entries, so we don't need to continue searching.
        if (SourceEntry && DestinationEntry)
        {
            break;
        }
    }

    // Both slots must contain items.
    if (!SourceEntry || !DestinationEntry)
    {
        return false;
    }

    // Both entries must refer to the same item.
    if (SourceEntry->ItemDefinition != DestinationEntry->ItemDefinition)
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

    return true;
}
