// CrazyTanks.cpp : Vasilevskiy Sergey
//wasilewsky@ukr.net
 
#include "stdafx.h"

#include <vector>
#include <iostream>
#include <conio.h>
#include <windows.h>
#include <ctime>
#include <iomanip>
#include <stdio.h>


using namespace std;
// field size
const unsigned int DIM_X = 30;
const unsigned int DIM_Y = 30;
//number of enemies
const unsigned int NUMBER_OF_ENEMY = 5;
//number of walls
const unsigned int NUMBER_OF_WALL = 10;
//maximum length of walls
const unsigned int MAX_LENGTH_OF_WALL = 10;
//game options   chance 1/n
const unsigned int CHANCE_CHANGE_DIRECRION = 2;
const unsigned int CHANCE_ENEMY_MOVE = 2;
const unsigned int CHANCE_SHOOT_ENEMY = 10;



class GameObject
{
public:
	int x, y; // coorditante 
	int direction; // 0 -north; 1 - south ; 2 - east; 3 - west
	int length;
	int gameObjId; // 0- empty, 1- wall, 2 - my tank, 3 -enemy tank , 4 - my bullet, 5 enemy bullet

	GameObject(int, int, int, int);
	GameObject();

	void generatePosition(int maxX, int maxY, int direction, int length);

	void move()
	{

		switch (direction)
		{
		case 0:
		{
			y--;
			break;
		}

		case 1:
		{
			y++;
			break;
		}
		case 2:
		{
			x++;
			break;
		}

		case 3:
		{
			x--;
			break;
		}

		default:
			break;
		}


	}
};

class Tank :public GameObject
{
public:
	bool enemy;
	Tank()
	{
		direction = 0;
		length = 1;
		enemy = true;
		gameObjId = 3;
	}


};

class Wall :public GameObject
{
public:
	Wall()
	{
		gameObjId = 1;
	}
	void move() {};
};

class Bullet :public GameObject
{
public:
	bool enemy;
	Bullet();
};

class Game
{


public:
	Game()
	{
		gameWin_ = false;
		gameOver_ = false;
		exit_ = false;
		lives_ = 3;
		score_ = 0;
		seconds_ = 0;
		minutes_ = 0;



	}
	// Initialize gaming variables and arrays
	void gameInit();
	// check of free space
	bool isPossibleSetArea(int x0, int y0, int x1, int y1);
	//check of free point
	int isPossibleSetPoint(int x0, int y0);
	// check the possibility of installing an object with a distance to other objects
	bool checkPosition(const GameObject &obj, int distance);
	//insert game object to array
	void populateMap(const GameObject &gameObject);
	//moove game object
	int moveGameObj(GameObject &gameObject, int direct);
	// hit the tank
	void tankGoal(Bullet &bullet);

	void graphics(); //Drawing game
	void gameLogic(); 
	void keyboard(); //Checking for input
	void start(); //Start 


private:
	int  direction_; // 0 -north; 1 - south ; 2 - east; 3 - west
	bool shoot_, gameOver_, gameWin_, exit_;
	int lives_, score_, minutes_, seconds_;

	int  map_[DIM_X][DIM_Y]; // objects in map 0-empty, 1-wall, 2- my tank, 3 -enemy tank , 4 - my bullet, 5 - enemy bullet
	Tank myTank_;
	vector<Tank> enemyTanks_;
	vector <Bullet> bullets_;
	vector <Wall> walls_;


};

void clearscreen()
{
	HANDLE hOut;
	COORD Position;

	hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	Position.X = 0;
	Position.Y = 0;
	SetConsoleCursorPosition(hOut, Position);
}

void sp(int choosecolor)
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), choosecolor); //FUNCTION OF COLOR
}

void s()
{
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
	//FUNCTION TO  FORGET COLOR,not sure how to say I know: Stop using color
}


int main()
{
	srand((unsigned)time(0)); //So random numbers will be always random.
	Game CrazyTanks; 
	CrazyTanks.start();
	return 0;
}

GameObject::GameObject(int _x, int _y, int _direction, int _length)
{
	x = _x; y = _y; direction = _direction; length = _length;
}

