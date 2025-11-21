// Copyright © 2025 Wilhelm Velde Koren. All Rights Reserved.

#include "EnemyPawn.h"
#include "PowerNode.h"  // Assuming PowerNode.h exists
#include "GridManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/BoxComponent.h"

// Constructor
AEnemyPawn::AEnemyPawn()
{
    PrimaryActorTick.bCanEverTick = true;
    CurrentHealth = MaxHealth;

    // Simple Box Mesh (assign cube in BP or editor)
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
    RootComponent = MeshComp;
    MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    MeshComp->SetCollisionObjectType(ECC_Pawn);
    MeshComp->SetNotifyRigidBodyCollision(true);

    // Collision Box (slightly larger for sweep)
    UBoxComponent* CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetupAttachment(RootComponent);
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionBox->SetCollisionResponseToAllChannels(ECR_Block);
    CollisionBox->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    CollisionBox->SetRelativeScale3D(FVector(1.2f));  // Buffer for movement sweep

    // No gravity/movement comp needed
}

// BeginPlay
void AEnemyPawn::BeginPlay()
{
    Super::BeginPlay();

    // Find GridManager (optional)
    TArray<AActor*> FoundGrids;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridManager::StaticClass(), FoundGrids);
    if (FoundGrids.Num() > 0)
    {
        GridMgr = Cast<AGridManager>(FoundGrids[0]);
        UE_LOG(LogTemp, Warning, TEXT("Enemy %s found GridManager"), *GetName());
    }

    FindClosestTarget();
}

// Tick - SIMPLE MOVEMENT
void AEnemyPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Search target periodically
    TimeSinceLastSearch += DeltaTime;
    if (!TargetNode && TimeSinceLastSearch >= SearchInterval)
    {
        FindClosestTarget();
        TimeSinceLastSearch = 0.f;
    }

    // Path recalc (optional)
    TimeSincePathRecalc += DeltaTime;
    if (GridMgr && TargetNode && TimeSincePathRecalc >= PathRecalcInterval)
    {
        CalculateGridPath();
        TimeSincePathRecalc = 0.f;
    }

    // Move/Attack
    if (TargetNode)
    {
        float Dist = FVector::Dist(GetActorLocation(), TargetNode->GetActorLocation());
        if (Dist <= AttackRange)
        {
            // Attack (same as before)
            const float Damage = 30.f * DeltaTime;
            TargetNode->TakeDamageCustom(Damage);
        }
        else
        {
            FVector NextLoc = GetNextMoveLocation();
            FVector Dir = (NextLoc - GetActorLocation()).GetSafeNormal();
            MoveToward(Dir, DeltaTime);
        }
    }
}

// Simple collision-aware move
void AEnemyPawn::MoveToward(FVector Dir, float DeltaTime)
{
    float Distance = MoveSpeed * DeltaTime;
    FVector Delta = Dir * Distance;

    FHitResult Hit;
    bool bMoved = GetWorld()->SweepSingleByObjectType(
        Hit, GetActorLocation(), GetActorLocation() + Delta,
        GetActorRotation().Quaternion(),
        FCollisionObjectQueryParams(ECC_Pawn),
        FCollisionShape::MakeBox(FVector(50.f))  // Sweep shape ~box size
    );

    if (!bMoved || !Hit.bBlockingHit)
    {
        // Free move
        SetActorLocation(GetActorLocation() + Delta);
    }
    else
    {
        // Slide along wall
        FVector SlideDelta = Delta - Hit.Normal * FVector::DotProduct(Delta, Hit.Normal);
        SetActorLocation(GetActorLocation() + SlideDelta);
    }

    // Orient to movement
    if (Dir.IsNearlyZero()) return;
    SetActorRotation(Dir.Rotation());
}

// Find target (same)
void AEnemyPawn::FindClosestTarget()
{
    TargetNode = nullptr;
    float BestDist = FLT_MAX;

    TArray<AActor*> Nodes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerNode::StaticClass(), Nodes);

    for (AActor* A : Nodes)
    {
        if (APowerNode* N = Cast<APowerNode>(A))
        {
            if (N)
            {
                float D = FVector::Dist(GetActorLocation(), N->GetActorLocation());
                if (D < BestDist)
                {
                    BestDist = D;
                    TargetNode = N;
                }
            }
        }
    }

    if (TargetNode)
    {
        UE_LOG(LogTemp, Log, TEXT("%s → %s (%.0f)"), *GetName(), *TargetNode->GetName(), BestDist);
    }
}

// Next point (same, fallback direct)
FVector AEnemyPawn::GetNextMoveLocation()
{
    if (PathPoints.IsValidIndex(CurrentPathIndex))
    {
        FVector Cur = PathPoints[CurrentPathIndex];
        if (FVector::Dist(GetActorLocation(), Cur) < 80.f)
        {
            ++CurrentPathIndex;
        }
        return Cur;
    }
    return TargetNode ? TargetNode->GetActorLocation() : GetActorLocation();
}

// Path calc (same)
void AEnemyPawn::CalculateGridPath()
{
    if (!GridMgr || !TargetNode) return;

    FIntPoint StartIdx = GridMgr->WorldToGridIndex(GetActorLocation());
    FIntPoint EndIdx = GridMgr->WorldToGridIndex(TargetNode->GetActorLocation());

    if (!GridMgr->IsValidIndex(StartIdx) || !GridMgr->IsValidIndex(EndIdx)) return;

    TArray<FIntPoint> GridPath = GridMgr->FindPath(StartIdx, EndIdx);

    PathPoints.Empty();
    CurrentPathIndex = 0;

    for (const FIntPoint& Idx : GridPath)
    {
        PathPoints.Add(GridMgr->GridToWorldCenter(Idx));
    }

    UE_LOG(LogTemp, Log, TEXT("Path: %d points"), PathPoints.Num());
}

// Damage (same)
void AEnemyPawn::TakeDamageCustom(float DamageAmount)
{
    if (DamageAmount <= 0.f) return;
    CurrentHealth -= DamageAmount;
    if (CurrentHealth <= 0.f)
    {
        UE_LOG(LogTemp, Log, TEXT("%s DIED"), *GetName());
        Destroy();
    }
}

void AEnemyPawn::TakeDamageFromCannon(float Damage)
{
    TakeDamageCustom(Damage);
}