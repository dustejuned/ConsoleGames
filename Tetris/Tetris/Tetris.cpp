#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <Windows.h>
#include <thread>
#include <vector>
using namespace std;

int screenHeight = 30;
int screenWidth = 80;
int fieldHeight = 18;
int fieldWidth = 12;
wstring tetrominos[7];
unsigned char *playingField = nullptr;

int Rotate(int curX, int curY, int rotate)
{
	int pi = 0;
	switch (rotate % 4)
	{
	case 0: // 0 degrees				// 0  1  2  3
		pi = curY * 4 + curX;			// 4  5  6  7
		break;							// 8  9 10 11
										//12 13 14 15

	case 1: // 90 degrees				//12  8  4  0
		pi = 12 + curY - (curX * 4);	//13  9  5  1
		break;							//14 10  6  2
										//15 11  7  3

	case 2: // 180 degrees				//15 14 13 12
		pi = 15 - (curY * 4) - curX;	//11 10  9  8
		break;							// 7  6  5  4
										// 3  2  1  0

	case 3: // 270 degrees				// 3  7 11 15
		pi = 3 - curY + (curX * 4);		// 2  6 10 14
		break;							// 1  5  9 13
	}									// 0  4  8 12

	return pi;
}

bool IsValidMove(int curTetromino, int curRotation, int newX, int newY)
{
	for (int px = 0; px < 4; px++)
	{
		for (int py = 0; py < 4; py++)
		{
			int rotateIndex = Rotate(px, py, curRotation);
			int fieldIndex = (newY + py) * fieldWidth + (newX + px);

			if ((newX + px) >= 0 && (newX + px) < fieldWidth)
			{
				if ((newY + py) >= 0 && (newY + py) < fieldHeight)
				{
					if (tetrominos[curTetromino][rotateIndex] != L'.' && playingField[fieldIndex] != 0)
					{
						return false;
					}
				}
			}
		}
	}

	return true;
}

