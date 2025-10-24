

#pragma once

#include "CoreMinimal.h"
#include "Building.h"
#include "Cannon.generated.h"

class AProjectile;
class AEnemy;


UCLASS()
class GROUP2_EXAM_API ACannon : public ABuilding
{
	GENERATED_BODY()

public:
	ACannon();

protected:
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

public:
	// Fire settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cannon")
	float FireInterval = 1.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cannon")
	float FireRange = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cannon")
	float DamagePerShot = 15.f;

	// Power consumed per shot
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cannon")
	float PowerPerShot = 10.f;

	// Projectile class (optional)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cannon")
	TSubclassOf<AProjectile> ProjectileClass;


private:
	FTimerHandle FireTimer;
	void TryFire();
	AEnemy* FindNearestEnemy() const;
};
