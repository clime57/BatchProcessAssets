// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "Factories/TextureFactory.h"

#include "UnrealEd.h" 
//#include "Editor.h"

#include "ScaleTextureFactory.generated.h"
/**
 * 
 */
UCLASS()
class UScaleTextureFactory : public UTextureFactory
{
	GENERATED_BODY()

	UPROPERTY()
	class UTexture* pOriginalTex;
private:
	float TexScale;
public:	
	void Import(float scale);
	void ReImportSelected(float scale, TArray<FAssetData>& SelectedAssets);
	UObject* ImportObject1(UClass* InClass, UObject* InOuter, FName InName, EObjectFlags InFlags, const TCHAR* Parms, bool& OutCanceled);
	UObject* FactoryCreateFile1(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const TCHAR* Parms, bool& bOutOperationCanceled);
	UObject* FactoryCreateBinary1
		(
			UClass*				Class,
			UObject*			InParent,
			FName				Name,
			EObjectFlags		Flags,
			UObject*			Context
			);
	UTexture* ImportTexture1(UTexture* OldTexture,
		const TArray<uint8>* RawPNG,
		ETextureSourceFormat TextureFormat,
		UClass* Class,
		UObject* InParent,
		FName Name);
	void OnNewImportRecord(UClass* AssetType, const FString& FileExtension, bool bSucceeded, bool bWasCancelled, const FDateTime& StartTime);
	TArray<uint8>*  GetReImportData(UTexture2D* Texture, TArray<uint8>* RawDataOut);
	//~ Begin UTextureFactory Interface
	virtual UTexture2D* CreateTexture2D(UObject* InParent, FName Name, EObjectFlags Flags) override;
	virtual UTextureCube* CreateTextureCube(UObject* InParent, FName Name, EObjectFlags Flags) override;
	//~ End UTextureFactory Interface
	UObject* ReImport(FAssetData& AssetData);
};
