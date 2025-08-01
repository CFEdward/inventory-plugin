// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/GridSlots/IG_GridSlot.h"

#include "Components/Image.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "Items/IG_InventoryItem.h"
#include "Widgets/ItemPopUp/IG_ItemPopUp.h"

void UIG_GridSlot::NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseEnter(MyGeometry, MouseEvent);

	GridSlotHovered.Broadcast(TileIndex, MouseEvent);
}

void UIG_GridSlot::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
	Super::NativeOnMouseLeave(MouseEvent);

	GridSlotUnhovered.Broadcast(TileIndex, MouseEvent);
}

FReply UIG_GridSlot::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	GridSlotClicked.Broadcast(TileIndex, MouseEvent);
	
	return FReply::Handled();
}

void UIG_GridSlot::SetInventoryItem(UIG_InventoryItem* Item)
{
	InventoryItem = Item;
}

void UIG_GridSlot::SetUnoccupiedTexture()
{
	GridSlotState = EIG_GridSlotState::Unoccupied;
	Image_GridSlot->SetBrush(Brush_Unoccupied);
}

void UIG_GridSlot::SetOccupiedTexture()
{
	GridSlotState = EIG_GridSlotState::Occupied;
	Image_GridSlot->SetBrush(Brush_Occupied);
}

void UIG_GridSlot::SetSelectedTexture()
{
	GridSlotState = EIG_GridSlotState::Selected;
	Image_GridSlot->SetBrush(Brush_Selected);
}

void UIG_GridSlot::SetGreyedOutTexture()
{
	GridSlotState = EIG_GridSlotState::GreyedOut;
	Image_GridSlot->SetBrush(Brush_GreyedOut);
}

void UIG_GridSlot::SetItemPopUp(UIG_ItemPopUp* PopUp)
{
	ItemPopUp = PopUp;
	ItemPopUp->SetGridIndex(GetIndex());
	ItemPopUp->OnNativeDestruct.AddUObject(this, &ThisClass::OnItemPopUpDestruct);
}

UIG_ItemPopUp* UIG_GridSlot::GetItemPopUp() const
{
	return ItemPopUp.Get();
}

void UIG_GridSlot::OnItemPopUpDestruct(UUserWidget* Menu)
{
	ItemPopUp.Reset();
}
