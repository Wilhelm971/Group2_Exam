#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "PowerLine.generated.h"

/**
 * APowerLine
 * 
 * Actor for visualizing power transmission as a spline-based line.
 * Used for temporary effects like shots or pulses.
 * 
 * @note Supports color, thickness, and lifetime for auto-destruction.
 */
UCLASS()
class GROUP2_EXAM_API APowerLine : public AActor
{
    GENERATED_BODY()

public:
    /**
     * Default constructor. Sets up spline and mesh.
     */
    APowerLine();

protected:
    /**
     * Applies default mesh and material after initialization.
     */
    virtual void PostInitializeComponents() override;

public:
    /** Spline component for line path. Read-only in Blueprints. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USplineComponent* SplineComp;

    /** Static mesh for line segments (e.g., cylinder). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    UStaticMesh* SegmentMesh;

    /** Base material for emissive effects. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    UMaterialInterface* BaseEmissiveMaterial;

    /** Parameter name for emissive color in material. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
    FName EmissiveColorParamName = TEXT("EmissiveColor");

    /**
     * Sets up the line between start and end points.
     * @param Start Starting world position.
     * @param End Ending world position.
     * @param Color Color for emissive material.
     * @param Lifetime Duration before auto-destroy (negative for permanent).
     */
    UFUNCTION(BlueprintCallable, Category = "Line")
    void SetLine(const FVector& Start, const FVector& End, const FColor& Color, float Lifetime = 2.0f);

    /** Thickness of the line (scale for mesh). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line")
    float Thickness = 0.06f;

private:
    /** Dynamic material instance for runtime color changes. */
    UPROPERTY()
    UMaterialInstanceDynamic* DynamicMaterial;

    /** Spline mesh component for rendering the line. */
    UPROPERTY()
    USplineMeshComponent* SplineMeshComp;

    /** Timer for auto-destruction. */
    FTimerHandle DestroyTimer;

    /**
     * Destroys the actor after lifetime expires.
     */
    UFUNCTION()
    void SelfDestroy();
};