// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Items/Manifest/IG_ItemManifest.h"
#include "StructUtils/InstancedStruct.h"
#include "IG_InventoryItem.generated.h"

UCLASS()
class INVENTORYGRID_API UIG_InventoryItem : public UObject
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }

	void SetItemManifest(const FIG_ItemManifest& Manifest);
	const FIG_ItemManifest& GetItemManifest() const { return ItemManifest.Get<FIG_ItemManifest>(); }
	FIG_ItemManifest& GetItemManifestMutable() { return ItemManifest.GetMutable<FIG_ItemManifest>(); }
	bool IsStackable() const;
	bool IsConsumable() const;
	int32 GetTotalStackCount() const { return TotalStackCount; }
	void SetTotalStackCount(const int32 Count) { TotalStackCount = Count; }
	
private:

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Inventory", meta = (BaseStruct = "/Script/InventoryGrid.IG_ItemManifest"))
	FInstancedStruct ItemManifest;

	UPROPERTY(Replicated)
	int32 TotalStackCount{0};
};

template<typename FragmentType> requires std::derived_from<FragmentType, FIG_ItemFragment>
const FragmentType* GetFragment(const UIG_InventoryItem* Item, const FGameplayTag& Tag)
{
	if (!IsValid(Item)) return nullptr;

	const FIG_ItemManifest& Manifest = Item->GetItemManifest();
	return Manifest.GetFragmentOfTypeWithTag<FragmentType>(Tag);
}