// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "IG_CompositeBase.generated.h"

UCLASS()
class INVENTORYGRID_API UIG_CompositeBase : public UUserWidget
{
	GENERATED_BODY()

public:

	using FuncType = TFunction<void(UIG_CompositeBase*)>;
	virtual void ApplyFunction(FuncType Function) {}
	
	void Expand();
	virtual void Collapse();

	FGameplayTag GetFragmentTag() const { return FragmentTag; }
	void SetFragmentTag(const FGameplayTag& Tag) { FragmentTag = Tag; }

protected:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (GameplayTagFilter = "FragmentTags"))
	FGameplayTag FragmentTag;
};
