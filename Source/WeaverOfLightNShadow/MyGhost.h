#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyGhost.generated.h"


class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class WEAVEROFLIGHTNSHADOW_API AMyGhost : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyGhost();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ghost");
	USphereComponent* HurtTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ghost");
	UStaticMeshComponent* GhostMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost");
	bool bRespawnOnKill = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost");
	float HoverAmplitude = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ghost");
	float HoverSpeed = 1.0f;

	FVector StartLoc;
	FTimerHandle HoverTimer;

	UFUNCTION()
	void OnHurtBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Ghost")
	void KillPlayer(AActor* Victim);
	virtual void KillPlayer_Implementation(AActor* Victim);

	void HoverTick();
};
