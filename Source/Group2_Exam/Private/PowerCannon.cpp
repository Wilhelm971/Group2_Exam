#include "PowerCannon.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "PowerLine.h"
#include "EnemyCharacter.h"

// =============================================================
// CLASS DESCRIPTION
// =============================================================
// APowerCannon: A powered node that attacks nearby enemies when powered.
// Extends APowerNode with firing logic and preview mode for placement.

// =============================================================
// CONSTRUCTOR
// =============================================================
// Sets up mesh and collision.
APowerCannon::APowerCannon()
{
    PrimaryActorTick.bCanEverTick = false;

    // Create mesh component.
    CannonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CannonMesh"));
    RootComponent = CannonMesh;
    CannonMesh->SetMobility(EComponentMobility::Movable);
    CannonMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CannonMesh->SetCollisionObjectType(ECC_WorldDynamic);
    CannonMesh->SetCollisionResponseToAllChannels(ECR_Block);
    CannonMesh->SetGenerateOverlapEvents(true);

    // Load default mesh (cylinder for testing).
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (MeshAsset.Succeeded())
    {
        CannonStaticMeshAsset = MeshAsset.Object;
    }
}

// =============================================================
// POST INITIALIZE COMPONENTS
// =============================================================
// Applies default mesh after components are created.
void APowerCannon::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (CannonMesh && CannonStaticMeshAsset)
    {
        CannonMesh->SetStaticMesh(CannonStaticMeshAsset);
    }
}

// =============================================================
// BEGIN PLAY
// =============================================================
void APowerCannon::BeginPlay()
{
    Super::BeginPlay();

    // Start firing timer always, but check conditions inside TryShoot.
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_Fire, this, &APowerCannon::TryShoot, FireInterval, true);
}

// =============================================================
// PREVIEW MODE
// =============================================================
// Toggles preview mode for building placement.
void APowerCannon::SetPreviewMode(bool bPreview)
{
    bIsPreviewMode = bPreview;

    if (bPreview)
    {
        CannonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        CannonMesh->SetVisibility(true);
        // Start with valid material.
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

// Checks if placement is valid (e.g., not too close to other nodes).
void APowerCannon::CheckPlacementValidity()
{
    bPlacementValid = true;

    // Get all power nodes.
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

// Updates material based on placement validity.
void APowerCannon::UpdatePreviewVisuals()
{
    if (!bIsPreviewMode) return;

    UMaterialInterface* MatToUse = bPlacementValid ? PreviewValidMaterial : PreviewInvalidMaterial;
    if (MatToUse)
    {
        CannonMesh->SetMaterial(0, MatToUse);
    }
}

// =============================================================
// POWER MANAGEMENT
// =============================================================
// Receives power (legacy, but kept for compatibility).
void APowerCannon::ReceivePower(APowerNode* FromNode)
{
    Super::ReceivePower(FromNode);
}

// Loses power.
void APowerCannon::LosePower()
{
    Super::LosePower();
}

// =============================================================
// FIRING LOGIC
// =============================================================
// Attempts to shoot the nearest enemy.
void APowerCannon::TryShoot()
{
    if (!bIsPowered || CurrentPower < 100.0f) return;

    // Find all enemies.
    TArray<AActor*> Enemies;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyCharacter::StaticClass(), Enemies);

    UE_LOG(LogTemp, Log, TEXT("Enemies Found"));

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
        CurrentPower -= 100.0f;
    }
}

// Fires at a specific enemy target.
void APowerCannon::FireAtEnemy(AActor* Target)
{
    if (!Target) return;

    // Apply damage if valid enemy.
    if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Target))
    {
        Enemy->TakeDamageCustom(Damage);
    }

    // Debug line for shot visualization.
    //DrawDebugLine(GetWorld(), GetActorLocation(), Target->GetActorLocation(), FColor::Red, false, 0.2f, 0, 2.0f);

    // Replace debug line with spline-based visualization (using PowerLine-like actor)
    if (ShotLineClass)
    {
        FVector Start = GetActorLocation();
        FVector End = Target->GetActorLocation();


        if (UStaticMesh* CannonAsset = CannonMesh->GetStaticMesh())
        {
            FBoxSphereBounds FromBounds = CannonAsset->GetBounds();
            Start += FromBounds.Origin;  // Negative offset to center
        }
       

        
        APowerLine* ShotLine = GetWorld()->SpawnActor<APowerLine>(ShotLineClass, FVector::ZeroVector, FRotator::ZeroRotator);
        if (ShotLine)
        {
            ShotLine->SetLine(Start, End, FColor::Red, 0.2f);  // Red color, disappears after 0.2 seconds
        }
    }

    UE_LOG(LogTemp, Log, TEXT("%s fired at %s"), *GetName(), *Target->GetName());
}