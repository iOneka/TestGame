// Copyright June Rhodes 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"
#include "Delegates/DelegateCombinations.h"
#include "GameFramework/OnlineReplStructs.h"
#include "Engine/GameInstance.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Types/OSBOnlineUserRef.h"
#include "Types/OSBFriendTypes.h"
#include "Types/OSBOnlineAccountCredential.h"
#include "Types/OSBOnlineErrorInfo.h"
#include "Types/OSBOnlineRecentPlayerRef.h"
#include "Types/OSBOnlineLoginStatus.h"
#include "Types/OSBBlockedQueryResultInfo.h"
#include "Types/OSBOnlineUserPrivilege.h"
#include "Types/OSBMessageTypes.h"
#include "Types/OSBChatTypes.h"
#include "Types/OSBExternalUITypes.h"
#include "Types/OSBFileTypes.h"
#include "Types/OSBPartyTypes.h"
#include "Types/OSBLobbyTypes.h"
#include "Types/OSBSessionTypes.h"
#include "Types/OSBLeaderboardTypes.h"
#include "Types/OSBStatsTypes.h"
#include "Types/OSBStoreV2Types.h"
#include "Types/OSBPurchaseTypes.h"
#include "Types/OSBEntitlementTypes.h"
#include "Types/OSBAchievementTypes.h"
#include "Types/OSBVoiceChatTypes.h"
#include "Types/OSBGameActivityTypes.h"
#include "Helpers/ArrayConversion.h"
#include "Helpers/UniqueNetIdConversion.h"
#include "Interfaces/OnlineMessageInterface.h"

#include "OnlineMessageSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMessage_EnumerateMessagesComplete_BP, int32, LocalUserNum, bool, Param1, FString, Param2);

// @parameters-not-compatible
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FMessage_ReadMessageComplete_BP, int32, LocalUserNum, bool, Param1, const FUniqueMessageId&, Param2, const FString&, Param3);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FMessage_SendMessageComplete_BP, int32, LocalUserNum, bool, Param1, FString, Param2);

// @parameters-not-compatible
// DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FMessage_DeleteMessageComplete_BP, int32, LocalUserNum, bool, Param1, const FUniqueMessageId&, Param2, const FString&, Param3);

UCLASS(BlueprintType)
class ONLINESUBSYSTEMBLUEPRINTS_API UOnlineMessageSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    friend class UOnlineHelpers;

private:
    struct FWorldContext* WorldContext;
    TWeakPtr<class IOnlineMessage, ESPMode::ThreadSafe> HandleWk;
    TSharedPtr<class IOnlineMessage, ESPMode::ThreadSafe> GetHandle();
    bool IsHandleValid(const TSharedPtr<class IOnlineMessage, ESPMode::ThreadSafe>& InHandle);
    TArray<FDelegateHandle> DelegateHandle_OnEnumerateMessagesComplete;
    TArray<FDelegateHandle> DelegateHandle_OnSendMessageComplete;

public:
    // When async functions use global events as callbacks (such as in identity and session),
    // we need to prevent multiple requests running at the same time where those results would
    // have the same identifying data in the callback (because then we don't know which result
    // belongs to which request). For these types of async functions, they use this set to make
    // sure that only one unique request runs at a time.
    TSet<FString> __InFlightRequests;

    static FName OSSNameToAssignDuringConstruction;
    FName OSSName = NAME_Default;
    UOnlineMessageSubsystem();
    virtual void BeginDestroy() override;

    UFUNCTION(BlueprintPure, Category = "Online|Message")
    bool IsSubsystemAvailable();

    UPROPERTY(BlueprintAssignable, Category = "Online|Message")
    FMessage_EnumerateMessagesComplete_BP OnEnumerateMessagesComplete;

    UPROPERTY(BlueprintAssignable, Category = "Online|Message")
    FMessage_SendMessageComplete_BP OnSendMessageComplete;

    UFUNCTION(BlueprintCallable, Category = "Online|Message")
    bool EnumerateMessages(int32 LocalUserNum);

    // @generator-incompatible
    // bool GetMessageHeaders(int32 LocalUserNum, TArray<TSharedRef<class FOnlineMessageHeader>>& OutHeaders);

    UFUNCTION(BlueprintCallable, Category = "Online|Message")
    bool ClearMessageHeaders(int32 LocalUserNum);

    // @generator-incompatible
    // bool ReadMessage(int32 LocalUserNum, const FUniqueMessageId& MessageId);

    // @generator-incompatible
    // TSharedPtr<class FOnlineMessage> GetMessage(int32 LocalUserNum, const FUniqueMessageId& MessageId);

    // @generator-incompatible
    // bool ClearMessage(int32 LocalUserNum, const FUniqueMessageId& MessageId);

    UFUNCTION(BlueprintCallable, Category = "Online|Message")
    bool ClearMessages(int32 LocalUserNum);

    // @generator-incompatible
    // bool SendMessage(int32 LocalUserNum, const TArray<TSharedRef<const FUniqueNetId>>& RecipientIds, const FString& MessageType, const FOnlineMessagePayload& Payload);

    // @generator-incompatible
    // bool DeleteMessage(int32 LocalUserNum, const FUniqueMessageId& MessageId);
};