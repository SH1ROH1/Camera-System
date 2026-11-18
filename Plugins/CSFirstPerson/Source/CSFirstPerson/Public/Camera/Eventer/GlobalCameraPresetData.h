#pragma once

#include "Camera/Eventer/CameraEventerSettings.h"
#include "Camera/GlobalCameraState.h"
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GlobalCameraPresetData.generated.h"

USTRUCT(BlueprintType)
struct FCameraEventerPresetAction {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
  FGlobalCameraState CameraState;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
  EGlobalCameraTransitionMode TransitionMode =
      EGlobalCameraTransitionMode::Smooth;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
  FName ConnectTargetName = NAME_None;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
  FName CameraPathActorName = NAME_None;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
  FCameraPathSettings PathSettings;
};

USTRUCT(BlueprintType)
struct FGlobalCameraEventerPreset {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preset")
  FName PresetName = NAME_None;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activation")
  FCameraEventerPresetAction Activation;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Restore")
  bool bRestoreOnExit = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Restore",
            meta = (EditCondition = "bRestoreOnExit", EditConditionHides))
  FCameraEventerPresetAction Restore;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
  bool bFireOnlyOnce = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger",
            meta = (ClampMin = "0.0"))
  float ActivationDelay = 0.f;
};

UCLASS()
class CSFIRSTPERSON_API UGlobalCameraPresetData : public UDataAsset {
  GENERATED_BODY()

public:
  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
  TArray<FGlobalCameraEventerPreset> Presets;

  bool FindPreset(FName PresetName,
                  FGlobalCameraEventerPreset &OutPreset) const;
  bool SavePreset(FName PresetName, const FCameraEventerSettings &Preset,
                  bool bOverwrite = false);
  bool DeletePreset(FName PresetName);
};