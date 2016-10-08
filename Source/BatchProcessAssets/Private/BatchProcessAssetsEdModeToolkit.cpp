// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BatchProcessAssetsPrivatePCH.h"
#include "BatchProcessAssetsEdMode.h"
#include "BatchProcessAssetsEdModeToolkit.h"

#include "ContentBrowserModule.h"
#include "ScaleTextureFactory.h"


#define LOCTEXT_NAMESPACE "FBatchProcessAssetsEdModeToolkit"

FBatchProcessAssetsEdModeToolkit::FBatchProcessAssetsEdModeToolkit()
{
	struct Locals
	{
		static bool IsWidgetEnabled()
		{
			//return GEditor->GetSelectedActors()->Num() != 0;
			return true;
		}

		static FReply OnButtonClick(float scale)
		{
			//USelection* SelectedActors = GEditor->GetSelectedActors();

			//// Let editor know that we're about to do something that we want to undo/redo
			//GEditor->BeginTransaction(LOCTEXT("MoveActorsTransactionName", "MoveActors"));

			//// For each selected actor
			//for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
			//{
			//	if (AActor* LevelActor = Cast<AActor>(*Iter))
			//	{
			//		// Register actor in opened transaction (undo/redo)
			//		LevelActor->Modify();
			//		// Move actor to given location
			//		LevelActor->TeleportTo(LevelActor->GetActorLocation() + InOffset, FRotator(0, 0, 0));
			//	}
			//}

			//// We're done moving actors so close transaction
			//GEditor->EndTransaction();
			ReImport(scale);
			return FReply::Handled();
		}

		static TSharedRef<SWidget> MakeButton(FText InLabel,float scale)
		{
			return SNew(SButton)
				.Text(InLabel)
				.OnClicked_Static(&Locals::OnButtonClick, scale);
		}
	};

	const float Factor = 256.0f;

	SAssignNew(ToolkitWidget, SBorder)
		.HAlign(HAlign_Center)
		.Padding(25)
		.IsEnabled_Static(&Locals::IsWidgetEnabled)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(50)
			[
				SNew(STextBlock)
				.AutoWrapText(true)
				.Text(LOCTEXT("HelperLabel", "..."))
			]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					Locals::MakeButton(LOCTEXT("ScaleTexturesButtonLabel1", "ScaleTextures 1/2"),1.0/2)
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					Locals::MakeButton(LOCTEXT("ScaleTexturesButtonLabel2", "ScaleTextures 1/4"),1.0/4)
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					Locals::MakeButton(LOCTEXT("ScaleTexturesButtonLabel3", "ScaleTextures 1/8"),1.0/8)
				] 
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					Locals::MakeButton(LOCTEXT("ScaleTexturesButtonLabel4", "ScaleTextures 1/16"),1.0/16)
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					Locals::MakeButton(LOCTEXT("ScaleTexturesButtonLabel5", "ScaleTextures 1/32"),1.0/32)
				]
			//+ SVerticalBox::Slot()
			//	.HAlign(HAlign_Center)
			//	.AutoHeight()
			//	[
			//		SNew(SHorizontalBox)
			//		+ SHorizontalBox::Slot()
			//		.AutoWidth()
			//		[
			//			Locals::MakeButton(LOCTEXT("LeftButtonLabel", "Left"), FVector(0, -Factor, 0))
			//		]
			//		+ SHorizontalBox::Slot()
			//			.AutoWidth()
			//			[
			//				Locals::MakeButton(LOCTEXT("RightButtonLabel", "Right"), FVector(0, Factor, 0))
			//			]
			//	]
			//+ SVerticalBox::Slot()
			//	.HAlign(HAlign_Center)
			//	.AutoHeight()
			//	[
			//		Locals::MakeButton(LOCTEXT("DownButtonLabel", "Down"), FVector(0, 0, -Factor))
			//	]

		];
}

FName FBatchProcessAssetsEdModeToolkit::GetToolkitFName() const
{
	return FName("BatchProcessAssetsEdMode");
}

FText FBatchProcessAssetsEdModeToolkit::GetBaseToolkitName() const
{
	return NSLOCTEXT("BatchProcessAssetsEdModeToolkit", "DisplayName", "BatchProcessAssetsEdMode Tool");
}

class FEdMode* FBatchProcessAssetsEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FBatchProcessAssetsEdMode::EM_BatchProcessAssetsEdModeId);
}


void FBatchProcessAssetsEdModeToolkit::ReImport(float scale)
{
	auto TextureFact = NewObject<UScaleTextureFactory>();
	TextureFact->Import(scale);
}

void FBatchProcessAssetsEdModeToolkit::ReImportSelected(float scale)
{
	//TArray<UObject*> ObjectsToExport;
	//const bool SkipRedirectors = false;
	//GetSelectedAssets(ObjectsToExport, SkipRedirectors);

	//if (ObjectsToExport.Num() > 0)
	//{
	//	ObjectTools::ExportObjects(ObjectsToExport, /*bPromptForEachFileName=*/true);
	//}
	TArray<FAssetData> SelectedAssets;
	IContentBrowserSingleton& ContentBrowserSingleton = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get();

	ContentBrowserSingleton.GetSelectedAssets(SelectedAssets);
	auto TextureFact = NewObject<UScaleTextureFactory>();
	TextureFact->ReImportSelected(scale, SelectedAssets);

	return;


	for (TArray<FAssetData>::TConstIterator it = SelectedAssets.CreateConstIterator(); it; it++)
	{
		//ExportBinary((*it).GetAsset());
	}
}






#undef LOCTEXT_NAMESPACE
