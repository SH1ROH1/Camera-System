
#pragma once

#include "Camera/Eventer/CameraEventerActionSettings.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "CameraPathComponent.generated.h"

class ASystemCamera;
class USplineComponent;

UCLASS(ClassGroup = (Camera), meta = (BlueprintSpawnableComponent))
class CSFIRSTPERSON_API UCameraPathComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UCameraPathComponent();

  void StartPath(ASystemCamera *InCamera,
                 const FCameraEventerActionSettings &Action);
  void StopPath();
  bool IsPathActive() const;

protected:
  virtual void
  TickComponent(float DeltaTime, ELevelTick TickType,
                FActorComponentTickFunction *ThisTickFunction) override;

private:
  UPROPERTY(Transient)
  TObjectPtr<ASystemCamera> Camera = nullptr;

  UPROPERTY(Transient)
  TObjectPtr<USplineComponent> ActiveSpline = nullptr;

  FGlobalCameraState CameraState;
  EGlobalCameraTransitionMode TransitionMode;
  float CurrentPathDistance = 0.f;
  float StartDistanceThreshold = 0.f;
  float PathOffset = 0.f;
  float FollowSmoothing = 5.f;
  bool bLoop = false;
  bool bPathActive = false;

  bool FindClosestPathDistance(float &OutDistance) const;
  void UpdatePath(float DeltaTime);
};
