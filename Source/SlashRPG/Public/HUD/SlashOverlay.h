// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

/**
 * 
 */
UCLASS()
class SLASHRPG_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	
	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGoldCount(int32 Gold);
	void SetSoulsCount(int32 Souls);
	

private:
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthProgressBar;
	
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* StaminaProgressBar;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* GoldCountText;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SoulsCountText;
};
