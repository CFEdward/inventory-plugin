// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Inventory/Spatial/IG_InventorySpatial.h"

#include "InventoryGrid.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/WidgetSwitcher.h"
#include "InventoryManagement/Components/IG_InventoryComponent.h"
#include "InventoryManagement/Utils/IG_InventoryStatics.h"
#include "Items/IG_InventoryItem.h"
#include "Widgets/Inventory/GridSlots/IG_EquippedGridSlot.h"
#include "Widgets/Inventory/HoverItem/IG_HoverItem.h"
#include "Widgets/Inventory/SlottedItems/IG_EquippedSlottedItem.h"
#include "Widgets/Inventory/Spatial/IG_InventoryGrid.h"
#include "Widgets/ItemDescription/IG_ItemDescription.h"

void UIG_InventorySpatial::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Equippables->OnClicked.AddDynamic(this, &ThisClass::ShowEquippables);
	Button_Consumables->OnClicked.AddDynamic(this, &ThisClass::ShowConsumables);
	Button_Craftables->OnClicked.AddDynamic(this, &ThisClass::ShowCraftables);

	Grid_Equippables->SetOwningCanvas(CanvasPanel);
	Grid_Consumables->SetOwningCanvas(CanvasPanel);
	Grid_Craftables->SetOwningCanvas(CanvasPanel);
	ShowEquippables();

	WidgetTree->ForEachWidget([this](UWidget* Widget)
		{
			UIG_EquippedGridSlot* EquippedGridSlot = Cast<UIG_EquippedGridSlot>(Widget);
			if (IsValid(EquippedGridSlot))
			{
				EquippedGridSlots.Add(EquippedGridSlot);
				EquippedGridSlot->EquippedGridSlotClicked.AddDynamic(this, &ThisClass::EquippedGridSlotClicked);
			}
		}
	);
}

void UIG_InventorySpatial::EquippedGridSlotClicked(UIG_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag)
{
	// Check to see if we can equip the Hover Item
	if (!CanEquipHoverItem(EquippedGridSlot, EquipmentTypeTag)) return;
	
	// Create an Equipped Slotted Item and add it to the Equipped Grid Slot (call EquippedGridSlot->OnItemEquipped())
	const float TileSize = UIG_InventoryStatics::GetInventoryWidget(GetOwningPlayer())->GetTileSize();
	const UIG_HoverItem* HoverItem = GetHoverItem();
	UIG_EquippedSlottedItem* EquippedSlottedItem = EquippedGridSlot->OnItemEquipped(
		HoverItem->GetInventoryItem(),
		EquipmentTypeTag,
		TileSize
	);
	EquippedSlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this, &ThisClass::EquippedSlottedItemClicked);
	
	// Inform the server that we've equipped an item (potentially unequipping an item as well)
	UIG_InventoryComponent* InventoryComponent = UIG_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComponent));
	InventoryComponent->Server_EquipSlotClicked(HoverItem->GetInventoryItem(), nullptr);
	if (GetOwningPlayer()->GetNetMode() != NM_DedicatedServer)
	{
		InventoryComponent->OnItemEquipped.Broadcast(HoverItem->GetInventoryItem());
	}
	
	// Clear the Hover Item
	Grid_Equippables->ClearHoverItem();
}

void UIG_InventorySpatial::EquippedSlottedItemClicked(UIG_EquippedSlottedItem* EquippedSlottedItem)
{
	// Remove the Item Description
	UIG_InventoryStatics::ItemUnhovered(GetOwningPlayer());
	if (IsValid(GetHoverItem()) && GetHoverItem()->IsStackable()) return;
	
	// Get Item to Equip
	UIG_InventoryItem* ItemToEquip = IsValid(GetHoverItem()) ? GetHoverItem()->GetInventoryItem() : nullptr;
	// Get Item to Unequip
	UIG_InventoryItem* ItemToUnequip = EquippedSlottedItem->GetInventoryItem();
	
	// Get the Equipped Grid Slot holding this item
	UIG_EquippedGridSlot* EquippedGridSlot = FindSlotWithEquippedItem(ItemToUnequip);
	
	// Clear the equipped grid slot of this item (set its inventory item to nullptr)
	ClearSlotOfItem(EquippedGridSlot);
	
	// Assign previously equipped item as the hover item
	Grid_Equippables->AssignHoverItem(ItemToUnequip);
	
	// Remove of the equipped slotted item from the equipped grid slot
	RemoveEquippedSlottedItem(EquippedSlottedItem);
	
	// Make a new equipped slotted item (for the item we held in HoverItem)
	MakeEquippedSlottedItem(EquippedSlottedItem, EquippedGridSlot, ItemToEquip);
	
	// Broadcast delegates for OnItemEquipped/OnItemUnequipped (from the IC)
	BroadcastSlotClickedDelegates(ItemToEquip, ItemToUnequip);
}

