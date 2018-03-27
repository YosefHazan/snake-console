// snake.cpp 

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <string.h>
#include <conio.h>
#include <fstream>
#include <ctime>
#include <time.h>
#include <stdio.h>      
#include <stdlib.h>     

using namespace std;

// VERSION 4
//
//TODO: VERSION 4.1
// load and saving // function - PrintPos will print string (not one 'char')
// control size of matrix // in build map move like the snake presses

struct ScoreBoards
{
	char name[11];
	int score;
};

//struct COORD
//{
	//short X;
	//short Y;
//};

const int COL_WORLD = 20;

//mix the rand function for randomal random
void RealRandom()
{
	int returnRand = ((clock() / CLOCKS_PER_SEC) % 2);
	for (returnRand; returnRand >= 0; returnRand--)
	{
		rand();
	}
}

//return true where the current location (loc) is wall
bool isWall(char matrix[][COL_WORLD], COORD loc, int valueWall, int valueSnake)
{
	bool iW = false;
	if ((matrix[loc.X][loc.Y] == valueWall) ||
		(matrix[loc.X][loc.Y] == valueSnake))
	{
		iW = true;
	}
	return (iW);
}

//put in frame of matrix the valueWall according to sizeWall
void ZeroingWall(char matrix[][COL_WORLD], int sizeRow, int sizeWall,
	char valueWall)
{
	//run according to sizeWall
	for (int runWall = 0; runWall < sizeWall; runWall++)
	{
		// run on row to put wall of coll
		for (int coll = 0; coll < sizeWall; coll++)
		{
			// right coll
			matrix[coll][runWall] = valueWall;
			// left coll
			matrix[coll][runWall + (COL_WORLD - sizeWall)] = valueWall;
		}

		// run on coll to put wall of row
		for (int row = 0; row < COL_WORLD; row++)
		{
			// up row
			matrix[runWall][row] = valueWall;
			// down row
			matrix[runWall + (COL_WORLD - sizeWall)][row] = valueWall;
		}
	}
}

// put in all matrix value of valueMatrix and put by sizeWall the valueWall
void ZeroingAllMatrix(char matrix[][COL_WORLD], int sizeRow, char valueMatrix, int sizeWall, int valueWall)
{
	for (int rowWall = 0; rowWall < sizeRow; rowWall++)
	{
		for (int colWall = 0; colWall < COL_WORLD; colWall++)
		{
			matrix[rowWall][colWall] = valueMatrix;
		}

		ZeroingWall(matrix, sizeRow, sizeWall, valueWall);
	}
}

// return new randomal location
COORD NewLocation(char matrix[][COL_WORLD], int sizeRow, int sizeWall, int empty)
{
	COORD NL;

	RealRandom();
	do
	{
		// put randomal location
		NL.X = rand() % (sizeRow - (sizeWall * 2)) + sizeWall;
		NL.Y = rand() % (COL_WORLD - (sizeWall * 2)) + sizeWall;

		// run til they empty location
	} while ((matrix[NL.X][NL.Y] != empty) && (matrix[NL.X][NL.Y] != '\0'));
	
	return(NL);
}

// print location specific
void PrintPos(HANDLE console, COORD pos, char value)
{
	SetConsoleCursorPosition(console, pos);
	cout << value;
}

// print the matrix
void Print(char matrix[][COL_WORLD], int sizeRow, int sizeWall, COORD pos)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	// if the size wall eqal zero put numner '2' (ist the corner of wall) //TODO: fix that commend
	bool isNeedPrintWall = (sizeWall == 0 ? true : false);
	int startMatrixPrint;
	int endMatrixRowPrint;
	int endMatrixColPrint;

	if (isNeedPrintWall)
	{ // initalization - dont print all matrix (need print the wall separately)
		startMatrixPrint = sizeWall;
		endMatrixRowPrint = sizeRow - sizeWall;
		endMatrixColPrint = COL_WORLD - sizeWall;
	}
	else
	{ // initalization - print all matrix (the wall is inside matrix)
		startMatrixPrint = 0;
		endMatrixRowPrint = sizeRow;
		endMatrixColPrint = COL_WORLD;
	}

	// initalization the display
	system("cls");
	SetConsoleTextAttribute(console, 27);
	SetConsoleCursorPosition(console, pos);

	// print the up wall
	for (int UPwall = 0; (UPwall < (sizeRow + 2)) && (isNeedPrintWall); UPwall++) // TODO: Fix that if
	{
		cout << '%';
	}

	// if it print the wall separately - drop one row
	if (isNeedPrintWall)
	{
		pos.Y++;
	}
	
	//print the matrix
	for (int row = startMatrixPrint; row < endMatrixRowPrint; row++)
	{
		// Left wall
		SetConsoleCursorPosition(console, { pos.X, pos.Y + (short)row });
		if (isNeedPrintWall)
		{
			cout << '%';
		}

		for (int col = startMatrixPrint; col < endMatrixColPrint; col++)
		{
			cout << matrix[col][row];
		}

		// Right wall
		if (isNeedPrintWall)
		{
			cout << '%';
		}
	}

	// Print down Wall
	SetConsoleCursorPosition(console, { pos.X, pos.Y + (short)(sizeRow - sizeWall) });
	for (int downWall = 0; (downWall < (sizeRow + 2)) && (isNeedPrintWall); downWall++)
	{
		cout << '%';
	}
}

