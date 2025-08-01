// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemDescription/IG_ItemDescription.h"

#include "Components/SizeBox.h"

FVector2D UIG_ItemDescription::GetBoxSize() const
{
	return SizeBox_Root->GetDesiredSize();
}

void UIG_ItemDescription::SetVisibility(ESlateVisibility InVisibility)
{
	for (const auto Child : GetChildren())
	{
		Child->Collapse();
	}
	
	Super::SetVisibility(InVisibility);
}
