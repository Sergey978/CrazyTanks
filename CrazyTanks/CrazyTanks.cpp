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
// размер пол€
const unsigned int DIM_X = 30;
const unsigned int DIM_Y = 30;
//число врагов
const unsigned int NUMBER_OF_ENEMY = 5;
//число стен
const unsigned int NUMBER_OF_WALL = 10;
//максимальна€ длина стен
const unsigned int MAX_LENGTH_OF_WALL = 10;
//параметры играбельности частота смены направлени€  и движени€, шанс выстрела 1/n
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

private:
	int  direction; // 0 -north; 1 - south ; 2 - east; 3 - west
	bool shoot, gameOver, gameWin, exit;
	int lives, score, minutes, seconds;


	int  map[DIM_X][DIM_Y]; // objects in map 0-empty, 1-wall, 2- my tank, 3 -enemy tank , 4 - my bullet, 5 - enemy bullet
	Tank myTank;
	vector<Tank> enemyTanks;
	vector <Bullet> bullets;
	vector <Wall> walls;


public:
	Game()
	{
		gameWin = false;
		gameOver = false;
		exit = false;
		lives = 3;
		score = 0;
		seconds = 0;
		minutes = 0;



	}

	void gameInit();
	// проверка свободной площади
	bool isPossibleSetArea(int x0, int y0, int x1, int y1);
	//проверка свободной 
	int isPossibleSetPoint(int x0, int y0);
	// возможность установки обьекта с дистанцией до других обьектов
	bool checkPosition(const GameObject &obj, int distance);
	void populateMap(const GameObject &obj);
	//пермещение игрового обьекта
	int moveGameObj(GameObject & obj, int direct);

	void tankGoal(static Bullet & bullet);

	void graphics(); //Drawing game
	void gameLogic(); //Does it need comment?
	void keyboard(); //Checking for input
	void start(); //Start -_-

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
	//FUNCTION TO ..EHM FORGET COLOR,not sure how to say I know: Stop using color
}


int main()
{
	srand((unsigned)time(0)); //So random numbers will be always random.
	Game CrazyTanks; //creating class m
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

//генерирует позицию игрового обьекта
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
// »нициализаци€ игровых перменных и массивов
void Game::gameInit()
{
	//заполнение границ стенками
	for (int y = 0; y < DIM_X; y++)
	{
		for (int x = 0; x < DIM_Y; x++)
		{
			map[x][y] = 0;

			// board
			if (x == 0 || x == DIM_Y - 1 || y == 0 || y == DIM_X - 1)
			{
				map[x][y] = 1;
			}

		}

	}

	//установка своего танка
	Tank newTank;

	newTank.x = DIM_X / 2;
	newTank.y = DIM_Y - 2;
	newTank.enemy = false;
	newTank.gameObjId = 2;

	myTank = newTank;
	populateMap(myTank);


	// установка преп€тствий
	while (walls.size() < NUMBER_OF_WALL)
	{
		Wall newWALL;
		newWALL.generatePosition(DIM_X, DIM_Y, 4, MAX_LENGTH_OF_WALL);
		if (checkPosition(newWALL, 1))
		{
			walls.push_back(newWALL);
			populateMap(newWALL);

		}
	}


	//установка вражеских танков
	while (enemyTanks.size() < NUMBER_OF_ENEMY)
	{
		Tank newTank;
		newTank.generatePosition(DIM_X, DIM_Y, 0, 1);
		if (checkPosition(newTank, 2))
		{
			enemyTanks.push_back(newTank);
			populateMap(newTank);
		}
	}

}
//ѕроверка на свободность пр€моугольника
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
			if (map[x][y] != 0)
			{
				return false;
			}
		}


	}

	return true;

}
//ѕроверка на свободность точки
int Game::isPossibleSetPoint(int x0, int y0)
{
	if (x0 <= 0 || y0 <= 0 || x0 >= DIM_X || y0 >= DIM_Y)
	{
		return -1;
	}

	if (map[x0][y0] != 0)
	{
		return map[x0][y0];
	}

	return 0;

}

