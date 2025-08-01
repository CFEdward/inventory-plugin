// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IG_SlottedItem.generated.h"

class UTextBlock;
class UIG_InventoryItem;
class UImage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSlottedItemClicked, const int32, GridIndex, const FPointerEvent&, MouseEvent);

UCLASS()
class INVENTORYGRID_API UIG_SlottedItem : public UUserWidget
{
	GENERATED_BODY()

public:

	FSlottedItemClicked OnSlottedItemClicked;
	
	void UpdateStackCount(const int32 StackCount) const;
	bool IsStackable() const { return bIsStackable; }
	void SetIsStackable(const bool bStackable) { bIsStackable = bStackable; }
	UImage* GetImageIcon() const { return Image_Icon; }
	void SetGridIndex(const int32 Index) { GridIndex = Index; }
	int32 GetGridIndex() const { return GridIndex; }
	void SetGridDimensions(const FIntPoint& Dimensions) { GridDimensions = Dimensions; }
	FIntPoint GetGridDimensions() const { return GridDimensions; }
	void SetInventoryItem(UIG_InventoryItem* Item);
	UIG_InventoryItem* GetInventoryItem() const { return InventoryItem.Get(); }
	void SetImageBrush(const FSlateBrush& Brush) const;

protected:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& Geometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override;
	
private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StackCount;

	int32 GridIndex{INDEX_NONE};
	FIntPoint GridDimensions;
	TWeakObjectPtr<UIG_InventoryItem> InventoryItem;
	bool bIsStackable{false};
};
