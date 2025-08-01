// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Composite/IG_Composite.h"
#include "IG_ItemDescription.generated.h"

class USizeBox;

UCLASS()
class INVENTORYGRID_API UIG_ItemDescription : public UIG_Composite
{
	GENERATED_BODY()

public:

	FVector2D GetBoxSize() const;

	virtual void SetVisibility(ESlateVisibility InVisibility) override;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;
};
