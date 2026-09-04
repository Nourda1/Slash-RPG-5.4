// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class SLASHRPG_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetSlotIndex(int32 InSlotIndex);

	int32 GetSlotIndex() const { return SlotIndex; }

private:

	UPROPERTY()
	int32 SlotIndex = INDEX_NONE;
};
