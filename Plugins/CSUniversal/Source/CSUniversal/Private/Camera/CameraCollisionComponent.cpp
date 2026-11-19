#include "Camera/CameraCollisionComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UCameraCollisionComponent::UCameraCollisionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

FVector UCameraCollisionComponent::ResolveCameraPosition(
	const FVector& Origin,
	const FVector& DesiredLocation,
	AActor* FollowTarget
) const
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return DesiredLocation;
	}

	FHitResult Hit;

	FCollisionQueryParams Params(
		SCENE_QUERY_STAT(SystemCameraCollision),
		false
	);

	if (IsValid(FollowTarget))
	{
		Params.AddIgnoredActor(FollowTarget);
	}

	if (AActor* Owner = GetOwner())
	{
		Params.AddIgnoredActor(Owner);
	}

	const bool bHit = World->SweepSingleByChannel(
		Hit,
		Origin,
		DesiredLocation,
		FQuat::Identity,
		ECC_Camera,
		FCollisionShape::MakeSphere(FMath::Max(CollisionRadius, 0.f)),
		Params
	);

	return bHit
		? Hit.Location + Hit.Normal * CollisionRadius
		: DesiredLocation;
}