GameObject::GameObject()
{
}

//generates the position of the game object
void GameObject::generatePosition(int maxX, int maxY, int direct, int maxLength)
{
	x = rand() % maxX + 1;
	y = rand() % maxY + 1;
	direction = rand() % (direct + 1);
	length = rand() % maxLength + 1;

}



Bullet::Bullet()
{
	length = 1;
}

void Game::gameInit()
{
	//sets wall on borders
	for (int y = 0; y < DIM_X; y++)
	{
		for (int x = 0; x < DIM_Y; x++)
		{
			map_[x][y] = 0;

			// board
			if (x == 0 || x == DIM_Y - 1 || y == 0 || y == DIM_X - 1)
			{
				map_[x][y] = 1;
			}

		}

	}

	//set my Tank
	Tank newTank;

	newTank.x = DIM_X / 2;
	newTank.y = DIM_Y - 2;
	newTank.enemy = false;
	newTank.gameObjId = 2;

	myTank_ = newTank;
	populateMap(myTank_);


	// sets walls
	while (walls_.size() < NUMBER_OF_WALL)
	{
		Wall newWALL;
		newWALL.generatePosition(DIM_X, DIM_Y, 4, MAX_LENGTH_OF_WALL);
		if (checkPosition(newWALL, 1))
		{
			walls_.push_back(newWALL);
			populateMap(newWALL);

		}
	}


	//set of enemy tanks
	while (enemyTanks_.size() < NUMBER_OF_ENEMY)
	{
		Tank newTank;
		newTank.generatePosition(DIM_X, DIM_Y, 0, 1);
		if (checkPosition(newTank, 2))
		{
			enemyTanks_.push_back(newTank);
			populateMap(newTank);
		}
	}

}

bool Game::isPossibleSetArea(int x0, int y0, int x1, int y1)
{
	if (x0 < 0 || y0 < 0 || x1 > DIM_X || y1 > DIM_Y)
	{
		return false;
	}

	for (int x = x0; x <= x1; x++)
	{
		for (int y = y0; y <= y1; y++)
		{
			if (map_[x][y] != 0)
			{
				return false;
			}
		}


	}

	return true;

}

int Game::isPossibleSetPoint(int x0, int y0)
{
	if (x0 <= 0 || y0 <= 0 || x0 >= DIM_X || y0 >= DIM_Y)
	{
		return -1;
	}

	if (map_[x0][y0] != 0)
	{
		return map_[x0][y0];
	}

	return 0;

}


bool Game::checkPosition(const GameObject &gameObject, int distance)
{
	int x0 = gameObject.x;
	int y0 = gameObject.y;
	int x1 = gameObject.x;
	int y1 = gameObject.y;

	switch (gameObject.direction)
	{
	case 0: // direction up
	{
		x0 = gameObject.x - distance;
		y0 = gameObject.y - gameObject.length - distance;
		x1 = gameObject.x + distance;
		y1 = gameObject.y + distance;

		if (!isPossibleSetArea(x0, y0, x1, y1))
		{
			return false;
		}
		return true;

	case 1: //direction down
	{
		x0 = gameObject.x - distance;
		y0 = gameObject.y - distance;
		x1 = gameObject.x + distance;
		y1 = gameObject.y + gameObject.length + distance;

		if (!isPossibleSetArea(x0, y0, x1, y1))
		{
			return false;
		}
		return true;
	}

	case 2: //direction right
	{
		x0 = gameObject.x - distance;
		y0 = gameObject.y - distance;
		x1 = gameObject.x + gameObject.length + distance;
		y1 = gameObject.y + distance;

		if (!isPossibleSetArea(x0, y0, x1, y1))
		{
			return false;
		}
		return true;

	}
	case 3: //direction left
	{
		x0 = gameObject.x - gameObject.length - distance;
		y0 = gameObject.y - distance;
		x1 = gameObject.x + distance;
		y1 = gameObject.y + distance;

		if (!isPossibleSetArea(x0, y0, x1, y1))
		{
			return false;
		}
		return true;
	}
	}
	default:
		return false;
	}

	return false;
}


