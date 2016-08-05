#pragma config(Sensor, S3,     colorSensor,   sensorEV3_Color)
#pragma config(Motor,  motorA, verticalMotor, tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorB, leftMotor,     tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC, rightMotor,    tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorD, armMotor,      tmotorEV3_Large, PIDControl, encoder)

// define global array variable to store the state of the board
// 6 rows, 7 columns
// values: 0 for empty, 1 for player, 2 for computer
int board[6][7];

// we don't need to differentiate between red or yellow
// because there will only be one new opponent piece every turn
#define colorMin 20

int numRobotMoves = 0;
int maxCol;

int moves[44] = {2,4,3,2,1,0,5,3,6,3,2,4,5,6,2,1,0,3,2,5,1,1,6,4,5,2,1,0,0,2,3,4,1,0,0,1,6,3,2,4,3,2,1,0};
int movesIndex = 0;

// define data structures to store the encoder values of certain positions
int sensorHorizontal[7] = {241, 344, 445, 544, 644, 745, 845};
int sensorVertical[6] = {467, 377, 287, 197, 105, 0};
int armHorizontal[7] = {353, 457, 557, 662, 769, 864, 967};
int armVertical[7] = {520, 515, 510, 510, 510, 510, 510};

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
				delay(200);
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

void computerMove()
{
	int column;
	// default to col 3 on first move
	if (numRobotMoves==0){
		column = 3;
	}
	// default to col 2 or 4 on second move
	else if (numRobotMoves==1){
		if (board[5][2]!=0){
			column = 4;
		}
		else if (board[5][4]!=0){
			column = 2;
		}
	}
	else {
		// random move otherwise
		if (movesIndex == 43)
		{
			movesIndex = 0;
		}
		// if the move is legal, take it
		while (board[0][moves[movesIndex]] != 0)
		{
			movesIndex++;
		}
		column = moves[movesIndex];
		// complete own 4 in a row if possible
		// horizontal
		for (int r = 5; r>= 0; r--)
		{
			for (int c = 0; c <= 3; c++)
			{
				if (board[r][c] == 2 && board[r][c+1] == 2 && board[r][c+2] == 2)
				{
					if (board[r][c+3] == 0)
					{
						column = c+3;
					}
				}
			}
		}
		// vertical
		for (int c = 0; c<=6; c++)
		{
			for (int r = 5; r>=3; r++)
			{
				int a = r-1, b = r-2, d=r-3;
				if (board[r][c] == 2 && board[a][c] == 2 && board[b][c] == 2)
				{
					if (board[d][c] == 0)
					{
							column = c;
					}
				}
			}
		}
		// diagonals (bottom left to top right)
		for (int r = 5; r >= 3; r--)
		{
			for (int c = 0; c<=3; c++)
			{
				int a = r-1, b = c+1, d=r-2, e=c+2, f=r-3, g=c+3;
				if (board[r][c] == 2 && board[a][b] == 2 && board[d][e] == 2)
				{
					if (board[f][g] == 0)
					{
						column = g;
					}
				}
			}
		}
		// diagonals (top left to bottom right)
		for (int r = 0; r<=2; r++)
		{
			for (int c = 0; c<=3; c++)
			{
				if (board[r][c] == 2 && board[r+1][c+1] == 2 && board[r+2][c+2] == 2)
				{
					if (board[r+3][c+3] == 0)
					{
						column = c+3;
					}
				}
			}
		}
		// prevent opponent 4 in a row if possible
		// horizontal
		for (int r = 5; r>= 0; r--)
		{
			for (int c = 0; c <= 3; c++)
			{
				if (board[r][c] == 1 && board[r][c+1] == 1 && board[r][c+2] == 1)
				{
					if (board[r][c+3] == 0)
					{
						column = c+3;
					}
				}
			}
		}
		// vertical
		for (int c = 0; c<=6; c++)
		{
			for (int r = 5; r>=3; r++)
			{
				int a=r-1,b=r-2,d=r-3;
				if (board[r][c] == 1 && board[a][c] == 1 && board[b][c] == 1)
				{
					if (board[d][c] == 0)
					{
							column = c;
					}
				}
			}
		}
		// diagonals (bottom left to top right)
		for (int r = 5; r >= 3; r--)
		{
			for (int c = 0; c<=3; c++)
			{
				int a=r-1,b=c+1,d=r-2,e=c+2,f=r-3,g=c+3;
				if (board[r][c] == 1 && board[a][b] == 1 && board[d][e] == 1)
				{
					if (board[f][g] == 0)
					{
						column = c+3;
					}
				}
			}
		}
		// diagonals (top left to bottom right)
		for (int r = 0; r<=2; r++)
		{
			for (int c = 0; c<=3; c++)
			{
				if (board[r][c] == 1 && board[r+1][c+1] == 1 && board[r+2][c+2] == 1)
				{
					if (board[r+3][c+3] == 0)
					{
						column = c+3;
					}
				}
			}
		}
	}
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