// Return new location of snake's head after the function move it
COORD moving(int direc, COORD corntLocHead, int sizeRow, bool isWantCycle, int sizeWall)
{
	//                             left,    up,    down, right 
	const COORD DIRECTTION[4] = { {0,-1}, {-1,0}, {1,0}, {0,1} };
	COORD tryHeadMove = corntLocHead;

	//check the string of cycle
	if (!isWantCycle)
	{ // user dosn't want
		tryHeadMove.Y += DIRECTTION[direc].Y;
		tryHeadMove.X += DIRECTTION[direc].X;
	}
	else
	{ // user want cycle 
		tryHeadMove.Y = (tryHeadMove.Y + DIRECTTION[direc].Y + COL_WORLD) % COL_WORLD;
		tryHeadMove.X = (tryHeadMove.X + DIRECTTION[direc].X + sizeRow) % sizeRow;
	}

	return (tryHeadMove);
}

// return true when the file name is exist // TODO: is have system function?
bool isHaveFileName(char* fileName)
{
	bool iHFN = false;
	fstream file;

	file.open(fileName, ios::in);
	// check: is file open?
	if (file.is_open())
	{
		file.close();
		iHFN = true;
	}

	return (iHFN);
}

// Write the matrix to a file
void WriteFile(char matrix[][COL_WORLD], int sizeRow, char finalName[])
{
	const int MAX_LETTER = 10;
	// this is the size of string that save the all name of file (+ 1 for '\0'
	const int SIZE_OF_NAME = 1 + (int)strlen(finalName) + MAX_LETTER;

	fstream file;
	int YYcontinue;
	// dynamic alocation - the user put here the name for file
	char* fileName = new char[SIZE_OF_NAME];

	cin.ignore();
	cout << "Create map name (" << MAX_LETTER << " letter only)" << endl;
	cin.getline(fileName, MAX_LETTER);

	//check isn't name of file that exisiting
	while (isHaveFileName(fileName))
	{
		cout << "\nATTENTION !!! its run over exisiting file" << endl
			<< "						continue?" << endl
			<< " 1 - yes" << endl
			<< " 2 - no" << endl;
		cin >> YYcontinue;
		if (YYcontinue == 1)
		{ // user want run over
			break;
		}
		else
		{ // user isn't want run over
			cin.ignore();
			cout << "Creat map name (" << MAX_LETTER << " letter only)" << endl;
			cin.getline(fileName, MAX_LETTER);
			// add the finalName to fileName
			strcat_s(fileName, SIZE_OF_NAME, finalName);
		}
	}

	file.open(fileName, ios::out);
	// check the file is open
	if (!file.is_open())
	{
		cout << "Can't open map name : " << fileName << endl;
		cin.ignore();
	}
	else
	{
		// Write to file
		for (int i1 = 0; i1 < sizeRow; i1++)
		{
			for (int i2 = 0; i2 < COL_WORLD; i2++)
			{
				file << matrix[i1][i2];
			}
		}


		file << flush;
		file.close();

	}

	// Free the dynamic alocation
	delete[] fileName;
}

