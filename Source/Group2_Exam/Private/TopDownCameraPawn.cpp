#include "TopDownCameraPawn.h"

/**
 * ATopDownCameraPawn: A custom pawn for top-down camera setup.
 * This class initializes a camera with a spring arm for isometric/top-down views.
 */

// =============================================================
// CONSTRUCTOR
// =============================================================
// Sets default values for the pawn and its components.
ATopDownCameraPawn::ATopDownCameraPawn()
{
	// Enable ticking for potential dynamic updates (can be disabled for performance).
	PrimaryActorTick.bCanEverTick = true;

	// Create root component as scene (no mesh needed).
	USceneComponent* RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	// Create spring arm: Default length and rotation for isometric view.
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 2500.f;  // Initial zoom level.
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));  // Downward angle.
	CameraBoom->bDoCollisionTest = false;  // No clipping.
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;

	// Create camera: Attached to boom; no pawn rotation control.
	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetupAttachment(CameraBoom);
	TopDownCamera->bUsePawnControlRotation = false;
}