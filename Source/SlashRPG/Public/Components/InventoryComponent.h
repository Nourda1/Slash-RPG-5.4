#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/InventoryEntry.h"
#include "InventoryComponent.generated.h"

class UItemDefinition;

UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class SLASHRPG_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UInventoryComponent();
	
	int32 AddItem(UItemDefinition* ItemDefinition, int32 Quantity);
	bool SplitStack(int32 SourceSlotIndex, int32 QuantityToSplit);
	bool MergeStacks(int32 SourceSlotIndex, int32 DestinationSlotIndex);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Testing")
	UItemDefinition* TestItemDefinition;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	int32 MaxSlots = 2;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventoryEntry> InventoryEntries;
	
};