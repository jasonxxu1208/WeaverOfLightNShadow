// Fill out your copyright notice in the Description page of Project Settings.

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
	// Sets default values for this actor's properties
	AMyWand();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WandMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpotLightComponent* StrongLight;

	// 功能函数
	UFUNCTION(BlueprintCallable, Category = "Wand")
	void ToggleLight(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Wand")
	void AttackEnemy(AActor* TargetEnemy);

	UFUNCTION(BlueprintCallable, Category = "Wand")
	void ActivateStrongLight();

	// 属性
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wand")
	int32 ChargeCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wand")
	float StrongLightDuration = 5.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wand")
	bool bIsStrongLightActive = false;

	UFUNCTION(BlueprintCallable, Category = "Wand")
	AActor* GetAimedActor(float MaxDistance = 500.0f);

	UFUNCTION(BlueprintCallable, Category = "Wand")
	bool HasCharges() const { return ChargeCount > 0; }

private:
	bool ConsumeCharge();
	void DeactivateStrongLight();
	FTimerHandle StrongLightTimer;
};