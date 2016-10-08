// Fill out your copyright notice in the Description page of Project Settings.
#include "BatchProcessAssetsPrivatePCH.h"
#include "ScaleTextureFactory.h"
#include "AssetRegistryModule.h"
#include "MaterialExpressionTextureSample.h"
#include "NormalMapIdentification.h"

UTexture* UScaleTextureFactory::ImportTexture1(UTexture* OldTexture,
	const TArray<uint8>* RawPNG,
	ETextureSourceFormat TextureFormat,
	UClass* Class,
	UObject* InParent,
	FName Name)
{
	EObjectFlags Flags = RF_Dynamic;
	const int32 NewWidth = (int32)(OldTexture->Source.GetSizeX()*TexScale);
	const int32 NewHeight = (int32)(OldTexture->Source.GetSizeY()*TexScale);
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
		Texture->SRGB = true;
		{
			uint8* MipData = Texture->Source.LockMip(0);
			FMemory::Memcpy(MipData, RawPNG->GetData(), RawPNG->Num());
		}
		Texture->Source.UnlockMip(0);
	}

	return Texture;
}


UObject* UScaleTextureFactory::FactoryCreateBinary1
(
	UClass*				Class,
	UObject*			InParent,
	FName				Name,
	EObjectFlags		Flags,
	UObject*			Context
	)
{

	TCHAR*		Type = TEXT("TGA");
	FEditorDelegates::OnAssetPreImport.Broadcast(this, Class, InParent, Name, Type);//Type?以后要补上

	// if the texture already exists, remember the user settings
	UTexture* ExistingTexture = FindObject<UTexture>(InParent, *Name.ToString());
	UTexture2D* ExistingTexture2D = FindObject<UTexture2D>(InParent, *Name.ToString());
	if (!(ExistingTexture && ExistingTexture2D))
	{
		return nullptr;
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
	TextureMipGenSettings				ExistingMipGenSettings = TextureMipGenSettings(0);

	bUsingExistingSettings = true;

	if (ExistingTexture && bUsingExistingSettings)
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

	TArray<uint8> RawPNG;
	//float scale = 0.0625;
	if (GetReImportData(ExistingTexture2D, &RawPNG) == nullptr) {
		return nullptr;
	}

	FTextureReferenceReplacer RefReplacer(ExistingTexture);

	UTexture* Texture = ImportTexture1(ExistingTexture, &RawPNG,Format, Class, InParent,Name);
	//if (!Texture)
	//{
	//	if (ExistingTexture)
	//	{
	//		// We failed to import over the existing texture. Make sure the resource is ready in the existing texture.
	//		ExistingTexture->UpdateResource();
	//	}

	//	//Warn->Logf(ELogVerbosity::Error, TEXT("Texture import failed"));
	//	FEditorDelegates::OnAssetPostImport.Broadcast(this, nullptr);
	//	return nullptr;
	//}

	//Replace the reference for the new texture with the existing one so that all current users still have valid references.
	RefReplacer.Replace(Texture);

	// Start with the value that the loader suggests.
	CompressionSettings = Texture->CompressionSettings;

	// Figure out whether we're using a normal map LOD group.
	bool bIsNormalMapLODGroup = false;
	if (LODGroup == TEXTUREGROUP_WorldNormalMap
		|| LODGroup == TEXTUREGROUP_CharacterNormalMap
		|| LODGroup == TEXTUREGROUP_VehicleNormalMap
		|| LODGroup == TEXTUREGROUP_WeaponNormalMap)
	{
		// Change from default to normal map.
		if (CompressionSettings == TC_Default)
		{
			CompressionSettings = TC_Normalmap;
		}
		bIsNormalMapLODGroup = true;
	}

	// Propagate options.
	Texture->CompressionSettings = CompressionSettings;

	// Packed normal map
	if (Texture->IsNormalMap())
	{
		Texture->SRGB = 0;
		if (!bIsNormalMapLODGroup)
		{
			LODGroup = TEXTUREGROUP_WorldNormalMap;
		}
	}

	//if (!FCString::Stricmp(Type, TEXT("ies")))
	//{
	//	LODGroup = TEXTUREGROUP_IESLightProfile;
	//}

	Texture->LODGroup = LODGroup;

	// Revert the LODGroup to the default if it was forcibly set by the texture being a normal map.
	// This handles the case where multiple textures are being imported consecutively and
	// LODGroup unexpectedly changes because some textures were normal maps and others weren't.
	if (LODGroup == TEXTUREGROUP_WorldNormalMap && !bIsNormalMapLODGroup)
	{
		LODGroup = TEXTUREGROUP_World;
	}

	Texture->CompressionNone = NoCompression;
	Texture->CompressionNoAlpha = NoAlpha;
	Texture->DeferCompression = bDeferCompression;
	Texture->bDitherMipMapAlpha = bDitherMipMapAlpha;

	if (Texture->MipGenSettings == TMGS_FromTextureGroup)
	{
		// unless the loader suggest a different setting
		Texture->MipGenSettings = MipGenSettings;
	}

	Texture->bPreserveBorder = bPreserveBorder;

	Texture->AssetImportData->Update(CurrentFilename);

	UTexture2D* Texture2D = Cast<UTexture2D>(Texture);

	// Restore user set options
	if (ExistingTexture && bUsingExistingSettings)
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
	else
	{
		Texture->bFlipGreenChannel = (bFlipNormalMapGreenChannel && Texture->IsNormalMap());
		// save user option
		GConfig->SetBool(TEXT("/Script/UnrealEd.EditorEngine"), TEXT("FlipNormalMapGreenChannel"), bFlipNormalMapGreenChannel, GEngineIni);
	}

	if (Texture2D)
	{
		// The texture has been imported and has no editor specific changes applied so we clear the painted flag.
		Texture2D->bHasBeenPaintedInEditor = false;
	}

	// Automatically detect if the texture is a normal map and configure its properties accordingly
	NormalMapIdentification::HandleAssetPostImport(this, Texture);//法线图导入判断，之后加上

	FEditorDelegates::OnAssetPostImport.Broadcast(this, Texture);

	// Invalidate any materials using the newly imported texture. (occurs if you import over an existing texture)
	Texture->PostEditChange();
	FFeedbackContext* Warn = nullptr;//之后修复
	 //If we are automatically creating a material for this texture...
	if (bCreateMaterial)
	{
		// Create the package for the material
		const FString MaterialName = FString::Printf(TEXT("%s_Mat"), *Name.ToString());
		const FString MaterialPackageName = FPackageName::GetLongPackagePath(InParent->GetName()) + TEXT("/") + MaterialName;
		UPackage* MaterialPackage = CreatePackage(nullptr, *MaterialPackageName);

		// Create the material
		UMaterialFactoryNew* Factory = NewObject<UMaterialFactoryNew>();
		UMaterial* Material = (UMaterial*)Factory->FactoryCreateNew(UMaterial::StaticClass(), MaterialPackage, *MaterialName, Flags, Context, Warn);

		// Notify the asset registry
		FAssetRegistryModule::AssetCreated(Material);

		// Create a texture reference for the texture we just imported and hook it up to the diffuse channel
		UMaterialExpression* Expression = NewObject<UMaterialExpression>(Material, UMaterialExpressionTextureSample::StaticClass());
		Material->Expressions.Add(Expression);
		TArray<FExpressionOutput> Outputs;

		// If the user hasn't turned on any of the link checkboxes, default "bRGBToBaseColor" to being on.
		if (!bRGBToBaseColor && !bRGBToEmissive && !bAlphaToRoughness && !bAlphaToEmissive && !bAlphaToOpacity && !bAlphaToOpacityMask)
		{
			bRGBToBaseColor = 1;
		}

		// Set up the links the user asked for
		if (bRGBToBaseColor)
		{
			Material->BaseColor.Expression = Expression;
			((UMaterialExpressionTextureSample*)Material->BaseColor.Expression)->Texture = Texture;

			Outputs = Material->BaseColor.Expression->GetOutputs();
			FExpressionOutput* Output = Outputs.GetData();
			Material->BaseColor.Mask = Output->Mask;
			Material->BaseColor.MaskR = Output->MaskR;
			Material->BaseColor.MaskG = Output->MaskG;
			Material->BaseColor.MaskB = Output->MaskB;
			Material->BaseColor.MaskA = Output->MaskA;
		}

		if (bRGBToEmissive)
		{
			Material->EmissiveColor.Expression = Expression;
			((UMaterialExpressionTextureSample*)Material->EmissiveColor.Expression)->Texture = Texture;

			Outputs = Material->EmissiveColor.Expression->GetOutputs();
			FExpressionOutput* Output = Outputs.GetData();
			Material->EmissiveColor.Mask = Output->Mask;
			Material->EmissiveColor.MaskR = Output->MaskR;
			Material->EmissiveColor.MaskG = Output->MaskG;
			Material->EmissiveColor.MaskB = Output->MaskB;
			Material->EmissiveColor.MaskA = Output->MaskA;
		}

		if (bAlphaToRoughness)
		{
			Material->Roughness.Expression = Expression;
			((UMaterialExpressionTextureSample*)Material->Roughness.Expression)->Texture = Texture;

			Outputs = Material->Roughness.Expression->GetOutputs();
			FExpressionOutput* Output = Outputs.GetData();
			Material->Roughness.Mask = Output->Mask;
			Material->Roughness.MaskR = 0;
			Material->Roughness.MaskG = 0;
			Material->Roughness.MaskB = 0;
			Material->Roughness.MaskA = 1;
		}

		if (bAlphaToEmissive)
		{
			Material->EmissiveColor.Expression = Expression;
			((UMaterialExpressionTextureSample*)Material->EmissiveColor.Expression)->Texture = Texture;

			Outputs = Material->EmissiveColor.Expression->GetOutputs();
			FExpressionOutput* Output = Outputs.GetData();
			Material->EmissiveColor.Mask = Output->Mask;
			Material->EmissiveColor.MaskR = 0;
			Material->EmissiveColor.MaskG = 0;
			Material->EmissiveColor.MaskB = 0;
			Material->EmissiveColor.MaskA = 1;
		}

		if (bAlphaToOpacity)
		{
			Material->Opacity.Expression = Expression;
			((UMaterialExpressionTextureSample*)Material->Opacity.Expression)->Texture = Texture;

			Outputs = Material->Opacity.Expression->GetOutputs();
			FExpressionOutput* Output = Outputs.GetData();
			Material->Opacity.Mask = Output->Mask;
			Material->Opacity.MaskR = 0;
			Material->Opacity.MaskG = 0;
			Material->Opacity.MaskB = 0;
			Material->Opacity.MaskA = 1;
		}

		if (bAlphaToOpacityMask)
		{
			Material->OpacityMask.Expression = Expression;
			((UMaterialExpressionTextureSample*)Material->OpacityMask.Expression)->Texture = Texture;

			Outputs = Material->OpacityMask.Expression->GetOutputs();
			FExpressionOutput* Output = Outputs.GetData();
			Material->OpacityMask.Mask = Output->Mask;
			Material->OpacityMask.MaskR = 0;
			Material->OpacityMask.MaskG = 0;
			Material->OpacityMask.MaskB = 0;
			Material->OpacityMask.MaskA = 1;
		}

		Material->TwoSided = bTwoSided;
		Material->BlendMode = Blending;
		Material->SetShadingModel(ShadingModel);

		Material->PostEditChange();
	}
	return Texture;


//return nullptr;
}
//
//
UObject* UScaleTextureFactory::FactoryCreateFile1(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const TCHAR* Parms, bool& bOutOperationCanceled)
{
	{
		ParseParms(Parms);
		return FactoryCreateBinary1(InClass, InParent, InName, Flags, nullptr);
	}
	return nullptr;
}

UObject* UScaleTextureFactory::ImportObject1(UClass* InClass, UObject* InOuter, FName InName, EObjectFlags InFlags, const TCHAR* Parms, bool& OutCanceled)
{
	UObject* Result = nullptr;
	{
		// sanity check the file size of the impending import and prompt
		// the user if they wish to continue if the file size is very large
		{
			//UE_LOG(BatchProcessAssetsLog, Log, TEXT("FactoryCreateFile: %s with %s (%i %i %s)"), *InClass->GetName(), *GetClass()->GetName(), bCreateNew, bText, *Filename);

			Result = FactoryCreateFile1(InClass, InOuter, InName, InFlags, Parms, OutCanceled);
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


void SyncBrowserToAssets(const TArray<UObject*>& AssetsToSync)
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	ContentBrowserModule.Get().SyncBrowserToAssets(AssetsToSync, /*bAllowLockedBrowsers=*/true);
}

void UScaleTextureFactory::Import(float scale) {

	TexScale = scale;
	// Load the asset registry module
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	TArray<FAssetData> OutAssetDataList;
	// Form a filter from the paths
	FARFilter Filter;
	Filter.bRecursivePaths = true;
	new (Filter.ClassNames) FName("Texture2D");
	new (Filter.PackagePaths) FName("/Game");
	// Query for a list of assets in the selected paths
	AssetRegistryModule.Get().GetAssets(Filter, OutAssetDataList);
	TArray<UObject*> ReturnObjects;
	for (int32 i = 0; i < OutAssetDataList.Num(); ++i)
	{

		UObject* Result = ReImport(OutAssetDataList[i]);
		if (Result) 
		{
			ReturnObjects.Add(Result);
		}
	}
	//SlowTask.EnterProgressFrame(1);
	// Sync content browser to the newly created assets
	if (ReturnObjects.Num())
	{
		SyncBrowserToAssets(ReturnObjects);
	}
}





UObject* UScaleTextureFactory::ReImport(FAssetData& AssetData)
{
	UClass* ImportAssetType = UTexture::StaticClass();
	FString PackageName = AssetData.PackageName.ToString();
	bool bImportWasCancelled = false;
	UPackage* Pkg = CreatePackage(nullptr, *PackageName);//PackageNameΪ/Game/xxx.png
	Pkg->FullyLoad();//如果不加载，会查找不到
	UObject* Result = ImportObject1(ImportAssetType, Pkg, AssetData.AssetName, RF_Public | RF_Standalone, nullptr, bImportWasCancelled);
	FString FileExtension = TEXT("tga");

	bool bImportSucceeded = false;
	if (Result)
	{
		// Notify the asset registry
		FAssetRegistryModule::AssetCreated(Result);
		GEditor->BroadcastObjectReimported(Result);
		bImportSucceeded = true;
	}
	else
	{
		//const FText Message = FText::Format(LOCTEXT("ImportFailed_Generic", "Failed to import '{0}'. Failed to create asset '{1}'"), FText::FromString(Filename), FText::FromString(PackageName));
		//FMessageDialog::Open(EAppMsgType::Ok, Message);
		//UE_LOG(BatchProcessAssetsLog, Warning, TEXT("%s"), *Message.ToString());
	}
	// Refresh the supported class.  Some factories (e.g. FBX) only resolve their type after reading the file
	ImportAssetType = ResolveSupportedClass();
	OnNewImportRecord(ImportAssetType, FileExtension, bImportSucceeded, bImportWasCancelled, FDateTime::UtcNow());
	return Result;
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

TArray<uint8>* UScaleTextureFactory::GetReImportData(UTexture2D* Texture, TArray<uint8>* RawDataOut) {
	const bool bIsRGBA16 = Texture->Source.GetFormat() == TSF_RGBA16;
	const int32 BytesPerPixel = bIsRGBA16 ? 8 : 4;
	TArray<uint8> RawData;
	Texture->Source.GetMipData(RawData, 0);
	const int32 OriginalWidth = Texture->Source.GetSizeX();
	const int32 OriginalHeight = Texture->Source.GetSizeY();
	int32 newWidth = (int32)(OriginalWidth * TexScale);
	int32 newHeight = (int32)(OriginalHeight * TexScale);

	for (int32 j = 0; j < newHeight; j++)
	{
		for (int32 i = 0; i < newWidth; i++)
		{
			for (int32 byteIndex = 0; byteIndex < BytesPerPixel; byteIndex++)
			{
				int32 index = ((int32)(j / TexScale) * OriginalWidth + (int32)(i / TexScale)) * BytesPerPixel + byteIndex;
				if (index > RawData.Num() - 1) {
					//UE_LOG(BatchProcessAssetsLog, Error, TEXT("invalid num %d"),&index);
					int32 recordIndex = index;
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

void UScaleTextureFactory::ReImportSelected(float scale,TArray<FAssetData>& SelectedAssets) {
	TexScale = scale;
	// Load the asset registry module
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	//TArray<FAssetData> OutAssetDataList;
	// Form a filter from the paths
	FARFilter Filter;
	Filter.bRecursivePaths = true;
	new (Filter.ClassNames) FName("Texture2D");
	new (Filter.PackagePaths) FName("/Game");
	// Query for a list of assets in the selected paths
	//AssetRegistryModule.Get().GetAssets(Filter, OutAssetDataList);
	TArray<UObject*> ReturnObjects;
	for (int32 i = 0; i < SelectedAssets.Num(); ++i)
	{

		UObject* Result = ReImport(SelectedAssets[i]);
		if (Result)
		{
			ReturnObjects.Add(Result);
		}
	}
	//SlowTask.EnterProgressFrame(1);
	// Sync content browser to the newly created assets
	if (ReturnObjects.Num())
	{
		SyncBrowserToAssets(ReturnObjects);
	}
}