// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AWand.generated.h"


UCLASS()
class WEAVEROFLIGHTNSHADOW_API AAWand : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAWand();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WandMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Wand Functions")
	void ToggleLight();

	UFUNCTION(BlueprintCallable, Category = "Wand Functions")
	void Attack();

	UFUNCTION(BlueprintCallable, Category = "Wand Functions")
	void SuperIlluminate();

	UFUNCTION(BlueprintCallable, Category = "Wand Functions")
	bool HasCharges() const { return CurrentCharges > 0; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wand properties")
	int32 MaxCharges = 3;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Wand properties")
	int32 CurrentCharges;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wand properties")
	float SuperIlluminateRange = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wand properties")
	float AttackDamage = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wand properties")
	float AttackRange = 2000.0f;

	UFUNCTION(BlueprintImplementableEvent, Category = "Wand Events")
	void OnChargesUpdated(int32 NewCharges);

};
