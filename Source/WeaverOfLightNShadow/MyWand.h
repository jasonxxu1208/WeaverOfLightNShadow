

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Engine/Engine.h"
#include "MyWand.generated.h"

UCLASS()
class WEAVEROFLIGHTNSHADOW_API AMyWand : public AActor
{
	GENERATED_BODY()

public:
	AMyWand();

protected:
	virtual void BeginPlay() override;

public:
	//virtual void Tick(float DeltaTime) override;

	/** Mesh for the wand */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WandMesh;

	/** Spot light used for the ¡°Lumos¡± strong light */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpotLightComponent* StrongLight;

	// ------------ Gameplay API ------------

	/** Interact with torches in the world (does NOT consume charge) */
	UFUNCTION(BlueprintCallable, Category = "Wand")
	void ToggleLight(AActor* TargetActor);

	/** Attack an enemy (consumes charge) */
	UFUNCTION(BlueprintCallable, Category = "Wand")
	void AttackEnemy(AActor* TargetEnemy);

	/** Turn on strong cone light for a duration (consumes charge) */
	UFUNCTION(BlueprintCallable, Category = "Wand")
	void ActivateStrongLight();

	/** Raycast from camera to find aimed actor */
	UFUNCTION(BlueprintCallable, Category = "Wand")
	AActor* GetAimedActor(float MaxDistance = 1500.0f);

	UFUNCTION(BlueprintCallable, Category = "Wand")
	bool HasCharges() const { return ChargeCount > 0; }

	// ------------ Charges / duration ------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wand")
	int32 ChargeCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wand")
	float StrongLightDuration = 5.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wand")
	bool bIsStrongLightActive = false;

	// ------------ Strong light tuning ------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wand", meta = (ClampMin = "0.0"))
	float StrongLightIntensity = 50000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wand", meta = (ClampMin = "0.0"))
	float StrongLightAttenuationRadius = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wand", meta = (ClampMin = "0.0"))
	float StrongLightInnerCone = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wand", meta = (ClampMin = "0.0"))
	float StrongLightOuterCone = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wand")
	FLinearColor StrongLightColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wand")
	bool bStrongLightCastShadows = true;

private:
	bool ConsumeCharge();
	void DeactivateStrongLight();
	void ApplyStrongLightSettings();
	FTimerHandle StrongLightTimer;
};