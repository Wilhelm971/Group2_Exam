#pragma once

#include "CoreMinimal.h"
#include "PowerNode.h"
#include "PowerCannon.generated.h"



UCLASS()
class GROUP2_EXAM_API APowerCannon : public APowerNode
{
	GENERATED_BODY()

public:
	APowerCannon();

protected:
	virtual void BeginPlay() override;

public:
	// Called when this cannon is powered and ready to shoot
	virtual void ReceivePower(APowerNode* FromNode) override;

	// Called when this cannon loses power
	virtual void LosePower() override;

	// Try to shoot at nearby enemies
	void TryShoot();

	// How often the cannon fires (if powered)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cannon")
	float FireInterval = 2.0f;

	// How far the cannon can detect and shoot enemies
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cannon")
	float AttackRange = 1500.0f;

	// Projectile or damage settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cannon")
	float Damage = 25.0f;

private:
	FTimerHandle TimerHandle_Fire;
	void FireAtEnemy(AActor* Target);

	
};
