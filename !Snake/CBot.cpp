#include "CBot.h"

CBot::CBot()
{

}

CBot::~CBot()
{

}

void CBot::BotSnakeInit()
{
	BotStartSnake = new POS;
	BotStartSnake->x = 30;
	BotStartSnake->y = 30;
	BotFinishSnake = BotStartSnake;
	for (int i = 0; i < 2; i++)
	{
		BotFinishSnake->Next = new POS;
		BotFinishSnake->Next->Prev = BotFinishSnake;
		BotFinishSnake = BotFinishSnake->Next;
		BotFinishSnake->x = BotFinishSnake->Prev->x;
		BotFinishSnake->y = BotFinishSnake->Prev->y - 1;
	}
	BotSnakeVect = SNAKE_DOWN;

	TreeFirstNode = new TreeNode;
	BotBuildTree(0, TreeFirstNode);
}

void CBot::BotSnakeAdd()
{
	POS *BUF = new POS;
	BUF->x = BotFinishSnake->x - 1;
	BUF->y = BotFinishSnake->y - 1;
	BotFinishSnake->Next = BUF;
	BUF->Prev = BotFinishSnake;
	BotFinishSnake = BUF;
	CountLuserSteps = 0;
}

void CBot::BotSnakeDelete()
{
	POS *X;
	if (BotStartSnake)
		while (BotStartSnake)
		{
			X = BotStartSnake->Next;
			delete BotStartSnake;
			BotStartSnake = X;
		}
	BotStartSnake = NULL;
	BotFinishSnake = NULL;

	BotDeleteTree(0, TreeFirstNode);
	delete TreeFirstNode;
}

void CBot::BotBuildTree(char height, TreeNode* Node)
{
	if (height == Mind) return;

	Node->ChildNodes[0] = new TreeNode;
	Node->ChildNodes[0]->Parent = Node;
	BotBuildTree(height + 1, Node->ChildNodes[0]);
	Node->ChildNodes[1] = new TreeNode;
	Node->ChildNodes[1]->Parent = Node;
	BotBuildTree(height + 1, Node->ChildNodes[1]);
	Node->ChildNodes[2] = new TreeNode;
	Node->ChildNodes[2]->Parent = Node;
	BotBuildTree(height + 1, Node->ChildNodes[2]);
	Node->ChildNodes[3] = new TreeNode;
	Node->ChildNodes[3]->Parent = Node;
	BotBuildTree(height + 1, Node->ChildNodes[3]);
}

void CBot::BotDeleteTree(char height, TreeNode* Node)
{
	if (Node->ChildNodes[0])
	{
		BotDeleteTree(height, Node->ChildNodes[0]);
		delete Node->ChildNodes[0];
	}
	if (Node->ChildNodes[1])
	{
		BotDeleteTree(height, Node->ChildNodes[1]);
		delete Node->ChildNodes[1];
	}
	if (Node->ChildNodes[2])
	{
		BotDeleteTree(height, Node->ChildNodes[2]);
		delete Node->ChildNodes[2];
	}
	if (Node->ChildNodes[3])
	{
		BotDeleteTree(height, Node->ChildNodes[3]);
		delete Node->ChildNodes[3];
	}

	if (height > Mind) return;
}

