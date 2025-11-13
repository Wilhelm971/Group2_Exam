
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PowerNode.h"               // TargetTower (Cannon / Core)
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

	// ----- Stats -------------------------------------------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MoveSpeed = 400.f;

	// ----- Damage ------------------------------------------------
	UFUNCTION(BlueprintCallable, Category = "Damage")
	void TakeDamageCustom(float DamageAmount);

	UFUNCTION()
	void TakeDamageFromCannon(float Damage);

	// ----- AI ----------------------------------------------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class APowerNode* TargetTower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Pathfinding")
	float PathRecalcInterval = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Pathfinding")
	bool bUsePathfinding = true;

private:
	// ----- AI helpers -------------------------------------------
	void FindClosestTarget();
	void CalculatePathToTarget();
	FVector GetNextPathPoint();
	void AttackTarget();

	// ----- Timers -----------------------------------------------
	float TimeSinceLastSearch = 0.f;
	float TimeSincePathRecalc = 0.f;
	static constexpr float SearchInterval = 0.5f;

	// ----- Path data (A*) ---------------------------------------
	TArray<FVector> PathPoints;          // only the world locations we need
	FVector NextWaypoint;

protected:
	// Ignore UE damage system
	virtual float TakeDamage(float DamageAmount,
	                         struct FDamageEvent const& DamageEvent,
	                         class AController* EventInstigator,
	                         class AActor* DamageCauser) override;
};