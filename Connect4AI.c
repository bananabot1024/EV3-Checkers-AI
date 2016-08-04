#pragma config(Sensor, S3,     colorSensor,   sensorEV3_Color)
#pragma config(Motor,  motorA, verticalMotor, tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorB, leftMotor,     tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC, rightMotor,    tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorD, armMotor,      tmotorEV3_Large, PIDControl, encoder)

// define global array variable to store the state of the board
// 6 rows, 7 columns
// values: 0 for empty, 1 for player, 2 for computer
int board[6][7];
int boardMinimax[6][7];

// we don't need to differentiate between red or yellow
// because there will only be one new opponent piece every turn
#define colorMin 20

int numRobotMoves = 0;
int maxCol;

// define data structures to store the encoder values of certain positions
int sensorHorizontal[7] = {248, 354, 457, 557, 660, 762, 865};
int sensorVertical[6] = {467, 377, 287, 197, 102, 0};
int armHorizontal[7] = {356, 460, 560, 665, 772, 867, 970};
int armVertical[6] = {520, 515, 510, 510, 510, 510};

// global variables for checking 3 in a rows in minimax heuristic
// [0][x] = left row, [1][x] = left col, [2][x] = middle row, [3][x] = middle col, ...
int playerThrees[6][15];
// 'v' = vertical, 'h' = horizontal, 'b' = diagonal bottom left to top right, 't' = diagonal top left to bottom right
char playerType[15];
int robotThrees[6][15];
char robotType[15];
int numPlayerThrees = 0;
int numRobotThrees = 0;

void nextTurnSound()
{
	// beep
	playTone(100,30);
	while(bSoundActive)
		sleep(1);
}

void rotateArm()
{
	// rotate arm downwards to pick up piece
	setMotorTarget(armMotor, 50, 50);
	waitUntilMotorStop(armMotor);
	delay(200);
	setMotorTarget(armMotor, 120, 100);
	waitUntilMotorStop(armMotor);
	delay(200);
	// rotate arm back up to position 0
	setMotorTarget(armMotor, 0, 10);
	waitUntilMotorStop(armMotor);
}

void moveToLocation(int h, int v)
{
	// move horizontally
	setMotorTarget(leftMotor, h, 35);
	setMotorTarget(rightMotor, h, 35);
	waitUntilMotorStop(leftMotor);
	waitUntilMotorStop(rightMotor);
	// move vertically
	setMotorTarget(verticalMotor, v, 100);
	waitUntilMotorStop(verticalMotor);
}

void senseComputerPiece()
{
	while (true)
	{
		// if computer piece is sensed
		if (getColorReflected(S3) >= colorMin)
		{
			delay(1000);
			moveToLocation(110, 0);
			// pick up the chip
			rotateArm();
			break;
		}
	}
}

void playEndSound(int winner)
{
	if (winner == 1)
	{
		// play winning sound
		playSoundFile("/home/root/lms2012/prjs/rc/win2");
		sleep(2000);
	}
	else if (winner == 0){
		// play tie sound (beep 3 times)
		for (int i = 0; i < 3; i++)
		{
			playTone(100,30);
			while(bSoundActive)
				sleep(1);
			delay(300);
		}
	}
	else
	{
		// play losing sound
		playSoundFile("/home/root/lms2012/prjs/rc/lose");
		sleep(4000);
	}
}

int checkWinner()
{
	// check horizontal
	for (int row = 0; row <= 5; row++)
	{
		for (int colStart = 0; colStart <= 3; colStart++)
		{
			if (board[row][colStart] == 1 && board[row][colStart + 1] == 1 && board[row][colStart + 2] == 1 && board[row][colStart + 3] == 1)
			{
				return 1;
			}
			if (board[row][colStart] == 2 && board[row][colStart + 1] == 2 && board[row][colStart + 2] == 2 && board[row][colStart + 3] == 2)
			{
				return 2;
			}
		}
	}

	// check vertical
	for (int col = 0; col <= 6; col++)
	{
		for (int rowStart = 0; rowStart <= 2; rowStart++)
		{
			if (board[rowStart][col] == 1 && board[rowStart + 1][col] == 1 && board[rowStart + 2][col] == 1 && board[rowStart + 3][col] == 1)
			{
				return 1;
			}
			if (board[rowStart][col] == 2 && board[rowStart + 1][col] == 2 && board[rowStart + 2][col] == 2 && board[rowStart + 3][col] == 2)
			{
				return 2;
			}
		}
	}

	// check top left to bottom right diagonal
	for (int leftCol = 0; leftCol <= 3; leftCol++)
	{
		for (int topRow = 0; topRow <= 2; topRow++)
		{
			if (board[topRow][leftCol] == 1 && board[topRow + 1][leftCol + 1] == 1 && board[topRow + 2][leftCol + 2] == 1 && board[topRow + 3][leftCol + 3] == 1)
			{
				return 1;
			}
			if (board[topRow][leftCol] == 2 && board[topRow + 1][leftCol + 1] == 2 && board[topRow + 2][leftCol + 2] == 2 && board[topRow + 3][leftCol + 3] == 2)
			{
				return 2;
			}
		}
	}

	// check bottom left to top right diagonal
	for (int leftCol = 0; leftCol <= 3; leftCol++)
	{
		for (int bottomRow = 5; bottomRow >= 3; bottomRow--)
		{
			int a = bottomRow - 1, b = leftCol + 1, c = bottomRow - 2, d = leftCol + 2, e = bottomRow - 3, f = leftCol + 3;
			if (board[bottomRow][leftCol] == 1 && board[a][b] == 1 && board[c][d] == 1 && board[e][f] == 1)
			{
				return 1;
			}
			if (board[bottomRow][leftCol] == 2 && board[a][b] == 2 && board[c][d] == 2 && board[e][f] == 2)
			{
				return 2;
			}
		}
	}
	return 0;
}

