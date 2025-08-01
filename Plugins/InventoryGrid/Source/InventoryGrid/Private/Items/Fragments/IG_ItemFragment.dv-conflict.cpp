#include "Items/Fragments/IG_ItemFragment.h"

#include "EquipmentManagement/EquipActor/IG_EquipActor.h"
#include "Widgets/Composite/IG_CompositeBase.h"
#include "Widgets/Composite/IG_Leaf_Image.h"
#include "Widgets/Composite/IG_Leaf_LabeledValue.h"
#include "Widgets/Composite/IG_Leaf_Text.h"

void FIG_WidgetItemFragment::Assimilate(UIG_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite)) return;
	Composite->Expand();
}

bool FIG_WidgetItemFragment::MatchesWidgetTag(const UIG_CompositeBase* Composite) const
{
	return Composite->GetFragmentTag().MatchesTagExact(GetFragmentTag());
}

void FIG_ImageFragment::Assimilate(UIG_CompositeBase* Composite) const
{
	FIG_WidgetItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	const UIG_Leaf_Image* Image = Cast<UIG_Leaf_Image>(Composite);
	if (!IsValid(Image)) return;

	Image->SetImage(Icon);
	Image->SetBoxSize(IconDimensions);
	Image->SetImageSize(IconDimensions);
}

void FIG_TextFragment::Assimilate(UIG_CompositeBase* Composite) const
{
	FIG_WidgetItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	const UIG_Leaf_Text* LeafText = Cast<UIG_Leaf_Text>(Composite);
	if (!IsValid(LeafText)) return;

	LeafText->SetText(FragmentText);
}

void FIG_LabeledNumberFragment::Assimilate(UIG_CompositeBase* Composite) const
{
	FIG_WidgetItemFragment::Assimilate(Composite);
	if (!MatchesWidgetTag(Composite)) return;

	const UIG_Leaf_LabeledValue* LabeledValue = Cast<UIG_Leaf_LabeledValue>(Composite);
	if (!IsValid(LabeledValue)) return;

	LabeledValue->SetText_Label(Text_Label, bCollapseLabel);
	FNumberFormattingOptions Options;
	Options.MinimumFractionalDigits = MinFractionalDigits;
	Options.MaximumFractionalDigits = MaxFractionalDigits;
	LabeledValue->SetText_Value(FText::AsNumber(Value, &Options), bCollapseValue);
}

void FIG_LabeledNumberFragment::Manifest()
{
	FIG_WidgetItemFragment::Manifest();

	if (bRandomizeOnManifest)
	{
		Value = FMath::FRandRange(Min, Max);
	}
	bRandomizeOnManifest = false;
}

void FIG_ConsumableFragment::Manifest()
{
	FIG_WidgetItemFragment::Manifest();

	for (auto& Modifier : ConsumeModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.Manifest();
	}
}

void FIG_ConsumableFragment::Assimilate(UIG_CompositeBase* Composite) const
{
	FIG_WidgetItemFragment::Assimilate(Composite);

	for (const auto& Modifier : ConsumeModifiers)
	{
		const auto& ModRef = Modifier.Get();
		ModRef.Assimilate(Composite);
	}
}

void FIG_ConsumableFragment::OnConsume(APlayerController* PC)
{
	for (auto& Modifier : ConsumeModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.OnConsume(PC);
	}
}

void FIG_HealingFragment::OnConsume(APlayerController* PC)
{
	// Get a stats component from the PC or the PC->GetPawn()
	// Get the ASC and apply a Gameplay Effect
	// Call an interface function for Healing()

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Health Item consumed! Healing by: %f"), Value));
}

void FIG_ManaFragment::OnConsume(APlayerController* PC)
{
	// Replenish Mana however you wish
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Mana Item consumed! Restoring Mana by: %f"), Value));
}

void FIG_EquipmentFragment::Manifest()
{
	FIG_WidgetItemFragment::Manifest();

	for (auto& Modifier : EquipModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.Manifest();
	}
}

void FIG_EquipmentFragment::Assimilate(UIG_CompositeBase* Composite) const
{
	FIG_WidgetItemFragment::Assimilate(Composite);

	for (const auto& Modifier : EquipModifiers)
	{
		const auto& ModRef = Modifier.Get();
		ModRef.Assimilate(Composite);
	}
}

void FIG_EquipmentFragment::OnEquip(APlayerController* PC)
{
	if (bEquipped) return;
	bEquipped = true;
	for (auto& Modifier : EquipModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.OnEquip(PC);
	}
}

void FIG_EquipmentFragment::OnUnequip(APlayerController* PC)
{
	if (!bEquipped) return;
	bEquipped = false;
	for (auto& Modifier : EquipModifiers)
	{
		auto& ModRef = Modifier.GetMutable();
		ModRef.OnUnequip(PC);
	}
}

void FIG_StrengthModifier::OnEquip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Strength increased by: %f"), Value));
}

void FIG_StrengthModifier::OnUnequip(APlayerController* PC)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Strength decreased by: %f"), Value));
}

AIG_EquipActor* FIG_EquipmentFragment::SpawnAttachedActor(USkeletalMeshComponent* AttachMesh) const
{
	if (!IsValid(EquipActorClass) || !IsValid(AttachMesh)) return nullptr;

	AIG_EquipActor* SpawnedActor = AttachMesh->GetWorld()->SpawnActor<AIG_EquipActor>(EquipActorClass);
	SpawnedActor->AttachToComponent(AttachMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketAttachPoint);

	return SpawnedActor;
}

void FIG_EquipmentFragment::DestroyAttachedActor() const
{
	if (EquippedActor.IsValid())
	{
		EquippedActor->Destroy();
	}
}

void FIG_EquipmentFragment::SetEquippedActor(AIG_EquipActor* EquipActor)
{
	EquippedActor = EquipActor;
}
