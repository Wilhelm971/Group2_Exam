

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NodeActor.generated.h"


UENUM(BlueprintType)
enum class ENodeState : uint8
{
	Default,
	Blocked,
	Open,
	Closed,
	Path,
	Start,
	Target
};

UCLASS(BlueprintType)
class GROUP2_EXAM_API ANodeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANodeActor(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;

	void CalculateFCost();

	void SetState(ENodeState NewState);

	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;

	void ApplyColor(const FLinearColor& Color);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Node")
	int32 X = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Node")
	int32 Y = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Node")
	bool bIsWalkable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Node")
	float TileCost = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Node")
	float GCost = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Node")
	float HCost = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Node")
	float FCost = 0.f;

	UPROPERTY()
	ANodeActor* Parent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* TileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="A* Node")
	ENodeState CurrentState = ENodeState::Default;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

};