// return true if user want exit, show Menu Midst Game
bool MenuMidstGame()
{
	const int RETURN_TO_GAME = 0;
	const int EXIT = 1;
	const int SIZE = 20;
	const char COURENT_MARKER[4] = "-> ";
	const char NOT_MARKER[2] = " ";
	char menuMidstGame[2][4] = { " " };
	int inputMenu;
	int index = RETURN_TO_GAME;
	int choise;
	bool isNeadMore;
	bool MMG = false;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	// Initialzion
	strcpy_s(menuMidstGame[0], COURENT_MARKER);

	do
	{
		isNeadMore = true;
		do
		{
			SetConsoleTextAttribute(console, 07);
			system("cls");
			cout << "\n\n\t would you like to do ?" << endl
				<< "\t" << menuMidstGame[RETURN_TO_GAME] << "return to game" << endl
				<< "\t" << menuMidstGame[EXIT] << "exit" << endl;
			inputMenu = _getwch();

			// If user not press ENTER need the function again
			if ((inputMenu == 224) || (inputMenu == 0)) 
			{
				inputMenu = _getwch();
			}

			switch (inputMenu) // Calculecation of move key in menu
			{
			case(80): // up
			{
				// move up the marker
				strcpy_s(menuMidstGame[index], NOT_MARKER);
				// index = (index+step+vec_size)%vec_size
				index = (index + 1 + (EXIT + 1)) % (EXIT + 1);
				strcpy_s(menuMidstGame[index], COURENT_MARKER);
				break;
			}
			case(72):
			{
				// move down the marker
				strcpy_s(menuMidstGame[index], NOT_MARKER);
				// index = (index+step+vec_size)%vec_size
				index = (index + (-1) + (EXIT + 1)) % (EXIT + 1);
				strcpy_s(menuMidstGame[index], COURENT_MARKER);
				break;
			}
			case(77):
			case(75):
			case(13):
			{
				choise = index;
				isNeadMore = false;
				break;
			}
			default:break;
			}
		} while (isNeadMore);
		switch (choise) // switch of action
		{
		case(EXIT):
		{
			MMG = true;
			break;
		}
		default: break;
		}
	} while ((!MMG) && (index != RETURN_TO_GAME));

	return (MMG);
}

// Return true when user want exit and change the direction by user
bool ProssingPressing(int* direcNow, char matrix[][COL_WORLD],
	int sizeRow, int sizeWall, COORD pos, bool isChangeDirec)
{
	const int UP = 0;
	const int DOWN = 1;
	const int RIGHT = 2;
	const int LEFT = 3;
	const int MENU = '\r';

	bool isUserWantExit = false;
	int PrPr = *direcNow;
	int key;

	// check id key is press
	if (_kbhit())
	{ // its press 
		// Get the press of user
		key = _getwch();
		if (key == MENU) // input == ENTER
		{
			// Function, menu
			if (MenuMidstGame())
			{
				isUserWantExit = true;
			}
			else
			{ // Function, delete display menu and print the game agin 
				Print(matrix, sizeRow, sizeWall, pos);
			}
		}
		else
		{
			if (key == 224) // input == direction key
			{
				key = _getwch();
				key = (key - 70) / 3;

				if (isChangeDirec) // if obstacle now - replase the key
				{
					switch (key)
					{
					case(UP):
					{
						key = DOWN;
						break;
					}
					case(DOWN):
					{
						key = UP;
						break;
					}
					case(RIGHT):
					{
						key = LEFT;
						break;
					}
					case(LEFT):
					{
						key = RIGHT;
						break;
					}
					default: break;
					}
				}

				switch (key) // dont allow to user move back
				{
				case(UP):
				{
					if (*direcNow != DOWN)
					{
						PrPr = key;
					}
					break;
				}
				case(DOWN):
				{
					if (*direcNow != UP)
					{
						PrPr = key;
					}
					break;
				}
				case(RIGHT):
				{
					if (*direcNow != LEFT)
					{
						PrPr = key;
					}
					break;
				}
				case(LEFT):
				{
					if (*direcNow != RIGHT)
					{
						PrPr = key;
					}
					break;
				}
				default: break;
				}
			}
		}
	}

	*direcNow = PrPr;
	return(isUserWantExit);
}

//Prosses the cordination of sneak tail
void NewTail(COORD Head, COORD currentLocation[], int sizeSnakeNow)
{
	// move forward the cordination of snake tail
	for (int index = sizeSnakeNow; index > 0; index--)
	{
		currentLocation[index] = currentLocation[index - 1];
	}
	currentLocation[0] = Head;
}

