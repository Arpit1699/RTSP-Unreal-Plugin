// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "RTSPToVideo/Public/RTSPStreamWidget.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeRTSPStreamWidget() {}

// Begin Cross Module References
RTSPTOVIDEO_API UClass* Z_Construct_UClass_URTSPStreamManager_NoRegister();
RTSPTOVIDEO_API UClass* Z_Construct_UClass_URTSPStreamWidget();
RTSPTOVIDEO_API UClass* Z_Construct_UClass_URTSPStreamWidget_NoRegister();
UMG_API UClass* Z_Construct_UClass_UImage_NoRegister();
UMG_API UClass* Z_Construct_UClass_UUserWidget();
UPackage* Z_Construct_UPackage__Script_RTSPToVideo();
// End Cross Module References

// Begin Class URTSPStreamWidget Function StartStream
struct Z_Construct_UFunction_URTSPStreamWidget_StartStream_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "RTSP" },
		{ "ModuleRelativePath", "Public/RTSPStreamWidget.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_URTSPStreamWidget_StartStream_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_URTSPStreamWidget, nullptr, "StartStream", nullptr, nullptr, nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_URTSPStreamWidget_StartStream_Statics::Function_MetaDataParams), Z_Construct_UFunction_URTSPStreamWidget_StartStream_Statics::Function_MetaDataParams) };
UFunction* Z_Construct_UFunction_URTSPStreamWidget_StartStream()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_URTSPStreamWidget_StartStream_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(URTSPStreamWidget::execStartStream)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->StartStream();
	P_NATIVE_END;
}
// End Class URTSPStreamWidget Function StartStream

// Begin Class URTSPStreamWidget Function StopStream
struct Z_Construct_UFunction_URTSPStreamWidget_StopStream_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[] = {
		{ "Category", "RTSP" },
		{ "ModuleRelativePath", "Public/RTSPStreamWidget.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FFunctionParams FuncParams;
};
const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_URTSPStreamWidget_StopStream_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_URTSPStreamWidget, nullptr, "StopStream", nullptr, nullptr, nullptr, 0, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UFunction_URTSPStreamWidget_StopStream_Statics::Function_MetaDataParams), Z_Construct_UFunction_URTSPStreamWidget_StopStream_Statics::Function_MetaDataParams) };
UFunction* Z_Construct_UFunction_URTSPStreamWidget_StopStream()
{
	static UFunction* ReturnFunction = nullptr;
	if (!ReturnFunction)
	{
		UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_URTSPStreamWidget_StopStream_Statics::FuncParams);
	}
	return ReturnFunction;
}
DEFINE_FUNCTION(URTSPStreamWidget::execStopStream)
{
	P_FINISH;
	P_NATIVE_BEGIN;
	P_THIS->StopStream();
	P_NATIVE_END;
}
// End Class URTSPStreamWidget Function StopStream

// Begin Class URTSPStreamWidget
void URTSPStreamWidget::StaticRegisterNativesURTSPStreamWidget()
{
	UClass* Class = URTSPStreamWidget::StaticClass();
	static const FNameNativePtrPair Funcs[] = {
		{ "StartStream", &URTSPStreamWidget::execStartStream },
		{ "StopStream", &URTSPStreamWidget::execStopStream },
	};
	FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
}
IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(URTSPStreamWidget);
UClass* Z_Construct_UClass_URTSPStreamWidget_NoRegister()
{
	return URTSPStreamWidget::StaticClass();
}
struct Z_Construct_UClass_URTSPStreamWidget_Statics
{
#if WITH_METADATA
	static constexpr UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
		{ "IncludePath", "RTSPStreamWidget.h" },
		{ "ModuleRelativePath", "Public/RTSPStreamWidget.h" },
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_StreamURL_MetaData[] = {
		{ "Category", "RTSP" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "// Blueprint exposed controls\n" },
#endif
		{ "ModuleRelativePath", "Public/RTSPStreamWidget.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "Blueprint exposed controls" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_VideoTarget_MetaData[] = {
		{ "Category", "RTSP" },
#if !UE_BUILD_SHIPPING
		{ "Comment", "/*UPROPERTY(meta = (BindWidget))\n    UImage* VideoTarget;*/" },
#endif
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Public/RTSPStreamWidget.h" },
#if !UE_BUILD_SHIPPING
		{ "ToolTip", "UPROPERTY(meta = (BindWidget))\n    UImage* VideoTarget;" },
#endif
	};
	static constexpr UECodeGen_Private::FMetaDataPairParam NewProp_StreamManager_MetaData[] = {
		{ "ModuleRelativePath", "Public/RTSPStreamWidget.h" },
	};
#endif // WITH_METADATA
	static const UECodeGen_Private::FStrPropertyParams NewProp_StreamURL;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_VideoTarget;
	static const UECodeGen_Private::FObjectPropertyParams NewProp_StreamManager;
	static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
	static UObject* (*const DependentSingletons[])();
	static constexpr FClassFunctionLinkInfo FuncInfo[] = {
		{ &Z_Construct_UFunction_URTSPStreamWidget_StartStream, "StartStream" }, // 3682849418
		{ &Z_Construct_UFunction_URTSPStreamWidget_StopStream, "StopStream" }, // 3961565138
	};
	static_assert(UE_ARRAY_COUNT(FuncInfo) < 2048);
	static constexpr FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
		TCppClassTypeTraits<URTSPStreamWidget>::IsAbstract,
	};
	static const UECodeGen_Private::FClassParams ClassParams;
};
const UECodeGen_Private::FStrPropertyParams Z_Construct_UClass_URTSPStreamWidget_Statics::NewProp_StreamURL = { "StreamURL", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(URTSPStreamWidget, StreamURL), METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_StreamURL_MetaData), NewProp_StreamURL_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_URTSPStreamWidget_Statics::NewProp_VideoTarget = { "VideoTarget", nullptr, (EPropertyFlags)0x001000000008000d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(URTSPStreamWidget, VideoTarget), Z_Construct_UClass_UImage_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_VideoTarget_MetaData), NewProp_VideoTarget_MetaData) };
const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_URTSPStreamWidget_Statics::NewProp_StreamManager = { "StreamManager", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, nullptr, nullptr, 1, STRUCT_OFFSET(URTSPStreamWidget, StreamManager), Z_Construct_UClass_URTSPStreamManager_NoRegister, METADATA_PARAMS(UE_ARRAY_COUNT(NewProp_StreamManager_MetaData), NewProp_StreamManager_MetaData) };
const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_URTSPStreamWidget_Statics::PropPointers[] = {
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_URTSPStreamWidget_Statics::NewProp_StreamURL,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_URTSPStreamWidget_Statics::NewProp_VideoTarget,
	(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_URTSPStreamWidget_Statics::NewProp_StreamManager,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_URTSPStreamWidget_Statics::PropPointers) < 2048);
