// Fill out your copyright notice in the Description page of Project Settings.
#include "BatchProcessAssetsPrivatePCH.h"
#include "ScaleTextureFactory.h"
#include "AssetRegistryModule.h"
#include "MaterialExpressionTextureSample.h"
#include "NormalMapIdentification.h"
#include "IContentBrowserSingleton.h"
#include "EngineAnalytics.h"
#include "AnalyticsEventAttribute.h"
#include "IAnalyticsProvider.h"
#if ENGINE_MINOR_VERSION >= 21
#include "Engine/VolumeTexture.h"
#endif


#define LOCTEXT_NAMESPACE "ScaleTextureFactory"

DECLARE_LOG_CATEGORY_EXTERN(LogScaleTextureFactory, Log, All);
DEFINE_LOG_CATEGORY(LogScaleTextureFactory);

#define MinTexSize 32
UTexture* UScaleTextureFactory::ReImportTexture(UTexture* OldTexture,
	const TArray<uint8>* RawPNG,
	ETextureSourceFormat TextureFormat,
	UClass* Class,
	UObject* InParent,
	FName Name,
	const FIntPoint& FinalSize)
{
	EObjectFlags Flags = RF_Dynamic;
	
	const int32 NewWidth = (int32)FinalSize.X;
	const int32 NewHeight = (int32)FinalSize.Y;
	UTexture2D* Texture = CreateTexture2D(InParent, Name, Flags);
	if (Texture)
	{
		Texture->Source.Init(
			NewWidth,
			NewHeight,
			/*NumSlices=*/ 1,
			/*NumMips=*/ 1,
			TextureFormat
			);
		{
			uint8* MipData = Texture->Source.LockMip(0);
			FMemory::Memcpy(MipData, RawPNG->GetData(), RawPNG->Num());
		}
		Texture->Source.UnlockMip(0);
	}

	return Texture;
}


