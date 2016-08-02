#pragma config(Sensor, S3,     colorSensor,   sensorEV3_Color)
#pragma config(Motor,  motorA, verticalMotor, tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorB, leftMotor,     tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC, rightMotor,    tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorD, armMotor,      tmotorEV3_Large, PIDControl, encoder)

// define global array variable to store the state of the board
// 6 rows, 7 columns
// 0 for empty, 1 for opponent, 2 for robot
int board[6][7];

#define colorMin 20

// define data structures to store the encoder values of certain positions
#define sensorHorizontal[7] [250, 356, 460, 560, 662, 765, 867]
#define armHorizontal[7] [356, 460, 560, 662, 765, 867, 970]
//#define sensorVertical[6]
//#define armTop 

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
	  if (getColorReflected(S3) >= colorMin)
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
			if (board[topLeftRow][topLeftColumn] == 1 && board[topLeftRow + 1][topLeftColumn + 1] == 1 && board[topLeftRow + 2][topLeftColumn + 2] == 1 && board[topLeftRow + 3][topLeftColumn + 3] == 1)
			{
				return 1;
			}
			if (board[topLeftRow][topLeftColumn] == 2 && board[topLeftRow + 1][topLeftColumn + 1] == 2 && board[topLeftRow + 2][topLeftColumn + 2] == 2 && board[topLeftRow + 3][topLeftColumn + 3] == 2)
			{
				return 2;
			}
		}
	}

	// check bottom left to top right diagonal
	for (int c = 0; c <= 3; c++)
	{
		for (int r = 0; r <= 2; r++)
		{
			if (board[r][c] == 1 && board[r + 1][c + 1] == 1 && board[r + 2][c + 2] == 1 && board[r + 3][c + 3] == 1)
			{
				return 1;
			}
			if (board[r][c] == 2 && board[r + 1][c + 1] == 2 && board[r + 2][c + 2] == 2 && board[r + 3][c + 3] == 2)
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

// turn: 1 = computer, -1 = opponent
int minimax(int depth, int turn)
{
	if (depth == 0)
	{

	}
	// final game state: rank according to win, draw, loss
	// intermediate game states:
	// if computer move: game state = max rank of available moves
	// if opponent move: game state = min rank of available moves
	// TODO
	int column = 0;
	return column;
}

// algorithm implements minimax with alpha-beta pruning
void computerMove()
{
	// int column = minimax();
	// TODO
	// move to location
	// fling arm
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
		// computer determines best move
		computerMove();
		// return home
		moveToLocation(0, 0);
		// let the user know it's their turn
		nextTurnSound();
	}
}
