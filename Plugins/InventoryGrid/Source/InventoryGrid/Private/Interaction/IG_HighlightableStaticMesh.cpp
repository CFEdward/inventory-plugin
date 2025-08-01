// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/IG_HighlightableStaticMesh.h"

void UIG_HighlightableStaticMesh::Highlight_Implementation()
{
	SetOverlayMaterial(HighlightMaterial);
}

void UIG_HighlightableStaticMesh::UnHighlight_Implementation()
{
	SetOverlayMaterial(nullptr);
}
