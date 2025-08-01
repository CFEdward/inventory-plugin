// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IG_Leaf.h"
#include "Items/Fragments/IG_FragmentTags.h"
#include "IG_Leaf_Image.generated.h"

class USizeBox;
class UImage;

UCLASS()
class INVENTORYGRID_API UIG_Leaf_Image : public UIG_Leaf
{
	GENERATED_BODY()

public:

	UIG_Leaf_Image() { FragmentTag = FragmentTags::Icon; }

	void SetImage(UTexture2D* Texture) const;
	void SetImageSize(const FVector2D& Size) const;
	FVector2D GetImageSize() const;
	void SetBoxSize(const FVector2D& Size) const;
	
private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox_Icon;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;
};
