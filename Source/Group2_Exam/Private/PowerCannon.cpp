#include "PowerCannon.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "PowerLine.h"
#include "EnemyCharacter.h"

/**
 * APowerCannon: A powered node that attacks nearby enemies when powered.
 * Extends APowerNode with firing logic and preview mode for placement.
 */

// =============================================================
// CONSTRUCTOR
// =============================================================
// Sets up mesh and collision.
APowerCannon::APowerCannon()
{
    PrimaryActorTick.bCanEverTick = false;  // Timer-based updates.

    // Create mesh: Movable for placement.
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

    // Start firing timer always; checks inside TryShoot handle conditions.
    GetWorld()->GetTimerManager().SetTimer(TimerHandle_Fire, this, &APowerCannon::TryShoot, FireInterval, true);
}

// =============================================================
// PREVIEW MODE
// =============================================================
// (truncated in doc; assuming full logic for validity checks).
void APowerCannon::SetPreviewMode(bool bPreview)
{
    bIsPreviewMode = bPreview;
    if (bPreview)
    {
        CannonMesh->SetMaterial(0, PreviewValidMaterial);  // Initial valid.
        CannonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    else
    {
        CannonMesh->SetMaterial(0, NormalMaterial);
        CannonMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    }
}

// Checks placement validity.
void APowerCannon::CheckPlacementValidity()
{
    bPlacementValid = true;  // Assume valid; check distance.

    TArray<AActor*> Nodes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerNode::StaticClass(), Nodes);

    for (AActor* NodeActor : Nodes)
    {
        if (NodeActor != this)
        {
            float Dist = FVector::Dist(GetActorLocation(), NodeActor->GetActorLocation());
            if (Dist < MinPlacementDistance)
            {
                bPlacementValid = false;
                break;
            }
        }
    }

    UpdatePreviewVisuals();
}

// Updates preview materials.
void APowerCannon::UpdatePreviewVisuals()
{
    if (!bIsPreviewMode) return;

    UMaterialInterface* Mat = bPlacementValid ? PreviewValidMaterial : PreviewInvalidMaterial;
    CannonMesh->SetMaterial(0, Mat);
}

// =============================================================
// POWER MANAGEMENT
// =============================================================
// Receives power (legacy, but kept for compatibility).
void APowerCannon::ReceivePower(APowerNode* FromNode)
{
    Super::ReceivePower(FromNode);
    // Intent: Start attacking when powered.
}

// Loses power.
void APowerCannon::LosePower()
{
    Super::LosePower();
    // Intent: Stop attacking.
}

// =============================================================
// FIRING LOGIC
// =============================================================
// Attempts to shoot the nearest enemy.
void APowerCannon::TryShoot()
{
    if (!bIsPowered || CurrentPower < 100.0f) return;  // Power threshold for shot.

    // Find enemies.
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
        CurrentPower -= 100.0f;  // Consume power per shot.
    }
}

// Fires at a specific enemy target.
void APowerCannon::FireAtEnemy(AActor* Target)
{
    if (!Target) return;

    // Apply damage if enemy.
    if (AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(Target))
    {
        Enemy->TakeDamageFromCannon(Damage);
    }

    // Visualize shot with line.
    if (ShotLineClass)
    {
        FVector Start = GetActorLocation();
        FVector End = Target->GetActorLocation();
        APowerLine* ShotLine = GetWorld()->SpawnActor<APowerLine>(ShotLineClass, FVector::ZeroVector, FRotator::ZeroRotator);
        if (ShotLine)
        {
            ShotLine->SetLine(Start, End, FColor::Red, 0.2f);  // Short lifetime for flash.
        }
    }

    UE_LOG(LogTemp, Log, TEXT("%s fired at %s"), *GetName(), *Target->GetName());
}