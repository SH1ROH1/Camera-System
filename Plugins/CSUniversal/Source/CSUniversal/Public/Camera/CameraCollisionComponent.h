#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraCollisionComponent.generated.h"

class AActor;

UCLASS(ClassGroup = (Camera), meta = (BlueprintSpawnableComponent))
class CSUNIVERSAL_API UCameraCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCameraCollisionComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Collision", meta = (ClampMin = "0.0"))
	float CollisionRadius = 15.f;

	FVector ResolveCameraPosition(
		const FVector& Origin,
		const FVector& DesiredLocation,
		AActor* FollowTarget
	) const;
};