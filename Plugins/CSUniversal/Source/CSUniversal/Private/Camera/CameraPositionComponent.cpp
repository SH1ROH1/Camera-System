#include "Camera/CameraPositionComponent.h"
#include "GameFramework/Actor.h"

UCameraPositionComponent::UCameraPositionComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCameraPositionComponent::InitializePosition(
    const FGlobalCameraState& State,
    AActor* PositionTarget
)
{
    FixedWorldPosition = GetWorldPositionFromState(State, PositionTarget);
}

void UCameraPositionComponent::SetFixedWorldPosition(const FVector& NewPosition)
{
    FixedWorldPosition = NewPosition;
}

FVector UCameraPositionComponent::GetFixedWorldPosition() const
{
    return FixedWorldPosition;
}

FVector UCameraPositionComponent::GetWorldPositionFromState(
    const FGlobalCameraState& State,
    AActor* PositionTarget
) const
{
    if (State.Position.PositionMode == EGlobalCameraPositionMode::World)
    {
        return State.Position.Position;
    }

    if (IsValid(PositionTarget))
    {
        return PositionTarget->GetActorTransform().TransformPosition(
            State.Position.Position
        );
    }

    return State.Position.Position;
}

FVector UCameraPositionComponent::GetRelativePositionFromWorld(
    const FVector& WorldPosition,
    AActor* PositionTarget
) const
{
    if (IsValid(PositionTarget))
    {
        return PositionTarget->GetActorTransform().InverseTransformPosition(
            WorldPosition
        );
    }

    return WorldPosition;
}

void UCameraPositionComponent::ConvertPositionSpace(FGlobalCameraState& State, EGlobalCameraPositionMode NewPositionMode, AActor* PositionTarget)
{
    AActor* Owner = GetOwner();
    if (!IsValid(Owner))
    {
        return;
    }

    const FVector WorldPosition = Owner->GetActorLocation();

    State.Position.PositionMode = NewPositionMode;

    if (NewPositionMode == EGlobalCameraPositionMode::World)
    {
        State.Position.Position = WorldPosition;
    }
    else
    {
        State.Position.Position = GetRelativePositionFromWorld(WorldPosition, PositionTarget);
    }

    State.Rotation.Rotation = Owner->GetActorRotation();

    if (State.Target.FollowTarget == EGlobalCameraFollowTarget::None)
    {
        FixedWorldPosition = WorldPosition;
    }
}