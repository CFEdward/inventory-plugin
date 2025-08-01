// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/IG_GridTypes.h"
#include "IG_InventoryBase.generated.h"

class UIG_HoverItem;
class UIG_ItemComponent;

UCLASS()
class INVENTORYGRID_API UIG_InventoryBase : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual FIG_SlotAvailabilityResult HasRoomForItem(UIG_ItemComponent* ItemComponent) const { return FIG_SlotAvailabilityResult(); }

	virtual void OnItemHovered(UIG_InventoryItem* Item) {}
	virtual void OnItemUnhovered() {}
	virtual bool HasHoverItem() const { return false; }
	virtual UIG_HoverItem* GetHoverItem() const { return nullptr; }
	virtual float GetTileSize() const { return 0.f; }
};
