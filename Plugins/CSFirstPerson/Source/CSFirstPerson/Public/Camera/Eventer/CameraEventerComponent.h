#pragma once

#include "Camera/Eventer/CameraEventerSettings.h"
#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "CameraEventerComponent.generated.h"

class ASystemCamera;
class UCameraPathComponent;

UCLASS(ClassGroup = (Camera), meta = (BlueprintSpawnableComponent))
class CSFIRSTPERSON_API UCameraEventerComponent : public UActorComponent {
  GENERATED_BODY()

public:
  UCameraEventerComponent();

protected:
  virtual void BeginPlay() override;

  UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Camera")
  TObjectPtr<ASystemCamera> Camera = nullptr;

public:
  UFUNCTION(BlueprintCallable, Category = "Camera")
  void InitializeCamera();

  void SetCamera(ASystemCamera *NewCamera);

  UFUNCTION(BlueprintCallable, Category = "Camera")
  void ApplyAction(const FCameraEventerActionSettings &Action);

  void ActivateEventer(const FCameraEventerSettings &EventerSettings);
  void DeactivateEventer(const FCameraEventerSettings &EventerSettings);

  UFUNCTION(BlueprintPure, Category = "Camera")
  ASystemCamera *GetCamera() const;
};
