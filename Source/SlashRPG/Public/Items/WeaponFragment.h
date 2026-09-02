#pragma once

#include "CoreMinimal.h"
#include "Items/ItemFragment.h"
#include "WeaponFragment.generated.h"

UCLASS(BlueprintType, EditInlineNew)
class SLASHRPG_API UWeaponFragment : public UItemFragment
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float Damage = 40.0f;
};