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
	bool AddItem(UItemDefinition* ItemDefinition, int32 Quantity);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory|Testing")
	UItemDefinition* TestItemDefinition;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	int32 MaxSlots = 32;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventoryEntry> InventoryEntries;
	
};