//ѕроверка на возможность вставить в массив игрового ќбьекта
bool Game::checkPosition(const GameObject & obj, int distance)
{
	int x0 = obj.x;
	int y0 = obj.y;
	int x1 = obj.x;
	int y1 = obj.y;

	switch (obj.direction)
	{
	case 0: // direction up
	{
		x0 = obj.x - distance;
		y0 = obj.y - obj.length - distance;
		x1 = obj.x + distance;
		y1 = obj.y + distance;

		if (!isPossibleSetArea(x0, y0, x1, y1))
		{
			return false;
		}
		return true;

	case 1: //direction down
	{
		x0 = obj.x - distance;
		y0 = obj.y - distance;
		x1 = obj.x + distance;
		y1 = obj.y + obj.length + distance;

		if (!isPossibleSetArea(x0, y0, x1, y1))
		{
			return false;
		}
		return true;
	}

	case 2: //direction right
	{
		x0 = obj.x - distance;
		y0 = obj.y - distance;
		x1 = obj.x + obj.length + distance;
		y1 = obj.y + distance;

		if (!isPossibleSetArea(x0, y0, x1, y1))
		{
			return false;
		}
		return true;

	}
	case 3: //direction left
	{
		x0 = obj.x - obj.length - distance;
		y0 = obj.y - distance;
		x1 = obj.x + distance;
		y1 = obj.y + distance;

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

//вставка игрового обьекта в массив
void Game::populateMap(const GameObject & obj)
{
	int x0 = obj.x;
	int y0 = obj.y;
	int x1 = obj.x;
	int y1 = obj.y;
	int lng = obj.length - 1;

	switch (obj.direction)
	{
	case 0: // direction up
	{
		y0 = obj.y - lng;
		break;
	}


	case 1: //direction down
	{
		y1 = obj.y + lng;
		break;
	}

	case 2: //direction right
	{
		x1 = obj.x + lng;
		break;
	}
	case 3: //direction left
	{
		x0 = obj.x - lng;
	}

	}

	for (int y = y0; y <= y1; y++)
	{
		for (int x = x0; x <= x1; x++)
		{
			map[x][y] = obj.gameObjId;
		}
	}


}

// движение игрового обьекта
int Game::moveGameObj(GameObject & obj, int direct)
{
	int barrierId = 0;
	int oldX = obj.x;
	int oldY = obj.y;
	obj.direction = direct;
	obj.move();
	barrierId = isPossibleSetPoint(obj.x, obj.y);
	if (barrierId != 0 && obj.gameObjId != 4 && obj.gameObjId != 5)
	{
		obj.x = oldX;
		obj.y = oldY;
	}
	else
	{
		map[oldX][oldY] = 0;

	}

	return barrierId;

}


// ѕопадание в танк
void Game::tankGoal(Bullet & bullet)
{
	if (bullet.enemy == true && bullet.x == myTank.x && bullet.y == myTank.y)
	{
		//my tank is hit

		lives--;

	}
	else if (bullet.enemy == false)
	{
		// enemy tank hit
		if (enemyTanks.empty() == false) {

			for (int i = enemyTanks.size() - 1; i >= 0; i--) {
				if (enemyTanks[i].x == bullet.x && enemyTanks[i].x == bullet.x)
				{
					map[enemyTanks[i].x][enemyTanks[i].y] = 0;

					enemyTanks.erase(enemyTanks.begin() + i);
					score++;
					continue;
				}

			}
		}

	}
}

// ќтображене игры
void Game::graphics()
{
	for (int y = 0; y < DIM_X; y++)
	{
		for (int x = 0; x < DIM_Y; x++)
		{
			switch (map[x][y])
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
			cout << " Lives   " << lives << endl;
			break;
		}
		case DIM_Y / 2 + 1:
		{
			cout << " Score   " << score << endl;
			break;
		}

		case DIM_Y / 2 + 2:
		{
			cout << " Time   " << minutes << " : " << seconds << ' ' << endl;
			break;
		}

		case DIM_Y / 2 + 4:
		{
			if (gameOver == true)
			{
				cout << " Game over!    " << endl;
				break;
			}

			if (gameWin == true)
			{
				cout << " You Win!    " << endl;
				break;
			}
		}

		case DIM_Y / 2 + 6:
		{
			if (gameOver == true || gameWin == true)
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

//ћеханика игры
void Game::gameLogic()
{

	seconds = (clock() / 1000) % 60;
	minutes = (clock() / 1000) / 60;
	if (lives == 0)
	{
		gameOver = true;
	}

	if (enemyTanks.size() == 0)
	{
		gameWin = true;
	}
	// move my tank
	direction = -1;
	shoot = 0;
	keyboard();
	int oldX = myTank.x;
	int oldY = myTank.y;
	if (direction >= 0)
	{

		moveGameObj(myTank, direction);
		populateMap(myTank);

	}

	// If my Tank shoot
	if (shoot == 1)
	{
		Bullet myBullet;
		myBullet.direction = myTank.direction;
		myBullet.x = myTank.x;
		myBullet.y = myTank.y;
		myBullet.enemy = false;
		myBullet.gameObjId = 4;
		myBullet.length = 1;
		bullets.push_back(myBullet);
	}

	// движение и стрельба вражеского танка
	for (int i = 0; i < enemyTanks.size(); i++)
	{
		int rndChangeDirect = rand() % CHANCE_CHANGE_DIRECRION;
		int newDirection = enemyTanks[i].direction;

		if (rndChangeDirect == 1)
		{
			newDirection = rand() % 4;
		}

		int rndMove = rand() % CHANCE_ENEMY_MOVE;

		if (rndMove == 1)
		{
			moveGameObj(enemyTanks[i], newDirection);
		}

		

		// вражеский танк стрел€ет

		int rndShoot = rand() % CHANCE_SHOOT_ENEMY;
		if ((myTank.x == enemyTanks[i].x || myTank.y == enemyTanks[i].y) && rndShoot == 1)
		{
			//determinne direction enemy bullет
			direction = 0;
			int deltaX = myTank.x - enemyTanks[i].x;
			int deltaY = myTank.y - enemyTanks[i].y;

			//up
			if (deltaX == 0 && deltaY < 0) { direction = 0; }
			//down
			if (deltaX == 0 && deltaY > 0) { direction = 1; }
			//right
			if (deltaY == 0 && deltaX > 0) { direction = 2; }
			//left
			if (deltaY == 0 && deltaX < 0) { direction = 3; }



			Bullet enemyBullet;
			enemyBullet.direction = direction;
			enemyBullet.x = enemyTanks[i].x;
			enemyBullet.y = enemyTanks[i].y;
			enemyBullet.enemy = true;
			enemyBullet.gameObjId = 5;
			bullets.push_back(enemyBullet);
		}


		populateMap(enemyTanks[i]);

	}

	//move bullets

	if (bullets.empty() == false) {
		int targetId = 0;

		for (int i = bullets.size() - 1; i >= 0; i--) {
			map[bullets[i].x][bullets[i].y] = 0;
			targetId = moveGameObj(bullets[i], bullets[i].direction);

			if (targetId != 0) {


				if (targetId == 2 || targetId == 3)
				{
					tankGoal(bullets[i]);
				}

				bullets.erase(bullets.begin() + i);
				continue;
			}
			populateMap(bullets[i]);
			populateMap(myTank);
		}
	}

}

//ѕроверка клавиатуры
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
			direction = 0;
			break;
		}

		case 80:
		{
			direction = 1;
			break;
		}

		case 75:
		{
			direction = 3;
			break;
		}

		case 77:
		{
			direction = 2;
			break;
		}
		case 32:
		{
			shoot = 1;
			break;
		}

		case 13:
		{
			exit = true;
			break;
		}



		default:
		{

			shoot = 0;
			direction = -1;
		}


		}

	}
}
//«апуск игры
void Game::start()
{
	gameInit();

	while (!gameOver && !gameWin) {

		gameLogic();
		graphics();
	}


	while (!exit)
	{
		keyboard();
		Sleep(50);
	}


}

