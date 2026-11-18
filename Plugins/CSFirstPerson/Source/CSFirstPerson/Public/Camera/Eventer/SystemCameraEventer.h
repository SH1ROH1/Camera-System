
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/TriggerVolume.h"
#include "GlobalCameraEventerSettings.h"
#include "GlobalCameraPresetData.h"
#include "SystemCamera.h"
#include "SystemCameraEventer.generated.h"

class USplineComponent;

USTRUCT(BlueprintType)
struct FGlobalCameraEventerPreset {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preset")
  FName PresetName;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preset")
  FGlobalCameraEventerSettings Settings;
};

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
  //==========================================================
  // State | Presets
  //==========================================================

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
  TObjectPtr<UGlobalCameraPresetData> PresetData = nullptr;

#if WITH_EDITOR

  UFUNCTION(CallInEditor, Category = "Presets")
  void SavePreset();

  UFUNCTION(CallInEditor, Category = "Presets")
  void LoadPreset();

  UFUNCTION(CallInEditor, Category = "Presets")
  void DeletePreset();

#endif

  //==========================================================
  // Settings
  //==========================================================

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Eventer",
            meta = (ShowOnlyInnerProperties))
  FGlobalCameraEventerSettings Settings;

  //===========================================================
  // AActors
  //==========================================================

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
  AActor *RestoreConnectTarget = nullptr;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
  AActor *ConnectTarget = nullptr;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
  AActor *CameraPathActor = nullptr;
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
  AActor *RestoreCameraPathActor = nullptr;

  //==========================================================
  // Runtime | Trigger
  //==========================================================

  UPROPERTY(Transient)
  float ActivationTimer = 0.f;

  UPROPERTY(Transient)
  TWeakObjectPtr<AActor> PendingActor;

private:
  //==========================================================
  // Camera
  //==========================================================

  UPROPERTY()
  TObjectPtr<ASystemCamera> GameCamera = nullptr;

  //==========================================================
  // Path
  //==========================================================



  UPROPERTY()
  TObjectPtr<USplineComponent> ActiveSpline = nullptr;

  bool bTriggered = false;
  bool bPathActive = false;
  bool bPathFinished = false;

  float CurrentPathDistance = 0.f;

  //==========================================================
  // Path Settings | Runtime
  //==========================================================

  float ActiveStartDistanceThreshold = 0.f;
  float ActivePathOffset = 0.f;
  float ActiveFollowSmoothing = 5.f;
  bool bActivePathLoop = false;

  //==========================================================
  // Functions
  //==========================================================

  void CheckExistingOverlap();

  void ApplyCameraSettings();

  void ActivateEventer(AActor *OtherActor);

  void StartPath();

  void UpdatePath(float DeltaTime);

  void StopPath();

  bool FindClosestPathDistance(float &OutDistance) const;

  void ApplyTargets(AActor *NewConnectTarget);

  void ApplyPathRotation();

#if WITH_EDITOR

  bool ConfirmOverwrite() const;

  AActor *FindActorByName(const FName &ActorName) const;

#endif
};
