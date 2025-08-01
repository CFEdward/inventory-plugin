#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "IG_FragmentTags.h"
#include "StructUtils/InstancedStruct.h"
#include "IG_ItemFragment.generated.h"

class AIG_EquipActor;
class UIG_CompositeBase;

USTRUCT(BlueprintType)
struct FIG_ItemFragment
{
	GENERATED_BODY()

public:
	
	FIG_ItemFragment() {}
	FIG_ItemFragment(const FIG_ItemFragment&) = default;
	FIG_ItemFragment& operator=(const FIG_ItemFragment&) = default;
	FIG_ItemFragment(FIG_ItemFragment&&) = default;
	FIG_ItemFragment& operator=(FIG_ItemFragment&&) = default;

	virtual void Manifest() {}

	FGameplayTag GetFragmentTag() const { return FragmentTag; }
	void SetFragmentTag(const FGameplayTag Tag) { FragmentTag = Tag; }
	
	virtual ~FIG_ItemFragment() {}

protected:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "FragmentTags"))
	FGameplayTag FragmentTag{FGameplayTag::EmptyTag};
};

USTRUCT(BlueprintType)
struct FIG_GridFragment : public FIG_ItemFragment
{
	GENERATED_BODY()

public:

	FIG_GridFragment() { FragmentTag = FragmentTags::Grid; }
	
	FIntPoint GetGridSize() const { return GridSize; }
	void SetGridSize(const FIntPoint Size) { GridSize = Size; }
	float GetGridPadding() const { return GridPadding; }
	void SetGridPadding(const float Padding) { GridPadding = Padding; }
	
private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FIntPoint GridSize{1};
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float GridPadding{0.f};
};

USTRUCT(BlueprintType)
struct FIG_StackableFragment : public FIG_ItemFragment
{
	GENERATED_BODY()

public:

	FIG_StackableFragment() { FragmentTag = FragmentTags::Stackable; }
	
	int32 GetMaxStackSize() const { return MaxStackSize; }
	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(const int32 Count) { StackCount = Count; }
	
private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxStackSize{1};
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 StackCount{1};
};

/*
 * Item fragment specifically for assimilation into a widget
 */
USTRUCT(BlueprintType)
struct FIG_WidgetItemFragment : public FIG_ItemFragment
{
	GENERATED_BODY()

public:

	virtual void Assimilate(UIG_CompositeBase* Composite) const;

protected:

	bool MatchesWidgetTag(const UIG_CompositeBase* Composite) const;
};

USTRUCT(BlueprintType)
struct FIG_ImageFragment : public FIG_WidgetItemFragment
{
	GENERATED_BODY()

public:

	FIG_ImageFragment() { FragmentTag = FragmentTags::Icon; }
	
	virtual void Assimilate(UIG_CompositeBase* Composite) const override;
	
	UTexture2D* GetIcon() const { return Icon; }

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UTexture2D> Icon{nullptr};
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D IconDimensions{50.f, 50.f};
};

USTRUCT(BlueprintType)
struct FIG_TextFragment : public FIG_WidgetItemFragment
{
	GENERATED_BODY()

public:

	virtual void Assimilate(UIG_CompositeBase* Composite) const override;

	FText GetText() const { return FragmentText; }
	void SetText(const FText& Text) { FragmentText = Text; }
	
private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (MultiLine))
	FText FragmentText;
};

USTRUCT(BlueprintType)
struct FIG_LabeledNumberFragment : public FIG_WidgetItemFragment
{
	GENERATED_BODY()

public:

	virtual void Assimilate(UIG_CompositeBase* Composite) const override;
	virtual void Manifest() override;

	/** When manifesting for the first time, this fragment will randomise */
	bool bRandomizeOnManifest{true};

protected:
	
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	float Value{0.f};
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Min{0.f};
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Max{0.f};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseLabel{false};
	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseValue{false};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MinFractionalDigits{1};
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxFractionalDigits{1};
	
private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText Text_Label{};
};

#pragma region ConsumeModifiers

USTRUCT(BlueprintType)
struct FIG_ConsumeModifier : public FIG_LabeledNumberFragment
{
	GENERATED_BODY()

public:

	virtual void OnConsume(APlayerController* PC) {}
};

USTRUCT(BlueprintType)
struct FIG_HealingFragment : public FIG_ConsumeModifier
{
	GENERATED_BODY()

public:

	virtual void OnConsume(APlayerController* PC) override;
};

USTRUCT(BlueprintType)
struct FIG_ManaFragment : public FIG_ConsumeModifier
{
	GENERATED_BODY()

public:

	virtual void OnConsume(APlayerController* PC) override;
};

#pragma endregion

USTRUCT(BlueprintType)
struct FIG_ConsumableFragment : public FIG_WidgetItemFragment
{
	GENERATED_BODY()

public:

	FIG_ConsumableFragment() { FragmentTag = FragmentTags::Consumable; }

	virtual void Manifest() override;
	virtual void Assimilate(UIG_CompositeBase* Composite) const override;
	
	void OnConsume(APlayerController* PC);

private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FIG_ConsumeModifier>> ConsumeModifiers;
};

#pragma region EquipModifiers

USTRUCT(BlueprintType)
struct FIG_EquipModifier : public FIG_LabeledNumberFragment
{
	GENERATED_BODY()

public:

	virtual void OnEquip(APlayerController* PC) {}
	virtual void OnUnequip(APlayerController* PC) {}
};

USTRUCT(BlueprintType)
struct FIG_StrengthModifier : public FIG_EquipModifier
{
	GENERATED_BODY()

public:

	virtual void OnEquip(APlayerController* PC) override;
	virtual void OnUnequip(APlayerController* PC) override;
};

#pragma endregion

USTRUCT(BlueprintType)
struct FIG_EquipmentFragment : public FIG_WidgetItemFragment
{
	GENERATED_BODY()

public:

	FIG_EquipmentFragment() { FragmentTag = FragmentTags::Equipment; }

	virtual void Manifest() override;
	virtual void Assimilate(UIG_CompositeBase* Composite) const override;
	
	void OnEquip(APlayerController* PC);
	void OnUnequip(APlayerController* PC);

	AIG_EquipActor* SpawnAttachedActor(USkeletalMeshComponent* AttachMesh) const;
	void DestroyAttachedActor() const;

	bool bEquipped{false};

	FGameplayTag GetEquipmentType() const { return EquipmentType; }
	void SetEquippedActor(AIG_EquipActor* EquipActor);

private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FIG_EquipModifier>> EquipModifiers;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<AIG_EquipActor> EquipActorClass{nullptr};
	TWeakObjectPtr<AIG_EquipActor> EquippedActor{nullptr};

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FName SocketAttachPoint{NAME_None};

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "GameItems.Equipment"))
	FGameplayTag EquipmentType{FGameplayTag::EmptyTag};
};