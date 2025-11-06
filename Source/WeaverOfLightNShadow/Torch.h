// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Torch.generated.h"

UCLASS()
class WEAVEROFLIGHTNSHADOW_API ATorch : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATorch();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Torch")
	UStaticMeshComponent* TorchMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Torch")
	UPointLightComponent* TorchLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Torch")
	bool bIsLit = true;

	UFUNCTION(BlueprintCallable, Category = "Torch")
	void ToggleLight();

	UFUNCTION(BlueprintCallable, Category = "Torch")
	void SetLit(bool bNewLit);
};
