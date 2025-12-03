/**
 * @file PowerLine.h
 * @brief Header file for the APowerLine class, which visualizes power connections or shots using splines.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "PowerLine.generated.h"

/**
 * @class APowerLine
 * @brief Actor for rendering a visual line (e.g., power connection or shot) using a spline.
 * 
 * Manages spline-based mesh rendering with customizable color, thickness, and lifetime.
 */
UCLASS()
class GROUP2_EXAM_API APowerLine : public AActor
{
	GENERATED_BODY()

public:
	/**
	 * @brief Default constructor for APowerLine.
	 * 
	 * Initializes components and default properties.
	 */
	APowerLine();

protected:
	/**
	 * @brief Called after all components have been initialized.
	 * 
	 * Sets up spline and mesh components.
	 */
	virtual void PostInitializeComponents() override;

public:
	/** 
	 * @brief Spline component for defining the line path.
	 * 
	 * Visible in the editor but read-only in blueprints.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USplineComponent* SplineComp;

	/** 
	 * @brief Static mesh used for each segment of the line.
	 * 
	 * Editable in the editor and blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	UStaticMesh* SegmentMesh;

	/** 
	 * @brief Base material for the emissive effect.
	 * 
	 * Editable in the editor and blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	UMaterialInterface* BaseEmissiveMaterial;

	/** 
	 * @brief Name of the parameter for emissive color in the material.
	 * 
	 * Editable in the editor and blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	FName EmissiveColorParamName = TEXT("EmissiveColor");

	/**
	 * @brief Sets the start and end points of the line, color, and optional lifetime.
	 * 
	 * @param Start The starting position of the line.
	 * @param End The ending position of the line.
	 * @param Color The color of the line.
	 * @param Lifetime The duration before self-destruction (default 2.0f seconds).
	 */
	UFUNCTION(BlueprintCallable, Category = "Line")
	void SetLine(const FVector& Start, const FVector& End, const FColor& Color, float Lifetime = 2.0f);

	/** 
	 * @brief Thickness of the line segments.
	 * 
	 * Editable in the editor and blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line")
	float Thickness = 0.06f;

private:
	/** 
	 * @brief Dynamic material instance for runtime modifications.
	 */
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	/** 
	 * @brief Spline mesh component for rendering the line.
	 */
	UPROPERTY()
	USplineMeshComponent* SplineMeshComp;

	/** 
	 * @brief Timer handle for self-destruction after lifetime.
	 */
	FTimerHandle DestroyTimer;

	/**
	 * @brief Destroys the actor after the lifetime expires.
	 */
	UFUNCTION()
	void SelfDestroy();
};