// Read from file to matrix
void ReadFile(char map[][COL_WORLD], int sizeRow, char finalName[])
{
	const int MAX_LETTER = 10;
	const int SIZE_OF_NAME = 1 + (int)strlen(finalName) + MAX_LETTER;

	fstream file;
	// dynamic alocation - the user put here the name for file
	char* fileName = new char[SIZE_OF_NAME];

	cin.ignore(); // TODO: is Need?
	cout << "Enter map name (" << MAX_LETTER << " letter only)" << endl;
	cin.getline(fileName, MAX_LETTER);
	// add the finalName to fileName
	strcat_s(fileName, SIZE_OF_NAME, finalName);

	file.open(fileName, ios::in);
	// check: is file open?
	if (!file.is_open())
	{
		cout << "cannot open file name: " << fileName << endl;
		cin.ignore();
	}
	else
	{
		for (int i1 = 0; i1 < sizeRow; i1++)
		{
			for (int i2 = 0;(!file.eof()) && (i2 < COL_WORLD); i2++)
			{
				map[i1][i2] = file.get();
			}
		}

		file.close();
	}

	delete[] fileName; // free dynamic alocation
}

// Print score boards from file
void ReadScoreBoards(const char NAME[])
{
	fstream file;
	ScoreBoards scb;
	COORD pos = { 5,2 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	system("cls");
	file.open(NAME, ios::in | ios::binary);
	// check: is file open?
	if (!file.is_open()) // TODO: not real board and real check is file exist
 	{
		cout << "\n\n\n\t........................................" << endl
			<< "\t: Play any game for start score boards :" << endl
			<< "\t........................................" << endl;
	}
	else
	{ // File is Open
		for (int i = 1; i < !file.eof(); i++)
		{
			file.read((char*)&scb, sizeof(scb));
			pos.Y++;
			SetConsoleCursorPosition(console, pos);
			cout << i << ")\t" << scb.name << "\t : " << scb.score;
			SetConsoleTextAttribute(console, (i * 11)); // color of text
			pos.Y++;
		}

		file.close();
	}

	cin.ignore();
}

// Return true when function put the new high score in the score boards
bool WriteScoreBoards(const char FILE_NAME[], const int SCORE)
{
	const int MAX_NAME_IN_BOARD = 10;
	const int OLD_AND_NEW_SCORE = 11;
	ScoreBoards scb[OLD_AND_NEW_SCORE];
	ScoreBoards help;
	const int MAX_LETTER = (sizeof(scb[0].name) / sizeof(scb[0].name[0])) - 1;
	fstream file;
	bool isNewHighScore = false;
	int indexNewHighScore;

	// Initalzation
	for (int newScore = 0; newScore < OLD_AND_NEW_SCORE; newScore++)
	{
		strcpy_s(scb[newScore].name, "-----");
		scb[newScore].score = 0;
	}

	scb[OLD_AND_NEW_SCORE].score = 0;

	// Read the current score boards
	file.open(FILE_NAME, ios::in | ios::binary);

	// check if file open
	if (file.is_open())
	{
		// take the score from file
		for (int  i = 0; i < MAX_LETTER; i++)
		{
			file.read((char*)&scb[i], sizeof(scb[i]));
		}

		file.close();
	}

	// Sort the high score
	for (int board = OLD_AND_NEW_SCORE - 1; board > 0; board--)
	{
		// check if the score is high score
		if (scb[board - 1].score < scb[board].score)
		{
			help = scb[board];
			scb[board] = scb[board - 1];
			scb[board - 1] = help;
			isNewHighScore = true;
			indexNewHighScore = board - 1;
		}
	}

	// Check is new high score
	if (isNewHighScore)
	{
		// TODO: cin.ignore()
		cout << "\t Enter your name (" << MAX_LETTER << " letter only" << endl;
		cin.getline(scb[indexNewHighScore].name, MAX_LETTER);

		// Write to file the new score boards
		file.open(FILE_NAME, ios::out | ios::binary);

		// Check the file is open
		if (file.is_open())
		{
			// Put the score in file
			for (int i = 0; i <= (MAX_NAME_IN_BOARD -2); i++)
			{
				file.write((char*)&scb[i], sizeof(scb[i]));
			}
			
			file.close();
		}
	}

	return(isNewHighScore);
}

// The user put value in matrix and save that at file
void BuildMap(char map[][COL_WORLD], int sizeRow, int WallValue, int empty, COORD pos)
{
	const int FLAG = 999;
	COORD cordination;
	int userValue;
	pos.Y -= 2;

	Print(map, sizeRow, 0, pos); // Function, print the map

	cout << "\nAll the map that you build is a cycle map\n\n" << endl
		<< "Put empty (" << empty << ") or wall (" << WallValue << ")" << endl
		<< "If you want to exit enter - " << FLAG << endl;
	cin >> userValue;

	// Run til the user put exit
	while ((userValue == empty) || (userValue == WallValue))
	{
		cout << "Please enter the location of wall (X,Y)" << endl
			<< "the range that you can to select is - " << endl
			<< "row - 0 til " << sizeRow << endl
			<< "col - 0 til " << COL_WORLD << endl;
		cin >> cordination.X >> cordination.Y;

		// Check the user put correct value
		if ((cordination.X >= 0) || (cordination.X <= sizeRow) || 
			(cordination.Y >= 0) || (cordination.Y <= COL_WORLD))
		{ // Put the wall in the map 
			map[cordination.X][cordination.Y] = userValue;
		}
		else
		{
			cout << "you have mistake" << endl;
			cin.ignore();
		}

		Print(map, sizeRow, 0, pos); // Function, print the map

		// Run again
		cout << "Put empty (" << empty << ") or wall (" << WallValue << ")" << endl
			<< "If you want to exit enter - " << FLAG << endl;
		cin >> userValue;
	}
}

void LowerSpeed(int* speed) // put down the speed
{
	// Check if the speed isn't less from zero
	if (*speed > 0)
	{
		if (*speed >= 250)
		{ // In 25 first eating, the speed is bigger very mach
			*speed -= 10;
		}
		else
		{ // After 25 eating, the speed is more grow up, but not so much 
			*speed -= 5;
		}
	}
}

// Return false when timer out, timer for food
bool y_timer(clock_t startTime, COORD locationTime)
{
	COORD m; m.X = locationTime.X; m.Y = locationTime.Y;
	double secondsPassed;
	double secoundToDelay = 8;
	bool flag = true;

	// Calcolution of timer
	secondsPassed = (clock() - startTime) / CLOCKS_PER_SEC;

	// Chack is time out
	if (secondsPassed >=  secoundToDelay)
	{
		flag = false;
	}

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), m);
	cout << "time for food - " << secoundToDelay - secondsPassed;
	return(flag);
}

