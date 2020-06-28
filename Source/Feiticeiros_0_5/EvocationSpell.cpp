// Fill out your copyright notice in the Description page of Project Settings.

#include "EvocationSpell.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AEvocationSpell::AEvocationSpell()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;

    DamageType = UDamageType::StaticClass();
    Damage = 50.0f;

    static ConstructorHelpers::FObjectFinder<UParticleSystem> DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
    if (DefaultExplosionEffect.Succeeded())
    {
        ExplosionEffect = DefaultExplosionEffect.Object;
    }

    //Definition for the SphereComponent that will serve as the Root component for the projectile and its collision.
    SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("RootComponent"));
    SphereComponent->InitSphereRadius(12.5f);
    SphereComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));

    //SphereComponent->IgnoreActorWhenMoving(GetOwner(), true);    
    RootComponent = SphereComponent;

    //Registering the Projectile Impact function on a Hit event.
    if (Role == ROLE_Authority)
    {
        SphereComponent->OnComponentHit.AddDynamic(this, &AEvocationSpell::OnProjectileImpact);
    }

    //Definition for the Mesh that will serve as our visual representation.
    static ConstructorHelpers::FObjectFinder<UStaticMesh> DefaultMesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
    StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    StaticMesh->SetupAttachment(RootComponent);

    if (DefaultMesh.Succeeded())
    {
        StaticMesh->SetStaticMesh(DefaultMesh.Object);
        StaticMesh->RelativeLocation = FVector(0.0f, 0.0f, -12.5f);
        StaticMesh->RelativeScale3D = FVector(0.25f, 0.25f, 0.25f);
    }

    //Definition for the Projectile Movement Component.
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovementComponent->SetUpdatedComponent(SphereComponent);
    ProjectileMovementComponent->InitialSpeed = 1000.0f;
    ProjectileMovementComponent->MaxSpeed = 1000.0f;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
}

// Called when the game starts or when spawned
void AEvocationSpell::BeginPlay()
{
	Super::BeginPlay();

    SphereComponent->MoveIgnoreActors.AddUnique(Instigator);

    SetLifeSpan(4.0f);	
}

// Called every frame
void AEvocationSpell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEvocationSpell::OnProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor)
    {
        UGameplayStatics::ApplyPointDamage(OtherActor, Damage, NormalImpulse, Hit, Instigator->Controller, this, DamageType);
    }

    Destroy();
}

void AEvocationSpell::Destroyed()
{
    FVector spawnLocation = GetActorLocation();
    UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, spawnLocation, FRotator::ZeroRotator, true, EPSCPoolMethod::AutoRelease);
}