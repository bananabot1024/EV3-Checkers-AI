#pragma config(Sensor, S3,     colorSensor,    sensorEV3_Color)
#pragma config(Motor,  motorA,          verticalMotor, tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorB,          leftMotor,     tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC,          rightMotor,    tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorD,          armMotor,      tmotorEV3_Large, PIDControl, encoder)

// change this depending on sensor read values
#define redMin 20
#define redMax 30
#define yellowMin 40
#define yellowMax 50

// define global array variable to store the state of the board
// 7 across, 6 down
// 0 for empty, 1 for opponent, 2 for robot
int board[7][6];

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
	}
	else
	{
		// play losing sound
	}
	// exit program here (or, if not possible, break out of all loops in main)
}

void checkWiner()
{
	bool opponentWon = true, robotWon = true;
	// check horizontal
	for (int row = 0; row <= 5; row++)
	{
		for (int colStart = 0; colStart <= 3; colStart++)
		{
			opponentWon = true, robotWon = true;
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
	}
	else if (opponentWon)
	{
		playEndSound(true);
	}
	// check vertical
	for (int col = 0; col <= 6; col++)
	{
		for (int rowStart = 0; rowStart <= 2; rowStart++)
		{
			opponentWon = true, robotWon = true;
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
	// check top left to bottom right diagonal


	// check bottom left to top right diagonal
}

void determinePieceLocation()
{

}

void sensePlayerPiece()
{

}

void determineNextMove()
{

}

task main()
{
	// sync the two drive train motors because the robot only moves straight
	setMotorSync(leftMotor, rightMotor, 0, 80);

}
