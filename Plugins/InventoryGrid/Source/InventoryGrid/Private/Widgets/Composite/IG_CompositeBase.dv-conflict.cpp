// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Composite/IG_CompositeBase.h"

void UIG_CompositeBase::Expand()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UIG_CompositeBase::Collapse()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