// This is almost the exact same thing as checkWinner()
// but this is because RobotC doesn't allow you to pass arrays into functions
// so the easiest way to check if there is a winner for two different board arrays
// is to write a separate function for each array needed to be checked
int checkWinnerMinimax()
{
	// check horizontal
	for (int row = 0; row <= 5; row++)
	{
		for (int colStart = 0; colStart <= 3; colStart++)
		{
			if (boardMinimax[row][colStart] == 1 && boardMinimax[row][colStart + 1] == 1 && boardMinimax[row][colStart + 2] == 1 && boardMinimax[row][colStart + 3] == 1)
			{
				return 1;
			}
			if (boardMinimax[row][colStart] == 2 && boardMinimax[row][colStart + 1] == 2 && boardMinimax[row][colStart + 2] == 2 && boardMinimax[row][colStart + 3] == 2)
			{
				return 2;
			}
		}
	}

	// check vertical
	for (int col = 0; col <= 6; col++)
	{
		for (int rowStart = 0; rowStart <= 2; rowStart++)
		{
			if (boardMinimax[rowStart][col] == 1 && boardMinimax[rowStart + 1][col] == 1 && boardMinimax[rowStart + 2][col] == 1 && boardMinimax[rowStart + 3][col] == 1)
			{
				return 1;
			}
			if (boardMinimax[rowStart][col] == 2 && boardMinimax[rowStart + 1][col] == 2 && boardMinimax[rowStart + 2][col] == 2 && boardMinimax[rowStart + 3][col] == 2)
			{
				return 2;
			}
		}
	}

	// check top left to bottom right diagonal
	for (int leftCol = 0; leftCol <= 3; leftCol++)
	{
		for (int bottomRow = 5; bottomRow >= 3; bottomRow--)
		{
			int i = -3;
			if (boardMinimax[bottomRow][leftCol] == 1 && boardMinimax[bottomRow + 1][leftCol + 1] == 1 && boardMinimax[bottomRow + 2][leftCol + 2] == 1 && boardMinimax[bottomRow -i][leftCol + 3] == 1)
			{
				return 1;
			}
			if (boardMinimax[bottomRow][leftCol] == 2 && boardMinimax[bottomRow + 1][leftCol + 1] == 2 && boardMinimax[bottomRow + 2][leftCol + 2] == 2 && boardMinimax[bottomRow -i][leftCol + 3] == 2)
			{
				return 2;
			}
		}
	}

	// check bottom left to top right diagonal
	for (int leftCol = 0; leftCol <= 3; leftCol++)
	{
		for (int bottomRow = 5; bottomRow >= 3; bottomRow--)
		{
			if (boardMinimax[bottomRow][leftCol] == 1 && boardMinimax[bottomRow - 1][leftCol + 1] == 1 && boardMinimax[bottomRow - 2][leftCol + 2] == 1 && boardMinimax[bottomRow - 3][leftCol + 3] == 1)
			{
				return 1;
			}
			if (boardMinimax[bottomRow][leftCol] == 2 && boardMinimax[bottomRow - 1][leftCol + 1] == 2 && boardMinimax[bottomRow - 2][leftCol + 2] == 2 && boardMinimax[bottomRow - 3][leftCol + 3] == 2)
			{
				return 2;
			}
		}
	}
	return 0;
}

void findPlayerPiece()
{
	// loop through each bottom empty piece (possible locations of user move)
	for (int column = 0; column <= 6; column++)
	{
		for (int row = 5; row >= 0; row--)
		{
			if (board[row][column] == 0)
			{
				// move there
				moveToLocation(sensorHorizontal[column], sensorVertical[row]);
				// check if piece there
				if (getColorReflected(S3) >= colorMin)
				{
					board[row][column] = 1;
					return;
				}
				else
				{
					break;
				}
			}
		}
	}
}

int singleImmediatelyPlayable(int index, int who)
{
	//if player
	if (who == 1)
	{
		// coordinates
		// leftmost piece in the 3-in-a-row
		int first[2] = {playerThrees[0][index], playerThrees[1][index]};
		// middle piece
		int second[2] = {playerThrees[2][index], playerThrees[3][index]};
		// rightmost piece
		int third[2] = {playerThrees[4][index], playerThrees[5][index]};
		// test1 = piece on left to be checked
		int test1[2];
		// test2 = piece on right to be checked
		int test2[2];
		// 0: none, 1: left, 2: right, 3: both
		int key = 0;
		if (playerType[index] == 'h') {
			test1[0] = first[0] + 1;
			test1[1] = first[1] - 1;
			test2[0] = third[0] + 1;
			test2[1] = third[1] + 1;
			if (test1[0] <= 5 && test1[1] >= 0){
				if (boardMinimax[test1[0]][test1[1]] != 0)
				{
					key++;
				}
			}
			if (test2[0] <= 5 && test2[1] <= 6){
				if (boardMinimax[test2[0]][test2[1]]!=0)
				{
					key += 2;
				}
			}
		}
		else if (playerType[index] == 'v'){
			// for vertical 3 in a rows, if it is open, it will always be playable
			return 1;
		}
		else if (playerType[index] == 'b'){
			test1[0] = first[0] + 2;
			test1[1] = first[1] - 1;
			test2[0] = third[0];
			test2[1] = third[1] + 1;
			if (test1[0] <= 5 && test1[1] >= 0){
				if (boardMinimax[test1[0]][test1[1]] != 0)
				{
					key++;
				}
			}
			if (test2[1] <= 6 && test2[0] >= 0){
				if (boardMinimax[test2[0]][test2[1]] != 0)
				{
					key += 2;
				}
			}
		}
		else if (playerType[index] == 't'){
			test1[0] = first[0];
			test1[1] = first[1] - 1;
			test2[0] = third[0] + 2;
			test2[1] = third[1] + 1;
			if (test1[0] >= 0 && test1[1] >= 0){
				if (boardMinimax[test1[0]][test1[1]] != 0)
				{
					key++;
				}
			}
			if (test2[0] <= 5 && test2[1] <= 6){
				if (boardMinimax[test2[0]][test2[1]]!=0)
				{
					key += 2;
				}
			}
		}
	}
	//if robot
	if (who == 2)
	{
		// coordinates
		// leftmost piece in the 3-in-a-row
		int first[2] = {robotThrees[0][index], robotThrees[1][index]};
		// middle piece
		int second[2] = {robotThrees[2][index], robotThrees[3][index]};
		// rightmost piece
		int third[2] = {robotThrees[4][index], robotThrees[5][index]};
		// test1 = piece on left to be checked
		int test1[2];
		// test2 = piece on right to be checked
		int test2[2];
		// 0: none, 1: left, 2: right, 3: both
		int key = 0;
		if (robotType[index] == 'h'){
			test1[0] = first[0] + 1;
			test1[1] = first[1] - 1;
			test2[0] = third[0] + 1;
			test2[1] = third[1] + 1;
			if (test1[0] <= 5 && test1[1] >= 0){
				if (boardMinimax[test1[0]][test1[1]]!=0)
				{
					key++;
				}
			}
			if (test2[0] <= 5 && test2[1] <= 6){
				if (boardMinimax[test2[0]][test2[1]]!=0)
				{
					key += 2;
				}
			}
		}
		else if (robotType[index] == 'v'){
			// for vertical 3 in a rows, if it is open, it will always be playable
			return 1;
		}
		else if (robotType[index] == 'b'){
			test1[0] = first[0] + 2;
			test1[1] = first[1] - 1;
			test2[0] = third[0];
			test2[1] = third[1] + 1;
			if (test1[0] <= 5 && test1[1] >= 0){
				if (boardMinimax[test1[0]][test1[1]] != 0)
				{
					key++;
				}
			}
			if (test2[1] <= 6 && test2[0] >= 0){
				if (boardMinimax[test2[0]][test2[1]] != 0)
				{
					key += 2;
				}
			}
		}
		else if (robotType[index] == 't'){
			test1[0] = first[0];
			test1[1] = first[1] - 1;
			test2[0] = third[0] + 2;
			test2[1] = third[1] + 1;
			if (test1[0] >= 0 && test1[1] >= 0){
				if (boardMinimax[test1[0]][test1[1]] != 0)
				{
					key++;
				}
			}
			if (test2[0] <= 5 && test2[1] <= 6){
				if (boardMinimax[test2[0]][test2[1]]!=0)
				{
					key += 2;
				}
			}
		}
	}
	return key;
}

