
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Engine/Engine.h"
#include "MyWand.generated.h"

/*
* AMyWand
* 
* The wand carried by character and responsible for all three spells logic
* Interact with torches(Light up/off)
* Attack enemies
* Casting Lumos(strong light to activate shadow bridge)
* 
* Except for that the wand also controls the related audio
*/
UCLASS()
class WEAVEROFLIGHTNSHADOW_API AMyWand : public AActor
{
	GENERATED_BODY()

public:
	AMyWand();

protected:
	// Initialize components and default state
	virtual void BeginPlay() override;

public:
	// ----------- Components ------------
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpotLightComponent* StrongLight;
	
	// ------------- Audio ----------------
	
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* AttackSound;

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* LumosSound;

	// ------------ Gameplay Actions ------------
	
	// Interact with torches in the world (does NOT consume charge)
	UFUNCTION(BlueprintCallable, Category = "Wand")
	void ToggleLight(AActor* TargetActor);

	// Attack an enemy (consumes charge)
	UFUNCTION(BlueprintCallable, Category = "Wand")
	void AttackEnemy(AActor* TargetEnemy);

	// Activate strong cone light for a few seconds(duration) (consumes charge)
	UFUNCTION(BlueprintCallable, Category = "Wand")
	void ActivateStrongLight();

	// Raycast from the camera to detect what is the wand is aiming at.
	UFUNCTION(BlueprintCallable, Category = "Wand")
	AActor* GetAimedActor(float MaxDistance = 1500.0f);

	// A helper method to check resource availability
	UFUNCTION(BlueprintCallable, Category = "Wand")
	bool HasCharges() const { return ChargeCount > 0; }

	// --------------Charges and duration -------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wand")
	int32 ChargeCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wand")
	float StrongLightDuration = 5.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wand")
	bool bIsStrongLightActive = false;

	//--------------- Strong light details -----------------

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
	// ----------------- Internal helper methods ------------
	
	// Check value of charges if less than 0 return false, 
	// if >0 return true and decrease charge by 1
	bool ConsumeCharge();

	// Disable Lumos and reset its active state
	void DeactivateStrongLight();

	// Apply the details of the light to spotlight component
	void ApplyStrongLightSettings();

	// Timer for Lumos
	FTimerHandle StrongLightTimer;
};