void UIG_InventorySpatial::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsValid(ItemDescription)) return;
	SetItemDescriptionSizeAndPosition(ItemDescription, CanvasPanel);
}

void UIG_InventorySpatial::SetItemDescriptionSizeAndPosition(UIG_ItemDescription* Description, UCanvasPanel* Canvas) const
{
	UCanvasPanelSlot* ItemDescriptionCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(Description);
	if (!IsValid(ItemDescriptionCPS)) return;

	const FVector2D ItemDescriptionSize = Description->GetBoxSize();
	ItemDescriptionCPS->SetSize(ItemDescriptionSize);

	const FVector2D ClampedPosition = UIG_WidgetUtils::GetClampedWidgetPosition(
		UIG_WidgetUtils::GetWidgetSize(Canvas),
		ItemDescriptionSize,
		UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer())
	);

	ItemDescriptionCPS->SetPosition(ClampedPosition);
}

bool UIG_InventorySpatial::CanEquipHoverItem(const UIG_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag) const
{
	if (!IsValid(EquippedGridSlot) || EquippedGridSlot->GetInventoryItem().IsValid()) return false;

	const UIG_HoverItem* HoverItem = GetHoverItem();
	if (!IsValid(HoverItem)) return false;

	const UIG_InventoryItem* HeldItem = HoverItem->GetInventoryItem();

	return HasHoverItem() && IsValid(HeldItem) && !HoverItem->IsStackable() &&
		HeldItem->GetItemManifest().GetItemCategory() == EIG_ItemCategory::Equippable &&
			HeldItem->GetItemManifest().GetItemType().MatchesTag(EquipmentTypeTag);
}

UIG_EquippedGridSlot* UIG_InventorySpatial::FindSlotWithEquippedItem(UIG_InventoryItem* EquippedItem) const
{
	auto* FoundEquippedGridSlot = EquippedGridSlots.FindByPredicate([EquippedItem](const UIG_EquippedGridSlot* GridSlot)
		{
			return GridSlot->GetInventoryItem() == EquippedItem;
		}
	);

	return FoundEquippedGridSlot ? *FoundEquippedGridSlot : nullptr;
}

void UIG_InventorySpatial::ClearSlotOfItem(UIG_EquippedGridSlot* EquippedGridSlot)
{
	if (IsValid(EquippedGridSlot))
	{
		EquippedGridSlot->SetEquippedSlottedItem(nullptr);
		EquippedGridSlot->SetInventoryItem(nullptr);
	}
}

void UIG_InventorySpatial::RemoveEquippedSlottedItem(UIG_EquippedSlottedItem* EquippedSlottedItem)
{
	if (!IsValid(EquippedSlottedItem)) return;

	if (EquippedSlottedItem->OnEquippedSlottedItemClicked.IsAlreadyBound(this, &ThisClass::EquippedSlottedItemClicked))
	{
		EquippedSlottedItem->OnEquippedSlottedItemClicked.RemoveDynamic(this, &ThisClass::EquippedSlottedItemClicked);
	}
	EquippedSlottedItem->RemoveFromParent();
}

void UIG_InventorySpatial::MakeEquippedSlottedItem(const UIG_EquippedSlottedItem* EquippedSlottedItem, UIG_EquippedGridSlot* EquippedGridSlot, UIG_InventoryItem* ItemToEquip)
{
	if (!IsValid(EquippedGridSlot)) return;

	UIG_EquippedSlottedItem* SlottedItem = EquippedGridSlot->OnItemEquipped(
		ItemToEquip,
		EquippedSlottedItem->GetEquipmentTypeTag(),
		UIG_InventoryStatics::GetInventoryWidget(GetOwningPlayer())->GetTileSize()
	);
	if (IsValid(SlottedItem)) SlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this, &ThisClass::EquippedSlottedItemClicked);

	EquippedGridSlot->SetEquippedSlottedItem(SlottedItem);
}

