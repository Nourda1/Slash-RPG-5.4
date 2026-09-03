#include "Components/InventoryComponent.h"

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	const bool bSuccess1 = AddItem(TestItemDefinition, 1);
	const bool bSuccess2 = AddItem(TestItemDefinition, 1);
	
}

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UInventoryComponent::AddItem(UItemDefinition* ItemDefinition, int32 Quantity)
{
	if (!ItemDefinition || Quantity <= 0)
	{
		return false;
	}
	
	int32 FoundSlotIndex = -1;
	
	for (int32 SlotIndex = 0; SlotIndex < MaxSlots; ++SlotIndex) //outer for loop
	{
		bool bSlotOccupied = false;
		
		for (const FInventoryEntry& Entry : InventoryEntries) // inner for loop
		{
		
			if (Entry.SlotIndex == SlotIndex)
			{
				bSlotOccupied = true;
				break;
			}
		}
		
		if (!bSlotOccupied) //checks if free slot
		{
			FoundSlotIndex = SlotIndex;
			break;
		}
	}
	
		if (FoundSlotIndex == -1)
		{
			return false;
		}
	FInventoryEntry NewEntry;
	NewEntry.SlotIndex = FoundSlotIndex;
	NewEntry.ItemDefinition = ItemDefinition;
	NewEntry.Quantity = Quantity;
	InventoryEntries.Add(NewEntry);
	return true;
	
}

