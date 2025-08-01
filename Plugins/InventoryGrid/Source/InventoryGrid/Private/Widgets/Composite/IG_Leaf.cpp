// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Composite/IG_Leaf.h"

void UIG_Leaf::ApplyFunction(const FuncType Function)
{
	Function(this);
}