// Return true when the food is need to be by timer, select new problem in any eat
int ControlObstacle(int* speed, bool* isChangeDirec, COORD locationTime,
	clock_t* startTime, HANDLE console)
{
	COORD m; m.X = locationTime.X; m.Y = locationTime.Y;
	const int MIN_PROBLEM = 1;
	const int MAX_PROBLEM = 4;
	const COORD LOCATION_PROBLEM = { 9,30 };
	int problemCode;
	int randomal;
	int CO = 0;

	*isChangeDirec = false;
	RealRandom(); // function
	SetConsoleCursorPosition(console, LOCATION_PROBLEM);
	cout << "					" << endl;
	
	// get randomal problem
	problemCode = rand() % MAX_PROBLEM + MIN_PROBLEM;
	switch (problemCode)
	{
	case(1): // speed
	{
		// lower the speed by random
		randomal = rand() % MAX_PROBLEM + MIN_PROBLEM;
		for (int i = 0; i < randomal; i++)
		{
			LowerSpeed(speed);
		}

		SetConsoleCursorPosition(console, LOCATION_PROBLEM);
		cout << "High speed ! " << endl;
		break;
	}
	case(2): // direc
	{
		*isChangeDirec = true;
		SetConsoleCursorPosition(console, LOCATION_PROBLEM);
		cout << "trouble direction !" << endl;
		break;
	}
	case(3): // food
	{
		CO = 3;
		// Start time for food
		*startTime = clock();
		y_timer(*startTime, locationTime);
		break;
	}
	default: break;
	}
	return(CO);
}

COORD AddPosition(COORD one, COORD twoo, COORD three)
{
	COORD AP;

	AP.X = one.X + twoo.X + three.X;
	AP.Y = one.Y + twoo.Y + three.Y;
	return(AP);
}

