#pragma once

#include "Libs.h"

struct TreeNode
{
	float weight = 0;
	bool availability = true;
	bool endlogic = false;
	TreeNode *ChildNodes[4] = { NULL, NULL, NULL, NULL };
	TreeNode *Parent = NULL;
};

class CBot
{
public:
	CBot();
	~CBot();
public:
	POS *BotStartSnake = NULL;
	POS *BotFinishSnake = NULL;
	TreeNode *TreeFirstNode = NULL;
	char BotSnakeVect = SNAKE_DOWN;
	int Aggression = 150;
	int Fear = 100;
	int Angry = 120;
	char Mind = 8;
	char CountLuserSteps = 0;
	char MaxCountLuserSteps = 40;
public:
	void BotSnakeInit();
	void BotSnakeAdd();
	void BotSnakeDelete();
public:
	void BotBuildTree(char height, TreeNode* Node);
	void BotDeleteTree(char height, TreeNode* Node);
	void BotSetWeights(TreeNode* Node, char height, char **Map, POS *FirstFood, POS *StartUserSnake, POS *StartBotSnake);
	void ChooseBotVect(TreeNode* Node, char height);
public:
	void MoveBotSnake(char** Map, POS* FirstFood, POS* StartUserSnake);
};