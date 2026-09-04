// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UInventorySlotWidget;
class UUniformGridPanel;
class UInventoryComponent;

UCLASS()
class SLASHRPG_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
	

	
protected:

	virtual void NativeConstruct() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TSubclassOf<UInventorySlotWidget> InventorySlotWidgetClass;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> InventoryGrid;

private:
	
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;
};
