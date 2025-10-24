

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"


class ABaseHQ;

UCLASS()
class GROUP2_EXAM_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemy();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy")
	float MaxHealth = 40.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy")
	float CurrentHealth = 40.f;

	// DMG per second to HQ when in range
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy")
	float DamagePerSecond = 10.f;

	// movement speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy")
	float MoveSpeed = 300.f;

	// Range to attack the HQ (distance)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy")
	float AttackRange = 120.f;

	// target HQ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy")
	ABaseHQ* HQTarget = nullptr;

	// simple health
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

private:
	void MoveTowardsHQ(float DeltaTime);
	void AttackHQ(float DeltaTime);
};
