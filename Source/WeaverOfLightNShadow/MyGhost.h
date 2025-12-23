#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyGhost.generated.h"


class USphereComponent;
class UStaticMeshComponent;

/*
* AMyGhost
* 
* An enemy AI that patrols between two points and chase the player
* when detected. The ghost hovers above walkable surfaces, killk the
* palyer on contact, and reacts to player proximity.
*/
UENUM(BlueprintType)
enum class EGhostState : uint8
{
	Patrol,
	Chase
};
UCLASS()
class WEAVEROFLIGHTNSHADOW_API AMyGhost : public AActor
{
	GENERATED_BODY()
	
public:	
	AMyGhost();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// ------------ AI Helpers -----------------
	void UpdateHover(float DeltaTime);
	void UpdateState(float DeltaTime);
	void MoveTowards(const FVector& Destination, float Speed, float DeltaTime);
	FVector GetPatrolA() const;
	FVector GetPatrolB() const;

public:
	// ------------ Components -----------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ghost");
	USphereComponent* HurtTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ghost");
	UStaticMeshComponent* GhostMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost");
	bool bRespawnOnKill = true;

	// ------------ Movement and Behavior -----------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost");
	float HoverAmplitude = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost");
	float HoverSpeed = 1.0f;

	// Patrol, moving back and forth from start location to start location + WanderOffset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost")
	FVector WanderOffset = FVector(600.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost")
	float PatrolSpeed = 150.f;

	//Chase, chase the character when character within DetectRadius
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost")
	float DetectRadius = 900.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost")
	float LoseRadius = 1200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost")
	float ChaseSpeed = 300.f;

	//Distance at which the ghost is considered reach the switch position
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost");
	float PatrolSwitchDistance = 30.f;

	// ----------- Physics and Grounding -----------
	UPROPERTY(EditAnywhere, Category = "Ghost")
	float GravityZ = -980.f;
	
	UPROPERTY(EditAnywhere, Category = "Ghost")
	float MaxFallSpeed = 1000.f;

	// --------------- Audio ---------------
	
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* MoveSound;
	UPROPERTY(EditAnywhere, Category = "Audio")
	float MoveInterval_Patrol = 0.4f;
	UPROPERTY(EditAnywhere, Category = "Audio")
	float MoveInterval_Chase = 0.2f;
	float MoveTimer = 0.0f;
	// Change state from Patrol -> Chase
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* ChaseSound;

	
	FVector Velocity;
	bool bOnGround = false;

	//Gravity Helper
	bool IsStandingOnWalkable(FHitResult* OutHit = nullptr)const;

	// --------------- Combat and Interaction --------------
	UFUNCTION()
	void OnHurtBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ghost")
	void KillPlayer(AActor* Victim);
	virtual void KillPlayer_Implementation(AActor* Victim);

private:
	// --------- Internal state -----------
	FVector StartLoc;
	float HoverTime = 0.f;
	bool bGoingToB = true;
	EGhostState CurrentState = EGhostState::Patrol;
};
