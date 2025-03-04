// Fill out your copyright notice in the Description page of Project Settings.


#include "FWaitUntilLatent.h"
//
//bool UFWaitUntilLatent::bActive = false;
//
//void UFWaitUntilLatent::InternalCompleted()
//{
//	if (WorldContext)
//	{
//		WorldContext->GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
//		TimerHandle.Invalidate();
//		Completed.Broadcast();
//		UFWaitUntilLatent::bActive = false;
//	}
//}
//
//void UFWaitUntilLatent::InternalTick()
//{
//	if (ConditionToCheck->ShouldFinishWaitUntil)
//	{
//		InternalCompleted();
//	}
//}
//
//UFWaitUntilLatent* UFWaitUntilLatent::WaitUntil(const UObject* WorldContextObj, UBooleanWrapper* wrapper)
//{
//	UFWaitUntilLatent* Node = NewObject<UFWaitUntilLatent>();
//	if (Node)
//	{
//		Node->WorldContext = WorldContextObj;
//		Node->ConditionToCheck = wrapper;
//	}
//	return Node;
//}
//
//void UFWaitUntilLatent::Activate()
//{
//	if (UFWaitUntilLatent::bActive)
//	{
//		FFrame::KismetExecutionMessage(TEXT("Async action is already running"), ELogVerbosity::Warning);
//		return;
//	}
//
//	FFrame::KismetExecutionMessage(TEXT("Started Activate!"), ELogVerbosity::Log);
//
//	if (WorldContext)
//	{
//		FTimerDelegate TimerDelegate;
//		TimerDelegate.BindUObject(this, &UFWaitUntilLatent::InternalTick);
//		WorldContext->GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 0.1f, true);
//	}
//	else
//	{
//		FFrame::KismetExecutionMessage(TEXT("Invalid world context obj"), ELogVerbosity::Error);
//	}
//
//}


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