int singleOpen(int index, int who)
{
	// 0: neither, 1: left or top, 2: right or bottom, 3: both
	//vert is always 1 or 0
	int key = 0;
	// if player
	if (who == 1)
	{
		if (playerType[index] == 'h')
		{
			// if in bounds
			if (playerThrees[1][index] > 0)
			{
				// if left side open
				if (boardMinimax[playerThrees[0][index]][playerThrees[1][index] - 1] == 0)
				{
					key++;
				}
			}
			// if in bounds
			if (playerThrees[1][index] < 6)
			{
				// if right side open
				if (boardMinimax[playerThrees[0][index]][playerThrees[5][index] + 1] == 0)
				{
					key+=2;
				}
			}
		}
		else if (playerType[index] == 'v')
		{
			// if in bounds
			if (playerThrees[0][index] > 0)
			{
				// if top open
				if (boardMinimax[playerThrees[0][index] - 1][index] == 0)
				{
					key++;
				}
			}
		}
		else if (playerType[index] == 'b')
		{
			// if in bounds
			if (playerThrees[0][index] < 5 && playerThrees[1][index] > 0)
			{
				// if bottom left side open
				if (boardMinimax[playerThrees[0][index] + 1][playerThrees[1][index] - 1] == 0)
				{
					key++;
				}
			}
			// if in bounds
			if (playerThrees[4][index] > 0 && playerThrees[5][index] < 6)
			{
				// if top right side open
				if (boardMinimax[playerThrees[4][index] - 1][playerThrees[5][index] + 1] == 0)
				{
					key+=2;
				}
			}
		}
		else if (playerType[index] == 't')
		{
			// if in bounds
			if (playerThrees[0][index] > 0 && playerThrees[1][index] > 0)
			{
				// if top left side open
				if (boardMinimax[playerThrees[0][index] - 1][playerThrees[1][index] - 1] == 0)
				{
					key++;
				}
			}
			// if in bounds
			if (playerThrees[4][index] < 5 && playerThrees[5][index] < 6)
			{
				// if bottom right side open
				if (boardMinimax[playerThrees[4][index] + 1][playerThrees[5][index] + 1] == 0)
				{
					key+=2;
				}
			}
		}
	}
	// if robot
	else if (who == 2)
	{
		if (robotType[index] == 'h')
		{
			// if in bounds
			if (robotThrees[1][index] > 0)
			{
				// if left side open
				if (boardMinimax[robotThrees[0][index]][robotThrees[1][index] - 1] == 0)
				{
					key++;
				}
			}
			// if in bounds
			if (robotThrees[1][index] < 6)
			{
				// if right side open
				if (boardMinimax[robotThrees[0][index]][robotThrees[5][index] + 1] == 0)
				{
					key+=2;
				}
			}
		}
		else if (robotType[index] == 'v')
		{
			// if in bounds
			if (robotThrees[0][index] > 0)
			{
				// if top open
				if (boardMinimax[robotThrees[0][index] - 1][index] == 0)
				{
					key++;
				}
			}
		}
		else if (robotType[index] == 'b')
		{
			// if in bounds
			if (robotThrees[0][index] < 5 && robotThrees[1][index] > 0)
			{
				// if bottom left side open
				if (boardMinimax[robotThrees[0][index] + 1][robotThrees[1][index] - 1] == 0)
				{
					key++;
				}
			}
			// if in bounds
			if (robotThrees[4][index] > 0 && robotThrees[5][index] < 6)
			{
				// if top right side open
				if (boardMinimax[robotThrees[4][index] - 1][robotThrees[5][index] + 1] == 0)
				{
					key+=2;
				}
			}
		}
		else if (robotType[index] == 't')
		{
			// if in bounds
			if (robotThrees[0][index] > 0 && robotThrees[1][index] > 0)
			{
				// if top left side open
				if (boardMinimax[robotThrees[0][index] - 1][robotThrees[1][index] - 1] == 0)
				{
					key++;
				}
			}
			// if in bounds
			if (robotThrees[4][index] < 5 && robotThrees[5][index] < 6)
			{
				// if bottom right side open
				if (boardMinimax[robotThrees[4][index] + 1][robotThrees[5][index] + 1] == 0)
				{
					key+=2;
				}
			}
		}
	}
	return key;
}

