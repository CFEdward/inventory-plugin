// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IG_ItemPopUp.generated.h"

class USizeBox;
class UTextBlock;
class USlider;
class UButton;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FPopUpMenuSplit, int32, SplitAmount, int32, Index);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPopUpMenuDrop, int32, Index);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPopUpMenuConsume, int32, Index);

/**
 * The item popup widget shows up when right-clicking on an item
 * in the inventory grid
 */
UCLASS()
class INVENTORYGRID_API UIG_ItemPopUp : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
	void SetSliderParams(const float Max, const float Value) const;
	
	void CollapseSplitButton() const;
	void CollapseConsumeButton() const;
	
	FPopUpMenuSplit OnSplit;
	FPopUpMenuDrop OnDrop;
	FPopUpMenuConsume OnConsume;

	int32 GetSplitAmount() const;
	FVector2D GetBoxSize() const;
	void SetGridIndex(const int32 Index) { GridIndex = Index; }
	int32 GetGridIndex() const { return GridIndex; }
	
private:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Split;
	UFUNCTION()
	void SplitButtonClicked();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Drop;
	UFUNCTION()
	void DropButtonClicked();
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Consume;
	UFUNCTION()
	void ConsumeButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> Slider_Split;
	UFUNCTION()
	void SliderValueChanged(const float Value);
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SplitAmount;

	int32 GridIndex{INDEX_NONE};
};
