#include "NodeActor.h"
#include "Components/TextRenderComponent.h"
#include "PowerCannon.h"
#include "EnemyCharacter.h"
#include "Components/BoxComponent.h"

APowerCannon* PowerCannon;
AEnemyCharacter* EnemyCharacter;
APowerNode* PowerNode;
// =============================================================
// CLASS DESCRIPTION
// =============================================================
// ANodeActor: Represents a single node in the grid system.
// Used for pathfinding and grid-based navigation.

// =============================================================
// CONSTRUCTOR
// =============================================================
// Sets default values.
ANodeActor::ANodeActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Enable ticking if needed for dynamic updates.
	PrimaryActorTick.bCanEverTick = false;

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	SetRootComponent(TileMesh);

	TileMesh->SetMobility(EComponentMobility::Static);
	TileMesh->SetCollisionObjectType(ECC_WorldStatic);  // Explicitly set to WorldStatic
	TileMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TileMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TileMesh->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	TileMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);  // Add this
	TileMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);  // Add this
	TileMesh->bHiddenInGame = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	CollisionBox->SetupAttachment(TileMesh);
	CollisionBox->SetGenerateOverlapEvents(true);
	CollisionBox->SetMobility(EComponentMobility::Static);

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ANodeActor::OnCollisionOverlap);
	CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ANodeActor::OnCollisionEnd);
}

/**
 * @brief Called when the actor is constructed in the editor or at runtime.
 * 
 * Creates a dynamic material and applies the initial state color.
 * 
 * @param Transform The transform of the actor.
 */
void ANodeActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!DynamicMaterial)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(TileMesh->GetMaterial(0), this);
		TileMesh->SetMaterial(0, DynamicMaterial);
	}

	SetState(bIsWalkable ? ENodeState::Default : ENodeState::Blocked);
}

/**
 * @brief Applies a color to the node's dynamic material.
 * 
 * Updates the "Color" vector parameter in the material.
 * 
 * @param Color The linear color to apply.
 */
void ANodeActor::ApplyColor(const FLinearColor& Color)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue(TEXT("Color"), Color);
	}
}

/**
 * @brief Handles overlap begin events for the collision box.
 * 
 * Changes node state to Target if overlapping with a PowerNode.
 * 
 * @param ColliderComp The overlapping component.
 * @param OtherActor The other actor involved.
 * @param OtherComp The other component.
 * @param OtherBodyIndex The body index.
 * @param bFromSweep Whether from a sweep.
 * @param SweepResult The sweep result.
 */
void ANodeActor::OnCollisionOverlap(
	UPrimitiveComponent* ColliderComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
	)
{
	if (OtherActor == PowerNode && (OtherActor != this) && OtherComp) 
	{
		
		SetState(ENodeState::Target);
	}
}

/**
 * @brief Handles overlap end events for the collision box.
 * 
 * Reverts node state to Default if no longer overlapping with a PowerNode.
 * 
 * @param ColliderComp The overlapping component.
 * @param OtherActor The other actor involved.
 * @param OtherComp The other component.
 * @param OtherBodyIndex The body index.
 */
void ANodeActor::OnCollisionEnd(UPrimitiveComponent* ColliderComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherActor == PowerNode && (OtherActor != this) && OtherComp) 
	{
		
		SetState(ENodeState::Default);
	}
}


/**
 * @brief Sets the node's state and applies the corresponding color.
 * 
 * Uses a switch to map states to specific colors.
 * 
 * @param NewState The new state to apply.
 */
void ANodeActor::SetState(ENodeState NewState)
{
	CurrentState = NewState;

	switch (NewState)
	{
	case ENodeState::Default:
		ApplyColor(FLinearColor::White);
		break;
	case ENodeState::Blocked:
		ApplyColor(FLinearColor::Red);
		break;
	case ENodeState::Open:
		ApplyColor(FLinearColor::Yellow);
		break;
	case ENodeState::Closed:
		ApplyColor(FLinearColor::Blue);
		break;
	case ENodeState::Path:
		ApplyColor(FLinearColor::Green);
		break;
	case ENodeState::Start:
		ApplyColor(FLinearColor(0, 255, 255, 255));
		break;
	case ENodeState::Target:
		ApplyColor(FLinearColor(195, 115, 0, 255));
		break;
	default:
		break;
	}
}

/**
 * @brief Called when the actor is clicked.
 * 
 * Toggles walkability and updates the state accordingly.
 * 
 * @param ButtonPressed The button or key pressed.
 */
void ANodeActor::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

	bIsWalkable = !bIsWalkable;
	SetState(bIsWalkable ? ENodeState::Default : ENodeState::Blocked);
}