

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PowerCore.h"
#include "EnemyCharacter.generated.h"




UCLASS()
class GROUP2_EXAM_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Health system
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Stats")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stats")
	float MoveSpeed = 400.0f;

	UFUNCTION(BlueprintCallable, Category="Damage")
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, 
							 class AController* EventInstigator, class AActor* DamageCauser) override;
	
	UFUNCTION()
	void TakeDamageFromCannon(float Damage);

	// Target: Main PowerCore
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI")
	APowerCore* TargetBase;

private:
	// AI: Find closest PowerCore (robust spawn timing)
	void FindTargetBase();

	// Movement retry timer (if no target on spawn)
	UPROPERTY()
	float TimeSinceLastSearch = 0.0f;

	static constexpr float SearchInterval = 0.5f;  // Retry every 0.5s
};


