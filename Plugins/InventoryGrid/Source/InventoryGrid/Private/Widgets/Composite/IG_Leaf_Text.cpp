// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Composite/IG_Leaf_Text.h"

#include "Components/TextBlock.h"

void UIG_Leaf_Text::NativePreConstruct()
{
	Super::NativePreConstruct();

	checkf(Text_LeafText, TEXT("Make sure the widget is bound!"));
	FSlateFontInfo FontInfo = Text_LeafText->GetFont();
	FontInfo.Size = FontSize;
	Text_LeafText->SetFont(FontInfo);
}

void UIG_Leaf_Text::SetText(const FText& Text) const
{
	Text_LeafText->SetText(Text);
}
