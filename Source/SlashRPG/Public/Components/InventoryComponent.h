#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/InventoryEntry.h"
#include "InventoryComponent.generated.h"

class UItemDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class SLASHRPG_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UInventoryComponent();

	int32 AddItem(UItemDefinition* ItemDefinition, int32 Quantity);
	int32 RemoveItem(UItemDefinition* ItemDefinition, int32 Quantity);

	bool SplitStack(int32 SourceSlotIndex, int32 QuantityToSplit);
	bool MergeStacks(int32 SourceSlotIndex, int32 DestinationSlotIndex);
	bool MoveItem(int32 SourceSlotIndex, int32 DestinationSlotIndex);

	int32 GetMaxSlots() const;
	const FInventoryEntry* GetEntryAtSlot(int32 SlotIndex) const;
	bool IsSlotOccupied(int32 SlotIndex) const;
	
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Testing")
	UItemDefinition* TestItemDefinition;
	
protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	int32 MaxSlots = 6;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventoryEntry> InventoryEntries;

private:

	FInventoryEntry* FindEntryBySlot(int32 SlotIndex);
	const FInventoryEntry* FindEntryBySlot(int32 SlotIndex) const;

	int32 FindFirstEmptySlot() const;
	
	
};