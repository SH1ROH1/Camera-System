#include "FPCharacter.h"
// Engine
#include "Engine/World.h"
// Game
#include "Engine/EngineTypes.h"
// Movement
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
// Camera
#include "SystemCamera.h"


AFPCharacter::AFPCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFPCharacter::BeginPlay()
{
	Super::BeginPlay();
	bUseControllerRotationYaw = false;
	if (!CameraClass)
	{
		UE_LOG(LogTemp, Error, TEXT("CameraClass == nullptr"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("CameraClass = %s"), *GetNameSafe(CameraClass));

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CameraActor = GetWorld()->SpawnActor<ASystemCamera>(CameraClass, GetActorLocation(), FRotator::ZeroRotator, Params);

	if (!CameraActor)
	{
		UE_LOG(LogTemp, Error, TEXT("FAILED TO SPAWN CAMERA"));
		return;
	}

	CameraActor->SetPlayerTarget(this);

	UE_LOG(LogTemp, Warning, TEXT("Camera spawned: %s"), *GetNameSafe(CameraActor));

	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController == nullptr"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("PlayerController: %s"), *GetNameSafe(PC));

	PC->SetViewTargetWithBlend(CameraActor, 0.2f);

	UE_LOG(LogTemp, Warning, TEXT("ViewTarget = %s"), *GetNameSafe(PC->GetViewTarget()));
}

void AFPCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ApplyMovement(DeltaTime);
	UpdateCameraRotation(DeltaTime);
}

void AFPCharacter::Move_X(float Value)
{
	MoveX = Value;
}

void AFPCharacter::Move_Z(float Value)
{
	MoveZ = Value;
}

void AFPCharacter::ApplyMovement(float DeltaTime)
{
	FVector Movement;

	if (CameraActor) {
		FVector Forward = CameraActor->GetActorForwardVector();
		FVector Right = CameraActor->GetActorRightVector();
		Forward.Z = 0.f;
		Right.Z = 0.f;
		Forward.Normalize();
		Right.Normalize();
		Movement = Forward * MoveX + Right * MoveZ;
	}
	else { Movement = FVector(MoveX, MoveZ, 0.f); }
	Movement.Z = 0.f;

	if (!Movement.IsNearlyZero())
	{
		Movement.Normalize();
		AddMovementInput(Movement, 1.f);
	}
}

void AFPCharacter::Look_X(float Value) { LookX = Value; }

void AFPCharacter::Look_Y(float Value) { LookY = Value; }

void AFPCharacter::UpdateCameraRotation(float deltaTime) const
{
	if (!bUseMouseLook || !CameraActor)
		return;
	CameraActor->UpdateCameraRotation(LookX, LookY);
}

void AFPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Movement bindings
	PlayerInputComponent->BindAxis("MoveZ", this, &AFPCharacter::Move_Z);
	PlayerInputComponent->BindAxis("MoveX", this, &AFPCharacter::Move_X);
	// Mouse bindings
	PlayerInputComponent->BindAxis("LookX", this, &AFPCharacter::Look_X);
	PlayerInputComponent->BindAxis("LookY", this, &AFPCharacter::Look_Y);
}

ASystemCamera* AFPCharacter::GetCameraActor() const { return CameraActor; }

