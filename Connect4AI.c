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

void nextTurnSound()
{
	// beep
	playTone(100,500);
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
}

void checkWiner()
{
	// check horizontal
	// check vertical
	// check diagonal
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
	setMotorSync(leftMotor, rightMotor);

	rotateArm();

}
