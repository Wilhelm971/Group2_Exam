#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PowerNode.h"
#include "GridManager.h"          // <-- NEW
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

	// ---------- STATS ----------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MoveSpeed = 300.f;

	// ---------- DAMAGE ----------
	UFUNCTION(BlueprintCallable, Category = "Damage")
	void TakeDamageCustom(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void TakeDamageFromCannon(float Damage);

	// ---------- AI ----------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class APowerNode* TargetTower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float PathRecalcInterval = 1.f;

private:
	// ----- Helpers -----
	void FindClosestTarget();
	void CalculateGridPath();               // <-- NEW – uses GridManager A*
	FVector GetNextPathPoint();
	void AttackTarget();

	// ----- Timers -----
	float TimeSinceLastSearch = 0.f;
	float TimeSincePathRecalc = 0.f;
	static constexpr float SearchInterval = 0.5f;

	// ----- Path data -----
	TArray<FVector> PathPoints;   // world-space points from GridManager
	int32 CurrentPathIndex = 0;

	// ----- Grid reference -----
	UPROPERTY()
	class AGridManager* GridMgr;
};