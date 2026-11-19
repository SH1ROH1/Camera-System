
#pragma once

#include "CoreMinimal.h"
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
    LookAtAim UMETA(DisplayName = "Look At Aim"),
    LookAtPlayer UMETA(DisplayName = "Look At Player")
};

USTRUCT(BlueprintType)
struct FCameraPositionSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
    EGlobalCameraPositionMode PositionMode = EGlobalCameraPositionMode::PlayerTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
    FVector Position = FVector(0.f, -600.f, 600.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
    bool bUseCollision = false;
};

USTRUCT(BlueprintType)
struct FCameraTargetSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
    EGlobalCameraFollowTarget FollowTarget = EGlobalCameraFollowTarget::PlayerTarget;
};

USTRUCT(BlueprintType)
struct FCameraRotationSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    FRotator Rotation = FRotator(-60.f, 0.f, 0.f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    float LockOnRotation = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    bool bUseTargetRotation = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation")
    bool bUsePathRotation = false;
};

USTRUCT(BlueprintType)
struct FCameraMovementSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float MovementSpeed = 8.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float RotationSpeed = 8.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
    float BlendSpeed = 5.f;
};

USTRUCT(BlueprintType)
struct FGlobalCameraLensSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lens")
    float FieldOfView = 90.f;
};

USTRUCT(BlueprintType)
struct FCameraAimSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
    bool bLookAtTarget = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
    EGlobalCameraLookMode LookMode = EGlobalCameraLookMode::LookAtTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
    float AimDistance = 1000.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Aim")
    float AimHeight = 0.f;
};

USTRUCT(BlueprintType)
struct FCameraOrbitSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
    float OrbitYaw = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit", meta = (ClampMin = "-89.0", ClampMax = "89.0"))
    float OrbitPitch = -20.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit", meta = (ClampMin = "-89.0", ClampMax = "89.0"))
    float MinPitch = -80.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit", meta = (ClampMin = "-89.0", ClampMax = "89.0"))
    float MaxPitch = 80.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit", meta = (ClampMin = "1.0"))
    float OrbitDistance = 600.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Orbit")
    bool bInvertThirdPersonPitch = false;
};

USTRUCT(BlueprintType)
struct FCameraControlSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control")
    EGlobalCameraControlMode ControlMode = EGlobalCameraControlMode::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Control", meta = (ClampMin = "0.0"))
    float MouseSensitivity = 2.f;
};

USTRUCT(BlueprintType)
struct FGlobalCameraState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera State")
    FCameraPositionSettings Position;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera State")
    FCameraTargetSettings Target;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera State")
    FCameraRotationSettings Rotation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera State")
    FCameraMovementSettings Movement;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera State")
    FGlobalCameraLensSettings Lens;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera State")
    FCameraAimSettings Aim;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera State")
    FCameraOrbitSettings Orbit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera State")
    FCameraControlSettings Control;
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
