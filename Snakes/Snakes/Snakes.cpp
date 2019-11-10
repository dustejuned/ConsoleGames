#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <Windows.h>
#include <thread>
#include <vector>
#include <list>
using namespace std;

int screenHeight = 30;
int screenWidth = 120;

struct SnakeSegement
{
	int x;
	int y;
};

int main()
{
	
	wchar_t *screen = new wchar_t[screenWidth*screenHeight];

	for (size_t i = 0; i < screenWidth * screenHeight; i++)
	{
		screen[i] = L' ';
	}

	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	SetConsoleActiveScreenBuffer(hConsole);

	DWORD dwBytesWritten = 0;

	while (1)
	{
		bool gameOver = false;
		list<SnakeSegement> snake = { {60, 15}, {61, 15}, {62, 15}, {63, 15}, {64, 15}, {65, 15}, {66, 15}, {67, 15}, {68, 15}, {69, 15} };
		int foodX = 30;
		int foodY = 15;
		int snakeDirection = 3;
		int score = 0;
		bool isLeftKey = false, isRightKey = false, isOldLeftKey = true, isOldRightKey = false;

		while (!gameOver)
		{
			auto t1 = chrono::system_clock::now();
			while ((chrono::system_clock::now() - t1) < (snakeDirection % 2 == 0 ? 200ms : 120ms))
			{
				isRightKey = (0x8000 & GetAsyncKeyState((unsigned char)('\x27'))) != 0;
				isLeftKey = (0x8000 & GetAsyncKeyState((unsigned char)('\x25'))) != 0;

				if (isRightKey && !isOldRightKey)
				{
					snakeDirection++;
					if (snakeDirection == 4) snakeDirection = 0;
				}
				if (isLeftKey && !isOldLeftKey)
				{
					snakeDirection--;
					if (snakeDirection == -1) snakeDirection = 3;
				}

				isOldLeftKey = isLeftKey;
				isOldRightKey = isRightKey;
			}

			// Clear Screen
			for (int i = 0; i < screenWidth*screenHeight; i++) screen[i] = L' ';

			// Draw Stats & Border
			for (int i = 0; i < screenWidth; i++)
			{
				screen[i] = L'=';
				screen[2 * screenWidth + i] = L'=';
			}
			wsprintf(&screen[screenWidth + 45], L"SCORE: %d", score);


			switch (snakeDirection)
			{
				//up
			case 0:
				snake.push_front({ snake.front().x, snake.front().y - 1 });
				break;
				//right
			case 1:
				snake.push_front({ snake.front().x + 1, snake.front().y });
				break;
				//down
			case 2:
				snake.push_front({ snake.front().x, snake.front().y + 1 });
				break;
			case 3:
				//left
				snake.push_front({ snake.front().x - 1, snake.front().y });	
				break;
				
			}

			if (snake.front().x >= screenWidth || snake.front().x < 0)
			{
				gameOver = true;
			}
			if (snake.front().y < 3 || snake.front().y >= screenHeight)
			{
				gameOver = true;
			}

			//check collision with itself
			for (list<SnakeSegement>::iterator i = snake.begin(); i != snake.end(); i++)
			{
				if (i != snake.begin() && i->x == snake.front().x && i->y == snake.front().y)
				{
					gameOver = true;
				}
			}

			snake.pop_back();

			//collect food
			if (snake.front().x == foodX && snake.front().y == foodY)
			{
				score += 50;
				foodX = rand() % screenWidth;
				foodY = (rand() % (screenHeight - 3)) + 3;

				for (int i = 0; i < 3; i++)
				{
					snake.push_back({ snake.back().x, snake.back().y });
				}
			}							

			//draw snake body
			for (auto s : snake)
			{
				screen[s.y * screenWidth + s.x] = gameOver ? L'+' : L'¤';
			}
			//draw snake head
			screen[snake.front().y * screenWidth + snake.front().x] = gameOver ? L'X' : L'Ø';

			//draw food
			screen[foodY * screenWidth + foodX] = L'%';

			if(gameOver)
				wsprintf(&screen[screenWidth * 15 + 45], L"Game Over!!! Press Space to continue.", score);

			WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, { 0, 0 }, &dwBytesWritten);
		}
		while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);
		
	}



	//CloseHandle(hConsole);

	//cout << "Game Over!! Score:" << 10 << endl;
	//system("pause");

	return 0;
}