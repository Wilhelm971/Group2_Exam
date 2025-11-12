#include "TopDownPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"


ATopDownPlayerController::ATopDownPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void ATopDownPlayerController::BeginPlay()
{
	Super::BeginPlay();

	

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		if (InputMapping)
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}

	ControlledPawn = GetPawn();
}

void ATopDownPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		if (MoveAction)
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::HandleMove);

		if (ZoomAction)
			EnhancedInput->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ATopDownPlayerController::HandleZoom);

		// BUILDING INPUTS
		if (BuildAction)
			EnhancedInput->BindAction(BuildAction, ETriggerEvent::Started, this, &ATopDownPlayerController::ToggleBuildingMode);

		if (PlaceAction)
			EnhancedInput->BindAction(PlaceAction, ETriggerEvent::Started, this, &ATopDownPlayerController::PlaceBuilding);

		if (CancelAction)
			EnhancedInput->BindAction(CancelAction, ETriggerEvent::Started, this, &ATopDownPlayerController::CancelBuilding);
	}
}

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

	// NEW: Update preview if active

	if (bBuildingModeActive)
	{
		UpdatePreviewPosition();
	}
}


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

void ATopDownPlayerController::HandleZoom(const FInputActionValue& Value)
{
	const float ZoomValue = Value.Get<float>();
	if (FMath::IsNearlyZero(ZoomValue)) return;

	TargetArmLength -= ZoomValue * ZoomSpeed * GetWorld()->GetDeltaSeconds();
	TargetArmLength = FMath::Clamp(TargetArmLength, MinZoom, MaxZoom);

}

void ATopDownPlayerController::ToggleBuildingMode(const FInputActionValue& Value)
{
	
	// UE_LOG(LogTemp, Log, TEXT("C is pressed"));
	if (bBuildingModeActive)
	{
		// You can press c to cancel the building of the cannon
		CancelBuildingMode();
	}
	else
	{
		StartBuildingMode();
	}
}

void ATopDownPlayerController::PlaceBuilding(const FInputActionValue& Value)
{
	if (!bBuildingModeActive || !PreviewCannon || !PreviewCannon->bPlacementValid) return;

	// FINALIZE PLACEMENT
	PreviewCannon->SetActorEnableCollision(true);
	PreviewCannon->SetPreviewMode(false);

	UE_LOG(LogTemp, Log, TEXT("Cannon placed!"));

	PreviewCannon = nullptr;
	bBuildingModeActive = false;

	// TODO: Spend currency, play SFX, VFX
}

void ATopDownPlayerController::CancelBuilding(const FInputActionValue& Value)
{
	if (bBuildingModeActive)
	{
		CancelBuildingMode();
	}
}

void ATopDownPlayerController::StartBuildingMode()
{

	
	if (!CannonToPlaceClass) return;

	
	
	
	bBuildingModeActive = true;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	FVector SpawnLoc(0.f); // Will be updated immediately
	PreviewCannon = GetWorld()->SpawnActor<APowerCannon>(CannonToPlaceClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);

	if (PreviewCannon)
	{
		PreviewCannon->SetActorEnableCollision(false);
		PreviewCannon->SetPreviewMode(true);
		UE_LOG(LogTemp, Log, TEXT("Building mode started - preview active"));
	}
}

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

void ATopDownPlayerController::UpdatePreviewPosition()
{
	if (!PreviewCannon || !ControlledPawn) return;

	FHitResult HitResult;
	if (GetHitResultUnderCursor(PlacementTraceChannel, true, HitResult))
	{
		if (HitResult.bBlockingHit)
		{
			FVector PlaceLocation = HitResult.Location;
			PreviewCannon->SetActorLocation(PlaceLocation);
			PreviewCannon->SetActorRotation(FRotator::ZeroRotator);  // Fixed rotation - customize later

			PreviewCannon->CheckPlacementValidity();
		}
		else
		{
			// No valid surface
			PreviewCannon->bPlacementValid = false;
			PreviewCannon->UpdatePreviewVisuals();
		}
	}
}


