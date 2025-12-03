#include "EnemyCharacter.h"

#include "DormantPowerCores.h"
#include "GameManager.h"
#include "PowerNode.h"
#include "PowerCannon.h"
#include "PowerCore.h"
#include "DormantPowerCores.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GridManager.h"
#include "GameManager.h"
#include "TopDownPlayerController.h"


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

    MaxHealth = 100.0f;
    

    

    // Movement setup.
    UCharacterMovementComponent* MoveComp = GetCharacterMovement();
    MoveComp->bOrientRotationToMovement = true;
    MoveComp->RotationRate = FRotator(0.f, 540.f, 0.f);
    MoveComp->MaxWalkSpeed = MoveSpeed;
    MoveComp->bRequestedMoveUseAcceleration = false;

    MoveComp->AirControl = 1.0f;  // Full control in air (matches ground speed).
    MoveComp->FallingLateralFriction = 0.0f;  // No drag while falling.

    // Collision setup.
    GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
    GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Block);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
    GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

    // Healthbar setup
    HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
    HealthBarComponent->SetupAttachment(RootComponent);
    
}

// =============================================================
// BEGIN PLAY
// =============================================================
// Finds grid manager and initial target.
void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    // Find GameManager
    TArray<AActor*> GameManagers;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameManager::StaticClass(), GameManagers);
    if (GameManagers.Num() > 0)
    {
        GameManager = Cast<AGameManager>(GameManagers[0]);
    }

    CurrentHealth = MaxHealth;

    

    // Health Bar
    if (HealthBarComponent)
    {
        UUserWidget* WidgetObject = HealthBarComponent->GetWidget();
        HealthBar = Cast<UW_HealthBar>(WidgetObject);
    }


    // Snap to ground if not already.
    FHitResult SnapHit;
    FVector SnapEnd = GetActorLocation() - FVector(0, 0, 200.0f);  // Short down trace.
    if (GetWorld()->LineTraceSingleByChannel(SnapHit, GetActorLocation(), SnapEnd, ECC_WorldStatic, FCollisionQueryParams::DefaultQueryParam))
    {
        if (SnapHit.bBlockingHit)
        {
            FVector NewLoc = SnapHit.Location + FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
            SetActorLocation(NewLoc);
            GetCharacterMovement()->SetMovementMode(MOVE_Walking);
            UE_LOG(LogTemp, Log, TEXT("%s SNAPPED to ground Z=%.1f"), *GetName(), NewLoc.Z);
        }
    }

    
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
    UpdateHealthBar();
}

