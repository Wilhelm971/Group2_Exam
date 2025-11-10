

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "TopDownPlayerController.generated.h"

/**
 * A Player Controller for a top-down tower defence game
 * Handles camera movement, mouse cursor visibility and later the buying new towers
 */


class UInputMappingContext;
class UInputAction;

UCLASS()
class GROUP2_EXAM_API ATopDownPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATopDownPlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaSeconds) override;

private:
	// --- CAMERA ---
	UPROPERTY(EditAnywhere, Category="Camera")
	float PanSpeed = 2000.0f;

	UPROPERTY(EditAnywhere, Category="Camera")
	float ZoomSpeed = 5000.0f;

	UPROPERTY(EditAnywhere, Category="Camera")
	float MinZoom = 1500.0f;

	UPROPERTY(EditAnywhere, Category="Camera")
	float MaxZoom = 5000.0f;

	// Current zoom target length
	float TargetArmLength = 2500.0f;

	// Cached reference to the controlled pawn (the top-down camera pawn)
	APawn* ControlledPawn;



public:
	// --- INPUT SYSTEM ---
	UPROPERTY(EditDefaultsOnly, Category="Enhanced Input")
	UInputMappingContext* InputMapping;

	UPROPERTY(EditDefaultsOnly, Category="Enhanced Input")
	UInputAction* MoveAction;

	UPROPERTY(EditDefaultsOnly, Category="Enhanced Input")
	UInputAction* ZoomAction;

	// --- INPUT HANDLERS ---
	void HandleMove(const FInputActionValue& Value);
	void HandleZoom(const FInputActionValue& Value);
	
};
