// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IG_Leaf.h"
#include "IG_Leaf_LabeledValue.generated.h"

class UTextBlock;

UCLASS()
class INVENTORYGRID_API UIG_Leaf_LabeledValue : public UIG_Leaf
{
	GENERATED_BODY()

public:
	
	void SetText_Label(const FText& Text, const bool bCollapse) const;
	void SetText_Value(const FText& Text, const bool bCollapse) const;

protected:

	virtual void NativePreConstruct() override;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Label;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 FontSize_Label{12};

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Value;
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 FontSize_Value{18};
	
};
