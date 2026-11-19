
#include "Camera/CameraRotationComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"

UCameraRotationComponent::UCameraRotationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

FRotator UCameraRotationComponent::GetTargetRotation(
    const FGlobalCameraState& State,
    AActor* FollowTarget
) const
{
    if (
        !State.Rotation.bUseTargetRotation ||
        !IsValid(FollowTarget)
    )
    {
        return FRotator::ZeroRotator;
    }

    FRotator TargetRotation = FollowTarget->GetActorRotation();
    TargetRotation.Yaw += State.Rotation.LockOnRotation;

    return TargetRotation;
}

FVector UCameraRotationComponent::GetLookAtLocation(
    const FGlobalCameraState& State,
    AActor* FollowTarget,
    AActor* PlayerTarget,
    AActor* ConnectTarget,
    bool& bFoundLocation
) const
{
    bFoundLocation = false;

    if (State.Aim.LookMode == EGlobalCameraLookMode::LookAtPlayer)
    {
        if (!IsValid(PlayerTarget))
        {
            return FVector::ZeroVector;
        }

        bFoundLocation = true;

        return PlayerTarget->GetActorLocation() +
            FVector::UpVector * State.Aim.AimHeight;
    }

    if (
        State.Control.ControlMode == EGlobalCameraControlMode::FirstPerson &&
        IsValid(ConnectTarget)
    )
    {
        bFoundLocation = true;

        return ConnectTarget->GetActorLocation() +
            FVector::UpVector * State.Aim.AimHeight;
    }

    if (
        State.Control.ControlMode == EGlobalCameraControlMode::ThirdPerson &&
        IsValid(PlayerTarget) &&
        IsValid(ConnectTarget)
    )
    {
        bFoundLocation = true;

        return (
            PlayerTarget->GetActorLocation() +
            ConnectTarget->GetActorLocation()
        ) * 0.5f + FVector::UpVector * State.Aim.AimHeight;
    }

    AActor* LookAtTarget = IsValid(FollowTarget)
        ? FollowTarget
        : PlayerTarget;

    if (!IsValid(LookAtTarget))
    {
        return FVector::ZeroVector;
    }

    const FVector TargetLocation = LookAtTarget->GetActorLocation();

    if (State.Aim.LookMode == EGlobalCameraLookMode::LookAtTarget)
    {
        bFoundLocation = true;

        return TargetLocation +
            FVector::UpVector * State.Aim.AimHeight;
    }

    const FVector TargetRight = LookAtTarget->GetActorRightVector();

    const float ShoulderOffset = FMath::Clamp(
        State.Aim.AimDistance * 0.05f,
        10.f,
        100.f
    );

    bFoundLocation = true;

    return TargetLocation +
        TargetRight * ShoulderOffset +
        FVector::UpVector * State.Aim.AimHeight;
}

FRotator UCameraRotationComponent::CalculateDesiredRotation(
    const FGlobalCameraState& State,
    const FVector& CameraLocation,
    AActor* FollowTarget,
    AActor* PlayerTarget,
    AActor* ConnectTarget,
    bool& bShouldUpdateStateRotation
) const
{
    bShouldUpdateStateRotation = false;

    if (State.Rotation.bUseTargetRotation)
    {
        if (IsValid(FollowTarget))
        {
            return GetTargetRotation(State, FollowTarget);
        }

        return State.Rotation.Rotation;
    }

    if (!State.Aim.bLookAtTarget)
    {
        return State.Rotation.Rotation;
    }

    bool bFoundLocation = false;

    const FVector LookAtLocation = GetLookAtLocation(
        State,
        FollowTarget,
        PlayerTarget,
        ConnectTarget,
        bFoundLocation
    );

    if (!bFoundLocation)
    {
        return State.Rotation.Rotation;
    }

    bShouldUpdateStateRotation = true;

    return UKismetMathLibrary::FindLookAtRotation(
        CameraLocation,
        LookAtLocation
    );
}