void UIG_InventorySpatial::BroadcastSlotClickedDelegates(UIG_InventoryItem* ItemToEquip, UIG_InventoryItem* ItemToUnequip) const
{
	UIG_InventoryComponent* IC = UIG_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(IC));

	IC->Server_EquipSlotClicked(ItemToEquip, ItemToUnequip);
	if (GetOwningPlayer()->GetNetMode() != NM_DedicatedServer)
	{
		IC->OnItemEquipped.Broadcast(ItemToEquip);
		IC->OnItemUnequipped.Broadcast(ItemToUnequip);
	}
}

FReply UIG_InventorySpatial::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	ActiveGrid->DropItem();
	return FReply::Handled();
}

FIG_SlotAvailabilityResult UIG_InventorySpatial::HasRoomForItem(UIG_ItemComponent* ItemComponent) const
{
	switch (UIG_InventoryStatics::GetItemCategoryFromItemComp(ItemComponent))
	{
	case EIG_ItemCategory::Equippable:
		return Grid_Equippables->HasRoomForItem(ItemComponent);
	case EIG_ItemCategory::Consumable:
		return Grid_Consumables->HasRoomForItem(ItemComponent);
	case EIG_ItemCategory::Craftable:
		return Grid_Craftables->HasRoomForItem(ItemComponent);
		
	default:
		UE_LOG(LogInventory, Error, TEXT("ItemComponent doesn't have a valid Item Category."));
		return FIG_SlotAvailabilityResult();
	}
}

void UIG_InventorySpatial::OnItemHovered(UIG_InventoryItem* Item)
{
	const auto& Manifest = Item->GetItemManifest();
	UIG_ItemDescription* DescriptionWidget = GetItemDescription();
	DescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);

	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);
	FTimerDelegate DescriptionTimerDelegate;
	DescriptionTimerDelegate.BindLambda([this, &Manifest, DescriptionWidget]()
		{
			GetItemDescription()->SetVisibility(ESlateVisibility::HitTestInvisible);
			Manifest.AssimilateInventoryFragments(DescriptionWidget);
		}
	);
	GetOwningPlayer()->GetWorldTimerManager().SetTimer(DescriptionTimer, DescriptionTimerDelegate, DescriptionTimerDelay, false);
}

void UIG_InventorySpatial::OnItemUnhovered()
{
	GetItemDescription()->SetVisibility(ESlateVisibility::Collapsed);
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);
}

bool UIG_InventorySpatial::HasHoverItem() const
{
	if (Grid_Equippables->HasHoverItem()) return true;
	if (Grid_Consumables->HasHoverItem()) return true;
	if (Grid_Craftables->HasHoverItem()) return true;

	return false;
}

UIG_HoverItem* UIG_InventorySpatial::GetHoverItem() const
{
	if (!ActiveGrid.IsValid()) return nullptr;
	return ActiveGrid->GetHoverItem();
}

float UIG_InventorySpatial::GetTileSize() const
{
	return Grid_Equippables->GetTileSize();
}

void UIG_InventorySpatial::ShowEquippables()
{
	SetActiveGrid(Grid_Equippables, Button_Equippables);
}

void UIG_InventorySpatial::ShowConsumables()
{
	SetActiveGrid(Grid_Consumables, Button_Consumables);
}

void UIG_InventorySpatial::ShowCraftables()
{
	SetActiveGrid(Grid_Craftables, Button_Craftables);
}

UIG_ItemDescription* UIG_InventorySpatial::GetItemDescription()
{
	if (!IsValid(ItemDescription))
	{
		ItemDescription = CreateWidget<UIG_ItemDescription>(GetOwningPlayer(), ItemDescriptionClass);
		CanvasPanel->AddChildToCanvas(ItemDescription);
	}

	return ItemDescription;
}

void UIG_InventorySpatial::DisableButton(UButton* Button) const
{
	Button_Equippables->SetIsEnabled(true);
	Button_Consumables->SetIsEnabled(true);
	Button_Craftables->SetIsEnabled(true);
	Button->SetIsEnabled(false);
}

void UIG_InventorySpatial::SetActiveGrid(UIG_InventoryGrid* Grid, UButton* Button)
{
	if (ActiveGrid.IsValid())
	{
		ActiveGrid->HideCursor();
		ActiveGrid->OnHide();
	}
	
	ActiveGrid = Grid;
	if (ActiveGrid.IsValid()) ActiveGrid->ShowCursor();
	DisableButton(Button);
	Switcher->SetActiveWidget(Grid);
}
