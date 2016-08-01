#pragma config(Sensor, S3,     colorSensor,   sensorEV3_Color)
#pragma config(Motor,  motorA, verticalMotor, tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorB, leftMotor,     tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC, rightMotor,    tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorD, armMotor,      tmotorEV3_Large, PIDControl, encoder)

// computer is red, player is yellow
// red 50, yellow 72
#define redMin 43
#define redMax 57
#define yellowMin 65
#define yellowMax 80

// define array(s) to store the encoder values of certain positions
// including arm fling, color sense (horizontal) needed locations and vertical
// TODO

// define global array variable to store the state of the board
// 6 down across (6 rows), 7 across (7 columns)
// 0 for empty, 1 for opponent, 2 for robot
int board[6][7];

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
		// if red sensed
	  if (getColorReflected(S3) >= redMin && getColorReflected(S3) <= redMax)
		{
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
	for (int topLeftColumn = 0; topLeftColumn <= 3; topLeftColumn++)
	{
		for (int topLeftRow = 0; topLeftRow <= 2; topLeftRow++)
		{
			if (board[topLeftRow][topLeftColumn] == 1 && board[topLeftRow + 1][topLeftColumn + 1] == 1 && board[topLeftRow + 2][topLeftColumn + 2] == 1 board[topLeftRow + 3][topLeftColumn + 3] == 1)
			{
				return 1;
			}
			if (board[topLeftRow][topLeftColumn] == 2 && board[topLeftRow + 1][topLeftColumn + 1] == 2 && board[topLeftRow + 2][topLeftColumn + 2] == 2 board[topLeftRow + 3][topLeftColumn + 3] == 2)
			{
				return 2;
			}
		}
	}

	// check bottom left to top right diagonal
	for (int bottomLeftColumn = 0; bottomLeftColumn <= 3; bottomLeftColumn++)
	{
		for (int bottomLeftRow = 3; bottomLeftRow <= 5; bottomLeftRow++)
		{
			if (board[topLeftRow][topLeftColumn] == 1 && board[topLeftRow - 1][topLeftColumn + 1] == 1 && board[topLeftRow - 2][topLeftColumn + 2] == 1 board[topLeftRow - 3][topLeftColumn + 3] == 1)
			{
				return 1;
			}
			if (board[topLeftRow][topLeftColumn] == 2 && board[topLeftRow - 1][topLeftColumn + 1] == 2 && board[topLeftRow - 2][topLeftColumn + 2] == 2 board[topLeftRow - 3][topLeftColumn + 3] == 2)
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
	for (int column = 6; column >= 0; column--)
	{
		for (int row = 5; row >= 0; row--)
		{
			if (board[row][column] == 0)
			{
				// move there
				// TODO

				// check if piece there
				if (getColorReflected(S3) > yellowMin && getColorReflected(S3) < yellowMax)
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

// turn: 1 = computer, -1 = opponent
int minimax(int depth, int boardCopy[6][7], int turn)
{
	if (depth == 0
	// final game state: rank according to win, draw, loss
	// intermediate game states:
	// if computer move: game state = max rank of available moves
	// if opponent move: game state = min rank of available moves
	// TODO
}

// algorithm implements minimax with alpha-beta pruning
void computerMove()
{
	column = minimax();
	// TODO

}

task main()
{
	// sync the two drive train motors because the robot only moves straight
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
		// computer determines best move and executes it
		computerMove();
		// return home
		moveToLocation(0, 0);
		// let the user know it's their turn
		nextTurnSound();
	}
}
