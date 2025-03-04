// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LatentActionManager.h"
#include "LatentActions.h"
#include "FWaitUntilLatent.generated.h"

/**
 * Signatures of execution pins in the editor
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBPNodeOutputPin);

UCLASS(BlueprintType)
class EXPLORATION_API UBooleanWrapper : public UObject
{
	GENERATED_BODY()

	UBooleanWrapper() : ShouldFinishWaitUntil(false) {};

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldFinishWaitUntil;
};

/**
 * 
 */
class UFWaitUntilLatent : public FPendingLatentAction
{
private:
	UBooleanWrapper* Wrapper;

public:
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;
	
	UFWaitUntilLatent(UBooleanWrapper* wrapper, const FLatentActionInfo& LatentInfo)
	: Wrapper(wrapper)
	, ExecutionFunction(LatentInfo.ExecutionFunction)
	, OutputLink(LatentInfo.Linkage)
	, CallbackTarget(LatentInfo.CallbackTarget)
{
}

	virtual void UpdateOperation(FLatentResponse& Response) override;

#if WITH_EDITOR
	// Returns a human readable description of the latent operation's current state
	virtual FString GetDescription() const override
	{
		return FString("Delays execution until conditions are met");
	}
#endif
};

UCLASS()
class EXPLORATION_API UMyBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContextObject"), Category = "MyBPFunctions")
	static void WaitUntil(UBooleanWrapper* wrapper, FLatentActionInfo LatentInfo);

};
