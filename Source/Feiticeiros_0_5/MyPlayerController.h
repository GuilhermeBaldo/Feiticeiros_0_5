// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FEITICEIROS_0_5_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AMyPlayerController();

protected:
	
	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** Function for beginning evocation spell casting. This should only be triggered by the local player.*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void StartCast();

	/** Function for ending weapon fire. Once this is called, the player can use StartFire again.*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void StopCast();

	/** Server function for spawning projectiles.*/
	UFUNCTION(Server, Reliable, WithValidation)
	void HandleCast();

	/** A timer handle used for providing the fire rate delay in-between spawns.*/
	FTimerHandle CastingTimer;

	/** If true, we are in the process of firing projectiles. */
	bool bIsCasting;

	/** Delay between shots in seconds. Used to control fire rate for our test projectile, but also to prevent an overflow of server functions from binding SpawnProjectile directly to input.*/
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float CastRate;

	/** The type of projectile the character is going to fire.*/
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|Projectile")
	TSubclassOf<class AEvocationSpell> EvocationSpellClass;
};