UObject* UScaleTextureFactory::FactoryReCreateBinary
(
	UClass*				Class,
	UObject*			InParent,
	FName				Name,
	EObjectFlags		Flags,
	UObject*			Context
)
{
	if (!bIsNotReallyModifyOriginalTex)
	{
		TCHAR*		Type = TEXT("TGA");
		FEditorDelegates::OnAssetPreImport.Broadcast(this, Class, InParent, Name, Type);//Type的意义并不是很大
	}

	// if the texture already exists, remember the user settings
	UTexture* ExistingTexture = FindObject<UTexture>(InParent, *Name.ToString());
	UTexture2D* ExistingTexture2D = FindObject<UTexture2D>(InParent, *Name.ToString());
	if (!(ExistingTexture && ExistingTexture2D))
	{
		return nullptr;
	}

	const int32 MinMyTexSize = FMath::Min<int32>(ExistingTexture->Source.GetSizeX(), ExistingTexture->Source.GetSizeY());
	//int32 MaxTextureSize = (int32)(FMath::Max<int32>(ExistingTexture->Source.GetSizeX(), ExistingTexture->Source.GetSizeY()) * TexScale);
	int32 MaxTextureSize = iMaxTexSize;


	if (MaxTextureSize <= MinTexSize && MinMyTexSize > MaxTextureSize)
	{
		MaxTextureSize = MinTexSize;
	}

	if (bIsNotReallyModifyOriginalTex)
	{
		if (/*(ExistingTexture->MaxTextureSize && (ExistingTexture->MaxTextureSize <= MaxTextureSize || ExistingTexture->MaxTextureSize <= MinTexSize))
			|| */MinMyTexSize <= MinTexSize)
		{
			return nullptr;
		}

		if (MaxTextureSize <= 0)
		{
			MaxTextureSize = 0;
		}
		else
		{
			MaxTextureSize = FMath::Min<int32>(FMath::RoundUpToPowerOfTwo(MaxTextureSize), ExistingTexture->GetMaximumDimension());
		}
		ExistingTexture->MaxTextureSize = MaxTextureSize;
		ExistingTexture->UpdateResource();

		return ExistingTexture;
	}

	TGuardValue<UTexture*> OriginalTexGuardValue(pOriginalTex, ExistingTexture);
	#if WITH_EDITORONLY_DATA
	ETextureSourceFormat Format = ExistingTexture->Source.GetFormat();
	#endif
	TextureAddress						ExistingAddressX = TA_Wrap;
	TextureAddress						ExistingAddressY = TA_Wrap;
	TextureFilter						ExistingFilter = TF_Default;
	TextureGroup						ExistingLODGroup = TEXTUREGROUP_World;
	TextureCompressionSettings			ExistingCompressionSettings = TC_Default;
	int32									ExistingLODBias = 0;
	int32									ExistingNumCinematicMipLevels = 0;
	bool								ExistingNeverStream = false;
	bool								ExistingSRGB = false;
	bool								ExistingPreserveBorder = false;
	bool								ExistingNoCompression = false;
	bool								ExistingNoAlpha = false;
	bool								ExistingDeferCompression = false;
	bool 								ExistingDitherMipMapAlpha = false;
	bool 								ExistingFlipGreenChannel = false;
	float								ExistingAdjustBrightness = 1.0f;
	float								ExistingAdjustBrightnessCurve = 1.0f;
	float								ExistingAdjustVibrance = 0.0f;
	float								ExistingAdjustSaturation = 1.0f;
	float								ExistingAdjustRGBCurve = 1.0f;
	float								ExistingAdjustHue = 0.0f;
	float								ExistingAdjustMinAlpha = 0.0f;
	float								ExistingAdjustMaxAlpha = 1.0f;
	FVector4							ExistingAlphaCoverageThresholds = FVector4(0, 0, 0, 0);
	TextureMipGenSettings				ExistingMipGenSettings = TextureMipGenSettings(0);


	if (ExistingTexture)
	{
		// save settings
		if (ExistingTexture2D)
		{
			ExistingAddressX = ExistingTexture2D->AddressX;
			ExistingAddressY = ExistingTexture2D->AddressY;
		}
		ExistingFilter = ExistingTexture->Filter;
		ExistingLODGroup = ExistingTexture->LODGroup;
		ExistingCompressionSettings = ExistingTexture->CompressionSettings;
		ExistingLODBias = ExistingTexture->LODBias;
		ExistingNumCinematicMipLevels = ExistingTexture->NumCinematicMipLevels;
		ExistingNeverStream = ExistingTexture->NeverStream;
		ExistingSRGB = ExistingTexture->SRGB;
		ExistingPreserveBorder = ExistingTexture->bPreserveBorder;
		ExistingNoCompression = ExistingTexture->CompressionNone;
		ExistingNoAlpha = ExistingTexture->CompressionNoAlpha;
		ExistingDeferCompression = ExistingTexture->DeferCompression;
		ExistingFlipGreenChannel = ExistingTexture->bFlipGreenChannel;
		ExistingDitherMipMapAlpha = ExistingTexture->bDitherMipMapAlpha;
		ExistingAlphaCoverageThresholds = ExistingTexture->AlphaCoverageThresholds;
		ExistingAdjustBrightness = ExistingTexture->AdjustBrightness;
		ExistingAdjustBrightnessCurve = ExistingTexture->AdjustBrightnessCurve;
		ExistingAdjustVibrance = ExistingTexture->AdjustVibrance;
		ExistingAdjustSaturation = ExistingTexture->AdjustSaturation;
		ExistingAdjustRGBCurve = ExistingTexture->AdjustRGBCurve;
		ExistingAdjustHue = ExistingTexture->AdjustHue;
		ExistingAdjustMinAlpha = ExistingTexture->AdjustMinAlpha;
		ExistingAdjustMaxAlpha = ExistingTexture->AdjustMaxAlpha;
		ExistingMipGenSettings = ExistingTexture->MipGenSettings;
	}

	if (ExistingTexture2D)
	{
		// Update with new settings, which should disable streaming...
		ExistingTexture2D->UpdateResource();
	}
	FIntPoint FinalSize;
	GetFinalSize(FIntPoint(ExistingTexture->Source.GetSizeX(), ExistingTexture->Source.GetSizeY()), FinalSize, MaxTextureSize);

	TArray<uint8> RawPNG;

	if (GetReImportData(ExistingTexture2D, &RawPNG, FinalSize) == nullptr) {
		return nullptr;
	}

	FTextureReferenceReplacer RefReplacer(ExistingTexture);

	UTexture* Texture = ReImportTexture(ExistingTexture, &RawPNG,Format, Class, InParent,Name, FinalSize);

	//Replace the reference for the new texture with the existing one so that all current users still have valid references.
	RefReplacer.Replace(Texture);

	Texture->CompressionNone = NoCompression;
	Texture->CompressionNoAlpha = NoAlpha;
	Texture->DeferCompression = bDeferCompression;
	Texture->bDitherMipMapAlpha = bDitherMipMapAlpha;
	Texture->AlphaCoverageThresholds = AlphaCoverageThresholds;

	if (Texture->MipGenSettings == TMGS_FromTextureGroup)
	{
		// unless the loader suggest a different setting
		Texture->MipGenSettings = MipGenSettings;
	}

	Texture->bPreserveBorder = bPreserveBorder;
#if ENGINE_MINOR_VERSION <= 20
	Texture->AssetImportData->Update(CurrentFilename);
#else
	Texture->AssetImportData->Update(CurrentFilename, FileHash.IsValid() ? &FileHash : nullptr);
#endif
	UTexture2D* Texture2D = Cast<UTexture2D>(Texture);

	// Restore user set options
	if (ExistingTexture)
	{
		if (Texture2D)
		{
			Texture2D->AddressX = ExistingAddressX;
			Texture2D->AddressY = ExistingAddressY;
		}

		Texture->Filter = ExistingFilter;
		Texture->LODGroup = ExistingLODGroup;
		Texture->CompressionSettings = ExistingCompressionSettings;
		Texture->LODBias = ExistingLODBias;
		Texture->NumCinematicMipLevels = ExistingNumCinematicMipLevels;
		Texture->NeverStream = ExistingNeverStream;
		Texture->SRGB = ExistingSRGB;
		Texture->bPreserveBorder = ExistingPreserveBorder;
		Texture->CompressionNone = ExistingNoCompression;
		Texture->CompressionNoAlpha = ExistingNoAlpha;
		Texture->DeferCompression = ExistingDeferCompression;
		Texture->bDitherMipMapAlpha = ExistingDitherMipMapAlpha;
		Texture->AlphaCoverageThresholds = ExistingAlphaCoverageThresholds;
		Texture->bFlipGreenChannel = ExistingFlipGreenChannel;
		Texture->AdjustBrightness = ExistingAdjustBrightness;
		Texture->AdjustBrightnessCurve = ExistingAdjustBrightnessCurve;
		Texture->AdjustVibrance = ExistingAdjustVibrance;
		Texture->AdjustSaturation = ExistingAdjustSaturation;
		Texture->AdjustRGBCurve = ExistingAdjustRGBCurve;
		Texture->AdjustHue = ExistingAdjustHue;
		Texture->AdjustMinAlpha = ExistingAdjustMinAlpha;
		Texture->AdjustMaxAlpha = ExistingAdjustMaxAlpha;
		Texture->MipGenSettings = ExistingMipGenSettings;
	}

	if (Texture2D)
	{
		// The texture has been imported and has no editor specific changes applied so we clear the painted flag.
		Texture2D->bHasBeenPaintedInEditor = false;
	}

	// Automatically detect if the texture is a normal map and configure its properties accordingly
	//NormalMapIdentification::HandleAssetPostImport(this, Texture);

	FEditorDelegates::OnAssetPostImport.Broadcast(this, Texture);

	// Invalidate any materials using the newly imported texture. (occurs if you import over an existing texture)
	Texture->PostEditChange();

#if ENGINE_MINOR_VERSION >= 21
	// Invalidate any volume texture that was built on this texture.
	if (Texture2D)
	{
		for (TObjectIterator<UVolumeTexture> It; It; ++It)
		{
			UVolumeTexture* VolumeTexture = *It;
			if (VolumeTexture && VolumeTexture->Source2DTexture == Texture2D)
			{
				VolumeTexture->UpdateSourceFromSourceTexture();
				VolumeTexture->UpdateResource();
			}
		}
	}
#endif
	return Texture;
}
//
//
UObject* UScaleTextureFactory::FactoryReCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const TCHAR* Parms, bool& bOutOperationCanceled)
{
	{
		ParseParms(Parms);
		return FactoryReCreateBinary(InClass, InParent, InName, Flags, nullptr);
	}
	return nullptr;
}

