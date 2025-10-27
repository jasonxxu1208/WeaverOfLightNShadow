// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
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

	// 最基础的组件 - 只保留一个
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WandMesh;

	// 最基础的函数 - 先只声明，不添加复杂逻辑
	UFUNCTION(BlueprintCallable, Category = "Wand")
	void SimpleToggleLight();

	// 最基础的属性 - 先只用最简单的
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wand")
	int32 ChargeCount = 3;
};