int main()
{

	bool gameOver = false;
	wchar_t *screen = new wchar_t[screenWidth*screenHeight];
	playingField = new unsigned char[fieldWidth*fieldHeight];
	bool userInputs[4];
	//4X4 tertraminos
	tetrominos[0].append(L"..X.");
	tetrominos[0].append(L"..X.");
	tetrominos[0].append(L"..X.");
	tetrominos[0].append(L"..X.");

	tetrominos[1].append(L"..X.");
	tetrominos[1].append(L".XX.");
	tetrominos[1].append(L"..X.");
	tetrominos[1].append(L"....");

	tetrominos[2].append(L"....");
	tetrominos[2].append(L".XX.");
	tetrominos[2].append(L".XX.");
	tetrominos[2].append(L"....");

	tetrominos[3].append(L".X..");
	tetrominos[3].append(L".XX.");
	tetrominos[3].append(L"..X.");
	tetrominos[3].append(L"....");

	tetrominos[4].append(L".X..");
	tetrominos[4].append(L".XX.");
	tetrominos[4].append(L".X..");
	tetrominos[4].append(L"....");

	tetrominos[5].append(L".X..");
	tetrominos[5].append(L".X..");
	tetrominos[5].append(L".XX.");
	tetrominos[5].append(L"....");

	tetrominos[6].append(L"..X.");
	tetrominos[6].append(L"..X.");
	tetrominos[6].append(L".XX.");
	tetrominos[6].append(L"....");




	for (size_t i = 0; i < screenWidth * screenHeight; i++)
	{
		screen[i] = L' ';
	}

	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	SetConsoleActiveScreenBuffer(hConsole);

	DWORD dwBytesWritten = 0;

	//Define boundaries playing field
	for (int i = 0; i < fieldWidth; i++)
	{
		for (int j = 0; j < fieldHeight; j++)
		{
			playingField[j * fieldWidth + i] = (i == 0 || i == fieldWidth - 1 || j == fieldHeight - 1) ? 3 : 0;
		}
	}

	int currentX = fieldWidth / 2;
	int currentY = 0;
	int currentTetromino = 0;
	int nextTetromino = rand() % 7;
	int currentRotation = 0;
	int speedCount = 0;
	int speed = 20;
	int steps = 0;
	bool forceDown = false;
	bool rotateHold = false;
	vector<int> lines;
	int score = 0;
	while (!gameOver) //game loop
	{
		this_thread::sleep_for(50ms); // Small Step = 1 Game Tick
		speedCount++;
		steps++;
		forceDown = (speedCount == speed);
		
		if (steps % 15 == 0)
			speed = speed > 10 ? speed-- : speed;
		

		//handle user input
		for (int i = 0; i < 4; i++)
		{
			userInputs[i] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[i]))) != 0; //VK_Right, VK_Left, VK_Down, Z
		}


		currentX += (userInputs[0] && IsValidMove(currentTetromino, currentRotation, currentX + 1, currentY)) ? 1 : 0;
		
		currentX -= (userInputs[1] && IsValidMove(currentTetromino, currentRotation, currentX - 1, currentY)) ? 1 : 0;
		
		currentY += (userInputs[2] && IsValidMove(currentTetromino, currentRotation, currentX, currentY + 1)) ? 1 : 0;

		if (userInputs[3])
		{
			currentRotation += (rotateHold && IsValidMove(currentTetromino, currentRotation + 1, currentX, currentY)) ? 1 : 0;
			rotateHold = false;
		}
		else
		{
			rotateHold = true;
		}

		if (forceDown)
		{
			speedCount = 0;

			if (IsValidMove(currentTetromino, currentRotation, currentX, currentY + 1))
			{
				currentY++;
			}
			else
			{
				for (int x = 0; x < 4; x++)
				{
					for (int y = 0; y < 4; y++)
					{
						if (tetrominos[currentTetromino][Rotate(x, y, currentRotation)] != L'.')
						{
							//lock it to screen if does not fit
							playingField[(currentY + y) * fieldWidth + (currentX + x)] = 1;
						}
					}
				}

				//Check if it fills every column in a row
				for (int y = 0; y < 4; y++)
				{
					if (currentY + y < fieldHeight - 1)
					{
						bool hasLine = true;
						for (int x = 1; x < fieldWidth - 1; x++)
						{
							hasLine &= playingField[(currentY + y) * fieldWidth + x] != 0;
						}

						if (hasLine)
						{
							for (int x = 1; x < fieldWidth - 1; x++)
							{
								playingField[(currentY + y) * fieldWidth + x] = 2;
							}
							lines.push_back(currentY + y);
						}
					}

				}

				score += lines.size() * 100;
				currentTetromino = nextTetromino;
				nextTetromino = rand() % 7;
				currentX = fieldWidth / 2;
				currentY = 0;
				currentRotation = 0;

				if (!IsValidMove(currentTetromino, currentRotation, currentX, currentY))
					gameOver = true;
			}
		}

		if (!lines.empty())
		{
			WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);
			this_thread::sleep_for(400ms); // Delay a bit

			

			for (auto &line : lines)
			{
				for (int x = 1; x < fieldWidth - 1; x++)
				{
					for (int y = line; y > 0; y--)
					{
						playingField[y * fieldWidth + x] = playingField[(y - 1) * fieldWidth + x];						
					}
					playingField[x] = 0;
				}
			}
			lines.clear();
		}


		//Draw playing field
		for (int i = 0; i < fieldWidth; i++)
		{
			for (int j = 0; j < fieldHeight; j++)
			{
				screen[(j + 2) * screenWidth + (i + 2)] = L" ¤°#"[playingField[j*fieldWidth + i]];
			}
		}

		//Draw current Tetromino
		for (int x = 0; x < 4; x++)
		{
			for (int y = 0; y < 4; y++)
			{
				if (tetrominos[currentTetromino][Rotate(x, y, currentRotation)] != L'.')
				{
					screen[(currentY + y + 2) * screenWidth + (currentX + x + 2)] = L'¤';
				}
			}
		}

		swprintf_s(&screen[2 * screenWidth + 15], 8, L" Next  ");
		//Draw next tetromino
		{
			for (int x = 0; x < 4; x++)
			{
				for (int y = 0; y < 4; y++)
				{
					if (tetrominos[nextTetromino][Rotate(x, y, 0)] != L'.')
					{
						screen[(3 + y) * screenWidth + (15 + x)] = L'¤';
					}
					else
					{
						screen[(3 + y) * screenWidth + (15 + x)] = L' ';
					}
				}
			}
		}

		swprintf_s(&screen[7 * screenWidth + 15], 8, L" Score ");
		swprintf_s(&screen[8 * screenWidth + 15], 16, L"%8d       ", score);

		WriteConsoleOutputCharacter(hConsole, screen, screenWidth * screenHeight, { 0, 0 }, &dwBytesWritten);
	}

	CloseHandle(hConsole);

	cout << "Game Over!! Score:" << score << endl;
	system("pause");
	return 0;
}