/**
* @file TopDownCameraPawn.h
 * @brief Header file for the ATopDownCameraPawn class, handling top-down camera setup.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TopDownCameraPawn.generated.h"

/**
 * @class ATopDownCameraPawn
 * @brief Custom pawn for handling top-down camera setup with spring arm.
 * 
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
	/** 
	 * @brief Sets default values for this pawn's properties.
	 */
	ATopDownCameraPawn();

	// =============================================================
	// COMPONENTS
	// =============================================================
	/** 
	 * @brief Spring arm component for camera positioning and zooming.
	 * 
	 * Visible in the editor but read-only in blueprints.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* CameraBoom;

	/** 
	 * @brief Camera component for top-down view.
	 * 
	 * Visible in the editor but read-only in blueprints.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* TopDownCamera;
};