


#include "NodeActor.h"
#include "PowerCannon.h"
#include "Components/TextRenderComponent.h"
#include "Components/BoxComponent.h"


ANodeActor::ANodeActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	SetRootComponent(TileMesh);

	TileMesh->SetMobility(EComponentMobility::Static);
	TileMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TileMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TileMesh->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	TileMesh->bHiddenInGame = false;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	CollisionBox->SetupAttachment(TileMesh);
	CollisionBox->SetGenerateOverlapEvents(true);
	CollisionBox->SetMobility(EComponentMobility::Static);

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ANodeActor::OnCollisionOverlap);
	//CollisionBox->OnComponentEndOverlap.AddDynamic(this, &ANodeActor::OnCollisionEnd);
}

void ANodeActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!DynamicMaterial)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(TileMesh->GetMaterial(0), this);
		TileMesh->SetMaterial(0, DynamicMaterial);
	}

	SetState(bIsWalkable ? ENodeState::Default : ENodeState::Blocked);
	CalculateFCost();
}

void ANodeActor::CalculateFCost()
{
	FCost = GCost + HCost;

	const FString Text = FString::Printf(TEXT("F:%.1f\nT:%.1f"), FCost, TileCost);
}

void ANodeActor::ApplyColor(const FLinearColor& Color)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue(TEXT("Color"), Color);
	}
}

void ANodeActor::OnCollisionOverlap(
	UPrimitiveComponent* ColliderComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
	)
{
	if (OtherActor && (OtherActor != this) && OtherComp) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Overlap Begin"));
		SetState(ENodeState::Blocked);
	}
}

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


void ANodeActor::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

	bIsWalkable = !bIsWalkable;
	SetState(bIsWalkable ? ENodeState::Default : ENodeState::Blocked);
}
	