// =============================================================
// TICK
// =============================================================
// Handles target searching, path recalc, movement, and attacking.
void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Debug movement (remove later).
    static float DebugTimer = 0.f;
    DebugTimer += DeltaTime;
    if (DebugTimer >= 0.5f)
    {
        bool bHasTarget = TargetTower && IsValid(TargetTower);
        float DistToTarget = bHasTarget ? FVector::Dist(GetActorLocation(), TargetTower->GetActorLocation()) : -1.f;
        FVector NextPt = GetNextPathPoint();
        float DistToNext = FVector::Dist(GetActorLocation(), NextPt);
        float VelSize = GetCharacterMovement()->Velocity.Size();
        bool bGrounded = GetCharacterMovement()->IsMovingOnGround();
        int32 PathLen = PathPoints.Num();

        UE_LOG(LogTemp, Log, TEXT("🔍 %s | Target=%s | DistT=%.0f | DistN=%.0f | Vel=%.0f | Grounded=%s | Path=%d"),
               *GetName(),
               bHasTarget ? *TargetTower->GetName() : TEXT("NONE"),
               DistToTarget,
               DistToNext,
               VelSize,
               bGrounded ? TEXT("YES") : TEXT("NO"),
               PathLen);

        DebugTimer = 0.f;
    }
    
    // Retry finding target periodically.
    TimeSinceLastSearch += DeltaTime;
    if (TimeSinceLastSearch >= SearchInterval)
    {
        FindClosestTarget();
        TimeSinceLastSearch = 0.f;
    }
    
    // Recalculate path periodically.
    TimeSincePathRecalc += DeltaTime;
    PathRecalcInterval = 2.0f;
    if (TargetTower && TimeSincePathRecalc >= PathRecalcInterval)
    {
        CalculateGridPath();
        bDoPathfinding = false;
        TimeSincePathRecalc = 0.f;
    }
    
    // Move or attack if target exists.
    if (TargetTower && IsValid(TargetTower))
    {
        FVector Next = GetNextPathPoint();
        FVector Dir = (Next - GetActorLocation()).GetSafeNormal();

        if (!Dir.IsNearlyZero())
        {
            AddMovementInput(Dir, 1.0f);
        }

        float DistToTarget = FVector::Dist(GetActorLocation(), TargetTower->GetActorLocation());
        if (DistToTarget <= AttackRange)
        {
            AttackTarget();
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
                    if (N->bIsPlaced)
                    {
                        BestDist = D;
                        TargetTower = N;
                    }
                }
            }
        }
    }

    if (TargetTower)
    {
        UE_LOG(LogTemp, Log, TEXT("%s → %s (%.0f)"), *GetName(),
               *TargetTower->GetName(), BestDist);
        UE_LOG(LogTemp, Log, TEXT("target found"));
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



    bool bStartValid = GridMgr->IsValidIndex(StartIdx);
    bool bEndValid = GridMgr->IsValidIndex(EndIdx);

    
    UE_LOG(LogTemp, Log, TEXT("🗺️ %s PATH CALC | StartIdx=%d,%d (%s) | EndIdx=%d,%d (%s)"),
           *GetName(), StartIdx.X, StartIdx.Y, bStartValid ? TEXT("OK") : TEXT("OUT"),
           EndIdx.X, EndIdx.Y, bEndValid ? TEXT("OK") : TEXT("OUT"));

    TArray<FIntPoint> GridPath = GridMgr->FindPath(StartIdx, EndIdx);

    PathPoints.Empty();
    CurrentPathIndex = 0;
    if (GridPath.Num() > 0)
    {
        for (const FIntPoint& Idx : GridPath)
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

void AEnemyCharacter::UpdateHealthBar()
{
    UE_LOG(LogTemp, Log, TEXT("Test Damage 1"));

    if (!HealthBar) return;
    
    float EnemyPercent = 0.0f;
    
    EnemyPercent = CurrentHealth / MaxHealth;

    HealthBar->SetProgress(EnemyPercent);


    UE_LOG(LogTemp, Log, TEXT("Test Damage 2"));
    
}

// Gets the next waypoint in the path.
FVector AEnemyCharacter::GetNextPathPoint()
{
    if (PathPoints.IsValidIndex(CurrentPathIndex))
    {
        FVector Cur = PathPoints[CurrentPathIndex];
        float Dist = FVector::Dist(GetActorLocation(), Cur);

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
    if (bIsTowerDestroyed == true)
    {
        TargetTower = nullptr;
    }
}

// Applies damage to the enemy.
void AEnemyCharacter::TakeDamageCustom(float DamageAmount)
{
    if (DamageAmount <= 0.f) return;
    
    CurrentHealth -= DamageAmount;

    UpdateHealthBar();
    if (CurrentHealth <= 0.f)
    {
        // Reward coins
        if (ATopDownPlayerController* PC = Cast<ATopDownPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0)))
        {
            PC->CurrentCoins += Bounty;
            UE_LOG(LogTemp, Log, TEXT("Player earned %.0f coins from %s"), Bounty, *GetName());
        }
        
        UE_LOG(LogTemp, Log, TEXT("%s DIED"), *GetName());
        if (GameManager)
        {
            GameManager->OnEnemyDeath(this);
        }
        Destroy();
    }
}