int doubleOpen(int i, int j, int who)
{
	int type1 = singleOpen(i, who);
	int type2 = singleOpen(j, who);
	// check left
	if ((type1 == 1 || type1 == 3) && (type2 == 1 || type2 == 3))
	{
		return true;
	}
	// check right
	if ((type1 == 2 || type2 == 3) && (type2 == 2 || type2 == 3))
	{
		return true;
	}
	return false;
}

// "score" assigned to board configuration depending on how beneficial the move is
int minimaxHeuristic()
{
	bool minimaxVisited[6][7];
	int score = 0;
	// check 4 in a row
	// if opponent won
	if (checkWinnerMinimax() == 1)
	{
		return -10000;
	}
	// if robot won
	if (checkWinnerMinimax() == 2)
	{
		return 10000;
	}
	// 3 in a rows
	// [0][x] = left row, [1][x] = left col, [2][x] = middle row, [3][x] = middle col, ...
	// reset arrays
	for (int i = 0; i < 15; i++)
	{
		for (int j = 0; j < 6; j++)
		{
			playerThrees[j][i] = 0;
			robotThrees[j][i] = 0;
		}
	}
	// 'v' = vertical, 'h' = horizontal, 'b' = diagonal bottom left to top right, 't' = diagonal top left to bottom right
	for (int i = 0; i < 15; i++)
	{
		playerType[i] = ' ';
		robotType[i] = ' ';
	}
	numPlayerThrees = 0;
	numRobotThrees = 0;
	// horizontal
	for (int r = 5; r>= 0; r--)
	{
		for (int c = 0; c <= 4; c++)
		{
			// if player 3 in a row
			if (boardMinimax[r][c] == 1 && boardMinimax[r][c+1] == 1 && boardMinimax[r][c+2] == 1)
			{
				playerThrees[0][numPlayerThrees] = r;
				playerThrees[1][numPlayerThrees] = c;
				playerThrees[2][numPlayerThrees] = r;
				playerThrees[3][numPlayerThrees] = c+1;
				playerThrees[4][numPlayerThrees] = r;
				playerThrees[5][numPlayerThrees] = c+2;
				playerType[numPlayerThrees] = 'h';
				numPlayerThrees++;
			}
			// if robot 3 in a row
			if (boardMinimax[r][c] == 2 && boardMinimax[r][c+1] == 2 && boardMinimax[r][c+2] == 2)
			{
				robotThrees[0][numRobotThrees] = r;
				robotThrees[1][numRobotThrees] = c;
				robotThrees[2][numRobotThrees] = r;
				robotThrees[3][numRobotThrees] = c+1;
				robotThrees[4][numRobotThrees] = r;
				robotThrees[5][numRobotThrees] = c+2;
				robotType[numRobotThrees] = 'h';
				numRobotThrees++;
			}
		}
	}
	// vertical
	for (int c = 0; c<=6; c++)
	{
		for (int r = 0; r<=3; r++)
		{
			// if player 3 in a row
			if (boardMinimax[r][c] == 1 && boardMinimax[r+1][c] == 1 && boardMinimax[r+2][c] == 1)
			{
				playerThrees[0][numPlayerThrees] = r;
				playerThrees[1][numPlayerThrees] = c;
				playerThrees[2][numPlayerThrees] = r+1;
				playerThrees[3][numPlayerThrees] = c;
				playerThrees[4][numPlayerThrees] = r+2;
				playerThrees[5][numPlayerThrees] = c;
				playerType[numPlayerThrees] = 'v';
				numPlayerThrees++;
			}
			// if robot 3 in a row
			if (boardMinimax[r][c] == 2 && boardMinimax[r+1][c] == 2 && boardMinimax[r+2][c] == 2)
			{
				robotThrees[0][numRobotThrees] = r;
				robotThrees[1][numRobotThrees] = c;
				robotThrees[2][numRobotThrees] = r+1;
				robotThrees[3][numRobotThrees] = c;
				robotThrees[4][numRobotThrees] = r+2;
				robotThrees[5][numRobotThrees] = c;
				robotType[numRobotThrees] = 'v';
				numRobotThrees++;
			}
		}
	}
	// diagonals (bottom left to top right)
	for (int r = 5; r >= 2; r--)
	{
		for (int c = 0; c<=4; c++)
		{
			// if player 3 in a row
			if (boardMinimax[r][c] == 1 && boardMinimax[r-1][c+1] == 1 && boardMinimax[r-2][c+2] == 1)
			{
				playerThrees[0][numPlayerThrees] = r;
				playerThrees[1][numPlayerThrees] = c;
				playerThrees[2][numPlayerThrees] = r-1;
				playerThrees[3][numPlayerThrees] = c+1;
				playerThrees[4][numPlayerThrees] = r-2;
				playerThrees[5][numPlayerThrees] = c+2;
				playerType[numPlayerThrees] = 'b';
				numPlayerThrees++;
			}
			// if robot 3 in a row
			if (boardMinimax[r][c] == 2 && boardMinimax[r-1][c+1] == 2 && boardMinimax[r-2][c+2] == 2)
			{
				robotThrees[0][numRobotThrees] = r;
				robotThrees[1][numRobotThrees] = c;
				robotThrees[2][numRobotThrees] = r-1;
				robotThrees[3][numRobotThrees] = c+1;
				robotThrees[4][numRobotThrees] = r-2;
				robotThrees[5][numRobotThrees] = c+2;
				robotType[numRobotThrees] = 'b';
				numRobotThrees++;
			}
		}
	}
	// diagonals (top left to bottom right)
	for (int r = 0; r<=3; r++)
	{
		for (int c = 0; c<=4; c++)
		{
			// if player 3 in a row
			if (boardMinimax[r][c] == 1 && boardMinimax[r+1][c+1] == 1 && boardMinimax[r+2][c+2] == 1)
			{
				playerThrees[0][numPlayerThrees] = r;
				playerThrees[1][numPlayerThrees] = c;
				playerThrees[2][numPlayerThrees] = r-1;
				playerThrees[3][numPlayerThrees] = c+1;
				playerThrees[4][numPlayerThrees] = r-2;
				playerThrees[5][numPlayerThrees] = c+2;
				playerType[numPlayerThrees] = 't';
				numPlayerThrees++;
			}
			// if robot 3 in a row
			if (boardMinimax[r][c] == 2 && boardMinimax[r+1][c+1] == 2 && boardMinimax[r+2][c+2] == 2)
			{
				robotThrees[0][numRobotThrees] = r;
				robotThrees[1][numRobotThrees] = c;
				robotThrees[2][numRobotThrees] = r+1;
				robotThrees[3][numRobotThrees] = c+1;
				robotThrees[4][numRobotThrees] = r+2;
				robotThrees[5][numRobotThrees] = c+2;
				robotType[numRobotThrees] = 't';
				numRobotThrees++;
			}
		}
	}
	// check 3s (double threats) for player
	for (int i = 0; i < numPlayerThrees; i++)
	{
		if (playerType[i] == 'h')
		{
			if (playerThrees[1][i] > 0)
			{
				if (boardMinimax[playerThrees[0][i]][playerThrees[1][i] - 1] == 0)
				{
					boardMinimax[playerThrees[0][i]][playerThrees[1][i] - 1] = 3;
				}
			}
			if (playerThrees[5][i] < 6)
			{
				if (boardMinimax[playerThrees[4][i]][playerThrees[5][i] + 1] == 0)
				{
					boardMinimax[playerThrees[4][i]][playerThrees[5][i] + 1] = 3;
				}
			}
		}
		else if (playerType[i] == 'v')
		{
			if (playerThrees[0][i] > 0)
			{
				if (boardMinimax[playerThrees[0][i] - 1][playerThrees[1][i]] == 0)
				{
					boardMinimax[playerThrees[0][i] - 1][playerThrees[1][i]] = 3;
				}
			}
			if (playerThrees[4][i] < 5)
			{
				if (boardMinimax[playerThrees[4][i] + 1][playerThrees[5][i]] == 0)
				{
					boardMinimax[playerThrees[4][i] + 1][playerThrees[5][i]] = 3;
				}
			}
		}
		else if (playerType[i] == 'b')
		{
			if (playerThrees[0][i] < 5 && playerThrees[1][i] > 0)
			{
				if (boardMinimax[playerThrees[0][i] + 1][playerThrees[1][i] - 1] == 0)
				{
					boardMinimax[playerThrees[0][i] + 1][playerThrees[1][i] - 1] = 3;
				}
			}
			if (playerThrees[4][i] > 0 && playerThrees[5][i] < 6)
			{
				if (boardMinimax[playerThrees[4][i] - 1][playerThrees[5][i] + 1] == 0)
				{
					boardMinimax[playerThrees[4][i] - 1][playerThrees[5][i] + 1] = 3;
				}
			}
		}
		else if (playerType[i] == 't')
		{
			if (playerThrees[0][i] > 0 && playerThrees[1][i] > 0)
			{
				if (boardMinimax[playerThrees[0][i] - 1][playerThrees[1][i] - 1] == 0)
				{
					boardMinimax[playerThrees[0][i] - 1][playerThrees[1][i] - 1] = 3;
				}
			}
			if (playerThrees[4][i] < 5 && playerThrees[5][i] < 6)
			{
				if (boardMinimax[playerThrees[4][i] + 1][playerThrees[5][i] + 1] == 0)
				{
					boardMinimax[playerThrees[4][i] + 1][playerThrees[5][i] + 1] = 3;
				}
			}
		}
	}
	// check for any two hotspots (3s) one right above the other
	for (int r = 0; r <= 4; r++)
	{
		for (int c = 0; c <= 6; c++)
		{
			// if so, decrement score by 850 (the remaining 150 is decremented later when counting pairs of 3s)
			// these 3s will also be marked visited then, so no need to mark visited now
			if (boardMinimax[r][c] == 3 && boardMinimax[r + 1][c] == 3)
			{
				score -= 850;
				// if immediately playable, increment score by another 1000
				if (r == 4 || boardMinimax[r + 2][c] != 0)
				{
					score -= 1000;
				}
			}
		}
	}
	// reset 3s to 0s
	for (int r = 0; r <= 5; r++)
	{
		for (int c = 0; c <= 6; c++)
		{
			if (boardMinimax[r][c] == 3)
			{
				boardMinimax[r][c] = 0;
			}
		}
	}
	// check pairs of 3s
	for (int i = 0; i < numPlayerThrees; i++)
	{
		for (int j = i + 1; j < numPlayerThrees; j++)
		{
			// if any intersection
			int a[3][2], b[3][2];
			a[0][0] = playerThrees[0][i];
			a[0][1] = playerThrees[1][i];
			a[1][0] = playerThrees[2][i];
			a[1][1] = playerThrees[3][i];
			a[2][0] = playerThrees[4][i];
			a[2][1] = playerThrees[5][i];
			b[0][0] = playerThrees[0][j];
			b[0][1] = playerThrees[1][j];
			b[1][0] = playerThrees[2][j];
			b[1][1] = playerThrees[3][j];
			b[2][0] = playerThrees[4][j];
			b[2][1] = playerThrees[5][j];
			for (int m = 0; m < 3; m++)
			{
				for (int n = 0; n < 3; n++)
				{
					if (a[m][0] == b[n][0] && a[m][1] == b[n][1])
					{
						// check if open (at least one side each)
						if (singleOpen(i, 1) > 0 && singleOpen(j, 1) > 0)
						{
							score -= 150;
							// mark as visited
							minimaxVisited[playerThrees[0][i]][playerThrees[1][i]] = true;
							minimaxVisited[playerThrees[2][i]][playerThrees[3][i]] = true;
							minimaxVisited[playerThrees[4][i]][playerThrees[5][i]] = true;
							minimaxVisited[playerThrees[0][j]][playerThrees[1][j]] = true;
							minimaxVisited[playerThrees[2][j]][playerThrees[3][j]] = true;
							minimaxVisited[playerThrees[4][j]][playerThrees[5][j]] = true;
						}
						// check if immediately playable (at least 1 side each)
						if (singleImmediatelyPlayable(i, 1) > 0 && singleImmediatelyPlayable(j, 1) > 0)
						{
							score -= 150;
						}
					}
				}
			}
		}
	}
	// check 3s (double threats for robot)
		for (int i = 0; i < numRobotThrees; i++)
	{
		if (robotType[i] == 'h')
		{
			if (robotThrees[1][i] > 0)
			{
				if (boardMinimax[robotThrees[0][i]][robotThrees[1][i] - 1] == 0)
				{
					boardMinimax[robotThrees[0][i]][robotThrees[1][i] - 1] = 3;
				}
			}
			if (robotThrees[5][i] < 6)
			{
				if (boardMinimax[robotThrees[4][i]][robotThrees[5][i] + 1] == 0)
				{
					boardMinimax[robotThrees[4][i]][robotThrees[5][i] + 1] = 3;
				}
			}
		}
		else if (robotType[i] == 'v')
		{
			if (robotThrees[0][i] > 0)
			{
				if (boardMinimax[robotThrees[0][i] - 1][robotThrees[1][i]] == 0)
				{
					boardMinimax[robotThrees[0][i] - 1][robotThrees[1][i]] = 3;
				}
			}
			if (robotThrees[4][i] < 5)
			{
				if (boardMinimax[robotThrees[4][i] + 1][robotThrees[5][i]] == 0)
				{
					boardMinimax[robotThrees[4][i] + 1][robotThrees[5][i]] = 3;
				}
			}
		}
		else if (robotType[i] == 'b')
		{
			if (robotThrees[0][i] < 5 && robotThrees[1][i] > 0)
			{
				if (boardMinimax[robotThrees[0][i] + 1][robotThrees[1][i] - 1] == 0)
				{
					boardMinimax[robotThrees[0][i] + 1][robotThrees[1][i] - 1] = 3;
				}
			}
			if (robotThrees[4][i] > 0 && robotThrees[5][i] < 6)
			{
				if (boardMinimax[robotThrees[4][i] - 1][robotThrees[5][i] + 1] == 0)
				{
					boardMinimax[robotThrees[4][i] - 1][robotThrees[5][i] + 1] = 3;
				}
			}
		}
		else if (robotType[i] == 't')
		{
			if (robotThrees[0][i] > 0 && robotThrees[1][i] > 0)
			{
				if (boardMinimax[robotThrees[0][i] - 1][robotThrees[1][i] - 1] == 0)
				{
					boardMinimax[robotThrees[0][i] - 1][robotThrees[1][i] - 1] = 3;
				}
			}
			if (robotThrees[4][i] < 5 && robotThrees[5][i] < 6)
			{
				if (boardMinimax[robotThrees[4][i] + 1][robotThrees[5][i] + 1] == 0)
				{
					boardMinimax[robotThrees[4][i] + 1][robotThrees[5][i] + 1] = 3;
				}
			}
		}
	}
	// check for any two hotspots (3s) one right above the other
	for (int r = 0; r <= 4; r++)
	{
		for (int c = 0; c <= 6; c++)
		{
			// if so, decrement score by 850 (the remaining 150 is decremented later when counting pairs of 3s)
			// these 3s will also be marked visited then, so no need to mark visited now
			if (boardMinimax[r][c] == 3 && boardMinimax[r + 1][c] == 3)
			{
				score += 850;
				// if immediately playable, increment score by another 1000
				if (r == 4 || boardMinimax[r + 2][c] != 0)
				{
					score += 1000;
				}
			}
		}
	}
	// reset 3s to 0s
	for (int r = 0; r <= 5; r++)
	{
		for (int c = 0; c <= 6; c++)
		{
			if (boardMinimax[r][c] == 3)
			{
				boardMinimax[r][c] = 0;
			}
		}
	}
	// check pairs of 3s
	for (int i = 0; i < numRobotThrees; i++)
	{
		for (int j = i + 1; j < numRobotThrees; j++)
		{
			// if any intersection
			int a[3][2], b[3][2];
			a[0][0] = robotThrees[0][i];
			a[0][1] = robotThrees[1][i];
			a[1][0] = robotThrees[2][i];
			a[1][1] = robotThrees[3][i];
			a[2][0] = robotThrees[4][i];
			a[2][1] = robotThrees[5][i];
			b[0][0] = robotThrees[0][j];
			b[0][1] = robotThrees[1][j];
			b[1][0] = robotThrees[2][j];
			b[1][1] = robotThrees[3][j];
			b[2][0] = robotThrees[4][j];
			b[2][1] = robotThrees[5][j];
			for (int m = 0; m < 3; m++)
			{
				for (int n = 0; n < 3; n++)
				{
					if (a[m][0] == b[n][0] && a[m][1] == b[n][1])
					{
						// check if open (at least one side each)
						if (singleOpen(i, 1) > 0 && singleOpen(j, 1) > 0)
						{
							score += 150;
							// mark as visited
							minimaxVisited[robotThrees[0][i]][robotThrees[1][i]] = true;
							minimaxVisited[robotThrees[2][i]][robotThrees[3][i]] = true;
							minimaxVisited[robotThrees[4][i]][robotThrees[5][i]] = true;
							minimaxVisited[robotThrees[0][j]][robotThrees[1][j]] = true;
							minimaxVisited[robotThrees[2][j]][robotThrees[3][j]] = true;
							minimaxVisited[robotThrees[4][j]][robotThrees[5][j]] = true;
						}
						// check if immediately playable (at least 1 side each)
						if (singleImmediatelyPlayable(i, 1) > 0 && singleImmediatelyPlayable(j, 1) > 0)
						{
							score += 150;
						}
					}
				}
			}
		}
	}
	for (int i = 0; i < numRobotThrees; i++)
	{
		if (robotType[i] == 'h')
		{
			if (robotThrees[1][i] > 0)
			{
				if (boardMinimax[robotThrees[0][i]][robotThrees[1][i] - 1] == 0)
				{
					boardMinimax[robotThrees[0][i]][robotThrees[1][i] - 1] = 3;
				}
			}
			if (robotThrees[5][i] < 6)
			{
				if (boardMinimax[robotThrees[4][i]][robotThrees[5][i] + 1] == 0)
				{
					boardMinimax[robotThrees[4][i]][robotThrees[5][i] + 1] = 3;
				}
			}
		}
		else if (robotType[i] == 'v')
		{
			if (robotThrees[0][i] > 0)
			{
				if (boardMinimax[robotThrees[0][i] - 1][robotThrees[1][i]] == 0)
				{
					boardMinimax[robotThrees[0][i] - 1][robotThrees[1][i]] = 3;
				}
			}
			if (robotThrees[4][i] < 5)
			{
				if (boardMinimax[robotThrees[4][i] + 1][robotThrees[5][i]] == 0)
				{
					boardMinimax[robotThrees[4][i] + 1][robotThrees[5][i]] = 3;
				}
			}
		}
		else if (robotType[i] == 'b')
		{
			if (robotThrees[0][i] < 5 && robotThrees[1][i] > 0)
			{
				if (boardMinimax[robotThrees[0][i] + 1][robotThrees[1][i] - 1] == 0)
				{
					boardMinimax[robotThrees[0][i] + 1][robotThrees[1][i] - 1] = 3;
				}
			}
			if (robotThrees[4][i] > 0 && robotThrees[5][i] < 6)
			{
				if (boardMinimax[robotThrees[4][i] - 1][robotThrees[5][i] + 1] == 0)
				{
					boardMinimax[robotThrees[4][i] - 1][robotThrees[5][i] + 1] = 3;
				}
			}
		}
		else if (robotType[i] == 't')
		{
			if (robotThrees[0][i] > 0 && robotThrees[1][i] > 0)
			{
				if (boardMinimax[robotThrees[0][i] - 1][robotThrees[1][i] - 1] == 0)
				{
					boardMinimax[robotThrees[0][i] - 1][robotThrees[1][i] - 1] = 3;
				}
			}
			if (robotThrees[4][i] < 5 && robotThrees[5][i] < 6)
			{
				if (boardMinimax[robotThrees[4][i] + 1][robotThrees[5][i] + 1] == 0)
				{
					boardMinimax[robotThrees[4][i] + 1][robotThrees[5][i] + 1] = 3;
				}
			}
		}
	}
	// check for any two hotspots (3s) one right above the other
	for (int r = 0; r <= 4; r++)
	{
		for (int c = 0; c <= 6; c++)
		{
			// if so, decrement score by 850 (the remaining 150 is decremented later when counting pairs of 3s)
			// these 3s will also be marked visited then, so no need to mark visited now
			if (boardMinimax[r][c] == 3 && boardMinimax[r + 1][c] == 3)
			{
				score += 850;
				// if immediately playable, increment score by another 1000
				if (r == 4 || boardMinimax[r + 2][c] != 0)
				{
					score += 1000;
				}
			}
		}
	}
	// reset 3s to 0s
	for (int r = 0; r <= 5; r++)
	{
		for (int c = 0; c <= 6; c++)
		{
			if (boardMinimax[r][c] == 3)
			{
				boardMinimax[r][c] = 0;
			}
		}
	}
	// check pairs of 3s
	for (int i = 0; i < numRobotThrees; i++)
	{
		for (int j = i + 1; j < numRobotThrees; j++)
		{
			// if any intersection
			int a[3][2], b[3][2];
			a[0][0] = robotThrees[0][i];
			a[0][1] = robotThrees[1][i];
			a[1][0] = robotThrees[2][i];
			a[1][1] = robotThrees[3][i];
			a[2][0] = robotThrees[4][i];
			a[2][1] = robotThrees[5][i];
			b[0][0] = robotThrees[0][j];
			b[0][1] = robotThrees[1][j];
			b[1][0] = robotThrees[2][j];
			b[1][1] = robotThrees[3][j];
			b[2][0] = robotThrees[4][j];
			b[2][1] = robotThrees[5][j];
			for (int m = 0; m < 3; m++)
			{
				for (int n = 0; n < 3; n++)
				{
					if (a[m][0] == b[n][0] && a[m][1] == b[n][1])
					{
						// check if open (at least one side each)
						if (singleOpen(i, 1) > 0 && singleOpen(j, 1) > 0)
						{
							score += 150;
							// mark as visited
							minimaxVisited[robotThrees[0][i]][robotThrees[1][i]] = true;
							minimaxVisited[robotThrees[2][i]][robotThrees[3][i]] = true;
							minimaxVisited[robotThrees[4][i]][robotThrees[5][i]] = true;
							minimaxVisited[robotThrees[0][j]][robotThrees[1][j]] = true;
							minimaxVisited[robotThrees[2][j]][robotThrees[3][j]] = true;
							minimaxVisited[robotThrees[4][j]][robotThrees[5][j]] = true;
						}
						// check if immediately playable (at least 1 side each)
						if (singleImmediatelyPlayable(i, 1) > 0 && singleImmediatelyPlayable(j, 1) > 0)
						{
							score += 150;
						}
					}
				}
			}
		}
	}

	// check remaining single 3 in a rows
	for (int i = 0; i < numPlayerThrees; i++)
	{
		// checks for 3s with 2 empty immediately playable on either side
		if (singleOpen(i, 1) == 3 && singleImmediatelyPlayable(i, 1) == 3)
		{
			score -= 1000;
		}
		// checks other misc 3s
		else if (!minimaxVisited[playerThrees[0][i]][playerThrees[1][i]] && singleOpen(i, 1) > 0)
		{
			score -= 100;
		}
	}
	for (int i = 0; i < numRobotThrees; i++)
	{
		// checks for 3s with 2 empty immediately playable on either side
		if (singleOpen(i, 2) == 3 && singleImmediatelyPlayable(i, 2) == 3)
		{
			score += 1000;
		}
		// checks other misc 3s
		else if (!minimaxVisited[robotThrees[0][i]][robotThrees[1][i]] && singleOpen(i, 2) > 0)
		{
			score += 100;
		}
	}
	// 2 in a rows
	for (int row = 0; row <= 5; row++)
	{
		for (int col = 0; col <= 5; col++) {
			// player horizontal
			if (boardMinimax[row][col] == 1 && boardMinimax[row][col + 1] == 1 && !minimaxVisited[row][col] && !minimaxVisited[row][col + 1])
			{
				// mark as visited
				minimaxVisited[row][col] = true;
				minimaxVisited[row][col + 1] = true;
				score -= 5;
			}
			// robot horizontal
			else if (boardMinimax[row][col] == 2 && boardMinimax[row][col + 1] == 2 && !minimaxVisited[row][col] && !minimaxVisited[row][col + 1])
			{
				// mark as visited
				minimaxVisited[row][col] = true;
				minimaxVisited[row][col + 1] = true;
				score += 5;
			}
		}
	}
	for (int col = 0; col <= 6; col++)
	{
		for (int row = 0; row <= 4; row++)
		{
			// player vertical
			if (boardMinimax[row][col] == 1 && boardMinimax[row + 1][col] == 1 && !minimaxVisited[row][col] && !minimaxVisited[row + 1][col])
			{
				// mark as visited
				minimaxVisited[row][col] = true;
				minimaxVisited[row + 1][col] = true;
				score -= 5;
			}
			// robot vertical
			else if (boardMinimax[row][col] == 2 && boardMinimax[row + 1][col] == 2 && !minimaxVisited[row][col] && !minimaxVisited[row + 1][col])
			{
				// mark as visited
				minimaxVisited[row][col] = true;
				minimaxVisited[row + 1][col] = true;
				score += 5;
			}
		}
	}
	for (int row = 5; row >= 1; row--)
	{
		for (int col = 0; col <= 5; col++)
		{
			int a = row-1;
			int b = col+1;
			// player bottom left to top right diagonal
			if (boardMinimax[row][col] == 1 && boardMinimax[a][b] == 1 && !minimaxVisited[row][col] && !minimaxVisited[a][b])
			{
				// mark as visited
				minimaxVisited[row][col] = true;
				minimaxVisited[a][b] = true;
				score -= 5;
			}
			// robot bottom left to top right diagonal
			else if (boardMinimax[row][col] == 2 && boardMinimax[a][b] == 2 && !minimaxVisited[row][col] && !minimaxVisited[a][b])
			{
				// mark as visited
				minimaxVisited[row][col] = true;
				minimaxVisited[a][b] = true;
				score += 5;
			}
		}
	}
	for (int row = 0; row <= 4; row++)
	{
		for (int col = 0; col <= 5; col++)
		{
			// player top left to bottom right diagonal
			if (boardMinimax[row][col] == 1 && boardMinimax[row + 1][col + 1] == 1 && !minimaxVisited[row][col] && !minimaxVisited[row + 1][col + 1])
			{
				// mark as visited
				minimaxVisited[row][col] = true;
				minimaxVisited[row + 1][col + 1] = true;
				score -= 5;
			}
			// robot top left to bottom right diagonal
			else if (boardMinimax[row][col] == 2 && boardMinimax[row + 1][col + 1] == 2 && !minimaxVisited[row][col] && !minimaxVisited[row + 1][col + 1])
			{
				// mark as visited
				minimaxVisited[row][col] = true;
				minimaxVisited[row + 1][col + 1] = true;
				score += 5;
			}
		}
	}

	// 1 single piece
	for (int row = 0; row <= 5; row++)
	{
		for (int col = 0; col <= 6; col++)
		{
			// if player piece
			if (!minimaxVisited[row][col] && minimaxVisited[row][col] == 1)
			{
				score -= 1;
			}
			// if robot piece
			else if (!minimaxVisited[row][col] && minimaxVisited[row][col] == 2)
			{
				score += 1;
			}
		}
	}
	return score;
}

