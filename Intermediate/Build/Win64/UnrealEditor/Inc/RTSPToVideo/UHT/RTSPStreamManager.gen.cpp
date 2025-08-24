// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "RTSPToVideo/Public/RTSPStreamManager.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeRTSPStreamManager() {}

// Begin Cross Module References
COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
RTSPTOVIDEO_API UClass* Z_Construct_UClass_URTSPStreamManager();
RTSPTOVIDEO_API UClass* Z_Construct_UClass_URTSPStreamManager_NoRegister();
UPackage* Z_Construct_UPackage__Script_RTSPToVideo();
// End Cross Module References

// Begin Class URTSPStreamManager
void URTSPStreamManager::StaticRegisterNativesURTSPStreamManager()
{
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(URTSPStreamManager);
UClass* Z_Construct_UClass_URTSPStreamManager_NoRegister()
{
	return URTSPStreamManager::StaticClass();
}
struct Z_Construct_UClass_URTSPStreamManager_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "RTSPStreamManager.h" },
		{ "ModuleRelativePath", "Public/RTSPStreamManager.h" },
	};
#endif // WITH_METADATA
	static UObject* (*const DependentSingletons[])();
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<URTSPStreamManager>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
UObject* (*const Z_Construct_UClass_URTSPStreamManager_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UObject,
	(UObject* (*)())Z_Construct_UPackage__Script_RTSPToVideo,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_URTSPStreamManager_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_URTSPStreamManager_Statics::ClassParams = {
	&URTSPStreamManager::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	nullptr,
	nullptr,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	0,
	0,
	0,
	0x001000A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_URTSPStreamManager_Statics::Class_MetaDataParams), Z_Construct_UClass_URTSPStreamManager_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_URTSPStreamManager()
{
	if (!Z_Registration_Info_UClass_URTSPStreamManager.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_URTSPStreamManager.OuterSingleton, Z_Construct_UClass_URTSPStreamManager_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_URTSPStreamManager.OuterSingleton;
}
template<> RTSPTOVIDEO_API UClass* StaticClass<URTSPStreamManager>()
{
	return URTSPStreamManager::StaticClass();
}
URTSPStreamManager::URTSPStreamManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(URTSPStreamManager);
URTSPStreamManager::~URTSPStreamManager() {}
// End Class URTSPStreamManager

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_arpit_Documents_Unreal_Projects_ExpermentalProject_Plugins_RTSPToVideo_Source_RTSPToVideo_Public_RTSPStreamManager_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_URTSPStreamManager, URTSPStreamManager::StaticClass, TEXT("URTSPStreamManager"), &Z_Registration_Info_UClass_URTSPStreamManager, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(URTSPStreamManager), 3865948951U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_arpit_Documents_Unreal_Projects_ExpermentalProject_Plugins_RTSPToVideo_Source_RTSPToVideo_Public_RTSPStreamManager_h_2285358564(TEXT("/Script/RTSPToVideo"),
	Z_CompiledInDeferFile_FID_Users_arpit_Documents_Unreal_Projects_ExpermentalProject_Plugins_RTSPToVideo_Source_RTSPToVideo_Public_RTSPStreamManager_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_arpit_Documents_Unreal_Projects_ExpermentalProject_Plugins_RTSPToVideo_Source_RTSPToVideo_Public_RTSPStreamManager_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
