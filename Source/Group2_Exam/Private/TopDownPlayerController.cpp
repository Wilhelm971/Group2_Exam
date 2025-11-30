#include "TopDownPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PowerCannon.h"
#include "PowerCore.h"
#include "PowerNetworkSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

/**
 * ATopDownPlayerController: Handles player input for camera movement, zooming, and building placement.
 * Manages economy with timed coin earning.
 */

// =============================================================
// CONSTRUCTOR
// =============================================================
// Sets default input properties.
ATopDownPlayerController::ATopDownPlayerController()
{
    bShowMouseCursor = true;  // Enable cursor for UI/building.
    DefaultMouseCursor = EMouseCursor::Crosshairs;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    CurrentCoins = 100.0f;  // Starting coins for player.
}

// =============================================================
// BEGIN PLAY
// =============================================================
// Initializes input mappings and caches controlled pawn.
void ATopDownPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Add enhanced input mapping context for actions.
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
    {
        if (InputMapping)
        {
            Subsystem->AddMappingContext(InputMapping, 0);  // Priority 0 for base mappings.
        }
    }

    ControlledPawn = GetPawn();  // Cache for camera operations.

    // Start coin earning timer (every 1 second for passive income).
    GetWorld()->GetTimerManager().SetTimer(CoinTimerHandle, this, &ATopDownPlayerController::EarnCoins, 1.0f, true);

    // Create HUD widget if class is set (for displaying coins, etc.).
    if (HUDWidgetClass)
    {
        HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
        if (HUDWidget)
        {
            HUDWidget->AddToViewport();
        }
    }
}

// =============================================================
// GET ACTIVE CORE COUNT
// =============================================================
// Utility to count active PowerCores (for win/lose conditions or UI).
int32 ATopDownPlayerController::GetActiveCoreCount() const
{
    TArray<AActor*> Cores;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerCore::StaticClass(), Cores);
    return Cores.Num();
}

// =============================================================
// GET CORE HEALTH PERCENT
// =============================================================
// Utility to get health percentage of PowerCores (truncated in doc; assuming for UI).
float ATopDownPlayerController::GetCoreHealthPercent() const
{
    TArray<AActor*> Cores;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerCore::StaticClass(), Cores);
    float TotalHealth = 0.0f;
    float TotalMaxHealth = 0.0f;
    for (AActor* Actor : Cores)
    {
        if (APowerCore* Core = Cast<APowerCore>(Actor))
        {
            TotalHealth += Core->CurrentHealth;
            TotalMaxHealth += Core->MaxHealth;
        }
    }
    return TotalMaxHealth > 0.0f ? TotalHealth / TotalMaxHealth : 0.0f;
}

// =============================================================
// SETUP INPUT COMPONENT
// =============================================================
// Binds enhanced input actions.
void ATopDownPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(InputComponent))
    {
        // Bind movement, zoom, etc., to handlers.
        EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::HandleMove);
        EnhancedInputComp->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::HandleZoom);
        EnhancedInputComp->BindAction(BuildAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::ToggleBuildingMode);
        EnhancedInputComp->BindAction(PlaceAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::PlaceBuilding);
        EnhancedInputComp->BindAction(CancelAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::CancelBuilding);
    }
}

// =============================================================
// TICK
// =============================================================
// Updates preview during building mode.
void ATopDownPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    // Only update if building active (perf optimization).
    if (bBuildingModeActive)
    {
        UpdatePreviewPosition();
    }
}

// =============================================================
// INPUT HANDLERS
// =============================================================
// Handles camera panning (truncated in doc; assuming moves ControlledPawn).
void ATopDownPlayerController::HandleMove(const FInputActionValue& Value)
{
    if (!ControlledPawn) return;

    const FVector2D MoveValue = Value.Get<FVector2D>();
    if (MoveValue.IsNearlyZero()) return;

    const FVector Forward = FVector::ForwardVector;
    const FVector Right = FVector::RightVector;

    FVector MoveDir = (Forward * MoveValue.Y + Right * MoveValue.X).GetSafeNormal();
    ControlledPawn->AddActorWorldOffset(MoveDir * PanSpeed * GetWorld()->GetDeltaSeconds(), true);

}

