// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "RTSPStreamWidget.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef RTSPTOVIDEO_RTSPStreamWidget_generated_h
#error "RTSPStreamWidget.generated.h already included, missing '#pragma once' in RTSPStreamWidget.h"
#endif
#define RTSPTOVIDEO_RTSPStreamWidget_generated_h

#define FID_arth_Plugins_RTSPToVideo_Source_RTSPToVideo_Public_RTSPStreamWidget_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execStopStream); \
	DECLARE_FUNCTION(execStartStream);


#define FID_arth_Plugins_RTSPToVideo_Source_RTSPToVideo_Public_RTSPStreamWidget_h_13_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesURTSPStreamWidget(); \
	friend struct Z_Construct_UClass_URTSPStreamWidget_Statics; \
public: \
	DECLARE_CLASS(URTSPStreamWidget, UUserWidget, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/RTSPToVideo"), NO_API) \
	DECLARE_SERIALIZER(URTSPStreamWidget)


#define FID_arth_Plugins_RTSPToVideo_Source_RTSPToVideo_Public_RTSPStreamWidget_h_13_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API URTSPStreamWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	URTSPStreamWidget(URTSPStreamWidget&&); \
	URTSPStreamWidget(const URTSPStreamWidget&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, URTSPStreamWidget); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(URTSPStreamWidget); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(URTSPStreamWidget) \
	NO_API virtual ~URTSPStreamWidget();


#define FID_arth_Plugins_RTSPToVideo_Source_RTSPToVideo_Public_RTSPStreamWidget_h_10_PROLOG
#define FID_arth_Plugins_RTSPToVideo_Source_RTSPToVideo_Public_RTSPStreamWidget_h_13_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_arth_Plugins_RTSPToVideo_Source_RTSPToVideo_Public_RTSPStreamWidget_h_13_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_arth_Plugins_RTSPToVideo_Source_RTSPToVideo_Public_RTSPStreamWidget_h_13_INCLASS_NO_PURE_DECLS \
	FID_arth_Plugins_RTSPToVideo_Source_RTSPToVideo_Public_RTSPStreamWidget_h_13_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> RTSPTOVIDEO_API UClass* StaticClass<class URTSPStreamWidget>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_arth_Plugins_RTSPToVideo_Source_RTSPToVideo_Public_RTSPStreamWidget_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
