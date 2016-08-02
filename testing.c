#pragma config(Sensor, S3,     colorSensor,   sensorEV3_Color)
#pragma config(Motor,  motorA, verticalMotor, tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorB, leftMotor,     tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorC, rightMotor,    tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor,  motorD, armMotor,      tmotorEV3_Large, PIDControl, encoder)

// define global array variable to store the state of the board
// 6 rows, 7 columns
// values: 0 for empty, 1 for player, 2 for computer
int board[6][7] = {{0,0,1,1,1,1,0},
					 				{2,0,1,2,2,2,0},
					 				{0,0,0,0,0,0,0},
					 				{0,0,0,0,0,0,0},
									{0,0,0,0,0,0,0},
									{0,0,0,0,0,0,0}};



// we don't need to differentiate between red or yellow
// there will only be one new opponent piece every turn
#define colorMin 20

int numRobotMoves = 0;

// define data structures to store the encoder values of certain positions
#define sensorHorizontal [250, 356, 460, 560, 662, 765, 867]
#define armHorizontal [356, 460, 560, 662, 765, 867, 970]
// #define sensorVertical[6]
// #define armTop

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
		for (int r = 5; r >= 3; r--)
		{
			if (board[r][c] == 1 && board[r - 1][c + 1] == 1 && board[r - 2][c + 2] == 1 && board[r - 3][c + 3] == 1)
			{
				return 1;
			}
			if (board[r][c] == 2 && board[r - 1][c + 1] == 2 && board[r - 2][c + 2] == 2 && board[r - 3][c + 3] == 2)
			{
				return 2;
			}
		}
	}
	return 0;
}


task main()
{

}
