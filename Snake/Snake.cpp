#include "stdafx.h"
#include <iostream>
#include <thread>
#include <vector>
#include <Windows.h>
#include <limits>


int screenWidth = 120;
int screenHeight = 30;

int fieldWidth = 30;
int fieldHeight = 15;

unsigned char * field = nullptr;
char *screen = nullptr;


void draw_board() {
	screen = new char[screenWidth*screenHeight];
	for (int i = 0; i < screenWidth*screenHeight; i++) screen[i] = ' ';

	field = new unsigned char[fieldWidth*fieldHeight];
	for (int i = 0; i < fieldWidth; ++i)
		for (int j = 0; j < fieldHeight; ++j)
			field[j * fieldWidth + i] = (i == 0 || j == 0 || i == fieldWidth - 1 || j == fieldHeight - 1) ? 1 : 0;


}

void move_position(bool position[], int iter) {
	for (int i = 0; i < 4; ++i) {
		if (i == iter)
			position[i] = true;
		else
			position[i] = false;
	}
}

//Check if snake hit a wall or himself
bool check_if_fit(int currentX, int currentY) {
	if (screen[currentY * screenWidth + currentX] == '*' || screen[currentY * screenWidth + currentX] == 'o')
		return false;
	return true;
}

int main()
{
	draw_board();
	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);
	DWORD dwBytesWritten = 0;


	while (1) {

			int currentX = 20;
			int currentY = 10;
			bool currentDirection[4] = { false, true, false, false };
			int nextPosition = currentY * screenWidth + currentX;


			int speed = 20;
			int speedCount = 0;
			bool moveFlag = false;

			int spawnPoint = (rand() % (fieldHeight - 3) + 6) * screenWidth + (rand() % (fieldWidth - 3) + 11);
			int score = 0;

			bool key[4];
			bool gameResetFlag = true;

			std::vector<int> snake = { nextPosition,  nextPosition + 1, nextPosition + 2 , nextPosition + 3 };
			reverse(snake.begin(), snake.end());
			int head = snake[0];

			//Game loop
			using namespace std::chrono_literals;
			while (gameResetFlag) {

				//Timing
				std::this_thread::sleep_for(5ms);
				speedCount++;

				if (speedCount == speed) {
					moveFlag = true;
					speedCount = 0;
				}

				//Input 
				for (int k = 0; k < 4; k++)
					key[k] = (0x8000 & GetAsyncKeyState((unsigned char)("WASD"[k]))) != 0;

				if (key[3]) {
					move_position(currentDirection, 3);
				}
				else if (key[2]) {
					move_position(currentDirection, 2);
				}
				else if (key[1]) {
					move_position(currentDirection, 1);
				}
				else if (key[0]) {
					move_position(currentDirection, 0);
				}

				//Move snake
				if (moveFlag) {
					if (currentDirection[3]) {
						if (!check_if_fit(currentX + 1, currentY))
							gameResetFlag = false;
						currentX += 1;
					}
					else if (currentDirection[2]) {
						if (!check_if_fit(currentX, currentY + 1))
							gameResetFlag = false;
						currentY += 1;
					}
					else if (currentDirection[1]) {
						if (!check_if_fit(currentX - 1, currentY))
							gameResetFlag = false;
						currentX -= 1;
					}
					else if (currentDirection[0]) {
						if (!check_if_fit(currentX, currentY - 1))
							gameResetFlag = false;
						currentY -= 1;
					}

					nextPosition = currentY * screenWidth + currentX;
					snake.insert(snake.begin(), nextPosition);
					snake.erase(snake.end() - 1);
					moveFlag = false;
				}

				//Spown another point and add score
				head = snake[0];
				if (head == spawnPoint) {
					snake.insert(snake.begin(), spawnPoint);
					spawnPoint = (rand() % (fieldHeight - 3) + 6) * screenWidth + (rand() % (fieldWidth - 3)+ 11);
					while (screen[spawnPoint] != ' ')
						spawnPoint = (rand() % (fieldHeight - 3) + 6) * screenWidth + (rand() % (fieldWidth - 3) + 11);

					score += 10;
					if ((score % 30) == 0 && score != 0) {
						speed -= 1;
						speedCount = 0;
					}

				}

				//Draw snake and point
				for (int i = 0; i < fieldWidth; ++i)
					for (int j = 0; j < fieldHeight; ++j)
						screen[(j + 5) * screenWidth + (i + 10)] = " *o"[field[j * fieldWidth + i]];

				for (auto && i : snake)
					screen[i] = 111;
				screen[head] = 64;
				screen[spawnPoint] = 36;



				// Draw Score
				sprintf_s(&screen[15 * screenWidth + 50], 32, "    PRESS 'SPACE' TO PLAY AGAIN");
				sprintf_s(&screen[2 * screenWidth + fieldWidth + 6], 16, "SCORE: %8d", score);

				WriteConsoleOutputCharacterA(console, screen, screenWidth * screenHeight, { 0,0 }, &dwBytesWritten);
			}

			//Wait until space key is pressed
			while ((0x8000 & GetAsyncKeyState((unsigned char)('\x20'))) == 0);

	}
    return 0;
}

