// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayEffectTypes.h"
#include "Quest/QuestNode.h"
#include "QuestAsset.h"
#include "QuestTypes.h"
#include "QuestComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FQuestOnQuestTagsChanged, const FGameplayTagContainer&);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestOnQuestEnd, class UQuestAsset*, Quest, EQuestCompleteStatus, Status);

DECLARE_MULTICAST_DELEGATE_OneParam(FQuestStoryTriggerInvokeSignature, const FStoryTrigger&);

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FQuestItemNode
{
	GENERATED_BODY()
private:
	TArray<TSharedPtr<FQuestItemNode>> CachedChilds;
	FDelegateHandle TagsAddedHandle;
	FDelegateHandle TagsRemovedHandle;
public:
	TWeakObjectPtr<class UQuestComponent> OwnerQC;
	friend class UQuestComponent;

	UPROPERTY()
	FGuid UUID;
	
	UPROPERTY()
	EQuestCompleteStatus Status;

	UPROPERTY()
	TArray<FGuid> Childs;

	UPROPERTY()
	TWeakObjectPtr<UQuestAsset> QuestAsset;

	const FQuestStageInfo& GetStage() const
	{
		return QuestAsset->Nodes[UUID];
	}
	
	bool operator==(const FQuestItemNode& Rhs) const
	{
		return UUID == Rhs.UUID;
	}
	
	bool operator==(const FQuestItemNode* Rhs) const
	{
		return UUID == Rhs->UUID;
	}
	struct FQuestItem& GetOwner(class UQuestComponent* Owner);
	
	TSharedPtr<FQuestItemNode> LoadNode(FGuid Uid, UQuestComponent* InOwner)
	{
		TSharedPtr<FQuestItemNode> Node = MakeShareable(new FQuestItemNode);
		Node->QuestAsset = QuestAsset;
		Node->Childs = QuestAsset->Joins[Uid].UIDs;
		Node->UUID = Uid;
		Node->OwnerQC = OwnerQC;
		return Node;
	}

	bool CkeckForActivate(class UQuestComponent* Owner) const;
	void OnTrigger(const FStoryTrigger& Trigger);
	bool MatchTringger(const FStoryTriggerCondition& condition, const FStoryTrigger& trigger);
	bool MatchTringgerParam(const FString& value, const FString& filter);
	void OnChangeStoryKey(const FGameplayTagContainer& key);

	TArray<TSharedPtr<FQuestItemNode>> GetNextStage(class UQuestComponent* Owner)
	{
		if (CachedChilds.Num() == 0)
		{
			for (auto child : Childs)
			{
				CachedChilds.Add(LoadNode(child, OwnerQC.Get()));
			}
		}
		return CachedChilds.FilterByPredicate([=](const TSharedPtr<FQuestItemNode>& node)
			{
				return node->CkeckForActivate(Owner);
			});
	}


	void Activate(class UQuestComponent* Owner);

	void Failed(class UQuestComponent* Owner);

	void Complete(class UQuestComponent* Owner);

	void Deactivate(class UQuestComponent* Owner);

	void SetStatus(EQuestCompleteStatus NewStatus, UQuestComponent* Owner);

	bool CkeckForComplete(UQuestComponent* Owner);

	bool CkeckForFailed(UQuestComponent* Owner);
	
	bool TryComplete(UQuestComponent* Owner);
};

USTRUCT(BlueprintType)
struct DIALOGSYSTEMRUNTIME_API FQuestItem
{
	GENERATED_BODY()
public:
	TSharedPtr<FQuestItemNode> RootNode;

	FQuestItemNode RootNode2;

	UPROPERTY(BlueprintReadOnly)
	EQuestCompleteStatus Status;

	UPROPERTY(BlueprintReadOnly)
	TArray<FQuestItemNode> ArchiveNodes;

	UPROPERTY(BlueprintReadOnly)
	TArray<FQuestItemNode> ActiveNodes;

	UPROPERTY(BlueprintReadOnly)
	AQuestScript* Script;

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UQuestAsset> Asset;

	FQuestItem()
		: Status(EQuestCompleteStatus::None)
		, Script(nullptr)
		, Asset(nullptr)
	{}

	TSharedPtr<FQuestItemNode> LoadNode(FGuid Uid, class UQuestComponent* InOwner)
	{
		TSharedPtr<FQuestItemNode> Node = MakeShareable(new FQuestItemNode);
		Node->Childs = Asset->Joins[Uid].UIDs;
		Node->UUID = Uid;
		Node->QuestAsset = Asset;
		Node->OwnerQC = InOwner;

		RootNode = Node;
		RootNode2 = *Node.Get();
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
	UPROPERTY(SaveGame)
	FQuestGameplayTagCountContainer QuestTags;
	
	UPROPERTY(SaveGame)
	TMap<FName, FQuestItem> ActiveQuests;
	
	//TArray<TSoftObjectPtr<class UQuestAsset>> ActiveQuests;
	TArray<TSoftObjectPtr<class UQuestAsset>> SucceededQuests;
	TArray<TSoftObjectPtr<class UQuestAsset>> FailedQuests;


	FQuestOnQuestTagsChanged OnTagsAdded;
	FQuestOnQuestTagsChanged OnTagsRemoved;

	FQuestStoryTriggerInvokeSignature OnTriggerInvoke;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FQuestOnQuestEnd OnQuestEnd;
	
public:	
	// Sets default values for this component's properties
	UQuestComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddQuestTags(const FGameplayTagContainer& InTags);
	void RemoveQuestTags(const FGameplayTagContainer& InTags);
	
	bool HasAllTags(const FGameplayTagContainer& InTags) const
	{
		return QuestTags.HasAllMatchingGameplayTags(InTags);
	}

	bool NotHaveAllTags(const FGameplayTagContainer& InTags) const
	{
		return !QuestTags.HasAllMatchingGameplayTags(InTags);
	}

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	void StartQuest(TAssetPtr<UQuestAsset> QuestAsset);

	void CompleteStage(
		TSharedPtr<FQuestItemNode> StageNode);
	void WaitStage(
		TSharedPtr<FQuestItemNode> StageNode);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	void EndQuest(UQuestAsset* Quest, EQuestCompleteStatus QuestStatus);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	TArray<UQuestRuntimeAsset*> GetQuests(EQuestCompleteStatus FilterStatus) const;

	void InvokeTrigger(const FStoryTrigger& InTrigger);
};
