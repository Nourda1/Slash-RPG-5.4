// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/SlashHUD.h"

#include "Blueprint/UserWidget.h"
#include "HUD/SlashOverlay.h"

void ASlashHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld(); //Gets reference to world

	if (World) //Checks to make sure not nullptr
	{
		APlayerController* Controller = World->GetFirstPlayerController(); //GetsPlayerController
		if(Controller && SlashOverlayClass) //Checks for null
		{
			SlashOverlay = CreateWidget<USlashOverlay>(Controller,SlashOverlayClass); //Creates the widget
			SlashOverlay->AddToViewport(); //Adds widget to viewport
		}
	}
	
}
