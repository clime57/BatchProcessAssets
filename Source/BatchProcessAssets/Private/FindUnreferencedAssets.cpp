// Fill out your copyright notice in the Description page of Project Settings.

#include "BatchProcessAssets.h"
#include "FindUnreferencedAssets.h"

void UFindUnreferencedAssets::Find() const
{
	TArray<FAssetData> AssetDataList;
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	FARFilter Filter;
	Filter.bRecursivePaths = true;
	new (Filter.PackagePaths) FName("/Game");
	AssetRegistryModule.Get().GetAssets(Filter, AssetDataList);

	TArray<FAssetIdentifier> UseLessAssets;

	TArray<FAssetIdentifier> SelectAssetId;

	TMap<FName, FAssetData> AssetDataMap;

	for (FAssetData& ad : AssetDataList)
	{
		SelectAssetId.Add(FAssetIdentifier(ad.PackageName));
		AssetDataMap.Add(ad.PackageName, ad);
	}
	int LoopTime = 10;
	FScopedSlowTask SlowTask(AssetDataList.Num() * LoopTime, FText::FromString("Searching"));
	SlowTask.MakeDialog();
	for (int i = 0 ; i < LoopTime ; i++)
	{
		for (FAssetIdentifier& AssetId : SelectAssetId)
		{
			SlowTask.EnterProgressFrame(1, FText::Format(FText::FromString("Parsing \"{0}\"..."), FText::FromName(AssetId.PackageName)));
			
			if (UseLessAssets.Contains(AssetId))continue;
			TArray<FAssetIdentifier> ReferenceNames;
			TArray<FAssetIdentifier> SearchStack(ReferenceNames);
			AssetRegistryModule.Get().GetReferencers(AssetId, ReferenceNames, EAssetRegistryDependencyType::All);
			if (ReferenceNames.Num() == 0 || (ReferenceNames.Num() == 1 && ReferenceNames[0] == AssetId) || IsRefrenceUseless(UseLessAssets, AssetId, ReferenceNames, AssetDataMap, SearchStack,AssetRegistryModule))
			{
				AddtoArray(UseLessAssets, AssetId);
			}
		}
	}
}

FORCEINLINE void UFindUnreferencedAssets::AddtoArray(TArray<FAssetIdentifier>& UseLessAssets, FAssetIdentifier& AssetId) const
{
	UseLessAssets.Add(AssetId);
	UE_LOG(BatchProcessAssetsLog, Log, TEXT("UnreferencedAssets = %s"), *(AssetId.PackageName.ToString()));
}

bool UFindUnreferencedAssets::IsRefrenceUseless(TArray<FAssetIdentifier>& UseLessAssets, FAssetIdentifier& AssetId, TArray<FAssetIdentifier> ReferenceNames, TMap<FName, FAssetData>& AssetDataMap, TArray<FAssetIdentifier>& SearchStack,FAssetRegistryModule& AssetRegistryModule) const
{
	bool IsUseLess = true;
	for (FAssetIdentifier& RefenceName : ReferenceNames)
	{
		if (!(RefenceName == AssetId))
		{
			auto FindAssetData = AssetDataMap.Find(RefenceName.PackageName);

			if (FindAssetData->AssetClass.Compare(FName("World")) == 0 ||
				FindAssetData->AssetClass.Compare(FName("MapBuildDataRegistry")) == 0)
			{
				IsUseLess = false;
				break;
			}

			if (!UseLessAssets.Contains(RefenceName))
			{
				TArray<FAssetIdentifier> ParerentReferenceNames;
				AssetRegistryModule.Get().GetReferencers(RefenceName, ParerentReferenceNames, EAssetRegistryDependencyType::All);
				if (ParerentReferenceNames.Num() > 0)
				{
					TArray<FAssetIdentifier>::TIterator it = ParerentReferenceNames.CreateIterator();
					for (; it.GetIndex() < ParerentReferenceNames.Num(); it++)
					{
						if (SearchStack.Contains(*it))
						{
							it.RemoveCurrent();
						}
						else
						{
							SearchStack.Add(*it);
						}
					}
					if (!IsRefrenceUseless(UseLessAssets, RefenceName, ParerentReferenceNames, AssetDataMap, SearchStack,AssetRegistryModule))
					{
						IsUseLess = false;
						break;
					}
				}
			}
		}
	}
	return IsUseLess;
}
