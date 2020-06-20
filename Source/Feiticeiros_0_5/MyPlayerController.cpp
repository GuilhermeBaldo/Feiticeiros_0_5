// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "GameFramework/Character.h"
#include "MyCharacter.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "EvocationSpell.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"

AMyPlayerController::AMyPlayerController()
{
    bShowMouseCursor = true;
    DefaultMouseCursor = EMouseCursor::Crosshairs;
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
    InputComponent->BindAction("CastEvocationSpell", IE_Pressed, this, &AMyPlayerController::Cast);
}

void AMyPlayerController::MoveForward(float Value)
{
    AMyCharacter* const MyCharacter = static_cast<AMyCharacter*>(this->GetCharacter());
    if (MyCharacter)
    {
        if (!MyCharacter->GetIsCasting())
        {
            AController* const MyController = MyCharacter->GetController();
            if ((MyController != NULL) && (Value != 0.0f))
            {
                // find out which way is forward
                const FRotator Rotation = MyController->GetControlRotation();
                const FRotator YawRotation(0, Rotation.Yaw, 0);

                // get forward vector
                const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
                MyCharacter->AddMovementInput(Direction, Value);
            }
        }
    }
}

void AMyPlayerController::MoveRight(float Value)
{ 
    AMyCharacter* const MyCharacter = static_cast<AMyCharacter*>(this->GetCharacter());
    if (MyCharacter)
    {
        if (!MyCharacter->GetIsCasting())
        {
            AController* const MyController = MyCharacter->GetController();
            if ((MyController != NULL) && (Value != 0.0f))
            {
                // find out which way is right
                const FRotator Rotation = MyController->GetControlRotation();
                const FRotator YawRotation(0, Rotation.Yaw, 0);

                // get right vector 
                const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
                // add movement in that direction
                MyCharacter->AddMovementInput(Direction, Value);
            }
        }
    }
}

void AMyPlayerController::Cast()
{
    AMyCharacter* const MyCharacter = static_cast<AMyCharacter*>(this->GetCharacter());
    if (MyCharacter) 
    {
        FHitResult Hit;
        GetHitResultUnderCursor(ECC_Visibility, false, Hit);

        if (GetWorld()) 
        {
            FVector2D MousePos;
            if (GetWorld()->GetGameViewport()->GetMousePosition(MousePos))
            {
                FVector2D ViewportSize;
                GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

                FVector2D ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);
    
                FVector2D Direction = MousePos - ViewportCenter;
                
                FVector Direction3D = FVector(-Direction.Y, Direction.X, 0);
                FRotator CastRotation = Direction3D.ToOrientationRotator();

                MyCharacter->SetActorRotation(CastRotation);

                MyCharacter->StartCast(FVector2D(-Direction.Y, Direction.X));
            }
            /*if (Hit.bBlockingHit)
            {
                // Get direction of the hit
                FVector HitDirection = Hit.ImpactPoint - MyCharacter->GetActorLocation();
                HitDirection.Z = 0;
                MyCharacter->StartCast(HitDirection);
            }*/
        }
    }
}
