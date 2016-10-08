// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
//Copy from Engine Source Code,did not modify anything.
namespace NormalMapIdentification
{
	/**
	 * Handle callback when an asset is imported.
	 * @param	InFactory	The texture factory being used.
	 * @param	InTexture	The texture that was imported.
	 */
	void HandleAssetPostImport( UTextureFactory* InFactory, UTexture* InTexture );
}
