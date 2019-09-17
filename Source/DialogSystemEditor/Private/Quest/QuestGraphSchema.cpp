#include "DialogSystemEditor.h"
#include "QuestEditorNodes.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphPin.h"
#include "GraphEditorSettings.h"
#include "GenericCommands.h"
#include "GraphEditorActions.h"
#include "RectConnectionDrawingPolicy.h"
#include "QuestGraphSchema.h"
#include "QaDSGraphSchema.h"
#include "Runtime/Slate/Public/Framework/MultiBox/MultiBoxBuilder.h"
#include "QuestEditorNodes.h"
#if ENGINE_MINOR_VERSION > 23
#include "ToolMenuEntry.h"
#include "ToolMenu.h"
#endif
#include "DialogEditorNodes.h"

#define LOCTEXT_NAMESPACE "FQuestSystemModule"

#if ENGINE_MINOR_VERSION < 24
void UQuestGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	auto FromPin = ContextMenuBuilder.FromPin;
	auto Graph = ContextMenuBuilder.CurrentGraph;
	auto OwnerOfTemp = ContextMenuBuilder.OwnerOfTemporaries;

	bool rootFound = false;
	for (auto node : Graph->Nodes)
	{
		if (Cast<UQuestRootEdGraphNode>(node))
		{
			rootFound = true;
			break;
		}
	}

	TArray<TSharedPtr<FEdGraphSchemaAction>> Actions;

	if (!rootFound)
		QaDSSchemaUtils::AddAction<UQuestRootEdGraphNode>(TEXT("Create Root Node"), TEXT(""), Actions, OwnerOfTemp);

	QaDSSchemaUtils::AddAction<UQuestStageEdGraphNode>(TEXT("Add Stage"), TEXT(""), Actions, OwnerOfTemp);

	for (TSharedPtr<FEdGraphSchemaAction> Action : Actions)
		ContextMenuBuilder.AddAction(Action);
}

void UQuestGraphSchema::GetContextMenuActions(const UEdGraph* CurrentGraph, const UEdGraphNode* InGraphNode, const UEdGraphPin* InGraphPin, FMenuBuilder* MenuBuilder, bool bIsDebugging) const
{
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Delete);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Cut);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Copy);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().Paste);
	MenuBuilder->AddMenuEntry(FGenericCommands::Get().SelectAll);

	if (!Cast<UDialogRootEdGraphNode>(InGraphNode))
		MenuBuilder->AddMenuEntry(FGenericCommands::Get().Duplicate);

	if (InGraphPin)
		MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().BreakPinLinks);
	else
		MenuBuilder->AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);

	Super::GetContextMenuActions(CurrentGraph, InGraphNode, InGraphPin, MenuBuilder, bIsDebugging);
}
#else
void UQuestGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	auto Graph = ContextMenuBuilder.CurrentGraph;
	auto OwnerOfTemp = ContextMenuBuilder.OwnerOfTemporaries;

	bool rootFound = false;
	for (auto node : Graph->Nodes)
	{
		if (Cast<UQuestRootEdGraphNode>(node))
		{
			rootFound = true;
			break;
		}
	}

	TArray<TSharedPtr<FEdGraphSchemaAction>> Actions;

	if (!rootFound)
		QaDSSchemaUtils::AddAction<UQuestRootEdGraphNode>(TEXT("Create Root Node"), TEXT(""), Actions, OwnerOfTemp);

	QaDSSchemaUtils::AddAction<UQuestStageEdGraphNode>(TEXT("Add Stage"), TEXT(""), Actions, OwnerOfTemp);

	for (TSharedPtr<FEdGraphSchemaAction> Action : Actions)
		ContextMenuBuilder.AddAction(Action);
}
void UQuestGraphSchema::GetContextMenuActions(class UToolMenu* Menu, class UGraphNodeContextMenuContext* Context) const
{
	Menu->AddMenuEntry("Base", FToolMenuEntry::InitMenuEntry(FGenericCommands::Get().Delete));
	Menu->AddMenuEntry("Base", FToolMenuEntry::InitMenuEntry(FGenericCommands::Get().Cut));
	Menu->AddMenuEntry("Base", FToolMenuEntry::InitMenuEntry(FGenericCommands::Get().Copy));
	Menu->AddMenuEntry("Base", FToolMenuEntry::InitMenuEntry(FGenericCommands::Get().Paste));
	Menu->AddMenuEntry("Base", FToolMenuEntry::InitMenuEntry(FGenericCommands::Get().SelectAll));

	if (!Cast<UDialogRootEdGraphNode>(Context->Node))
		Menu->AddMenuEntry("Base", FToolMenuEntry::InitMenuEntry(FGenericCommands::Get().Duplicate));

	if (Context->Pin)
		Menu->AddMenuEntry("Graph", FToolMenuEntry::InitMenuEntry(FGraphEditorCommands::Get().BreakPinLinks));
	else
		Menu->AddMenuEntry("Graph", FToolMenuEntry::InitMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks));

	//Super::GetContextMenuActions(CurrentGraph, InGraphNode, InGraphPin, MenuBuilder, bIsDebugging);
}
#endif
#undef LOCTEXT_NAMESPACE