// Handles camera zoom (adjusts spring arm length).
void ATopDownPlayerController::HandleZoom(const FInputActionValue& Value)
{
    const float ZoomValue = Value.Get<float>();
    if (FMath::IsNearlyZero(ZoomValue)) return;

    TargetArmLength -= ZoomValue * ZoomSpeed * GetWorld()->GetDeltaSeconds();
    TargetArmLength = FMath::Clamp(TargetArmLength, MinZoom, MaxZoom);

}

// Toggles building mode.
void ATopDownPlayerController::ToggleBuildingMode(const FInputActionValue& Value)
{
    if (bBuildingModeActive)
    {
        CancelBuildingMode();
    }
    else if (CurrentCoins >= CannonCost)  // Check affordability before starting.
    {
        StartBuildingMode();
    }
}

// Places building if valid.
void ATopDownPlayerController::PlaceBuilding(const FInputActionValue& Value)
{
    if (!bBuildingModeActive || !PreviewCannon || !PreviewCannon->bPlacementValid) return;

    // Deduct cost and spawn final cannon.
    CurrentCoins -= CannonCost;
    APowerCannon* PlacedCannon = GetWorld()->SpawnActor<APowerCannon>(CannonToPlaceClass, PreviewCannon->GetActorLocation(), PreviewCannon->GetActorRotation());
    if (PlacedCannon)
    {
        PlacedCannon->SetPreviewMode(false);  // Switch to normal mode.
    }

    CancelBuildingMode();  // End mode after placement.
}

// Cancels building.
void ATopDownPlayerController::CancelBuilding(const FInputActionValue& Value)
{
    if (bBuildingModeActive)
    {
        CancelBuildingMode();
    }
}

// Starts building mode and spawns preview.
void ATopDownPlayerController::StartBuildingMode()
{
    if (!CannonToPlaceClass) return;

    bBuildingModeActive = true;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    FVector SpawnLoc(0.f);  // Initial loc; updated in Tick.
    PreviewCannon = GetWorld()->SpawnActor<APowerCannon>(CannonToPlaceClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);

    if (PreviewCannon)
    {
        PreviewCannon->SetActorEnableCollision(false);  // Disable collision for preview.
        PreviewCannon->SetPreviewMode(true);
        UE_LOG(LogTemp, Log, TEXT("Building mode started - preview active"));
    }
}

// Cancels building mode and destroys preview.
void ATopDownPlayerController::CancelBuildingMode()
{
    if (PreviewCannon)
    {
        PreviewCannon->Destroy();
        PreviewCannon = nullptr;
    }

    bBuildingModeActive = false;
    UE_LOG(LogTemp, Log, TEXT("Building mode cancelled"));
}

// Updates preview position based on cursor.
void ATopDownPlayerController::UpdatePreviewPosition()
{
    if (!PreviewCannon || !ControlledPawn) return;

    FHitResult HitResult;
    if (GetHitResultUnderCursor(PlacementTraceChannel, true, HitResult))
    {
        if (HitResult.bBlockingHit)
        {
            FVector PlaceLocation = HitResult.Location;

            // Offset Z to hover above surface (adjust based on cannon mesh height).
            PlaceLocation.Z += 50.0f;
            PreviewCannon->SetActorLocation(PlaceLocation);
            PreviewCannon->SetActorRotation(FRotator::ZeroRotator);  // Fixed rotation for simplicity.

            PreviewCannon->CheckPlacementValidity();  // Validate position.
        }
        else
        {
            // Invalid hit: Mark as invalid for visuals.
            PreviewCannon->bPlacementValid = false;
            PreviewCannon->UpdatePreviewVisuals();
        }
    }
}

// =============================================================
// ECONOMY FUNCTIONS
// =============================================================
// Earns coins every second.
void ATopDownPlayerController::EarnCoins()
{
    CurrentCoins += CoinsPerSecond;
    UE_LOG(LogTemp, Log, TEXT("Earned %.0f coins. Total: %.0f"), CoinsPerSecond, CurrentCoins);
    // Intent: Passive income; update UI here if needed.
}