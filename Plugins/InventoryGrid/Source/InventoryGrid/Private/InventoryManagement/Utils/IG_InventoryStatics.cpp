// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryManagement/Utils/IG_InventoryStatics.h"

#include "InventoryManagement/Components/IG_InventoryComponent.h"
#include "Items/Components/IG_ItemComponent.h"
#include "Widgets/Inventory/InventoryBase/IG_InventoryBase.h"

UIG_InventoryBase* UIG_InventoryStatics::GetInventoryWidget(APlayerController* PC)
{
	const UIG_InventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC)) return nullptr;

	return IC->GetInventoryMenu();
}

void UIG_InventoryStatics::ItemHovered(const APlayerController* PC, UIG_InventoryItem* Item)
{
	const UIG_InventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC)) return;

	UIG_InventoryBase* InventoryBase = IC->GetInventoryMenu();
	if (!IsValid(InventoryBase)) return;

	if (InventoryBase->HasHoverItem()) return;

	InventoryBase->OnItemHovered(Item);
}

void UIG_InventoryStatics::ItemUnhovered(const APlayerController* PC)
{
	const UIG_InventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC)) return;

	UIG_InventoryBase* InventoryBase = IC->GetInventoryMenu();
	if (!IsValid(InventoryBase)) return;

	InventoryBase->OnItemUnhovered();
}

UIG_HoverItem* UIG_InventoryStatics::GetHoverItem(APlayerController* PC)
{
	const UIG_InventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC)) return nullptr;

	const UIG_InventoryBase* InventoryBase = IC->GetInventoryMenu();
	if (!IsValid(InventoryBase)) return nullptr;

	return InventoryBase->GetHoverItem();
}

UIG_InventoryComponent* UIG_InventoryStatics::GetInventoryComponent(const APlayerController* PlayerController)
{
	if (!IsValid(PlayerController)) return nullptr;

	UIG_InventoryComponent* InventoryComponent = PlayerController->FindComponentByClass<UIG_InventoryComponent>();
	return InventoryComponent;
}

EIG_ItemCategory UIG_InventoryStatics::GetItemCategoryFromItemComp(const UIG_ItemComponent* ItemComp)
{
	if (!IsValid(ItemComp)) return EIG_ItemCategory::None;
	return ItemComp->GetItemManifest().GetItemCategory();
}
