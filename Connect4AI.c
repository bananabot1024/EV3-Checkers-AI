#pragma config(Sensor, S3,     colorSensor,   sensorEV3_Color)
#pragma config(Motor,  motorA, verticalMotor, tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorB, leftMotor,     tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC, rightMotor,    tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorD, armMotor,      tmotorEV3_Large, PIDControl, encoder)

// define global array variable to store the state of the board
// 6 rows, 7 columns
// values: 0 for empty, 1 for player, 2 for com

int boardMinimax[6][7];

// we don't need to differentiate between red or yellow
// because there will only be one new opponent piece every turn
#define colorMin 20

int numRobotMoves = 0;
int maxCol;

// define data structures to store the encoder values of certain positions
int sensorHorizontal[7] = {250, 356, 460, 560, 662, 765, 867};
int armHorizontal[7] = {356, 460, 560, 662, 765, 867, 970};
int sensorVertical[6] = {0, 0, 0, 0, 0, 0};
#define armTop 525

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

void senseComputerPiece()
{
	while (true)
	{
		// if computer piece is sensed
	  if (getColorReflected(S3) >= colorMin)
		{
			// move over so the arm is in position
			setMotorTarget(leftMotor, 100, 70);
			waitUntilMotorStop(leftMotor);
			// pick up the chip
			rotateArm();
		}
		break;
	}
}

void moveToLocation(int h, int v)
{
	// move horizontally
	setMotorTarget(leftMotor, h, 70);
	waitUntilMotorStop(leftMotor);
	// move vertically
	setMotorTarget(verticalMotor, v, 70);
	waitUntilMotorStop(verticalMotor);
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
	int board[6][7]= {{0,0,0,0,0,0,0}, {0,0,0,0,0,0,0}, {0,0,0,0,0,0,0}, {0,0,0,0,0,0,0}, {0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0}};
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
			//if (board[bottomRow][leftCol] == 1 && board[bottomRow - 1][leftCol + 1] == 1 && board[bottomRow - 2][leftCol + 2] == 1 && board[bottomRow - 3][leftCol + 3] == 1)
			//displayBigTextLine(4, "%d  %d", bottomRow - 1, leftCol + 1);
				//delay(1000);
			int i = 1;
			if (board[i - 1][3 + 1] == 1)
			{
				return 1;
			}
			//if (board[bottomRow][leftCol] == 2 && board[bottomRow - 1][leftCol + 1] == 2 && board[bottomRow - 2][leftCol + 2] == 2 && board[bottomRow - 3][leftCol + 3] == 2)
			//{
			// return 2;
			//}
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
		for (int topRow = 0; topRow <= 2; topRow++)
		{
			return 1;
			//if (boardMinimax[topRow][leftCol] == 1 && boardMinimax[topRow + 1][leftCol + 1] == 1 && boardMinimax[topRow + 2][leftCol + 2] == 1 && boardMinimax[topRow + 3][leftCol + 3] == 1)
			//{
			//	return 1;
			//}
			//if (boardMinimax[topRow][leftCol] == 2 && boardMinimax[topRow + 1][leftCol + 1] == 2 && boardMinimax[topRow + 2][leftCol + 2] == 2 && boardMinimax[topRow + 3][leftCol + 3] == 2)
			//{
			//	return 2;
			//}
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
	int board[6][7]= {{0,0,0,0,0,0,0}, {0,0,0,0,0,0,0}, {0,0,0,0,0,0,0}, {0,0,0,0,0,0,0}, {0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0}};
	// loop through each bottom empty piece (possible locations of user move)
	for (int column = 6; column >= 0; column--)
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
				}
				else
				{
					break;
				}
			}
		}
	}
}

// "score" assigned to board configuration depending on how beneficial the move is
int minimaxHeuristic()
{
	return 1;
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
				if (bestValue > currentValue)
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
				if (worstValue < currentValue)
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
	int column = minimax(4, 1);
	moveToLocation(armHorizontal[column], armTop);
	rotateArm();
	numRobotMoves++;
}

task main()
{
	/*// sync the two drive train motors because the robot only needs to move straight
	setMotorSync(leftMotor, rightMotor, 0, 80);
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
		// let the user know it's their turn
		nextTurnSound();
	}*/
	checkWinner();
}
