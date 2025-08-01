// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "IG_HoverItem.generated.h"

class UTextBlock;
class UIG_InventoryItem;
class UImage;
/**
 * The HoverItem is the item that will appear and follow the mouse
 * when an inventory item on the grid has been clicked
 */
UCLASS()
class INVENTORYGRID_API UIG_HoverItem : public UUserWidget
{
	GENERATED_BODY()

public:
	
	void SetImageBrush(const FSlateBrush& Brush) const;
	int32 GetStackCount() const { return StackCount; }
	void UpdateStackCount(const int32 Count);
	FGameplayTag GetItemType() const;
	bool IsStackable() const { return bIsStackable; }
	void SetIsStackable(const bool bStacks);
	int32 GetPreviousGridIndex() const { return PreviousGridIndex; }
	void SetPreviousGridIndex(const int32 Index) { PreviousGridIndex = Index; }
	FIntPoint GetGridDimensions() const { return GridDimensions; }
	void SetGridDimensions(const FIntPoint& Dimensions) { GridDimensions = Dimensions; }
	UIG_InventoryItem* GetInventoryItem() const;
	void SetInventoryItem(UIG_InventoryItem* Item);

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StackCount;
	
	int32 PreviousGridIndex{INDEX_NONE};
	FIntPoint GridDimensions;
	TWeakObjectPtr<UIG_InventoryItem> InventoryItem;
	bool bIsStackable{false};
	int32 StackCount{0};
};
