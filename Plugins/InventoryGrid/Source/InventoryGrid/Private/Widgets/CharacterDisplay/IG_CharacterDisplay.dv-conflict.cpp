// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CharacterDisplay/IG_CharacterDisplay.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "EquipmentManagement/ProxyMesh/IG_ProxyMesh.h"
#include "Kismet/GameplayStatics.h"

void UIG_CharacterDisplay::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(this, AIG_ProxyMesh::StaticClass(), Actors);

	if (!Actors.IsValidIndex(0)) return;

	const AIG_ProxyMesh* ProxyMesh = Cast<AIG_ProxyMesh>(Actors[0]);
	if (!IsValid(ProxyMesh)) return;

	MeshComponent = ProxyMesh->GetMeshComponent();
}

void UIG_CharacterDisplay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bIsDragging) return;

	LastPosition = CurrentPosition;
	CurrentPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
	const float HorizontalDelta = LastPosition.X - CurrentPosition.X;

	if (!MeshComponent.IsValid()) return;
	MeshComponent->AddRelativeRotation(FRotator(0.f, HorizontalDelta, 0.f));
}

FReply UIG_CharacterDisplay::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	CurrentPosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
	LastPosition = CurrentPosition;
	
	bIsDragging = true;
	
	return FReply::Handled();
}

FReply UIG_CharacterDisplay::NativeOnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	bIsDragging = false;
	
	return FReply::Handled();
}

void UIG_CharacterDisplay::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	bIsDragging = false;
}