// algorithm implements minimax with alpha beta pruning
int minimax(int depth, bool robotTurn)
{
	if (depth == 0 || checkWinnerMinimax() == 1 || checkWinnerMinimax() == 2)
	{
		// return heuristic value of node
		return minimaxHeuristic();
	}
	// if maximizing player (computer)
	if (robotTurn)
	{
		int bestValue = -1000000;
		// loop through each child of node (response to current play)
		for (int col = 0; col < 7; col++)
		{
			// if legal (at least one space in that column is empty)
			if (boardMinimax[0][col] == 0)
			{
				// update boardMinimax
				int row = 5;
				for (row = 5; row >= 0; row--)
				{
					if (boardMinimax[row][col] == 0)
					{
						boardMinimax[row][col] = 2;
						break;
					}
				}
				int currentValue = 0;
				// call minimax again with new values
				currentValue = minimax(depth - 1, false);
				// update bestValue
				if (currentValue > bestValue)
				{
					bestValue = currentValue;
					// save the column number so it can be returned later
					maxCol = col;
				}
				// reset boardMinimax
				boardMinimax[row][col] = 0;
			}
		}
	}
	// if minimizing player (opponent)
	else
	{
		int worstValue = 1000000;
		// loop through each child of node (response to current play)
		for (int col = 0; col < 7; col++)
		{
			// if legal (at least one space in that column is empty)
			if (boardMinimax[0][col] == 0)
			{
				// update boardMinimax
				int row;
				for (row = 5; row >= 0; row--)
				{
					if (boardMinimax[row][col] == 0)
					{
						boardMinimax[row][col] = 1;
						break;
					}
				}
				int currentValue = 0;
				// call minimax again with new values
				currentValue = minimax(depth - 1, true);
				// update worstValue
				if (currentValue < worstValue)
				{
					worstValue = currentValue;
				}
				// reset boardMinimax
				boardMinimax[row][col] = 0;
			}
		}
	}
	return maxCol;
}

void computerMove()
{
	int column = minimax(4, true);
	moveToLocation(armHorizontal[column], armVertical[column]);
	rotateArm();
	numRobotMoves++;
	for (int row = 5; row >= 0; row--)
	{
		if (board[row][column] == 0)
		{
			board[row][column] = 2;
			break;
		}
	}
}

task main()
{
	// let the user know it's their turn
	nextTurnSound();
	while (true)
	{
		// sense and grab computer piece once user is done
		senseComputerPiece();
		// find where the opponent put their piece
		findPlayerPiece();
		// if winner, play sound and end program
		int winner = checkWinner();
		if (winner != 0)
		{
			playEndSound(winner);
			break;
		}
		// if not winner, check for ties
		if (numRobotMoves == 21)
		{
			playEndSound(0);
		}
		// computer determines best move
		computerMove();
		// return home
		moveToLocation(0, 0);
		winner = checkWinner();
		if (winner != 0)
		{
			playEndSound(winner);
			break;
		}
		// let the user know it's their turn
		nextTurnSound();
	}
	moveToLocation(0,0);
}
