// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/IG_InventoryItem.h"

#include "Items/Fragments/IG_ItemFragment.h"
#include "Net/UnrealNetwork.h"

void UIG_InventoryItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemManifest);
	DOREPLIFETIME(ThisClass, TotalStackCount);
}

void UIG_InventoryItem::SetItemManifest(const FIG_ItemManifest& Manifest)
{
	ItemManifest = FInstancedStruct::Make<FIG_ItemManifest>(Manifest);
}

bool UIG_InventoryItem::IsStackable() const
{
	const FIG_StackableFragment* Stackable = GetItemManifest().GetFragmentOfType<FIG_StackableFragment>();
	return Stackable != nullptr;
}

bool UIG_InventoryItem::IsConsumable() const
{
	return GetItemManifest().GetItemCategory() == EIG_ItemCategory::Consumable;
}
