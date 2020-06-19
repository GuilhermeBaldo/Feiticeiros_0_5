// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "GameFramework/Character.h"
#include "MyCharacter.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "EvocationSpell.h"

AMyPlayerController::AMyPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;

    //Initialize projectile class
    EvocationSpellClass = AEvocationSpell::StaticClass();

    //Initialize fire rate
    CastRate = 0.25f;
    bIsCasting = false;
}

void AMyPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
}

void AMyPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

    InputComponent->BindAxis("MoveForward", this, &AMyPlayerController::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &AMyPlayerController::MoveRight);

    // Handle casting
    InputComponent->BindAction("CastEvocationSpell", IE_Pressed, this, &AMyPlayerController::StartCast);
}

void AMyPlayerController::MoveForward(float Value)
{
    APawn* const MyPawn = GetPawn();
    if (MyPawn)
    {
        AController* const MyController = MyPawn->GetController();
        if ((MyController != NULL) && (Value != 0.0f))
        {
            // find out which way is forward
            const FRotator Rotation = MyController->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);

            // get forward vector
            const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
            MyPawn->AddMovementInput(Direction, Value);
        }
    }
}

void AMyPlayerController::MoveRight(float Value)
{
    APawn* const MyPawn = GetPawn();
    if (MyPawn)
    {
        AController* const MyController = MyPawn->GetController();
        if ((MyController != NULL) && (Value != 0.0f))
        {
            // find out which way is right
            const FRotator Rotation = MyController->GetControlRotation();
            const FRotator YawRotation(0, Rotation.Yaw, 0);

            // get right vector 
            const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
            // add movement in that direction
            MyPawn->AddMovementInput(Direction, Value);
        }
    }
}

void AMyPlayerController::StartCast()
{
    if (!bIsCasting)
    {
        bIsCasting = true;
        UWorld* World = GetWorld();
        World->GetTimerManager().SetTimer(CastingTimer, this, &AMyPlayerController::StopCast, CastRate, false);
        HandleCast();
    }
}

void AMyPlayerController::StopCast()
{
    bIsCasting = false;
}

void AMyPlayerController::HandleCast_Implementation()
{
    APawn* const MyPawn = GetPawn();
    if (MyPawn)
    { 
        FHitResult Hit;
        GetHitResultUnderCursor(ECC_Visibility, false, Hit);

        if (Hit.bBlockingHit)
        {
            FVector MouseDirection = Hit.ImpactPoint - MyPawn->GetActorLocation();
            MouseDirection.Z = 0;
            FRotator CastRotation = MouseDirection.ToOrientationRotator();
            FVector CastLocation = MyPawn->GetActorLocation() + CastRotation.RotateVector(FVector(100.0f, 0.0f, 0.0f));

            FActorSpawnParameters CastParameters;
            CastParameters.Instigator = MyPawn->Instigator;
            CastParameters.Owner = MyPawn;

            AEvocationSpell* spawnedProjectile = GetWorld()->SpawnActor<AEvocationSpell>(CastLocation, CastRotation, CastParameters);

        }
    }
}

bool AMyPlayerController::HandleCast_Validate()
{
    return true;
}