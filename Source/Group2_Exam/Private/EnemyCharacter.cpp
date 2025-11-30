#include "EnemyCharacter.h"

#include "GameManager.h"
#include "PowerNode.h"
#include "PowerCannon.h"
#include "PowerCore.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GridManager.h"
#include "TopDownPlayerController.h"

/**
 * AEnemyCharacter: AI enemy that moves toward and attacks power nodes.
 * Uses grid-based pathfinding for navigation. Rewards bounty on death.
 */

// =============================================================
// CONSTRUCTOR
// =============================================================
// Sets up movement and collision.
AEnemyCharacter::AEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;
    CurrentHealth = MaxHealth;  // Initialize health from MaxHealth.

    // Movement setup: Orient to direction, set speed, disable acceleration for precise control.
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    MoveComp->bOrientRotationToMovement = true;
    MoveComp->RotationRate = FRotator(0.f, 540.f, 0.f);
    MoveComp->MaxWalkSpeed = MoveSpeed;
    MoveComp->bRequestedMoveUseAcceleration = false;

    // Full air control to match ground movement; no lateral friction while falling.
    MoveComp->AirControl = 1.0f;
    MoveComp->FallingLateralFriction = 0.0f;

    // Collision: Pawn type, block world/static/dynamic, ignore other pawns.
    GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
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

    // Find GameManager for death notifications.
    TArray<AActor*> GameManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameManager::StaticClass(), GameManagers);
    if (!GameManagers.IsEmpty())
    {
        GameManager = Cast<AGameManager>(GameManagers[0]);
    }

    // Find GridManager for pathfinding.
    TArray<AActor*> GridManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGridManager::StaticClass(), GridManagers);
    if (!GridManagers.IsEmpty())
    {
        GridMgr = Cast<AGridManager>(GridManagers[0]);
    }

    // Initial target search.
    FindClosestTarget();
}

// =============================================================
// TICK
// =============================================================
// Updates AI: search targets, recalc paths, move, and attack.
void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Skip if pathfinding disabled or no GridManager.
    if (!bDoPathfinding || !GridMgr) return;

    // Periodic target search to handle destruction or new nodes.
    TimeSinceLastSearch += DeltaTime;
    if (TimeSinceLastSearch >= SearchInterval)
    {
        FindClosestTarget();
        TimeSinceLastSearch = 0.f;
    }

    // Skip if no target.
    if (!TargetTower) return;

    // Periodic path recalc to adapt to dynamic grid changes.
    TimeSincePathRecalc += DeltaTime;
    if (TimeSincePathRecalc >= PathRecalcInterval)
    {
        CalculateGridPath();
        TimeSincePathRecalc = 0.f;
    }

    // Move toward next path point.
    FVector NextPoint = GetNextPathPoint();
    FVector Dir = (NextPoint - GetActorLocation()).GetSafeNormal();
    AddMovementInput(Dir, 1.f);

    // Attack if close enough (intent: continuous damage over time).
    float Dist = FVector::Dist(GetActorLocation(), TargetTower->GetActorLocation());
    if (Dist <= AttackRange)
    {
        AttackTarget();
    }
}

// =============================================================
// TARGETING AND PATHFINDING
// =============================================================
// Finds the closest active power node.
void AEnemyCharacter::FindClosestTarget()
{
    TArray<AActor*> Nodes;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerNode::StaticClass(), Nodes);

    APowerNode* Closest = nullptr;
    float MinDist = FLT_MAX;

    for (AActor* NodeActor : Nodes)
    {
        if (APowerNode* Node = Cast<APowerNode>(NodeActor))
        {
            // Skip destroyed or invalid nodes.
            if (Node->bIsDestroyed || !IsValid(Node)) continue;

            float Dist = FVector::Dist(GetActorLocation(), Node->GetActorLocation());
            if (Dist < MinDist)
            {
                MinDist = Dist;
                Closest = Node;
            }
        }
    }

    // Update target if found.
    if (Closest)
    {
        TargetTower = Closest;
        CalculateGridPath();  // Recalc path immediately on new target.
    }
}

// Calculates A* path to target.
void AEnemyCharacter::CalculateGridPath()
{
    if (!TargetTower || !GridMgr) return;

    FIntPoint StartIdx = GridMgr->WorldToGridIndex(GetActorLocation());
    FIntPoint EndIdx = GridMgr->WorldToGridIndex(TargetTower->GetActorLocation());

    TArray<FIntPoint> PathIdx = GridMgr->FindPath(StartIdx, EndIdx);

    PathPoints.Empty();
    CurrentPathIndex = 0;

    if (!PathIdx.IsEmpty())
    {
        // Convert grid indices to world positions (centers for smooth movement).
        for (FIntPoint Idx : PathIdx)
        {
            PathPoints.Add(GridMgr->GridToWorldCenter(Idx));
        }
        UE_LOG(LogTemp, Log, TEXT("✅ %s A* PATH: %d points"), *GetName(), PathPoints.Num());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("❌ %s NO PATH (direct fallback)"), *GetName());
    }
}

// Gets the next waypoint in the path.
FVector AEnemyCharacter::GetNextPathPoint()
{
    if (PathPoints.IsValidIndex(CurrentPathIndex))
    {
        FVector Cur = PathPoints[CurrentPathIndex];
        float Dist = FVector::Dist(GetActorLocation(), Cur);

        // Advance if close (threshold to prevent jitter).
        if (Dist < 100.f)
        {
            ++CurrentPathIndex;
        }
        else
        {
            return Cur;
        }
        
        return Cur;
    }
    
    // Fallback: Direct to target if path exhausted.
    return TargetTower ? TargetTower->GetActorLocation() : GetActorLocation();
}

// =============================================================
// ATTACK AND DAMAGE
// =============================================================
// Attacks the current target with continuous damage.
void AEnemyCharacter::AttackTarget()
{
    if (!TargetTower || !IsValid(TargetTower)) return;

    // Damage scaled by time for frame-rate independence.
    const float Damage = 30.f * GetWorld()->DeltaTimeSeconds;
    TargetTower->TakeDamageCustom(Damage);
    
    // Clear target if destroyed.
    if (TargetTower->bIsDestroyed == true)
    {
        TargetTower = nullptr;
    }
}

// Applies damage to the enemy.
void AEnemyCharacter::TakeDamageCustom(float DamageAmount)
{
    if (DamageAmount <= 0.f) return;

    CurrentHealth -= DamageAmount;
    if (CurrentHealth <= 0.f)
    {
        // Reward player with bounty (intent: economy integration).
        if (ATopDownPlayerController* PC = Cast<ATopDownPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
        {
            PC->CurrentCoins += Bounty;
            UE_LOG(LogTemp, Log, TEXT("Player earned %.0f coins from %s"), Bounty, *GetName());
        }
        
        UE_LOG(LogTemp, Log, TEXT("%s DIED"), *GetName());
        
        // Notify GameManager for wave completion check.
        if (GameManager)
        {
            GameManager->OnEnemyDeath(this);
        }
        Destroy();
    }
}

// Damage from cannon (wrapper for custom damage).
void AEnemyCharacter::TakeDamageFromCannon(float Damage)
{
    TakeDamageCustom(Damage);
}