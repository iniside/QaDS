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
	FGuid UUID;
	
	UPROPERTY()
	FName Id;
	
	UPROPERTY()
	EQuestCompleteStatus Status;

	UPROPERTY()
	class UQuestProcessor* Processor;

	UPROPERTY()
	TArray<FGuid> Childs;

	UPROPERTY()
	class UQuestAsset* QuestAsset;

	const FQuestStageInfo& GetStage() const
	{
		return QuestAsset->Nodes[UUID];
	}
	
	bool operator==(const FQuestItemNode& Rhs)
	{
		return Id == Rhs.Id;
	}
	
	bool operator==(const FQuestItemNode* Rhs)
	{
		return Id == Rhs->Id;
	}
	struct FQuestItem& GetOwner(class UQuestComponent* Owner);
	
	FQuestItemNode LoadNode(FGuid Uid)
	{
		FQuestItemNode Node;
		Node.QuestAsset = QuestAsset;
		Node.Childs = QuestAsset->Joins[Uid].UIDs;
		Node.UUID = Uid;
		return Node;
	}

	bool CkeckForActivate(class UQuestComponent* Owner) const;

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
		Node.UUID = Uid;
		Node.QuestAsset = Asset;
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
	FGameplayTagContainer QuestTags;

	UPROPERTY(SaveGame)
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

	void CompleteStage(FQuestItemNode& StageNode);
	void WaitStage(FQuestItemNode& StageNode);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	void EndQuest(UQuestAsset* Quest, EQuestCompleteStatus QuestStatus);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Quest")
	TArray<UQuestRuntimeAsset*> GetQuests(EQuestCompleteStatus FilterStatus) const;
};