// Return the Score of Snake when the gmae is over/finish. run the Game
int RunSnake(bool isWantCycle, bool isWantObstacle, COORD pos,
	char map[][COL_WORLD], int sizeRow)
{
	const int FOOD_GROW_UP = 2;
	const int ROW_MAP = sizeRow;
	const int SIZE_WALL = 1;
	const int MAX_TAIL = (ROW_MAP * COL_WORLD) / 2;
	const int FOOD_SCORE = 2;
	const char WALL = '#';
	const char FOOD = 3;
	const char EMPTY = ' ';
	const char SNAKE = '@';
	const int EXIT_GAME = 1;
	const int FILE_NAME_LETTER = 10;
	const int INDEX_HEAD = 0;
	const int USER_WANT_SAVE = -1;
	const int USER_WANT_EXIT = -2;
	const COORD LOC_TIME = { 10,30 };
	const COORD LOC_SPEED = { 8,28 };
	const COORD LOC_SCORE = { 8,27 };

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD posOfCycle = { -1,-1 };
	clock_t startTime;
	int counterFood = 0;
	int firstWhy;
	int direc;
	int speed = 500;
	int tailSize = 1;
	int obstacleCode = 0;
	int printSizeWall = SIZE_WALL;
	bool isChangeDirec = false;
	bool isFinishGame = false;
	bool isTooFast = false;
	COORD tryNewLocHead;
	COORD locLastTail;
	COORD currentLocFood;
	COORD* locTail = new COORD[MAX_TAIL];

	// Code section

	// If isn't wall inside matrix
	if (isWantCycle)
	{
		// Fix the coursor for start print
		posOfCycle.X = -1;
		posOfCycle.Y = -1;
		printSizeWall = -1;
	}

	// Set the color of console normaly
	SetConsoleTextAttribute(console, 7);

	// Put the snake in center
	tryNewLocHead.Y = (short)(COL_WORLD / 2);
	tryNewLocHead.X = (short)(COL_WORLD / 2);

	// If have wall in tryNewlocHead change the location
	if (map[tryNewLocHead.X][tryNewLocHead.Y] == WALL)
	{
		// Function, new location for snake
		tryNewLocHead = NewLocation(map, sizeRow, SIZE_WALL, EMPTY);
	}
	locTail[INDEX_HEAD] = tryNewLocHead;
	map[tryNewLocHead.X][tryNewLocHead.Y] = SNAKE;

	// Function, new loc for food
	currentLocFood = NewLocation(map, ROW_MAP, SIZE_WALL, EMPTY);
	map[currentLocFood.X][currentLocFood.Y] = FOOD;

	// - Let's play - 

	// Print the map for user
	Print(map, ROW_MAP, SIZE_WALL, pos);
	SetConsoleCursorPosition(console, LOC_SCORE);
	cout << "press any to start" << endl;
	pos.X++;
	pos.Y++;

	// Choise the moving way
	direc = 1;
	firstWhy = _getwch();
	// Chack the user pressed the right key
	if (firstWhy == 224)
	{
		firstWhy = _getwch();
		if ((firstWhy == 80) || (firstWhy == 72) ||
			(firstWhy == 77) || (firstWhy == 75))
		{
			direc = (firstWhy - 70) / 3;
		}
	}
	else if (firstWhy == 0)
	{
		_getwch();
	}

	// Function, get the new location for head by direction
	tryNewLocHead = moving(direc, locTail[INDEX_HEAD], ROW_MAP, isWantCycle, SIZE_WALL);

	// Check if the location of snake isn't wall
	while (!isWall(map, tryNewLocHead, WALL, SNAKE))
	{
		// Put the head in new location
		map[tryNewLocHead.X][tryNewLocHead.Y] = SNAKE;
		PrintPos(console, AddPosition(pos, tryNewLocHead, posOfCycle), SNAKE);

		// Print the change
		SetConsoleCursorPosition(console, LOC_SCORE);
		cout << "your score is - " << counterFood << "   " << endl;
		SetConsoleCursorPosition(console, LOC_SPEED);
		cout << "your speed is - " << speed << "  " << endl;

		//Check if the snake eat the food
		if ((tryNewLocHead.X == currentLocFood.X) &&
			(tryNewLocHead.Y == currentLocFood.Y))
		{
			counterFood++;
			// Put food in randomal location
			currentLocFood = NewLocation(map, ROW_MAP, SIZE_WALL, EMPTY);
			map[currentLocFood.X][currentLocFood.Y] = FOOD;

			// Check if user select obstacle
			if (isWantObstacle)
			{
				// Select random obstacle
				obstacleCode =
					ControlObstacle(&speed, &isChangeDirec, LOC_TIME, &startTime, console);
			}
			// Put down the speed
			LowerSpeed(&speed);
			// All second eat - tailSize++, but tail isn't can to be bigger form half of world 
			if (((counterFood% FOOD_GROW_UP) == 0) &&
				(tailSize <= MAX_TAIL))
			{
				tailSize++;
			}
			PrintPos(console, AddPosition(pos, currentLocFood, posOfCycle), FOOD);
		}

		// Put empty by last local of snake
		NewTail(tryNewLocHead, locTail, tailSize);
		locLastTail = locTail[tailSize];
		map[locLastTail.X][locLastTail.Y] = EMPTY;
		PrintPos(console, AddPosition(pos, locLastTail, posOfCycle), EMPTY);

		// Save the new location of head
		locTail[INDEX_HEAD] = tryNewLocHead;

		// Control of speed
		Sleep(speed);

		// Function, getting the user input and return true if user want exit
		if (ProssingPressing(&direc, map, ROW_MAP, printSizeWall,
			{ pos.X - 1, pos.Y - 1 }, isChangeDirec)) //TODO: FIX (pos - {1,1})
		{
			// Break the Game
			return(USER_WANT_EXIT);
		}

		// Move the snake forward
		tryNewLocHead =
			moving(direc, locTail[INDEX_HEAD], ROW_MAP, isWantCycle, SIZE_WALL);

		// Check timer of food
		if (obstacleCode == 3)
		{
			// Delete from display old timer
			SetConsoleCursorPosition(console, LOC_TIME);
			cout << "                  ";

			// Check is timer out 
			if (!y_timer(startTime, LOC_TIME))
			{
				// Put the food by randomal location
				map[currentLocFood.X][currentLocFood.Y] = EMPTY;
				PrintPos(console, AddPosition(pos, currentLocFood, posOfCycle), EMPTY);
				currentLocFood = NewLocation(map, ROW_MAP, SIZE_WALL, EMPTY);
				map[currentLocFood.X][currentLocFood.Y] = FOOD;
				PrintPos(console, AddPosition(pos, currentLocFood, posOfCycle), FOOD);
				startTime = clock();
			}
		}
	}

	system("cls");
	cout << "\n\n\n\t***********************\n" << endl
		<< "\t oops, you crush,,,\n <<" << endl
		<< "\t***************************\n" << endl;

	delete[] locTail;
	return(counterFood);
}