void CBot::BotSetWeights(TreeNode* Node, char height, char **Map, POS *FirstFood, POS *StartUserSnake, POS *StartBotSnake)
{
	bool BUMP = false;
	bool KILLED_BY_USER = false;
	bool KILL_USER = false;
	bool USER_KILL_HIMSELF = false;
	bool BOT_KILL_HIMSELF = false;
	bool FOOD = false;

	Node->endlogic = true;
	Node->availability = true;
	Node->weight = 0;

	if (Map[StartBotSnake->x][StartBotSnake->y])
		BUMP = true;
	if (Map[StartUserSnake->x][StartUserSnake->y])
		BUMP = true;
	if (StartBotSnake->x <= 0 || StartBotSnake->y <= 0)
		BUMP = true;
	if (StartBotSnake->x >= 199 || StartBotSnake->y >= 199)
		BUMP = true;
	if (StartUserSnake->x <= 0 || StartUserSnake->y <= 0)
		Node->endlogic = true;
	if (StartUserSnake->x >= 199 || StartUserSnake->y >= 199)
		Node->endlogic = true;

	if (BUMP)
	{
		Node->weight = (-1 + log10(0.1 + height)) * Fear / log10(0.1 + height);
	}
	else
	{
		POS *FirstSn = StartUserSnake->Next;
		while (FirstSn)
		{
			if (FirstSn->x == StartUserSnake->x && FirstSn->y == StartUserSnake->y)
			{
				USER_KILL_HIMSELF = true;
				Node->availability = false;
				break;
			}

			if (FirstSn->x == StartBotSnake->x && FirstSn->y == StartBotSnake->y)
			{
				KILLED_BY_USER = true;
				Node->weight = (-1 + log10(0.1 + height)) * Fear / log10(0.1 + height);
				break;
			}
			FirstSn = FirstSn->Next;
		}

		if (!USER_KILL_HIMSELF && !KILLED_BY_USER)
		{
			POS *FirstSn = StartBotSnake->Next;
			while (FirstSn)
			{
				if (FirstSn->x == StartBotSnake->x && FirstSn->y == StartBotSnake->y)
				{
					BOT_KILL_HIMSELF = true;
					Node->availability = false;
					break;
				}

				if (FirstSn->x == StartUserSnake->x && FirstSn->y == StartUserSnake->y)
				{
					KILL_USER = true;
					Node->weight = (1 - log10(0.1 + height)) * Aggression / log10(0.1 + height);
					break;
				}
				FirstSn = FirstSn->Next;
			}

			if (!KILL_USER && !BOT_KILL_HIMSELF)
			{
				Node->endlogic = false;
				if (height % 2)
				{
					POS *FirstFd = FirstFood;
					while (FirstFd->Next)
					{
						if (FirstFd->Next->x == StartBotSnake->x && FirstFd->Next->y == StartBotSnake->y && FirstFd->Next->active)
						{
							FOOD = true;
							Node->weight = (1 - log10(0.1 + height)) * Angry / log10(0.1 + height);
							break;
						}
						FirstFd = FirstFd->Next;
					}
				}
			}
		}
	}

	if (height == Mind) return;

	if (!Node->endlogic)
		if (height % 2)
		{
			POS *FinishUserSnake = StartUserSnake;
			while (FinishUserSnake->Next)
			{
				FinishUserSnake = FinishUserSnake->Next;
			}
			POS *PreFinishUserSnake = FinishUserSnake->Prev;
			PreFinishUserSnake->Next = NULL;
			FinishUserSnake->Next = StartUserSnake;
			StartUserSnake->Prev = FinishUserSnake;
			FinishUserSnake->Prev = NULL;
			int OldX = FinishUserSnake->x;
			int OldY = FinishUserSnake->y;

			// Рассчет вверх
			FinishUserSnake->x = StartUserSnake->x;
			FinishUserSnake->y = StartUserSnake->y - 1;
			BotSetWeights(Node->ChildNodes[0], height + 1, Map, FirstFood, FinishUserSnake, StartBotSnake);

			// Рассчет вниз
			FinishUserSnake->x = StartUserSnake->x;
			FinishUserSnake->y = StartUserSnake->y + 1;
			BotSetWeights(Node->ChildNodes[1], height + 1, Map, FirstFood, FinishUserSnake, StartBotSnake);

			// Рассчет влево
			FinishUserSnake->x = StartUserSnake->x - 1;
			FinishUserSnake->y = StartUserSnake->y;
			BotSetWeights(Node->ChildNodes[2], height + 1, Map, FirstFood, FinishUserSnake, StartBotSnake);

			// Рассчет вправо
			FinishUserSnake->x = StartUserSnake->x + 1;
			FinishUserSnake->y = StartUserSnake->y;
			BotSetWeights(Node->ChildNodes[3], height + 1, Map, FirstFood, FinishUserSnake, StartBotSnake);

			FinishUserSnake->Next = NULL;
			PreFinishUserSnake->Next = FinishUserSnake;
			StartUserSnake->Prev = NULL;
			FinishUserSnake->Prev = PreFinishUserSnake;
			FinishUserSnake->x = OldX;
			FinishUserSnake->y = OldY;
		}
		else
		{
			POS *FinishBotSnake = StartBotSnake;
			while (FinishBotSnake->Next)
			{
				FinishBotSnake = FinishBotSnake->Next;
			}
			POS *PreFinishBotSnake = FinishBotSnake->Prev;
			PreFinishBotSnake->Next = NULL;
			FinishBotSnake->Next = StartBotSnake;
			StartBotSnake->Prev = FinishBotSnake;
			FinishBotSnake->Prev = NULL;
			int OldX = FinishBotSnake->x;
			int OldY = FinishBotSnake->y;

			// Рассчет вверх
			FinishBotSnake->x = StartBotSnake->x;
			FinishBotSnake->y = StartBotSnake->y - 1;
			BotSetWeights(Node->ChildNodes[0], height + 1, Map, FirstFood, StartUserSnake, FinishBotSnake);

			// Рассчет вниз
			FinishBotSnake->x = StartBotSnake->x;
			FinishBotSnake->y = StartBotSnake->y + 1;
			BotSetWeights(Node->ChildNodes[1], height + 1, Map, FirstFood, StartUserSnake, FinishBotSnake);
			
			// Рассчет влево
			FinishBotSnake->x = StartBotSnake->x - 1;
			FinishBotSnake->y = StartBotSnake->y;
			BotSetWeights(Node->ChildNodes[2], height + 1, Map, FirstFood, StartUserSnake, FinishBotSnake);

			// Рассчет вправо
			FinishBotSnake->x = StartBotSnake->x + 1;
			FinishBotSnake->y = StartBotSnake->y;
			BotSetWeights(Node->ChildNodes[3], height + 1, Map, FirstFood, StartUserSnake, FinishBotSnake);
			
			FinishBotSnake->Next = NULL;
			PreFinishBotSnake->Next = FinishBotSnake;
			StartBotSnake->Prev = NULL;
			FinishBotSnake->Prev = PreFinishBotSnake;
			FinishBotSnake->x = OldX;
			FinishBotSnake->y = OldY;
		}
	else
	{
		for (int i = 0; i < 4; i++)
			Node->ChildNodes[i]->weight = 0;
	}
	return;
}

