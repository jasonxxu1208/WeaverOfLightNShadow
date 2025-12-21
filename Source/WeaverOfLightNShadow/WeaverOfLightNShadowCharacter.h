// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "WeaverOfLightNShadowCharacter.generated.h"

// Forward declarations (avoid including MyWand.h in the header)
class AMyWand;
class UChildActorComponent;
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

/** A basic first person character */
UCLASS(abstract)
class AWeaverOfLightNShadowCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: first person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FirstPersonMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

protected:

	/** Helper to get the actual C++ wand actor instance */
	AMyWand* GetWand() const;

	// Input Actions
	UPROPERTY(EditAnywhere, Category = "Input") UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category = "Input") UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = "Input") UInputAction* LookAction;
	UPROPERTY(EditAnywhere, Category = "Input") UInputAction* MouseLookAction;

	// Wand inputs
	UPROPERTY(EditAnywhere, Category = "Input") UInputAction* ToggleTorchAction;  // F
	UPROPERTY(EditAnywhere, Category = "Input") UInputAction* AttackAction;       // LMB
	UPROPERTY(EditAnywhere, Category = "Input") UInputAction* LumosAction;       // Q

	// Handle Death
	UPROPERTY(EditAnywhere, Category = "Death")
	bool bIsDead = false;
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* DeathSound;
	bool bDeathTimerStarted = false;
	FTimerHandle DeathTimerHandle;
	void CheckKillZ();

	// Audio
	// Footsteps
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* FootstepSound;
	UPROPERTY(EditAnywhere, Category = "Audio")
	float FootstepInterval = 0.45f;
	float FootstepTimer = 0.0f;
	void HandleFootsteps(float DeltaTime);

	//Jump
	UPROPERTY(EditAnywhere, Category = "Audio")
	USoundBase* JumpSound;
	virtual void Landed(const FHitResult& Hit) override;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// Raw input handlers
	void MoveInput(const FInputActionValue& Value);
	void LookInput(const FInputActionValue& Value);
	void HandleToggleTorch(const FInputActionValue& Value);
	void HandleAttack(const FInputActionValue& Value);
	void HandleLumos(const FInputActionValue& Value);

	// Utility
	UFUNCTION(BlueprintCallable, Category = "Input") virtual void DoAim(float Yaw, float Pitch);
	UFUNCTION(BlueprintCallable, Category = "Input") virtual void DoMove(float Right, float Forward);
	UFUNCTION(BlueprintCallable, Category = "Input") virtual void DoJumpStart();
	UFUNCTION(BlueprintCallable, Category = "Input") virtual void DoJumpEnd();

	// Set up input action bindings
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

public:
	AWeaverOfLightNShadowCharacter();

	USkeletalMeshComponent* GetFirstPersonMesh() const { return FirstPersonMesh; }
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UFUNCTION(BlueprintCallable, Category = "Death")
	void Die();
};
