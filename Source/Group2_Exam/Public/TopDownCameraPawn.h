#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TopDownCameraPawn.generated.h"

/**
 * ATopDownCameraPawn
 * 
 * Custom pawn for handling top-down camera setup with spring arm.
 * Used for isometric/top-down views in the game.
 */
UCLASS()
class GROUP2_EXAM_API ATopDownCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	// =============================================================
	// CONSTRUCTOR
	// =============================================================
	/** Sets default values for this pawn's properties. */
	ATopDownCameraPawn();

	// =============================================================
	// COMPONENTS
	// =============================================================
	/** Spring arm component for camera positioning and zooming. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* CameraBoom;

	/** Camera component for top-down view. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* TopDownCamera;
};