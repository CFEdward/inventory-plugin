// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/HUD/IG_HUDWidget.h"

#include "InventoryManagement/Components/IG_InventoryComponent.h"
#include "InventoryManagement/Utils/IG_InventoryStatics.h"
#include "Widgets/HUD/IG_InfoMessage.h"

void UIG_HUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UIG_InventoryComponent* InventoryComponent = UIG_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	if (IsValid(InventoryComponent))
		InventoryComponent->NoRoomInInventory.AddDynamic(this, &ThisClass::OnNoRoom);
}

void UIG_HUDWidget::OnNoRoom()
{
	if (!IsValid(InfoMessage)) return;
	
	InfoMessage->SetMessage(FText::FromString("No Room In Inventory."));
}
