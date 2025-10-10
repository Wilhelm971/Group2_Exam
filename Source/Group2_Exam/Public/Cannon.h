

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "Cannon.generated.h"

/**
 * 
 */
UCLASS()
class GROUP2_EXAM_API ACannon : public ABuilding
{
	GENERATED_BODY()

public:
	ACannon();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void OnReceivePower(float amount) override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Cannon")
	float PowerNeededPerShot;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Cannon")
	float FireCooldown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Cannon")
	float TimeSinceLastFire;


protected:
	void TryFire();
};