UObject* UScaleTextureFactory::ReImportObject(UClass* InClass, UObject* InOuter, FName InName, EObjectFlags InFlags, const TCHAR* Parms, bool& OutCanceled)
{
	UObject* Result = nullptr;
	{
		// sanity check the file size of the impending import and prompt
		// the user if they wish to continue if the file size is very large
		{
			//UE_LOG(BatchProcessAssetsLog, Log, TEXT("FactoryCreateFile: %s with %s (%i %i %s)"), *InClass->GetName(), *GetClass()->GetName(), bCreateNew, bText, *Filename);

			Result = FactoryReCreateFile(InClass, InOuter, InName, InFlags, Parms, OutCanceled);
		}
	}

	if (Result != nullptr)
	{
		Result->MarkPackageDirty();
		ULevel::LevelDirtiedEvent.Broadcast();
		Result->PostEditChange();
	}
	CurrentFilename = TEXT("");
	return Result;
}

UObject* UScaleTextureFactory::ReImport(FAssetData& AssetData)
{
	FName Key("Dimensions");
	FString StrLeft;
	FString StrRight;
	AssetData.TagsAndValues.Find(Key)->Split("x",&StrLeft,&StrRight);

	//UE_LOG(LogScaleTextureFactory, Log, TEXT("left = %s right = %s"), *StrLeft, *StrRight);

	int32 TexWidth = FCString::Atoi(*StrLeft);
	int32 TexHight = FCString::Atoi(*StrRight);
	int32 MaxSize = FMath::Max<int32>(TexWidth, TexHight);
	if (MaxSize <= MinTexSize)
	{
		return nullptr;
	}

	UClass* ImportAssetType = UTexture::StaticClass();
	FString PackageName = AssetData.PackageName.ToString();
	bool bImportWasCancelled = false;
	UPackage* Pkg = CreatePackage(nullptr, *PackageName);//PackageNameΪ/Game/xxx.png

	Pkg->FullyLoad();//如果不加载，会查找不到
	UObject* Result = ReImportObject(ImportAssetType, Pkg, AssetData.AssetName, RF_Public | RF_Standalone, nullptr, bImportWasCancelled);
	FString FileExtension = TEXT("tga");

	bool bImportSucceeded = false;
	if (Result)
	{
		// Notify the asset registry
		FAssetRegistryModule::AssetCreated(Result);
		GEditor->BroadcastObjectReimported(Result);
		bImportSucceeded = true;

		ImportAssetType = ResolveSupportedClass();
		OnNewImportRecord(ImportAssetType, FileExtension, bImportSucceeded, bImportWasCancelled, FDateTime::UtcNow());
	}
	return Result;
}


