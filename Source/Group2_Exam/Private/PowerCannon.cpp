#include "PowerCannon.h"
#include "EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"


APowerCannon::APowerCannon()
{
	PrimaryActorTick.bCanEverTick = false;

	// Create Mesh Component
	CannonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CannonMesh"));
	RootComponent = CannonMesh;
	CannonMesh->SetMobility(EComponentMobility::Movable);
	CannonMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CannonMesh->SetCollisionObjectType(ECC_WorldDynamic);
	CannonMesh->SetCollisionResponseToAllChannels(ECR_Block);
	CannonMesh->SetGenerateOverlapEvents(true);

	// DEFAULT CYLINDER MESH (for testing - override in Blueprint/Editor)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (MeshAsset.Succeeded())
	{
		CannonStaticMeshAsset = MeshAsset.Object;
	}
	
}

void APowerCannon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CannonMesh && CannonStaticMeshAsset)
	{
		CannonMesh->SetStaticMesh(CannonStaticMeshAsset);
	}
}
void APowerCannon::BeginPlay()
{
	Super::BeginPlay();
}


void APowerCannon::SetPreviewMode(bool bPreview)
{
	bIsPreviewMode = bPreview;

	if (bPreview)
	{
		CannonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CannonMesh->SetVisibility(true);
		// Start with valid preview material
		if (PreviewValidMaterial)
		{
			CannonMesh->SetMaterial(0, PreviewValidMaterial);
		}
	}
	else
	{
		CannonMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		if (NormalMaterial)
		{
			CannonMesh->SetMaterial(0, NormalMaterial);
		}
	}

	CheckPlacementValidity();
}

void APowerCannon::CheckPlacementValidity()
{
	bPlacementValid = true;

	// Check distance to other PowerNodes
	TArray<AActor*> AllNodes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerNode::StaticClass(), AllNodes);

	for (AActor* NodeActor : AllNodes)
	{
		if (NodeActor == this || !IsValid(NodeActor)) continue;

		float Dist = FVector::Dist(GetActorLocation(), NodeActor->GetActorLocation());
		if (Dist < MinPlacementDistance)
		{
			bPlacementValid = false;
			break;
		}
	}

	UpdatePreviewVisuals();
}

void APowerCannon::UpdatePreviewVisuals()
{
	if (!bIsPreviewMode) return;

	UMaterialInterface* MatToUse = bPlacementValid ? PreviewValidMaterial : PreviewInvalidMaterial;
	if (MatToUse)
	{
		CannonMesh->SetMaterial(0, MatToUse);
	}
}

void APowerCannon::ReceivePower(APowerNode* FromNode)
{
	// Call parent to mark as powered
	Super::ReceivePower(FromNode);

	// Start firing loop if not already active
	if (!GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_Fire))
	{
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle_Fire, this, &APowerCannon::TryShoot, FireInterval, true);

		UE_LOG(LogTemp, Log, TEXT("%s is now powered and ready to fire!"), *GetName());
	}
}

void APowerCannon::LosePower()
{
	Super::LosePower();
	// Stop firing
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Fire);
	UE_LOG(LogTemp, Warning, TEXT("%s lost power and stopped firing."), *GetName());
}

void APowerCannon::TryShoot()
{

	if (!bIsPowered) return;

	// Find nearest enemy within AttackRange
	TArray<AActor*> Enemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), Enemies);

	AActor* ClosestEnemy = nullptr;
	float ClosestDist = FLT_MAX;

	for (AActor* Enemy : Enemies)
	{
		if (!Enemy) continue;

		float Dist = FVector::Dist(Enemy->GetActorLocation(), GetActorLocation());
		if (Dist < AttackRange && Dist < ClosestDist)
		{
			ClosestDist = Dist;
			ClosestEnemy = Enemy;
		}
	}

	if (ClosestEnemy)
	{
		FireAtEnemy(ClosestEnemy);
	}
	
}



void APowerCannon::FireAtEnemy(AActor* Target)
{
	
	if (!Target) return;

	// You could spawn a projectile or apply damage directly
	if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Target))
	{
		Enemy->TakeDamageFromCannon(Damage);
	}

	DrawDebugLine(GetWorld(), GetActorLocation(), Target->GetActorLocation(),
		FColor::Red, false, 0.2f, 0, 2.0f);

	UE_LOG(LogTemp, Log, TEXT("%s fired at %s"), *GetName(), *Target->GetName());
	
}
