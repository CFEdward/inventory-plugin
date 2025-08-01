// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Composite/IG_Leaf_LabeledValue.h"

#include "Components/TextBlock.h"

void UIG_Leaf_LabeledValue::NativePreConstruct()
{
	Super::NativePreConstruct();

	checkf(Text_Label && Text_Value, TEXT("Make sure the widgets are bound!"));
	FSlateFontInfo FontInfo_Label = Text_Label->GetFont();
	FontInfo_Label.Size = FontSize_Label;
	Text_Label->SetFont(FontInfo_Label);

	FSlateFontInfo FontInfo_Value = Text_Value->GetFont();
	FontInfo_Value.Size = FontSize_Value;
	Text_Value->SetFont(FontInfo_Value);
}

void UIG_Leaf_LabeledValue::SetText_Label(const FText& Text, const bool bCollapse) const
{
	if (bCollapse)
	{
		Text_Label->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	Text_Label->SetText(Text);
}

void UIG_Leaf_LabeledValue::SetText_Value(const FText& Text, const bool bCollapse) const
{
	if (bCollapse)
	{
		Text_Value->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}
	Text_Value->SetText(Text);
}
