
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GlobalCameraState.h"
#include "GlobalCameraEventerSettings.h"

#include "GlobalCameraPresetData.generated.h"

UCLASS()
class CSFIRSTPERSON_API UGlobalCameraPresetData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
  TArray<FGlobalCameraEventerSettings> Presets;


	bool FindPreset(FName PresetName, FGlobalCameraEventerSettings& OutPreset) const;
	bool SavePreset(FName PresetName, const FGlobalCameraEventerSettings& Preset, bool bOverwrite = false);
	bool DeletePreset(FName PresetName);
};