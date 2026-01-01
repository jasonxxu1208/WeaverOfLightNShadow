
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Torch.generated.h"

/*
* ATorch
* 
* A world ineraction actor representing a lightable torch
* Torches can be toggled on or off by the character's wand
* and serves as an environmental light sources
*/
UCLASS()
class WEAVEROFLIGHTNSHADOW_API ATorch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATorch();

protected:
	// Initialize components and default state
	virtual void BeginPlay() override;

public:	
	// ------------- Components -------------
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Torch")
	UStaticMeshComponent* TorchMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Torch")
	UPointLightComponent* TorchLight;

	// Wheather the torch is currenly lit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Torch")
	bool bIsLit = false;

	// ----------- Audio --------------

	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* LightupSound;
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* LightoffSound;

	// ------------- Gameplay Action --------------

	// Toggle the torch's state
	UFUNCTION(BlueprintCallable, Category = "Torch")
	void ToggleLight();

	// Set torch's state
	UFUNCTION(BlueprintCallable, Category = "Torch")
	void SetLit(bool bNewLit);
};
