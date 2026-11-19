#include "Camera/CameraTargetComponent.h"
#include "GameFramework/Actor.h"

UCameraTargetComponent::UCameraTargetComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCameraTargetComponent::BeginPlay()
{
    Super::BeginPlay();

    // Сохраняем прежнее поведение SystemCamera.
    if (!PlayerTarget)
    {
        const AActor* Owner = GetOwner();
        PlayerTarget = IsValid(Owner) ? Owner->GetOwner() : nullptr;
    }
}

void UCameraTargetComponent::SetPlayerTarget(AActor* NewTarget)
{
    PlayerTarget = NewTarget;
}

AActor* UCameraTargetComponent::GetPlayerTarget() const
{
    return PlayerTarget;
}

void UCameraTargetComponent::SetConnectTarget(AActor* NewTarget)
{
    ConnectTarget = NewTarget;
}

AActor* UCameraTargetComponent::GetConnectTarget() const
{
    return ConnectTarget;
}

AActor* UCameraTargetComponent::GetTarget() const
{
    return IsValid(PlayerTarget) ? PlayerTarget.Get() : ConnectTarget.Get();
}

AActor* UCameraTargetComponent::GetPositionTarget(
    EGlobalCameraPositionMode PositionMode
) const
{
    switch (PositionMode)
    {
        case EGlobalCameraPositionMode::PlayerTarget:
            return PlayerTarget;

        case EGlobalCameraPositionMode::ConnectTarget:
            return ConnectTarget;

        case EGlobalCameraPositionMode::World:
        default:
            return nullptr;
    }
}

AActor* UCameraTargetComponent::GetFollowTarget(
    EGlobalCameraFollowTarget FollowTargetMode
) const
{
    switch (FollowTargetMode)
    {
        case EGlobalCameraFollowTarget::PlayerTarget:
            return PlayerTarget;

        case EGlobalCameraFollowTarget::ConnectTarget:
            return ConnectTarget;

        case EGlobalCameraFollowTarget::None:
        default:
            return nullptr;
    }
}

AActor* UCameraTargetComponent::GetRotationTarget(
    EGlobalCameraFollowTarget FollowTargetMode,
    bool bUseTargetRotation
) const
{
    return bUseTargetRotation
        ? GetFollowTarget(FollowTargetMode)
        : nullptr;
}