void UScaleTextureFactory::SetImportParam(int32 MaxTexSize, bool IsNotReallyModifyOriginalTex)
{
	iMaxTexSize = MaxTexSize;
	bIsNotReallyModifyOriginalTex = IsNotReallyModifyOriginalTex;
}

void UScaleTextureFactory::OnNewImportRecord(UClass* AssetType, const FString& FileExtension, bool bSucceeded, bool bWasCancelled, const FDateTime& StartTime)
{
	// Don't attempt to report usage stats if analytics isn't available
	if (AssetType != nullptr && FEngineAnalytics::IsAvailable())
	{
		TArray<FAnalyticsEventAttribute> Attribs;
		Attribs.Add(FAnalyticsEventAttribute(TEXT("AssetType"), AssetType->GetName()));
		Attribs.Add(FAnalyticsEventAttribute(TEXT("FileExtension"), FileExtension));
		Attribs.Add(FAnalyticsEventAttribute(TEXT("Outcome"), bSucceeded ? TEXT("Success") : (bWasCancelled ? TEXT("Cancelled") : TEXT("Failed"))));
		FTimespan TimeTaken = 0.1;//FDateTime::UtcNow() - StartTime;
		Attribs.Add(FAnalyticsEventAttribute(TEXT("TimeTaken.Seconds"), (float)TimeTaken.GetTotalSeconds()));

		FEngineAnalytics::GetProvider().RecordEvent(TEXT("Editor.Usage.ImportAsset"), Attribs);
	}
}

