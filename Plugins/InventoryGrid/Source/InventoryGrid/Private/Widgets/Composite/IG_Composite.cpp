// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Composite/IG_Composite.h"

#include "Blueprint/WidgetTree.h"

void UIG_Composite::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	WidgetTree->ForEachWidget([this](UWidget* Widget)
		{
			UIG_CompositeBase* Composite = Cast<UIG_CompositeBase>(Widget);
			if (IsValid(Composite))
			{
				Children.Add(Composite);
				Composite->Collapse();
			}
		}
	);
}

void UIG_Composite::ApplyFunction(const FuncType Function)
{
	for (const auto& Child : Children)
	{
		Child->ApplyFunction(Function);
	}
}

void UIG_Composite::Collapse()
{
	for (const auto& Child : Children)
	{
		Child->Collapse();
	}
}
