#include "stdafx.h"
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <Windows.h>
#include <stdio.h>

using namespace std::chrono_literals;

int screenWidth = 120;
int screenHeight = 30;

int fieldWidth = 15;
int fieldHeight = 18;

int secondFieldWidth = 6;
int secondFieldHeight = 7;

unsigned char *secondField = nullptr;
unsigned char *field = nullptr;

void populate_vector(std::vector<std::string>& shapes) {

	shapes.push_back("..X...X...X...X.");
	shapes.push_back(".....XX...X...X.");
	shapes.push_back("......X..XX..X..");
	shapes.push_back(".....XX..XX.....");
	shapes.push_back("......X..XXX....");
	shapes.push_back(".....X...XX...X.");
	shapes.push_back(".....XX..X...X..");
}

void field_buffer() {

	field = new unsigned char[fieldWidth * fieldHeight];
	for (int i = 0; i < fieldWidth; ++i)
		for (int j = 0; j < fieldHeight; ++j)
			field[j * fieldWidth + i] = (i == 0 || j == fieldHeight - 1 || i == fieldWidth - 1) ? 1 : 0;
}

void second_field_buffer(){

	secondField = new unsigned char[secondFieldWidth * secondFieldHeight];
	for (int i = 0; i < secondFieldWidth; ++i)
		for (int j = 0; j < secondFieldHeight; ++j)
			secondField[j * secondFieldWidth + i] = (i == 0 || j == secondFieldHeight - 1 || i == secondFieldWidth - 1 || j == 0) ? 1 : 0;

}

int rotate(const int& i, const int& j, const int& currentRotation) {
	int res;
	switch (currentRotation % 4) {
		//0 degree
	case 0:
		res = j * 4 + i;
		break;
	case 1:
		res = 12 + j - (i * 4);
		break;
	case 2:
		res = 15 - (i * 4) - j;
		break;
	case 3:
		res = 3 - j + (i * 4);
		break;
	}
	return res;
}

void draw_field(char *screen) {
	
	for (int i = 0; i < fieldWidth; ++i)
		for (int j = 0; j < fieldHeight; ++j)
			screen[(j + 3) * screenWidth + (i + 7)] = " *o#"[field[j*fieldWidth + i]];
}

void second_draw_field(char *screen){

	for (int i = 0; i < secondFieldWidth; ++i)
		for (int j = 0; j < secondFieldHeight; ++j)
			screen[(j + 3) * screenWidth + (i + fieldWidth + 10)] = " *o"[secondField[j*secondFieldWidth + i]];
}

bool check_if_fits(const int& currentShape, const int& currentX, const int& currentY, int rotation,
					const std::vector<std::string>& shapes) {

	for(int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j) {
			if (shapes[currentShape][rotate(i, j, rotation)] == 'X')
				if (field[(currentY + j) * fieldWidth + (currentX +i)] != 0)
						return false;
		}
	return true;
}


int main() {

	char *screen = new char[screenWidth * screenHeight];
	for (int i = 0; i < screenWidth * screenHeight; ++i) screen[i] = ' ';
	HANDLE console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(console);
	unsigned long dwBytesWritten = 0;

	std::vector<std::string> shapes; 
	populate_vector(shapes);

	field_buffer();
	second_field_buffer();
	
	int bufferShape = rand() % 7;
	int currentShape = rand() % 7;
	int currentX = (fieldWidth - 3) / 2;
	int currentY = 0;
	int currentRotation = 0;
	int score = 0;

	bool key[4];
	bool keyFlag = false;

	int speed = 10;
	int speedcount = 0;

	std::vector<int> line;
	bool lineFlag = true;

	//Game Loop
	while (1) {


		//Game Delay
		std::this_thread::sleep_for(50ms);
		speedcount++;

		//Speed timing
		if (speedcount == speed) {
			speedcount = 0;
			currentY++;
		}

		//Input
		for (int i = 0; i < 4; ++i) {
			key[i] = ( 0b1000000000000000 & (GetAsyncKeyState((unsigned char)("DASW"[i])))) != 0;
		}
		currentX += (key[0] && check_if_fits(currentShape, currentX + 1, currentY, currentRotation, shapes)) ? 1 : 0;
		currentX -= (key[1] && check_if_fits(currentShape, currentX - 1, currentY, currentRotation, shapes)) ? 1 : 0;
		currentY += (key[2] && check_if_fits(currentShape, currentX, currentY + 1, currentRotation, shapes)) ? 1 : 0;

		if (key[3]) {
			currentRotation += ( !keyFlag && check_if_fits(currentShape, currentX, currentY + 1, currentRotation + 1, shapes)) ? 1 : 0;
			keyFlag = true;
		}
		else 
			keyFlag = false;

		//Check if line is full
		if (!check_if_fits(currentShape, currentX, currentY + 1, currentRotation, shapes)) {
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					if ((shapes[currentShape][rotate(i, j, currentRotation)] == 'X'))
						field[(currentY + j)* fieldWidth + (currentX + i)] = 2;

			for (int j = 0; j < 4; j++){
				lineFlag = true;
				for (int i = 0; i < fieldWidth; ++i) {
					if (field[(currentY + j) * fieldWidth + i] == 0) {
						lineFlag = false;
						break;
					}
				}
				if (lineFlag)
					if(currentY + j < fieldHeight-1) 
					line.push_back(currentY + j);
				}
			
			//Spawn next shape
			currentShape = bufferShape;
			bufferShape = rand() % 7;
			currentX = (fieldWidth - 3) / 2;
			currentY = 0;
			currentRotation = 0;

			//Close the Game
			if(!check_if_fits(currentShape, currentX, currentY, currentRotation, shapes)) break;

		}

		//clear line and add score 
		if (!line.empty()) {
			for (auto& iter : line)
				for (int i = 1; i < fieldWidth - 1; ++i) {
					for (int j = iter; j > 0; --j)
						field[j * fieldWidth + i] = field[(j - 1) * fieldWidth + i];
				}
			line.clear();
			score += 10;
		}
	
		//Draw Field
		draw_field(screen);
		second_draw_field(screen);

		//Draw Shape
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				if ((shapes[currentShape][rotate(i, j, currentRotation)] == 'X'))
					screen[(currentY + j + 3)*screenWidth + (currentX + i + 7)] = 111;

		//Draw second Shape
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				if ((shapes[bufferShape][j*4 + i] == 'X'))
					screen[(j + 4) * screenWidth + (i + fieldWidth + 11)] = 111;
		
		//Draw Score
		sprintf_s(&screen[2 * screenWidth + fieldWidth + 10], 16, "SCORE: %8d", score);

		//Display
		WriteConsoleOutputCharacterA(console, screen, screenWidth * screenHeight, { 0, 0 }, &dwBytesWritten);

	}
	//Display and of game and score
	CloseHandle(console);
	std::cout << "Game Over! Score: " << score << std::endl;
	system("PAUSE");
}