void main()
{
	const int SIZE_ROW = 20;
	const int SIZE_WALL = 1;
	const int MAX_TAIL = (SIZE_ROW * COL_WORLD) / 2;
	const char WALL = '#';
	const char EMPTY = ' ';
	const char SNAKE = '@';
	const char FINAL_FILE_MAP[] = ".snakemap";
	const char SCORE_BOARDS[] = "snake.score";

	// Menu const 
	const int START = 0;
	const int RESTART_GAME = 1;
	const int ENTER_MAP = 2;
	const int CHOOSE_MAP = 3;
	const int CYCLE = 4;
	const int OBSTACLE = 5;
	const int SCORE_BOARD = 6;
	const int EXIT = 7;
	const int SAVE_MAP = 1;
	const char COURENT_MARKER[] = "-> ";
	const char NOT_MARKER[] = "";
	const char ON[] = "on>";
	const char OFF[] = "<off";

	char map[SIZE_ROW][COL_WORLD] = { 0 };
	char keyGeneralMenu[9][4] = { "" };
	char onOrOffCycle[5] = { "" };
	char onOrOffObstacle[5] = { "" };
	char marker[4] = { "" };
	bool isWantCycle = true;
	bool isWantObstacle = false;
	bool isNeadLoopAgain;
	bool isUserMatrix = false;
	int index = START;
	int save;
	int input;
	int choise;
	int scoreOfPlayer;
	HWND console = GetConsoleWindow();
	RECT r;
	COORD BoardInCenter = { 8,4 };

	// Code section

	// Resize the console window
	GetWindowRect(console, &r);
	MoveWindow(console, 500, r.top, 450, 520, true);

	// Initalization
	strcpy_s(onOrOffCycle, ON);
	strcpy_s(onOrOffObstacle, OFF);
	strcpy_s(keyGeneralMenu[0], COURENT_MARKER);

	do
	{
		isNeadLoopAgain = true;
		do
		{
			// General Menu
			system("cls");
			system("Color A");
			cout << "\n\n\t WELCOME TO \"SNAKE\"" << endl
				<< '\t' << "would you like to do?\n" << endl
				<< '\t' << keyGeneralMenu[START] << "start" << endl
				<< '\t' << keyGeneralMenu[RESTART_GAME] << " restart the setting" << endl
				<< '\t' << keyGeneralMenu[ENTER_MAP] << " create new map" << endl
				<< '\t' << keyGeneralMenu[CHOOSE_MAP] << " choose map" << endl
				<< '\t' << keyGeneralMenu[CYCLE] << " cycle board " << onOrOffCycle << endl
				<< '\t' << keyGeneralMenu[OBSTACLE] << " obstacle " << onOrOffObstacle << endl
				<< '\t' << keyGeneralMenu[SCORE_BOARD] << " score board" << endl
				<< '\t' << keyGeneralMenu[EXIT] << " exit" << endl;
			input = _getwch();

			// if user not press ENTER need the function again
			if ((input == 224) || (input == 0))
			{
				input = _getwch();
			}

			switch (input) // Calculation of move key in menu
			{
			case(80): // UP
			{
				strcpy_s(keyGeneralMenu[index], NOT_MARKER);
				// Index = (index + step + vec_size) % vec_size
				index = (index + 1 + (EXIT + 1)) % (EXIT + 1);
				strcpy_s(keyGeneralMenu[index], COURENT_MARKER);
				break;
			}
			case(72): // DOWN
			{
				strcpy_s(keyGeneralMenu[index], NOT_MARKER);
				// Index = (index + step + vec_size) % vec_size
				index = (index + (-1) + (EXIT + 1)) % (EXIT + 1);
				strcpy_s(keyGeneralMenu[index], COURENT_MARKER);
				break;
			}
			case(77):
			case(75):
			case(13):
			{
				choise = index;
				isNeadLoopAgain = false;
				break;
			}
			default: break;
			}
		} while (isNeadLoopAgain);

		switch (choise) // Calculation of choise
		{
		case(START):
		{
			if (!isUserMatrix)
			{
				// Restart the map for new game
				if (isWantCycle)
				{ // Delete the wall from map
					ZeroingAllMatrix(map, SIZE_ROW, EMPTY, SIZE_WALL, EMPTY);
				}
				else
				{ // Put Wall in map 
					ZeroingAllMatrix(map, SIZE_ROW, EMPTY, SIZE_WALL, WALL);
				}
			}

			// Run the game
			scoreOfPlayer = RunSnake(isWantCycle, isWantObstacle, BoardInCenter, map, SIZE_ROW);

			// Check if the player dosn't reach score
			if (scoreOfPlayer > 0)
			{ // No - the user reach some point
				// Check if function put the score in board
				if (WriteScoreBoards((char*)SCORE_BOARDS,scoreOfPlayer))
				{
					// Print for user the score board
					ReadScoreBoards(SCORE_BOARDS);
				}
			}
			else
			{ // Yes - not reach point 
				cin.ignore(); // Wait for user attention
			}

			isUserMatrix = false;
			break;
		}
		case(RESTART_GAME):
		{
			isUserMatrix = false;
			// Zeroing all the map
			ZeroingAllMatrix(map, SIZE_ROW, EMPTY, SIZE_WALL, EMPTY);
			// Cycle setting
			isWantCycle = true;
			strcpy_s(onOrOffCycle, ON);
			// Obstacle setting
			isWantObstacle = false;
			strcpy_s(onOrOffObstacle, OFF);
			break;
		}
		case(ENTER_MAP):
		{
			// Function, the user make new map
			BuildMap(map, SIZE_ROW, WALL, EMPTY, BoardInCenter);
			cout << "If you want save this map press - " << SAVE_MAP << endl
				<< " Else press another number" << endl;
			cin >> save;

			if (save == SAVE_MAP)
			{
				// Function, save the map in file
				WriteFile(map, SIZE_ROW, (char*)FINAL_FILE_MAP);
			}

			isUserMatrix = true;
			break;
		}
		case(CHOOSE_MAP):
		{
			// Function, read map from file
			ReadFile(map, SIZE_ROW, (char*)FINAL_FILE_MAP);
			isUserMatrix = true;
			break;
		}
		case(CYCLE):
		{ // The first setting is isWantCycle == true, this case change that.
			
			if (isWantCycle)
			{ // Change the map to not cycle
				isWantCycle = false;
				strcpy_s(onOrOffCycle, OFF);

				// Put Wall in map
				ZeroingWall(map, SIZE_ROW, SIZE_WALL, WALL);
			}
			else
			{ // Change the map to cycle map
				isWantCycle = true;
				strcpy_s(onOrOffCycle, ON);

				// Put not Wall in map
				ZeroingWall(map, SIZE_ROW, SIZE_WALL, EMPTY);
			}

			break;
		}
		case(OBSTACLE):
		{ // The first setting is isWantObstacle == false, this case change that.
			if (isWantObstacle)
			{ // The user doesn't want obstacle
				isWantObstacle = false;
				strcpy_s(onOrOffObstacle, OFF);
			}
			else
			{ // The user want obstacle
				isWantObstacle = true;
				strcpy_s(onOrOffObstacle, ON);
			}

			break;
		}
		case(SCORE_BOARD): // Print the score board from file
		{
			ReadScoreBoards((char*)SCORE_BOARDS);
			break;
		}
		default: break;
		}

	} while (choise != EXIT);

	cout << "\n\n\t GOOD BYE, SEE YOU AGAIN SOON" << endl;
}

 
