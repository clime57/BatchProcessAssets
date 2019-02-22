// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FindUnreferencedAssets.generated.h"

/**
 * 
 */
UCLASS()
class BATCHPROCESSASSETS_API UFindUnreferencedAssets : public UObject
{
	GENERATED_BODY()
	
public:
	void Find() const;
private:
	FORCEINLINE void AddtoArray(TArray<FAssetIdentifier>& UseLessAssets,FAssetIdentifier& AssetId) const;
	bool IsRefrenceUseless(TArray<FAssetIdentifier>& UseLessAssets, FAssetIdentifier& AssetId, TArray<FAssetIdentifier> ReferenceNames, TMap<FName, FAssetData>& AssetDataMap, TArray<FAssetIdentifier>& SearchStack,class FAssetRegistryModule& AssetRegistryModule) const;
};
