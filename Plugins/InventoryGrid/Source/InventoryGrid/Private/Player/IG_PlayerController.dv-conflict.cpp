// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/IG_PlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Interaction/IG_Highlightable.h"
#include "InventoryManagement/Components/IG_InventoryComponent.h"
#include "Items/Components/IG_ItemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/HUD/IG_HUDWidget.h"

AIG_PlayerController::AIG_PlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

	TraceLength = 500.0;
	ItemTraceChannel = ECC_MAX;
}

void AIG_PlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (IsValid(Subsystem))
	{
		for (const UInputMappingContext* CurrentContext : DefaultIMCs)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}

	InventoryComponent = FindComponentByClass<UIG_InventoryComponent>();

	CreateHUDWidget();
}

void AIG_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TraceForItem();
}

void AIG_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &ThisClass::PrimaryInteract);
	EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &ThisClass::ToggleInventory);
}

void AIG_PlayerController::ToggleInventory()
{
	if (!InventoryComponent.IsValid()) return;

	InventoryComponent->ToggleInventoryMenu();
	if (InventoryComponent->IsMenuOpen())
	{
		HUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		HUDWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void AIG_PlayerController::TraceForItem()
{
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport)) return;
	
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	const FVector2D ViewportCenter = ViewportSize / 2.f;

	FVector TraceStart, Forward;
	if (!UGameplayStatics::DeprojectScreenToWorld(this, ViewportCenter, TraceStart, Forward)) return;

	const FVector TraceEnd = TraceStart + Forward * TraceLength;
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ItemTraceChannel);

	LastActor = ThisActor;
	ThisActor = HitResult.GetActor();
	
	if (!ThisActor.IsValid())
	{
		if (IsValid(HUDWidget)) HUDWidget->HidePickupMessage();
	}
	
	if (ThisActor == LastActor) return;

	if (ThisActor.IsValid())
	{
		if (UActorComponent* Highlightable = ThisActor->FindComponentByInterface(UIG_Highlightable::StaticClass()); IsValid(Highlightable))
		{
			IIG_Highlightable::Execute_Highlight(Highlightable);
		}
		
		const UIG_ItemComponent* ItemComponent = ThisActor->FindComponentByClass<UIG_ItemComponent>();
		if (!IsValid(ItemComponent)) return;

		if (IsValid(HUDWidget)) HUDWidget->ShowPickupMessage(ItemComponent->GetPickupMessage());
	}
	if (LastActor.IsValid())
	{
		if (UActorComponent* Highlightable = LastActor->FindComponentByInterface(UIG_Highlightable::StaticClass()); IsValid(Highlightable))
		{
			IIG_Highlightable::Execute_UnHighlight(Highlightable);
		}
	}
}

void AIG_PlayerController::PrimaryInteract()
{
	if (!ThisActor.IsValid()) return;

	UIG_ItemComponent* ItemComp = ThisActor->FindComponentByClass<UIG_ItemComponent>();
	if (!IsValid(ItemComp) || !InventoryComponent.IsValid()) return;

	InventoryComponent->TryAddItem(ItemComp);
}

void AIG_PlayerController::CreateHUDWidget()
{
	if (!IsLocalController()) return;

	HUDWidget = CreateWidget<UIG_HUDWidget>(this, HUDWidgetClass);
	if (IsValid(HUDWidget)) HUDWidget->AddToViewport();
}
