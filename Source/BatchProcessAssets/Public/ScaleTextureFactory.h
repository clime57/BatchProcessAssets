// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "UnrealEd.h" 
#include "Factories/TextureFactory.h"
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
	//float TexScale;
	int32 iMaxTexSize;
	bool bIsNotReallyModifyOriginalTex;
public:	
	void Import(float scale, int32 MaxTexSize, bool IsNotReallyModifyOriginalTex);
	void ReImportSelected(float scale, TArray<FAssetData>& SelectedAssets, int32 MaxTexSize, bool IsNotReallyModifyOriginalTex);
	UObject* ReImportObject(UClass* InClass, UObject* InOuter, FName InName, EObjectFlags InFlags, const TCHAR* Parms, bool& OutCanceled);
	UObject* FactoryReCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const TCHAR* Parms, bool& bOutOperationCanceled);
	UObject* FactoryReCreateBinary
		(
			UClass*				Class,
			UObject*			InParent,
			FName				Name,
			EObjectFlags		Flags,
			UObject*			Context
			);
	UTexture* ReImportTexture(UTexture* OldTexture,
		const TArray<uint8>* RawPNG,
		ETextureSourceFormat TextureFormat,
		UClass* Class,
		UObject* InParent,
		FName Name, 
		const FIntPoint& FinalSize);
	void OnNewImportRecord(UClass* AssetType, const FString& FileExtension, bool bSucceeded, bool bWasCancelled, const FDateTime& StartTime);
	void GetFinalSize(const FIntPoint& oldSize, FIntPoint& newSize, int32 MaxTextureSize);
	TArray<uint8>*  GetReImportData(UTexture2D* Texture, TArray<uint8>* RawDataOut, const FIntPoint& FinalSize);
	//~ Begin UTextureFactory Interface
	virtual UTexture2D* CreateTexture2D(UObject* InParent, FName Name, EObjectFlags Flags) override;
	virtual UTextureCube* CreateTextureCube(UObject* InParent, FName Name, EObjectFlags Flags) override;
	//~ End UTextureFactory Interface
	UObject* ReImport(FAssetData& AssetData);
	void ImportADL(TArray<FAssetData>& SelectedAssets);
};
