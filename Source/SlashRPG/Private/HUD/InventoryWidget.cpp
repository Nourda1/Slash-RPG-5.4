#include "HUD/InventoryWidget.h"
#include "HUD/InventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Characters/SlashCharacter.h"
#include "Components/InventoryComponent.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ASlashCharacter* SlashCharacter = Cast<ASlashCharacter>(GetOwningPlayerPawn());

	if (!SlashCharacter)
	{
		return;
	}

	InventoryComponent = SlashCharacter->GetInventoryComponent();

	if (!InventoryComponent)
	{
		return;
	}

	
	if (!InventorySlotWidgetClass || !InventoryGrid)
	{
		return;
	}

	constexpr int32 NumSlots = 32;
	constexpr int32 NumColumns = 8;

	for (int32 SlotIndex = 0; SlotIndex < NumSlots; ++SlotIndex)
	{
		UInventorySlotWidget* SlotWidget = CreateWidget<UInventorySlotWidget>(
			GetWorld(),
			InventorySlotWidgetClass
		);

		if (!SlotWidget)
		{
			continue;
		}
		SlotWidget->SetSlotIndex(SlotIndex);
		
		const int32 Row = SlotIndex / NumColumns;
		const int32 Column = SlotIndex % NumColumns;

		InventoryGrid->AddChildToUniformGrid(
			SlotWidget,
			Row,
			Column
		);
	}
}