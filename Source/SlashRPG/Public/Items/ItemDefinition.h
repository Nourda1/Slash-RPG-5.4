#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "ItemDefinition.generated.h"

class UItemFragment;
class UTexture2D;

UCLASS(BlueprintType)
class SLASHRPG_API UItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** Unique gameplay tag identifying this item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FGameplayTag ItemTag;

	/** Name displayed to the player. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	/** Description displayed in inventory/tooltips. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FText Description;

	/** Icon displayed in the inventory UI. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UTexture2D> Icon;

	/** Modular pieces of functionality/data attached to this item. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Item")
	TArray<TObjectPtr<UItemFragment>> Fragments;
};