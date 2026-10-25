#pragma once


#include "GlobalCameraState.generated.h"

UENUM(BlueprintType)
enum class EGlobalCameraTransitionMode : uint8
{
	Smooth UMETA(DisplayName = "Smooth"),
	Instant UMETA(DisplayName = "Instant")
};

UENUM(BlueprintType)
enum class EGlobalCameraPositionMode : uint8
{
	World UMETA(DisplayName = "World"),
	ConnectTarget UMETA(DisplayName = "Connect Target"),
	PlayerTarget UMETA(DisplayName = "Player Target")
};

UENUM(BlueprintType)
enum class EGlobalCameraFollowTarget : uint8
{
	None UMETA(DisplayName = "None"),
	ConnectTarget UMETA(DisplayName = "Connect Target"),
	PlayerTarget UMETA(DisplayName = "Player Target")
};

UENUM(BlueprintType)
enum class EGlobalCameraControlMode : uint8
{
	None UMETA(DisplayName = "None"),
	FirstPerson UMETA(DisplayName = "First Person"),
	ThirdPerson UMETA(DisplayName = "Third Person")
};

UENUM(BlueprintType)
enum class EGlobalCameraLookMode : uint8
{
	LookAtTarget UMETA(DisplayName = "Look At Target"),
	LookAtAim UMETA(DisplayName = "Look At Aim")
};

USTRUCT(BlueprintType)
struct FGlobalCameraState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	EGlobalCameraPositionMode PositionMode = EGlobalCameraPositionMode::PlayerTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
	FVector Position = FVector(0.f, -600.f, 600.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	EGlobalCameraFollowTarget FollowTarget = EGlobalCameraFollowTarget::PlayerTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bUseCollision = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	FRotator Rotation = FRotator(-60.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	float LockOnRotation = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	bool bUseTargetRotation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
	bool bUsePathRotation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool bLookAtTarget = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MovementSpeed = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float RotationSpeed = 8.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float FieldOfView = 90.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float BlendSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control")
	EGlobalCameraControlMode ControlMode = EGlobalCameraControlMode::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control")
	EGlobalCameraLookMode LookMode = EGlobalCameraLookMode::LookAtTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control", meta = (ClampMin = "0.0"))
	float MouseSensitivity = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control")
	float OrbitYaw = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control", meta = (ClampMin = "-89.0", ClampMax = "89.0"))
	float OrbitPitch = -20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control", meta = (ClampMin = "-89.0", ClampMax = "89.0"))
	float MinPitch = -80.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control", meta = (ClampMin = "-89.0", ClampMax = "89.0"))
	float MaxPitch = 80.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control", meta = (ClampMin = "1.0"))
	float OrbitDistance = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control", meta = (ClampMin = "0.0"))
	float AimDistance = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control")
	float AimHeight = 0.f;
};

USTRUCT(BlueprintType)
struct FGlobalCameraPreset
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Preset")
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Preset")
	FGlobalCameraState State;
};