// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IG_Leaf.h"
#include "IG_Leaf_Text.generated.h"

class UTextBlock;

UCLASS()
class INVENTORYGRID_API UIG_Leaf_Text : public UIG_Leaf
{
	GENERATED_BODY()

public:

	void SetText(const FText& Text) const;

protected:

	virtual void NativePreConstruct() override;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_LeafText;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 FontSize{12};
};