void Game::populateMap(const GameObject &gameObject)
{
	int x0 = gameObject.x;
	int y0 = gameObject.y;
	int x1 = gameObject.x;
	int y1 = gameObject.y;
	int lng = gameObject.length - 1;

	switch (gameObject.direction)
	{
	case 0: // direction up
	{
		y0 = gameObject.y - lng;
		break;
	}


	case 1: //direction down
	{
		y1 = gameObject.y + lng;
		break;
	}

	case 2: //direction right
	{
		x1 = gameObject.x + lng;
		break;
	}
	case 3: //direction left
	{
		x0 = gameObject.x - lng;
	}

	}

	for (int y = y0; y <= y1; y++)
	{
		for (int x = x0; x <= x1; x++)
		{
			map_[x][y] = gameObject.gameObjId;
		}
	}


}

// returns the id of the game object , if the obstruction
int Game::moveGameObj(GameObject &gameObject, int direct)
{
	int barrierId = 0;
	int oldX = gameObject.x;
	int oldY = gameObject.y;
	gameObject.direction = direct;
	gameObject.move();
	barrierId = isPossibleSetPoint(gameObject.x, gameObject.y);
	if (barrierId != 0 && gameObject.gameObjId != 4 && gameObject.gameObjId != 5)
	{
		gameObject.x = oldX;
		gameObject.y = oldY;
	}
	else
	{
		map_[oldX][oldY] = 0;

	}

	return barrierId;

}


// tank hit
void Game::tankGoal(Bullet &bullet)
{
	if (bullet.enemy == true && bullet.x == myTank_.x && bullet.y == myTank_.y)
	{
		//my tank is hit

		lives_--;

	}
	else if (bullet.enemy == false)
	{
		// enemy tank hit
		if (enemyTanks_.empty() == false) {

			for (int i = enemyTanks_.size() - 1; i >= 0; i--) {
				if (enemyTanks_[i].x == bullet.x && enemyTanks_[i].x == bullet.x)
				{
					map_[enemyTanks_[i].x][enemyTanks_[i].y] = 0;

					enemyTanks_.erase(enemyTanks_.begin() + i);
					score_++;
					continue;
				}

			}
		}

	}
}

// draw field
void Game::graphics()
{
	for (int y = 0; y < DIM_X; y++)
	{
		for (int x = 0; x < DIM_Y; x++)
		{
			switch (map_[x][y])
			{

			case 0:
			{
				cout << ' ';
				break;
			}

			case 1:
			{
				sp(6);
				std::cout << char(219);
				s();
				break;
			}

			case 2:
			{
				sp(4);
				std::cout << char(219);
				s();
				break;
			}

			case 3:
			{
				sp(9);
				std::cout << char(219);
				s();
				break;
			}

			case 4:
			{
				sp(4);
				std::cout << 'o';
				s();
				break;
			}

			case 5:
			{
				sp(9);
				std::cout << 'o';
				s();
				break;
			}


			default:
				break;
			}
		}

		switch (y)
		{
		case DIM_Y / 2:
		{
			cout << " Lives   " << lives_ << endl;
			break;
		}
		case DIM_Y / 2 + 1:
		{
			cout << " Score   " << score_ << endl;
			break;
		}

		case DIM_Y / 2 + 2:
		{
			cout << " Time   " << minutes_ << " : " << seconds_ << ' ' << endl;
			break;
		}

		case DIM_Y / 2 + 4:
		{
			if (gameOver_ == true)
			{
				cout << " Game over!    " << endl;
				break;
			}

			if (gameWin_ == true)
			{
				cout << " You Win!    " << endl;
				break;
			}
		}

		case DIM_Y / 2 + 6:
		{
			if (gameOver_ == true || gameWin_ == true)
			{
				cout << " Press Enter for exit   " << endl;
				break;
			}
		}

		default:
			cout << endl;
			break;
		}



	}

	clearscreen();
	Sleep(10);

}

