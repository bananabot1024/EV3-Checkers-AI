// This program is to test separate functions

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

void playEndSound(bool winnerUser)
{
	if (winnerUser)
	{
		// play winning sound
		playSoundFile("/home/root/lms2012/prjs/rc/win2");
		sleep(2000);
	}
	else
	{
		// play losing sound
		playSoundFile("/home/root/lms2012/prjs/rc/lose")
		sleep(4000);
	}
}

bool checkWiner()
{
	bool winner = false;
	bool opponentWon = true, robotWon = true;
	// check horizontal
	for (int row = 0; row <= 5; row++)
	{
		for (int colStart = 0; colStart <= 3; colStart++)
		{
			opponentWon = true;
			robotWon = true;
			for (int i = 0; i < 4; i++)
			{
				if (board[row][colStart + i] != 1)
				{
					opponentWon = false;
				}
				if (board[row][colStart + i] != 2)
				{
					robotWon = false;
				}
			}
		}
	}
	if (robotWon)
	{
		playEndSOund(false);
		winner = true;
	}
	else if (opponentWon)
	{
		playEndSound(true);
		winner = true;
	}

	// check vertical
	for (int col = 0; col <= 6; col++)
	{
		for (int rowStart = 0; rowStart <= 2; rowStart++)
		{
			opponentWon = true;
			robotWon = true;
			for (int i = 0; i < 4; i++)
			{
				if (board[rowStart + i][col] != 1)
				{
					opponentWon = false;
				}
				if (board[rowStart + i][col] != 2)
				{
					robotWon = false;
				}
			}
		}
	}
	if (robotWon)
	{
		playEndSOund(false);
		winner = true;
	}
	else if (opponentWon)
	{
		playEndSound(true);
		winner = true;
	}

	// check top left to bottom right diagonal
	for (int topLeftColumn = 0; topLeftColumn <= 3; topLeftColumn++)
	{
		for (int topLeftRow = 0; topLeftRow <= 2; topLeftRow++)
		{
			opponentWon = true;
			robotWon = true;
			for (int i = 0; i < 4; i++)
			{
				if (board[topLeftRow + i][topLeftColumn + i] != 1)
				{
					opponentWon = false;
				}
				if (board[topLeftRow + i][topLeftColumn + i] != 2)
				{
					robotWon = false;
				}
			}
		}
	}
	if (robotWon)
	{
		playEndSOund(false);
		winner = true;
	}
	else if (opponentWon)
	{
		playEndSound(true);
		winner = true;
	}

	// check bottom left to top right diagonal
	for (int bottomLeftColumn = 0; bottomLeftColumn <= 3; bottomLeftColumn++)
	{
		for (int bottomLeftRow = 3; bottomLeftRow <= 5; bottomLeftRow++)
		{
			opponentWon = true;
			robotWon = true;
			for (int i = 0; i < 4; i++)
			{
				if (board[bottomLeftRow - i][bottomLeftColumn + i] != 1)
				{
					opponentWon = false;
				}
				if (board[bottomLeftRow - i][bottomLeftColumn + i] != 2)
				{
					robotWon = false;
				}
			}
		}
	}
	if (robotWon)
	{
		playEndSound(false);
		winner = true;
	}
	else if (opponentWon)
	{
		playEndSound(true);
		winner = true;
	}
	return winner;
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

void determineNextMove()
{

}

task main()
{
	rotateArm();
}
