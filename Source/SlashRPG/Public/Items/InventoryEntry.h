#pragma once

#include "CoreMinimal.h"
#include "InventoryEntry.generated.h"

class UItemDefinition;

USTRUCT(BlueprintType)
struct SLASHRPG_API FInventoryEntry
{
	GENERATED_BODY()

public:

	/** Logical inventory slot occupied by this entry. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 SlotIndex = INDEX_NONE;

	/** Definition describing what item this entry represents. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<UItemDefinition> ItemDefinition = nullptr;

	/** Number of items represented by this entry. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Quantity = 1;
};