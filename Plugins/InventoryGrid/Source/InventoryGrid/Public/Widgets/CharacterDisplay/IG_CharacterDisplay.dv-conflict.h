// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IG_CharacterDisplay.generated.h"

UCLASS()
class INVENTORYGRID_API UIG_CharacterDisplay : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	
private:

	FVector2D CurrentPosition;
	FVector2D LastPosition;
	
	bool bIsDragging{false};
	
	TWeakObjectPtr<USkeletalMeshComponent> MeshComponent;
};
