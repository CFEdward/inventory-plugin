// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/GridSlots/IG_EquippedGridSlot.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "InventoryManagement/Utils/IG_InventoryStatics.h"
#include "Items/IG_InventoryItem.h"
#include "Items/Fragments/IG_ItemFragment.h"
#include "Widgets/Inventory/HoverItem/IG_HoverItem.h"
#include "Widgets/Inventory/SlottedItems/IG_EquippedSlottedItem.h"

void UIG_EquippedGridSlot::NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!IsAvailable()) return;
	const UIG_HoverItem* HoverItem = UIG_InventoryStatics::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem)) return;

	if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
	{
		SetOccupiedTexture();
		Image_GreyedOutIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UIG_EquippedGridSlot::NativeOnMouseLeave(const FPointerEvent& MouseEvent)
{
	if (!IsAvailable()) return;
	const UIG_HoverItem* HoverItem = UIG_InventoryStatics::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem)) return;

	if (IsValid(EquippedSlottedItem)) return;

	if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
	{
		SetUnoccupiedTexture();
		Image_GreyedOutIcon->SetVisibility(ESlateVisibility::Visible);
	}
}

FReply UIG_EquippedGridSlot::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	EquippedGridSlotClicked.Broadcast(this, EquipmentTypeTag);
	return FReply::Handled();
}

UIG_EquippedSlottedItem* UIG_EquippedGridSlot::OnItemEquipped(UIG_InventoryItem* Item, const FGameplayTag& EquipmentTag, const float TileSize)
{
	// Check the Equipment Type Tag
	if (!EquipmentTag.MatchesTagExact(EquipmentTypeTag)) return nullptr;
	
	// Get Grid Dimensions
	const FIG_GridFragment* GridFragment = GetFragment<FIG_GridFragment>(Item, FragmentTags::Grid);
	if (!GridFragment) return nullptr;
	
	// Calculate the Draw Size for the Equipped Slotted Item
	const FIntPoint GridDimensions = GridFragment->GetGridSize();
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2;
	//const FVector2D DrawSize = GridDimensions * IconTileWidth;
	const FVector2D DrawSize = FVector2D(SizeBox_Image->GetWidthOverride(), SizeBox_Image->GetHeightOverride());
	
	// Create the Equipped Slotted Item widget
	EquippedSlottedItem = CreateWidget<UIG_EquippedSlottedItem>(GetOwningPlayer(), EquippedSlottedItemClass);
	
	// Set the Slotted Item's Inventory Item
	EquippedSlottedItem->SetInventoryItem(Item);
	
	// Set the Slotted Item's Equipment Type Tag
	EquippedSlottedItem->SetEquipmentTypeTag(EquipmentTag);
	
	// Hide the Stack Count widget on the Slotted Item
	EquippedSlottedItem->UpdateStackCount(0);
	
	// Set Inventory Item on this class (the Equipped Grid Slot)
	SetInventoryItem(Item);
	
	// Set the Image Brush on the Equipped Slotted Item
	const FIG_ImageFragment* ImageFragment = GetFragment<FIG_ImageFragment>(Item, FragmentTags::Icon);
	if (!ImageFragment) return nullptr;
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = DrawSize;
	EquippedSlottedItem->SetImageBrush(Brush);
	
	// Add the Slotted Item as a child to this widget's Overlay
	Overlay_Root->AddChildToOverlay(EquippedSlottedItem);
	const FGeometry OverlayGeometry = Overlay_Root->GetCachedGeometry();
	auto OverlayPos = OverlayGeometry.Position;
	const auto OverlaySize = OverlayGeometry.Size;

	const float LeftPadding = OverlaySize.X / 2.f - DrawSize.X / 2.f;
	const float TopPadding = OverlaySize.Y / 2.f - DrawSize.Y / 2.f;
	
	UOverlaySlot* OverlaySlot = UWidgetLayoutLibrary::SlotAsOverlaySlot(EquippedSlottedItem);
	OverlaySlot->SetPadding(FMargin(LeftPadding, TopPadding));
	
	// Return the Equipped Slotted Item widget
	return EquippedSlottedItem;
}