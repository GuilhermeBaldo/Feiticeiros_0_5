// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "EvocationSpell.h"
#include "TimerManager.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 700.0f; // The camera follows at this distance behind the character   

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
    // are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

    //Initialize the player's Health
    MaxHealth = 100.0f;
    CurrentHealth = MaxHealth;

    //Initialize projectile class
    EvocationSpellClass = AEvocationSpell::StaticClass();

    //Initialize fire rate
    CastRate = 0.75f;
    bIsCasting = false;

    bIsDead = false;
}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//////////////////////////////////////////////////////////////////////////
// Replicated Properties

void AMyCharacter::GetLifetimeReplicatedProps(TArray <FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AMyCharacter, CurrentHealth);
}

void AMyCharacter::OnHealthUpdate()
{
    //Client-specific functionality
    if (IsLocallyControlled())
    {
        FString healthMessage = FString::Printf(TEXT("You now have %f health remaining."), CurrentHealth);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);

        if (CurrentHealth <= 0)
        {
            FString deathMessage = FString::Printf(TEXT("You have been killed."));
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, deathMessage);
            bIsDead = true;
        }
    }

    //Server-specific functionality
    if (Role == ROLE_Authority)
    {
        FString healthMessage = FString::Printf(TEXT("%s now has %f health remaining."), *GetFName().ToString(), CurrentHealth);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, healthMessage);
    }

    //Functions that occur on all machines. 
    /*
        Any special functionality that should occur as a result of damage or death should be placed here.
    */
}

void AMyCharacter::OnRep_CurrentHealth()
{
    OnHealthUpdate();
}


void AMyCharacter::SetCurrentHealth(float healthValue)
{
    if (Role == ROLE_Authority)
    {
        CurrentHealth = FMath::Clamp(healthValue, 0.f, MaxHealth);
        OnHealthUpdate();
    }
}

float AMyCharacter::TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float damageApplied = CurrentHealth - DamageTaken;
    SetCurrentHealth(damageApplied);
    return damageApplied;
}

void AMyCharacter::StartCast(FVector2D Direction)
{
    if (!bIsCasting)
    {
        bIsCasting = true;
        UWorld* World = GetWorld();
        World->GetTimerManager().SetTimer(CastingTimer, this, &AMyCharacter::StopCast, CastRate, false);
		castDirection = Direction;
    }
}

void AMyCharacter::StopCast()   
{
	HandleCast(castDirection);
    bIsCasting = false;
}

void AMyCharacter::HandleCast_Implementation(FVector2D Direction)
{
    //FRotator CastRotation = Direction.ToOrientationRotator();
    FVector Direction3D = FVector(Direction.X, Direction.Y, 0);
    FRotator CastRotation = Direction3D.ToOrientationRotator();
    FVector CastLocation = GetActorLocation() + CastRotation.RotateVector(FVector(100.0f, 0.0f, 0.0f));

    FActorSpawnParameters CastParameters;
    CastParameters.Instigator = this->Instigator;
    CastParameters.Owner = this;

    AEvocationSpell* spawnedProjectile = GetWorld()->SpawnActor<AEvocationSpell>(CastLocation, CastRotation, CastParameters);
}

bool AMyCharacter::HandleCast_Validate(FVector2D Direction)
{
    return true;
}
