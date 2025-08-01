// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/SlottedItems/IG_SlottedItem.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "InventoryManagement/Utils/IG_InventoryStatics.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "Items/IG_InventoryItem.h"

FReply UIG_SlottedItem::NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent)
{
	OnSlottedItemClicked.Broadcast(GridIndex, MouseEvent);
	return FReply::Handled();
}

void UIG_SlottedItem::NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	UIG_InventoryStatics::ItemHovered(GetOwningPlayer(), InventoryItem.Get());
}

void UIG_SlottedItem::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
	UIG_InventoryStatics::ItemUnhovered(GetOwningPlayer());
}

void UIG_SlottedItem::UpdateStackCount(const int32 StackCount) const
{
	if (StackCount > 0)
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Visible);
		Text_StackCount->SetText(FText::AsNumber(StackCount));
	}
	else
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UIG_SlottedItem::SetInventoryItem(UIG_InventoryItem* Item)
{
	InventoryItem = Item;
}

void UIG_SlottedItem::SetImageBrush(const FSlateBrush& Brush) const
{
	Image_Icon->SetBrush(Brush);
}
