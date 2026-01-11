// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(PRJ_NN, Log, All);

#define LOCAL_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetLocalRole()))
#define REMOTE_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetRemoteRole()))
#define SUBLOCAL_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetLocalRole()))
#define SUBREMOTE_ROLE_INFO *(UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetOwner()->GetRemoteRole()))
#define NETMODE_INFO ((GetNetMode() == ENetMode::NM_Client) ? *FString::Printf(TEXT("CLIENT%d"), GPlayInEditorID) : ((GetNetMode() == ENetMode::NM_Standalone) ? TEXT("STANDALONE") : TEXT("SERVER")))
#define NN_LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)

#define NN_LOG(Verbosity,Format,...) UE_LOG(PRJ_NN,Verbosity,TEXT("%s %s"),NN_LOG_CALLINFO, *FString::Printf(Format,##__VA_ARGS__))
#define NN_ERRER(Expr,...) {if(!(Expr)){NN_LOG(Error,TEXT("ASSERTION : %s"),TEXT("'"#Expr"'"));return __VA_ARGS__;}}
#define NN_NET_LOG(Verbosity,Format,...) UE_LOG(PRJ_NN,Verbosity,TEXT("[%s] [%s/%s] %s %s"),NETMODE_INFO,LOCAL_ROLE_INFO,REMOTE_ROLE_INFO,NN_LOG_CALLINFO, *FString::Printf(Format,##__VA_ARGS__))
#define NN_SUBNET_LOG(Verbosity,Format,...) UE_LOG(PRJ_NN,Verbosity,TEXT("[%s] [%s/%s] %s %s"),NETMODE_INFO,SUBLOCAL_ROLE_INFO,SUBREMOTE_ROLE_INFO,NN_LOG_CALLINFO, *FString::Printf(Format,##__VA_ARGS__))
#define PRINTSTR(Key,Time,Color,Format,...) GEngine->AddOnScreenDebugMessage(Key,Time,Color,FString::FromInt(Key) + FString(TEXT(" : ")) + FString::Printf(Format,##__VA_ARGS__))
#define GET_ENUM_DISPLAY_NAME(EnumType, Value) StaticEnum<EnumType>()->GetDisplayNameTextByValue(static_cast<uint8>(Value)).ToString()

