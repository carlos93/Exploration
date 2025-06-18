// Fill out your copyright notice in the Description page of Project Settings.


#include "FWaitUntilLatent.h"

void UFWaitUntilLatent::UpdateOperation(FLatentResponse& Response)
{
	Response.FinishAndTriggerIf(Wrapper->ShouldFinishWaitUntil, ExecutionFunction, OutputLink, CallbackTarget);
}

void UMyBPFunctionLibrary::WaitUntil(UBooleanWrapper* wrapper, FLatentActionInfo LatentInfo)
{
	if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(wrapper))
	{
		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();

		UFWaitUntilLatent* WaitUntilAction = LatentActionManager.FindExistingAction<UFWaitUntilLatent> (LatentInfo.CallbackTarget, LatentInfo.UUID);

		if (!WaitUntilAction)
		{
			WaitUntilAction = new UFWaitUntilLatent(wrapper, LatentInfo);
			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, WaitUntilAction);
		}
	}
}