UObject* (*const Z_Construct_UClass_URTSPStreamWidget_Statics::DependentSingletons[])() = {
	(UObject* (*)())Z_Construct_UClass_UUserWidget,
	(UObject* (*)())Z_Construct_UPackage__Script_RTSPToVideo,
};
static_assert(UE_ARRAY_COUNT(Z_Construct_UClass_URTSPStreamWidget_Statics::DependentSingletons) < 16);
const UECodeGen_Private::FClassParams Z_Construct_UClass_URTSPStreamWidget_Statics::ClassParams = {
	&URTSPStreamWidget::StaticClass,
	nullptr,
	&StaticCppClassTypeInfo,
	DependentSingletons,
	FuncInfo,
	Z_Construct_UClass_URTSPStreamWidget_Statics::PropPointers,
	nullptr,
	UE_ARRAY_COUNT(DependentSingletons),
	UE_ARRAY_COUNT(FuncInfo),
	UE_ARRAY_COUNT(Z_Construct_UClass_URTSPStreamWidget_Statics::PropPointers),
	0,
	0x00B010A0u,
	METADATA_PARAMS(UE_ARRAY_COUNT(Z_Construct_UClass_URTSPStreamWidget_Statics::Class_MetaDataParams), Z_Construct_UClass_URTSPStreamWidget_Statics::Class_MetaDataParams)
};
UClass* Z_Construct_UClass_URTSPStreamWidget()
{
	if (!Z_Registration_Info_UClass_URTSPStreamWidget.OuterSingleton)
	{
		UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_URTSPStreamWidget.OuterSingleton, Z_Construct_UClass_URTSPStreamWidget_Statics::ClassParams);
	}
	return Z_Registration_Info_UClass_URTSPStreamWidget.OuterSingleton;
}
template<> RTSPTOVIDEO_API UClass* StaticClass<URTSPStreamWidget>()
{
	return URTSPStreamWidget::StaticClass();
}
URTSPStreamWidget::URTSPStreamWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {}
DEFINE_VTABLE_PTR_HELPER_CTOR(URTSPStreamWidget);
URTSPStreamWidget::~URTSPStreamWidget() {}
// End Class URTSPStreamWidget

// Begin Registration
struct Z_CompiledInDeferFile_FID_Users_arpit_Documents_Unreal_Projects_ExpermentalProject_Plugins_RTSPToVideo_Source_RTSPToVideo_Public_RTSPStreamWidget_h_Statics
{
	static constexpr FClassRegisterCompiledInInfo ClassInfo[] = {
		{ Z_Construct_UClass_URTSPStreamWidget, URTSPStreamWidget::StaticClass, TEXT("URTSPStreamWidget"), &Z_Registration_Info_UClass_URTSPStreamWidget, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(URTSPStreamWidget), 574486996U) },
	};
};
static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_Users_arpit_Documents_Unreal_Projects_ExpermentalProject_Plugins_RTSPToVideo_Source_RTSPToVideo_Public_RTSPStreamWidget_h_3965658293(TEXT("/Script/RTSPToVideo"),
	Z_CompiledInDeferFile_FID_Users_arpit_Documents_Unreal_Projects_ExpermentalProject_Plugins_RTSPToVideo_Source_RTSPToVideo_Public_RTSPStreamWidget_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_Users_arpit_Documents_Unreal_Projects_ExpermentalProject_Plugins_RTSPToVideo_Source_RTSPToVideo_Public_RTSPStreamWidget_h_Statics::ClassInfo),
	nullptr, 0,
	nullptr, 0);
// End Registration
PRAGMA_ENABLE_DEPRECATION_WARNINGS
