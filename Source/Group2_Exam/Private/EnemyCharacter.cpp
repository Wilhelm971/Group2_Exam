// Copyright © 2025 Wilhelm Velde Koren. All Rights Reserved.

#include "EnemyCharacter.h"
#include "PowerNode.h"
#include "PowerCannon.h"
#include "PowerCore.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GridManager.h"

// =============================================================
// CLASS DESCRIPTION
// =============================================================
// AEnemyCharacter: AI enemy that moves toward and attacks power nodes.
// Uses grid-based pathfinding for navigation.

// =============================================================
// CONSTRUCTOR
// =============================================================
// Sets up movement and collision.
AEnemyCharacter::AEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    CurrentHealth = MaxHealth;

    // Movement setup.
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    MoveComp->bOrientRotationToMovement = true;
    MoveComp->RotationRate = FRotator(0.f, 540.f, 0.f);
    MoveComp->MaxWalkSpeed = MoveSpeed;
    MoveComp->GroundFriction = 4.f;
    MoveComp->BrakingDecelerationWalking = 0.f;
    MoveComp->bRequestedMoveUseAcceleration = false;

    // Collision setup.
    GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

// =============================================================
// BEGIN PLAY
// =============================================================
// Finds grid manager and initial target.
void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Find grid manager singleton.
    TArray<AActor*> Found;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridManager::StaticClass(), Found);
    if (Found.Num() > 0)
    {
        GridMgr = Cast<AGridManager>(Found[0]);
        UE_LOG(LogTemp, Warning, TEXT("Enemy %s found GridManager"), *GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("!!! NO GRIDMANAGER IN LEVEL !!!"));
    }

    FindClosestTarget();
}

// =============================================================
// TICK
// =============================================================
// Handles target searching, path recalc, movement, and attacking.
void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Retry finding target periodically.
    TimeSinceLastSearch += DeltaTime;
    if (!TargetTower && TimeSinceLastSearch >= SearchInterval)
    {
        FindClosestTarget();
        TimeSinceLastSearch = 0.f;
    }

    // Recalculate path periodically.
    TimeSincePathRecalc += DeltaTime;
    if (TargetTower && TimeSincePathRecalc >= PathRecalcInterval)
    {
        CalculateGridPath();
        TimeSincePathRecalc = 0.f;
    }

    // Move or attack if target exists.
    if (TargetTower && IsValid(TargetTower))
    {
        const float Dist = FVector::Dist(GetActorLocation(), TargetTower->GetActorLocation());

        if (Dist <= AttackRange)
        {
            AttackTarget();
        }
        else
        {
            const FVector Next = GetNextPathPoint();
            const FVector Dir = (Next - GetActorLocation()).GetSafeNormal();
            GetCharacterMovement()->Velocity = Dir * MoveSpeed;
        }
    }
}

// =============================================================
// TARGET FINDING
// =============================================================
// Finds the closest power node as target.
void AEnemyCharacter::FindClosestTarget()
{
    TargetTower = nullptr;
    float BestDist = FLT_MAX;

    TArray<AActor*> Nodes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerNode::StaticClass(), Nodes);

    for (AActor* A : Nodes)
    {
        if (APowerNode* N = Cast<APowerNode>(A))
        {
            if (IsValid(N))
            {
                const float D = FVector::Dist(GetActorLocation(), N->GetActorLocation());
                if (D < BestDist)
                {
                    BestDist = D;
                    TargetTower = N;
                }
            }
        }
    }

    if (TargetTower)
    {
        UE_LOG(LogTemp, Log, TEXT("%s → %s (%.0f)"), *GetName(),
               *TargetTower->GetName(), BestDist);
    }
}

// =============================================================
// PATHFINDING
// =============================================================
// Calculates A* path using grid manager.
void AEnemyCharacter::CalculateGridPath()
{
    if (!GridMgr || !TargetTower) return;

    FVector StartWorld = GetActorLocation();
    FVector EndWorld = TargetTower->GetActorLocation();

    FIntPoint StartIdx = GridMgr->WorldToGridIndex(StartWorld);
    FIntPoint EndIdx = GridMgr->WorldToGridIndex(EndWorld);

    TArray<FIntPoint> GridPath = GridMgr->FindPath(StartIdx, EndIdx);

    PathPoints.Empty();
    CurrentPathIndex = 0;

    if (GridPath.Num() > 0)
    {
        for (const FIntPoint& Idx : GridPath)
        {
            FVector World = GridMgr->GridToWorldCenter(Idx);
            PathPoints.Add(World);
        }

        UE_LOG(LogTemp, Log, TEXT("A* path: %d points"), PathPoints.Num());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No grid path to %s"), *TargetTower->GetName());
    }
}

// Gets the next waypoint in the path.
FVector AEnemyCharacter::GetNextPathPoint()
{
    if (PathPoints.IsValidIndex(CurrentPathIndex))
    {
        const FVector Cur = PathPoints[CurrentPathIndex];
        if (FVector::Dist(GetActorLocation(), Cur) < 60.f)
        {
            ++CurrentPathIndex;
        }
        return Cur;
    }

    // Fallback to direct target.
    return TargetTower ? TargetTower->GetActorLocation() : GetActorLocation();
}

// =============================================================
// ATTACK AND DAMAGE
// =============================================================
// Attacks the target with continuous damage.
void AEnemyCharacter::AttackTarget()
{
    if (!TargetTower || !IsValid(TargetTower)) return;

    const float Damage = 30.f * GetWorld()->DeltaTimeSeconds;
    TargetTower->TakeDamageCustom(Damage);
}

// Applies damage to the enemy.
void AEnemyCharacter::TakeDamageCustom(float DamageAmount)
{
    if (DamageAmount <= 0.f) return;

    CurrentHealth -= DamageAmount;
    if (CurrentHealth <= 0.f)
    {
        UE_LOG(LogTemp, Log, TEXT("%s DIED"), *GetName());
        Destroy();
    }
}

// Damage from cannon (wrapper for custom damage).
void AEnemyCharacter::TakeDamageFromCannon(float Damage)
{
    TakeDamageCustom(Damage);
}