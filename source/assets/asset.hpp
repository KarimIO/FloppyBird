#pragma once

enum class AssetType {
	Texture,
	Sound
};

struct Asset {
	AssetType type;
};

template<typename T>
struct SpecificAsset : Asset {

};

struct AssetReference {

};

template<typename T>
struct SpecificAssetReference : AssetReference {
	
};
