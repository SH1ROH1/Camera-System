
#pragma once

#include "Camera/Eventer/CameraEventerComponent.h"
#include "Camera/Eventer/CameraEventerSettings.h"
#include "Camera/Eventer/CameraPathComponent.h"
#include "Camera/Eventer/GlobalCameraPresetData.h"
#include "Camera/SystemCamera.h"
#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "SystemCameraEventer.generated.h"

UCLASS(Blueprintable)
class CSFIRSTPERSON_API ASystemCameraEventer : public ATriggerVolume {
  GENERATED_BODY()

public:
  ASystemCameraEventer();

protected:
  virtual void BeginPlay() override;
  virtual void Tick(float DeltaTime) override;

  UFUNCTION()
  void OnActorEntered(AActor *OverlappedActor, AActor *OtherActor);

  UFUNCTION()
  void OnActorLeft(AActor *OverlappedActor, AActor *OtherActor);

public:
  // Presets
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
  TObjectPtr<UGlobalCameraPresetData> PresetData = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
  FName PresetName = NAME_None;

#if WITH_EDITOR
  UFUNCTION(CallInEditor, Category = "Presets")
  void SavePreset();

  UFUNCTION(CallInEditor, Category = "Presets")
  void LoadPreset();

  UFUNCTION(CallInEditor, Category = "Presets")
  void DeletePreset();
#endif

  // Settings
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Eventer",
            meta = (ShowOnlyInnerProperties))
  FCameraEventerSettings Settings;

  // Components
  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  TObjectPtr<UCameraEventerComponent> CameraEventerComponent;

  UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
  TObjectPtr<UCameraPathComponent> CameraPathComponent;

  // Runtime | Trigger
  UPROPERTY(Transient)
  float ActivationTimer = 0.f;

  UPROPERTY(Transient)
  TWeakObjectPtr<AActor> PendingActor;

private:
  // Camera
  UPROPERTY()
  TObjectPtr<ASystemCamera> GameCamera = nullptr;

  // Runtime
  bool bTriggered = false;

  // Functions
  void CheckExistingOverlap();
  void ApplyCameraSettings();
  void ActivateEventer(AActor *OtherActor);

#if WITH_EDITOR
  bool ConfirmOverwrite() const;
  AActor *FindActorByName(const FName &ActorName) const;
#endif
};
