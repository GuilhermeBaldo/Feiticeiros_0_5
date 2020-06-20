// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS()
class FEITICEIROS_0_5_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

    /** Camera boom positioning the camera behind the character */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class USpringArmComponent* CameraBoom;

    /** Follow camera */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    class UCameraComponent* FollowCamera;

public:
	// Sets default values for this character's properties
	AMyCharacter();

    /** Property replication */
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    /** The player's maximum health. This is the highest that their health can be, and the value that their health starts at when spawned.*/
    UPROPERTY(EditDefaultsOnly, Category = "Health")
    float MaxHealth;

    /** The player's current health. When reduced to 0, they are considered dead.*/
    UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)
    float CurrentHealth;

    /** RepNotify for changes made to current health.*/
    UFUNCTION()
    void OnRep_CurrentHealth();

    /** Response to health being updated. Called on the server immediately after modification, and on clients in response to a RepNotify*/
    void OnHealthUpdate();

    /** Function for ending weapon fire. Once this is called, the player can use StartFire again.*/
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void StopCast();

    /** Server function for spawning projectiles.*/
    UFUNCTION(Server, Reliable, WithValidation)
    void HandleCast(FVector2D Direction);

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    /** Getter for Max Health.*/
    UFUNCTION(BlueprintPure, Category = "Health")
    FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

    /** Getter for Current Health.*/
    UFUNCTION(BlueprintPure, Category = "Health")
    FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

    /** Setter for Current Health. Clamps the value between 0 and MaxHealth and calls OnHealthUpdate. Should only be called on the server.*/
    UFUNCTION(BlueprintCallable, Category = "Health")
    void SetCurrentHealth(float healthValue);

    /** Getter for Max Health.*/
    UFUNCTION(BlueprintPure, Category = "Gameplay")
    FORCEINLINE bool GetIsCasting() const { return bIsCasting; }

    /** Function for beginning evocation spell casting. This should only be triggered by the local player.*/
    UFUNCTION(BlueprintCallable, Category = "Gameplay")
    void StartCast(FVector2D Direction);

    /** Event for taking damage. Overridden from APawn.*/
    UFUNCTION(BlueprintCallable, Category = "Health")
    float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
};