void UScaleTextureFactory::GetFinalSize(const FIntPoint& oldSize, FIntPoint& OutNewSize, int32 MaxTextureSize)
{
	int32 OriginalWidth = oldSize.X;
	int32 OriginalHeight = oldSize.Y;

	int32 newWidth = 0;
	int32 newHeight = 0;

	if (OriginalWidth == OriginalHeight)
	{
		if (newWidth < MaxTextureSize)
			newWidth = newHeight = MaxTextureSize;
	}
	else if (OriginalWidth > OriginalHeight)
	{
		if (newWidth < MaxTextureSize)
		{
			newWidth = MaxTextureSize;
			newHeight = (float)OriginalHeight / OriginalWidth * newWidth;
		}
	}
	else
	{
		if (newHeight < MaxTextureSize)
		{
			newHeight = MaxTextureSize;
			newWidth = (float)OriginalWidth / OriginalHeight * newHeight;
		}
	}

	OutNewSize.X = newWidth;
	OutNewSize.Y = newHeight;
}

TArray<uint8>* UScaleTextureFactory::GetReImportData(UTexture2D* Texture, TArray<uint8>* RawDataOut, const FIntPoint& FinalSize) {
	const bool bIsRGBA16 = Texture->Source.GetFormat() == TSF_RGBA16;
	const int32 BytesPerPixel = bIsRGBA16 ? 8 : 4;
	TArray<uint8> RawData;
	Texture->Source.GetMipData(RawData, 0);
	const int32 OriginalWidth = Texture->Source.GetSizeX();
	const int32 OriginalHeight = Texture->Source.GetSizeY();

	int32 newWidth = FinalSize.X;
	int32 newHeight = FinalSize.Y;

	float TexScaleW = (float)newWidth / OriginalWidth;
	float TexScaleH = (float)newHeight / OriginalHeight;
	
	for (int32 j = 0; j < newHeight; j++)
	{
		for (int32 i = 0; i < newWidth; i++)
		{
			for (int32 byteIndex = 0; byteIndex < BytesPerPixel; byteIndex++)
			{
				int32 index = ((int32)(j / TexScaleH) * OriginalWidth + (int32)(i / TexScaleW)) * BytesPerPixel + byteIndex;
				if (index > RawData.Num() - 1) {
					UE_LOG(LogScaleTextureFactory, Error, TEXT("invalid num %d"),&index);
					return nullptr;
				}
				uint8 data = RawData[index];
				RawDataOut->Add(data);
			}
		}
	}
	return RawDataOut;
}

UTexture2D* UScaleTextureFactory::CreateTexture2D(UObject* InParent, FName Name, EObjectFlags Flags)
{
	UTexture2D* pTex2D = Cast<UTexture2D>(pOriginalTex);

	if (pTex2D)
	{
		// Release the existing resource so the new texture can get a fresh one. Otherwise if the next call to Init changes the format
		// of the texture and UpdateResource is called the editor will crash in RenderThread
		pTex2D->ReleaseResource();
		return pTex2D;
	}
	else
	{
		return Super::CreateTexture2D(InParent, Name, Flags);
	}
}

UTextureCube* UScaleTextureFactory::CreateTextureCube(UObject* InParent, FName Name, EObjectFlags Flags)
{
	UTextureCube* pTexCube = Cast<UTextureCube>(pOriginalTex);
	if (pTexCube)
	{
		// Release the existing resource so the new texture can get a fresh one. Otherwise if the next call to Init changes the format
		// of the texture and UpdateResource is called the editor will crash in RenderThread
		pTexCube->ReleaseResource();
		return pTexCube;
	}
	else
	{
		return Super::CreateTextureCube(InParent, Name, Flags);
	}
}
#undef LOCTEXT_NAMESPACE
