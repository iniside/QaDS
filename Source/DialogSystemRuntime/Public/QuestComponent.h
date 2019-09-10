// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "Quest/QuestNode.h"
#include "QuestAsset.h"
#include "QuestComponent.generated.h"

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FQuestItemNode
{
	GENERATED_BODY()
private:
	TArray<FQuestItemNode> CachedChilds;
public:
	friend class UQuestComponent;
	UPROPERTY()
	FName Id;
	
	UPROPERTY()
	EQuestCompleteStatus Status;

	UPROPERTY()
	class UQuestProcessor* Processor;

	UPROPERTY(BlueprintReadOnly)
	UQuestAsset* Asset;

	UPROPERTY()
	TArray<FGuid> Childs;

	UPROPERTY(BlueprintReadOnly)
	FQuestStageInfo Stage;

	UPROPERTY()
	class UQuestAsset* OwnerQuest;


	struct FQuestItem& GetOwner(class UQuestComponent* Owner);
	
	FQuestItemNode LoadNode(FGuid Uid)
	{
		FQuestItemNode Node;
		Childs = Asset->Joins[Uid].UIDs;
		Stage = Asset->Nodes[Uid];
		return Node;
	}

	bool CkeckForActivate(class UQuestComponent* Owner) const
	{
		//if (Processor->StoryKeyManager->DontHasKey(Stage.CheckHasKeys))
		//	return false;
		//
		//if (Processor->StoryKeyManager->HasKey(Stage.CheckDontHasKeys))
		//	return false;

		for (auto& Conditions : Stage.Predicate)
		{
			//if (!Conditions.InvokeCheck(this))
			//	return false;
		}

		return true;
	}
	
	TArray<FQuestItemNode> GetNextStage(class UQuestComponent* Owner)
	{
		if (CachedChilds.Num() == 0)
		{
			for (auto child : Childs)
			{
				CachedChilds.Add(LoadNode(child));
			}
		}

		return CachedChilds.FilterByPredicate([=](const FQuestItemNode& node)
			{
				return node.CkeckForActivate(Owner);
			});
	}


	void Activate()
	{
		//OwnerQuest->ActiveNodes.Add(this);
		//
		//if (TryComplete())
		//	return;
		//
		//if (Stage.WaitHasKeys.Num() > 0 ||
		//	Stage.WaitDontHasKeys.Num() > 0 ||
		//	Stage.FailedIfGiveKeys.Num() > 0 ||
		//	Stage.FailedIfRemoveKeys.Num() > 0)
		//{
		//	Processor->StoryKeyManager->OnKeyRemoveBP.AddDynamic(this, &UQuestRuntimeNode::OnChangeStoryKey);
		//	Processor->StoryKeyManager->OnKeyAddBP.AddDynamic(this, &UQuestRuntimeNode::OnChangeStoryKey);
		//}
		//
		//if (Stage.FailedTriggers.Num() > 0 || Stage.WaitTriggers.Num() > 0)
		//{
		//	Processor->StoryTriggerManager->OnTriggerInvoke.AddDynamic(this, &UQuestRuntimeNode::OnTrigger);
		//}
	}

	void Failed()
	{
		//if (Stage.bFailedQuest || OwnerQuest->ActiveNodes.Num() == 1)
		//{
		//	Processor->EndQuest(OwnerQuest, EQuestCompleteStatus::Failed);
		//}
	}

	void Complete()
	{
		//if (Stage.ChangeOderActiveStagesState != EQuestCompleteStatus::None)
		//{
		//	auto nodes = OwnerQuest->ActiveNodes;
		//	for (auto stage : nodes)
		//	{
		//		if (stage == this)
		//			continue;
		//
		//		stage->SetStatus(Stage.ChangeOderActiveStagesState);
		//	}
		//}
		//
		//if (Stage.ChangeQuestState != EQuestCompleteStatus::None)
		//{
		//	Processor->EndQuest(OwnerQuest, Stage.ChangeQuestState);
		//}
		//
		//for (auto& Event : Stage.Action)
		//	Event.Invoke(this);
	}

	void Deactivate()
	{
		//OwnerQuest->ActiveNodes.Remove(this);
		//OwnerQuest->ArchiveNodes.Add(this);
		//
		//Processor->CompleteStage(this);
		//
		//Processor->StoryKeyManager->OnKeyRemoveBP.RemoveDynamic(this, &UQuestRuntimeNode::OnChangeStoryKey);
		//Processor->StoryKeyManager->OnKeyAddBP.RemoveDynamic(this, &UQuestRuntimeNode::OnChangeStoryKey);
		//Processor->StoryTriggerManager->OnTriggerInvoke.RemoveDynamic(this, &UQuestRuntimeNode::OnTrigger);
	}
	
	void SetStatus(EQuestCompleteStatus NewStatus)
	{
		if (NewStatus == Status)
			return;

		Status = NewStatus;

		switch (Status)
		{
		case EQuestCompleteStatus::None:
			break;
		case EQuestCompleteStatus::Active:
			Activate();
			break;
		case EQuestCompleteStatus::Completed:
			Complete();
			Deactivate();
			break;
		case EQuestCompleteStatus::Failed:
			Failed();
			Deactivate();
			break;
		case EQuestCompleteStatus::Skiped:
			Deactivate();
			break;
		}
	}

	bool CkeckForComplete(UQuestComponent* Owner)
	{
		unimplemented();
		//for (auto& cond : Stage.WaitTriggers)
		//{
		//	if (cond.TotalCount != 0)
		//		return false;
		//}
		//
		//if (Processor->StoryKeyManager->DontHasKey(Stage.WaitHasKeys))
		//	return false;
		//
		//if (Processor->StoryKeyManager->HasKey(Stage.WaitDontHasKeys))
		//	return false;
		//
		//for (auto& Conditions : Stage.WaitPredicate)
		//{
		//	if (!Conditions.InvokeCheck(this))
		//		return false;
		//}
		//
		return true;
	}

	bool CkeckForFailed(UQuestComponent* Owner)
	{
		unimplemented();
		//for (auto& cond : Stage.FailedTriggers)
		//{
		//	if (cond.TotalCount == 0)
		//		return true;
		//}
		//
		//if (Processor->StoryKeyManager->HasKey(Stage.FailedIfGiveKeys))
		//	return true;
		//
		//
		//if (Processor->StoryKeyManager->DontHasKey(Stage.FailedIfRemoveKeys))
		//	return true;
		//
		//
		//for (auto& Conditions : Stage.FailedPredicate)
		//{
		//	if (Conditions.InvokeCheck(this))
		//		return true;
		//}
		//
		return false;
	}

	
	bool TryComplete(UQuestComponent* Owner)
	{
		if (CkeckForFailed(Owner))
		{
			SetStatus(EQuestCompleteStatus::Failed);
		}
		else
		{
			if (!CkeckForComplete(Owner))
				return false;

			SetStatus(EQuestCompleteStatus::Completed);
		}
		return true;
	}
};

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FQuestItem
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	EQuestCompleteStatus Status;

	UPROPERTY(BlueprintReadOnly)
	TArray<FQuestItemNode> ArchiveNodes;

	UPROPERTY(BlueprintReadOnly)
	TArray<FQuestItemNode> ActiveNodes;

	UPROPERTY(BlueprintReadOnly)
	AQuestScript* Script;

	UPROPERTY(BlueprintReadOnly)
	UQuestAsset* Asset;

	FQuestItemNode LoadNode(FGuid Uid)
	{
		FQuestItemNode Node;
		Node.Childs = Asset->Joins[Uid].UIDs;
		Node.Stage = Asset->Nodes[Uid];
		Node.OwnerQuest = Asset;
		return Node;
	}
};

class UQuestRuntimeAsset;
class UQuestRuntimeNode;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIALOGSYSTEMRUNTIME_API UQuestComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	friend struct FQuestItemNode;
protected:
	UPROPERTY()
	FGameplayTagContainer QuestTags;

	UPROPERTY()
	TMap<FName, FQuestItem> ActiveQuests;
	
	//TArray<TSoftObjectPtr<class UQuestAsset>> ActiveQuests;
	TArray<TSoftObjectPtr<class UQuestAsset>> SucceededQuests;
	TArray<TSoftObjectPtr<class UQuestAsset>> FailedQuests;
public:	
	// Sets default values for this component's properties
	UQuestComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	void StartQuest(TAssetPtr<UQuestAsset> QuestAsset);

	void CompleteStage(UQuestRuntimeNode* Stage);
	void WaitStage(FQuestItemNode& Stage);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	void EndQuest(UQuestAsset* Quest, EQuestCompleteStatus QuestStatus);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	TArray<UQuestRuntimeAsset*> GetQuests(EQuestCompleteStatus FilterStatus) const;
};
