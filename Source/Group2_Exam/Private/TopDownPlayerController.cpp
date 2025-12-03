#include "TopDownPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PowerCannon.h"
#include "PowerCore.h"
#include "PowerNetworkSubsystem.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/Pawn.h"
#include "MyUserWidget.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// =============================================================
// CLASS DESCRIPTION
// =============================================================
// ATopDownPlayerController: Handles player input for camera movement, zooming, and building placement.

// =============================================================
// CONSTRUCTOR
// =============================================================
// Sets default input properties.
ATopDownPlayerController::ATopDownPlayerController()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Default;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    CurrentCoins = 350.0f;
}

// =============================================================
// BEGIN PLAY
// =============================================================
// Initializes input mappings and caches controlled pawn.
void ATopDownPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Add enhanced input mapping context.
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
    {
        if (InputMapping)
        {
            Subsystem->AddMappingContext(InputMapping, 0);
        }
    }

    ControlledPawn = GetPawn();

    // Start coin earning timer (every 1 second).
    GetWorld()->GetTimerManager().SetTimer(CoinTimerHandle, this, &ATopDownPlayerController::EarnCoins, 1.0f, true);


    // Create HUD
    if (HUDWidgetClass)
    {
        HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
        if (HUDWidget)
        {
            HUDWidget->AddToViewport();
        }
    }


}


int32 ATopDownPlayerController::GetActiveCoreCount() const
{
    TArray<AActor*> Cores;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerCore::StaticClass(), Cores);
    return Cores.Num();
}

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
// Binds input actions to handler functions.
void ATopDownPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(InputComponent))
    {
        if (MoveAction)
            EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::HandleMove);

        if (ZoomAction)
            EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::HandleZoom);

        // Building inputs.
        if (BuildAction)
            EnhancedInput->BindAction(BuildAction, ETriggerEvent::Started, this, &ATopDownPlayerController::ToggleBuildingMode);

        if (PlaceAction)
            EnhancedInput->BindAction(PlaceAction, ETriggerEvent::Started, this, &ATopDownPlayerController::PlaceBuilding);

        if (CancelAction)
            EnhancedInput->BindAction(CancelAction, ETriggerEvent::Started, this, &ATopDownPlayerController::CancelBuilding);
    }
}

// =============================================================
// TICK
// =============================================================
// Updates camera zoom interpolation and building preview position.
void ATopDownPlayerController::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (!ControlledPawn) return;

    if (USpringArmComponent* SpringArm = ControlledPawn->FindComponentByClass<USpringArmComponent>())
    {
        const float CurrentLength = SpringArm->TargetArmLength;
        const float NewLength = FMath::FInterpTo(CurrentLength, TargetArmLength, DeltaSeconds, 5.0f);
        SpringArm->TargetArmLength = NewLength;
    }

    // Update building preview if in building mode.
    if (bBuildingModeActive)
    {
        UpdatePreviewPosition();
    }
}

// =============================================================
// INPUT HANDLERS
// =============================================================
// Handles camera panning movement.
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

// Handles camera zooming.
void ATopDownPlayerController::HandleZoom(const FInputActionValue& Value)
{
    const float ZoomValue = Value.Get<float>();
    if (FMath::IsNearlyZero(ZoomValue)) return;

    TargetArmLength -= ZoomValue * ZoomSpeed * GetWorld()->GetDeltaSeconds();
    TargetArmLength = FMath::Clamp(TargetArmLength, MinZoom, MaxZoom);
}

// =============================================================
// BUILDING MODE
// =============================================================
// Toggles building mode on/off.
void ATopDownPlayerController::ToggleBuildingMode()
{
    if (bBuildingModeActive)
    {
        CancelBuildingMode();
    }
    else
    {
        StartBuildingMode();
    }
}

// Places the building if valid.
void ATopDownPlayerController::PlaceBuilding(const FInputActionValue& Value)
{
    if (!bBuildingModeActive || !PreviewCannon || !PreviewCannon->bPlacementValid) return;


    if (CurrentCoins < CannonCost)
    {
        UE_LOG(LogTemp, Warning, TEXT("Not enough coins to place cannon! Need %.0f, have %.0f"), CannonCost, CurrentCoins);
        return;  // Optionally add UI feedback here.
    }

    // Deduct cost.

    CurrentCoins -= CannonCost;
    
    // Finalize placement.
    PreviewCannon->SetActorEnableCollision(true);
    PreviewCannon->SetPreviewMode(false);
    PreviewCannon->bIsPlaced = true;

    UE_LOG(LogTemp, Log, TEXT("Cannon placed!"));

    PreviewCannon = nullptr;
    bBuildingModeActive = false;


    
    if (UWorld* World = GetWorld())
    {
        if (UPowerNetworkSubsystem* Net = World->GetSubsystem<UPowerNetworkSubsystem>())
        {
            UE_LOG(LogTemp, Log, TEXT("Updates connections after placing new building"));
            Net->RebuildConnections();
        }
    }

    
    // TODO: Implement SFX, VFX.
}

// Cancels building mode.
void ATopDownPlayerController::CancelBuilding(const FInputActionValue& Value)
{
    if (bBuildingModeActive)
    {
        CancelBuildingMode();
    }
}

// Starts building mode and spawns preview actor.
void ATopDownPlayerController::StartBuildingMode()
{
    if (!CannonToPlaceClass) return;

    bBuildingModeActive = true;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    FVector SpawnLoc(0.f);  // Updated in Tick.
    PreviewCannon = GetWorld()->SpawnActor<APowerCannon>(CannonToPlaceClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);

    if (PreviewCannon)
    {
        PreviewCannon->SetActorEnableCollision(false);
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

// Updates preview position based on cursor hit.
void ATopDownPlayerController::UpdatePreviewPosition()
{
    if (!PreviewCannon || !ControlledPawn) return;

    FHitResult HitResult;
    if (GetHitResultUnderCursor(PlacementTraceChannel, true, HitResult))
    {
        if (HitResult.bBlockingHit)
        {
            FVector PlaceLocation = HitResult.Location;

            // Offset Z to place on surface (adjust based on cannon height).
            PlaceLocation.X -= 50.0f;
            PlaceLocation.Y -= 50.0f;
            PlaceLocation.Z = 0.0f;
            PreviewCannon->SetActorLocation(PlaceLocation);
            PreviewCannon->SetActorRotation(FRotator::ZeroRotator);  // Fixed rotation.

            PreviewCannon->CheckPlacementValidity();
        }
        else
        {
            // Invalid surface.
            PreviewCannon->bPlacementValid = false;
            PreviewCannon->UpdatePreviewVisuals();
        }
    }
}


// =============================================================
// ECONOMY FUNCTIONS (NEW)
// =============================================================
// Earns coins every second.

void ATopDownPlayerController::EarnCoins()
{
    CurrentCoins += CoinsPerSecond;
    UE_LOG(LogTemp, Log, TEXT("Earned %.0f coins. Total: %.0f"), CoinsPerSecond, CurrentCoins);
}