void CBot::ChooseBotVect(TreeNode* Node, char height)
{
	if (Node->availability && !Node->endlogic)
	{
		if (height == (Mind - 1)) return;

		ChooseBotVect(Node->ChildNodes[0], height + 1);
		ChooseBotVect(Node->ChildNodes[1], height + 1);
		ChooseBotVect(Node->ChildNodes[2], height + 1);
		ChooseBotVect(Node->ChildNodes[3], height + 1);
	}
	float A = Node->weight;
	if (height)
		if (height % 2)
		{
			float max = Node->ChildNodes[0]->weight;
			for (int i = 1; i < 4; i++)
			{
				if (max < Node->ChildNodes[i]->weight)
					max = Node->ChildNodes[i]->weight;
			}
			Node->weight += max;
		}
		else
		{
			Node->weight = Node->ChildNodes[0]->weight;
			for (int i = 1; i < 4; i++)
			{
				Node->weight += Node->ChildNodes[i]->weight;
			}
		}
}

void CBot::MoveBotSnake(char** Map, POS* FirstFood, POS* StartUserSnake)
{
	float min = 1000;
	POS *Food = NULL;
	int OldX = 0, OldY = 0;
	
	POS *LastFd = FirstFood;
	while (LastFd->Next)
	{
		if (LastFd->Next->active)
		{
			float distance = sqrt(pow(BotStartSnake->x - LastFd->Next->x, 2) + pow(BotStartSnake->y - LastFd->Next->y, 2));
			if ((min > distance) && (distance < 80))
			{
				Food = LastFd->Next;
				min = distance;
				CountLuserSteps = 0;
			}
		}
		LastFd = LastFd->Next;
	}
	
	if (Food)
	{
		OldX = Food->x;
		OldY = Food->y;

		if (Food->x > (BotStartSnake->x + 2))
			Food->x = BotStartSnake->x + 2;
		if (Food->x < (BotStartSnake->x - 2))
			Food->x = BotStartSnake->x - 2;
		if (Food->y > (BotStartSnake->y + 2))
			Food->y = BotStartSnake->y + 2;
		if (Food->y < (BotStartSnake->y - 2))
			Food->y = BotStartSnake->y - 2;
	}

	BotSetWeights(TreeFirstNode, 0, Map, FirstFood, StartUserSnake, BotStartSnake);
	ChooseBotVect(TreeFirstNode, 0);
	
	if (Food)
	{
		Food->x = OldX;
		Food->y = OldY;
	}

	float max = 0;

	CountLuserSteps++;
	if (CountLuserSteps > MaxCountLuserSteps)
	{

		switch (BotSnakeVect)
		{
		case SNAKE_UP:
			BotSnakeVect = (SNAKE_DOWN + 1 + rand() % 3) % 4;
			break;
		case SNAKE_DOWN:
			BotSnakeVect = (SNAKE_UP + 1 + rand() % 3) % 4;
			break;
		case SNAKE_LEFT:
			BotSnakeVect = (SNAKE_RIGTH + 1 + rand() % 3) % 4;
			break;
		case SNAKE_RIGTH:
			BotSnakeVect = (SNAKE_LEFT + 1 + rand() % 3) % 4;
			break;
		}
		
		CountLuserSteps = 0;
		max = TreeFirstNode->ChildNodes[BotSnakeVect]->weight;
	}
	else
		max = TreeFirstNode->ChildNodes[BotSnakeVect]->weight;

	for(int i = 0; i < 4; i++)
		if (TreeFirstNode->ChildNodes[i]->availability && (TreeFirstNode->ChildNodes[i]->weight > max))
		{
			max = TreeFirstNode->ChildNodes[i]->weight;
			BotSnakeVect = i;
		}
	

	POS *LastSn = BotFinishSnake;

	while (LastSn->Prev)
	{
		LastSn->x = LastSn->Prev->x;
		LastSn->y = LastSn->Prev->y;
		LastSn = LastSn->Prev;
	}

	switch (BotSnakeVect)
	{
	case SNAKE_UP:
		BotStartSnake->y--;
		break;
	case SNAKE_DOWN:
		BotStartSnake->y++;
		break;
	case SNAKE_LEFT:
		BotStartSnake->x--;
		break;
	case SNAKE_RIGTH:
		BotStartSnake->x++;
		break;
	}
}