//Logic of game
void Game::gameLogic()
{

	seconds_ = (clock() / 1000) % 60;
	minutes_ = (clock() / 1000) / 60;
	if (lives_ == 0)
	{
		gameOver_ = true;
	}

	if (enemyTanks_.size() == 0)
	{
		gameWin_ = true;
	}

	// move my tank
	direction_ = -1;
	shoot_ = 0;
	keyboard();
	int oldX = myTank_.x;
	int oldY = myTank_.y;
	if (direction_ >= 0)
	{

		moveGameObj(myTank_, direction_);
		populateMap(myTank_);

	}

	// If my Tank shoot
	if (shoot_ == 1)
	{
		Bullet myBullet;
		myBullet.direction = myTank_.direction;
		myBullet.x = myTank_.x;
		myBullet.y = myTank_.y;
		myBullet.enemy = false;
		myBullet.gameObjId = 4;
		myBullet.length = 1;
		bullets_.push_back(myBullet);
	}

	// movement and shooting of enemy tank
	for (int i = 0; i < enemyTanks_.size(); i++)
	{
		int rndChangeDirect = rand() % CHANCE_CHANGE_DIRECRION;
		int newDirection = enemyTanks_[i].direction;

		if (rndChangeDirect == 1)
		{
			newDirection = rand() % 4;
		}

		int rndMove = rand() % CHANCE_ENEMY_MOVE;

		if (rndMove == 1)
		{
			moveGameObj(enemyTanks_[i], newDirection);
		}

		

		// enemy tank shoots

		int rndShoot = rand() % CHANCE_SHOOT_ENEMY;
		if ((myTank_.x == enemyTanks_[i].x || myTank_.y == enemyTanks_[i].y) && rndShoot == 1)
		{
			//determinne direction enemy bull��
			direction_ = 0;
			int deltaX = myTank_.x - enemyTanks_[i].x;
			int deltaY = myTank_.y - enemyTanks_[i].y;

			//up
			if (deltaX == 0 && deltaY < 0) { direction_ = 0; }
			//down
			if (deltaX == 0 && deltaY > 0) { direction_ = 1; }
			//right
			if (deltaY == 0 && deltaX > 0) { direction_ = 2; }
			//left
			if (deltaY == 0 && deltaX < 0) { direction_ = 3; }



			Bullet enemyBullet;
			enemyBullet.direction = direction_;
			enemyBullet.x = enemyTanks_[i].x;
			enemyBullet.y = enemyTanks_[i].y;
			enemyBullet.enemy = true;
			enemyBullet.gameObjId = 5;
			bullets_.push_back(enemyBullet);
		}


		populateMap(enemyTanks_[i]);

	}

	//bullet movement
	if (bullets_.empty() == false) {
		int targetId = 0;

		for (int i = bullets_.size() - 1; i >= 0; i--) {
			map_[bullets_[i].x][bullets_[i].y] = 0;
			targetId = moveGameObj(bullets_[i], bullets_[i].direction);

			if (targetId != 0) {


				if (targetId == 2 || targetId == 3)
				{
					tankGoal(bullets_[i]);
				}

				bullets_.erase(bullets_.begin() + i);
				continue;
			}
			populateMap(bullets_[i]);
			populateMap(myTank_);
		}
	}

}

void Game::keyboard()
{

	if (_kbhit()) //If player clicks something

	{
		int key = _getch();

		//Now this click is key
		switch (key)

		{
		case 72:

		{
			direction_ = 0;
			break;
		}

		case 80:
		{
			direction_ = 1;
			break;
		}

		case 75:
		{
			direction_ = 3;
			break;
		}

		case 77:
		{
			direction_ = 2;
			break;
		}
		case 32:
		{
			shoot_ = 1;
			break;
		}

		case 13:
		{
			exit_ = true;
			break;
		}



		default:
		{

			shoot_ = 0;
			direction_ = -1;
		}


		}

	}
}

void Game::start()
{
	gameInit();

	while (!gameOver_ && !gameWin_) {

		gameLogic();
		graphics();
	}


	while (!exit_)
	{
		keyboard();
		Sleep(50);
	}


}

