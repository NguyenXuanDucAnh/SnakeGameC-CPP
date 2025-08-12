#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

#include <unistd.h>

//detect nhấn phím
#include <termios.h>
#include <fcntl.h>

#include <sys/time.h>

struct timeval time_start;
struct timeval time_end;

#define CHARACTER_HEAD_SNAKE 'X' // Character representing the snake head
#define CHARACTER_BODY_SNAKE 'o' // Character representing the snake body

#define POSITION_VERTICAL_AXIS 1 // Height of the console in characters (Y-axis)
#define POSITION_HORIZONTAL_AXIS 1 // Width of the console in characters (X-axis)

// height and width of the console border
#define HEIGHT_BORDER 	25
#define WIDTH_BORDER 	50

#define XPOS_ORIGINAL WIDTH_BORDER/2 //tọa độ x gốc của đầu rắn khi khởi tạo
#define YPOS_ORIGINAL HEIGHT_BORDER/2 //tọa độ x gốc của đầu rắn khi khởi tạo

#define MAX_DOT_OF_SNAKE 100
#define NUM_SNAKE 5 // Số lượng đoạn của rắn khi khởi tạo

#define HEAD_INDEX 0 // Index of the snake head in the array

enum Direction_Keyboard {
	KEY_UP = 'w',
	KEY_DOWN = 's',
	KEY_LEFT = 'a',
	KEY_RIGHT = 'd',
	KEY_QUIT = 'q'
};

enum Direction_move {
	UP_DIR= 1,
	DOWN_DIR = 2,
	LEFT_DIR = 3,
	RIGHT_DIR = 4
};

enum Level_GAME {
	LEVEL_1 = 250,
	LEVEL_2 = 200,
	LEVEL_3 = 150,
	LEVEL_4 = 100,
	LEVEL_5 = 50
};

int g_XPOS[MAX_DOT_OF_SNAKE];
int g_YPOS[MAX_DOT_OF_SNAKE];
int g_numSnake = 3; // Số lượng đoạn của rắn
int level = LEVEL_1; // Default level
bool g_gameOver = false; // Flag to indicate if the game is over
uint32_t g_score = 0; // Score of the game

void gotoXY (int x, int y);
void clear_console (void);
int RandomNumber (int max_number);
void setNonBlocking(bool enable);
char _getch_keyboard_press();
long long getCurrentTimeInMilliseconds();

void DrawBorderInit (void);
void DrawSnakeInit (void);
void DrawSnake (int *xarray, int *yarray, int numSnake);
void UpdateSnakePosition (int *xArray, int *yArray, int numSnake, int newX, int newY);
void DrawPrey (int x, int y); // vẽ mồi
bool CheckGameOver ();
void UpdateScore ();
void SelectLevel (int &level);

void WelcomeMessage() {
	std::cout << "Welcome to the Snake Game!" << std::endl;

	std::cout << "Enter the level of the game (1-5): ";
	std::cin >> level; // Get the level from user input
	SelectLevel(level); // Select the level based on user input

	std::cout << "Use 'w', 'a', 's', 'd' to move the snake." << std::endl;
	std::cout << "Press 'q' to quit the game." << std::endl;
	std::cout << "Good luck!" << std::endl;
	sleep(0.5);
	std::cout << ">>> Press Any Key to Joint game <<<" << std::endl;
}
void CheckPressAnyKey(){
	while ( _getch_keyboard_press() == NULL);
}

int main (int argc, char * argv[]){
	
	bool b_confirmkey = false; // Variable to confirm key press

	char key = NULL; // Variable to store keyboard input
	char lastkey = NULL; // Variable to store the last keyboard input

	int newX = XPOS_ORIGINAL; // New X position of the snake head
	int newY = YPOS_ORIGINAL; // New Y position of the snake head

	char direction = RIGHT_DIR; // Variable to store the direction of movement

	int xPrey = RandomNumber(WIDTH_BORDER - 1); // Random X position for prey
	int yPrey = RandomNumber(HEIGHT_BORDER - 1); // Random Y position for prey

	WelcomeMessage();
	CheckPressAnyKey();

	clear_console();

	DrawBorderInit();
	DrawSnakeInit();
	DrawPrey(xPrey, yPrey); // Draw the prey at its position

	UpdateScore ();

	setNonBlocking(true); // Set terminal to non-blocking mode
	int lastTime = getCurrentTimeInMilliseconds(); // Get the current time in milliseconds
	int currentTime = lastTime; // Initialize current time

	sleep (1);

	for (;;) {
		currentTime = getCurrentTimeInMilliseconds(); // Get the current time in milliseconds
		//draw screen
		if (currentTime - lastTime >= level) { // Update every second
			lastTime = currentTime; // Update last time
			UpdateSnakePosition (g_XPOS, g_YPOS, g_numSnake, newX, newY);
			DrawSnake (g_XPOS, g_YPOS, g_numSnake);

			// auto move the snake
			if (direction == UP_DIR) {
				newY--;
			} else if (direction == DOWN_DIR) {
				newY++;
			} else if (direction == LEFT_DIR) {
				newX--;
			} else if (direction == RIGHT_DIR) {
				newX++;
			}

			if (newY == 1) {
				newY = HEIGHT_BORDER-1; // Prevent going out of bounds
			}
			else if (newY >= HEIGHT_BORDER) {
				newY = POSITION_VERTICAL_AXIS + 1; // Prevent going out of bounds
			}
			if (newX == 1) {
				newX = WIDTH_BORDER-1; // Prevent going out of bounds
			}
			else if (newX == WIDTH_BORDER) {
				newX = POSITION_HORIZONTAL_AXIS + 1; // Prevent going out of bounds
			}
		}
		// check keyboard input
		key = _getch_keyboard_press(); // Read keyboard input
		if (key != NULL){
			if (key == KEY_UP && direction != DOWN_DIR) { // Move up
				direction = UP_DIR; // Update direction
				b_confirmkey = true; // Confirm key press
			} else if (key == KEY_DOWN && direction != UP_DIR) { // Move down
				direction = DOWN_DIR; // Update direction
				b_confirmkey = true; // Confirm key press
			} else if (key == KEY_LEFT && direction != RIGHT_DIR) { // Move left
				direction = LEFT_DIR; // Update direction
				b_confirmkey = true; // Confirm key press
			} else if (key == KEY_RIGHT && direction != LEFT_DIR) { // Move right
				direction = RIGHT_DIR; // Update direction
				b_confirmkey = true; // Confirm key press
			} else if (key == KEY_QUIT) { // Quit the game
				break;
			}

			if ( lastkey != key && b_confirmkey == true)
			{
				lastkey = key; // Update last key pressed
				b_confirmkey = false; // Confirm key press
				// Update the snake position based on the new direction
				if (direction == UP_DIR) {
					newY--;
					newX = g_XPOS[HEAD_INDEX]; // Keep the X position of the snake head
				} else if (direction == DOWN_DIR) {
					newY++;
					newX = g_XPOS[HEAD_INDEX]; // Keep the X position of the snake head
				} else if (direction == LEFT_DIR) {
					newX--;
					newY = g_YPOS[HEAD_INDEX]; // Keep the Y position of the snake head
				} else if (direction == RIGHT_DIR) {
					newX++;
					newY = g_YPOS[HEAD_INDEX]; // Keep the Y position of the snake head
				}

				if (newY == 1) {
					newY = HEIGHT_BORDER-1; // Prevent going out of bounds
				}
				else if (newY >= HEIGHT_BORDER) {
					newY = POSITION_VERTICAL_AXIS + 1; // Prevent going out of bounds
				}
				if (newX == 1) {
					newX = WIDTH_BORDER-1; // Prevent going out of bounds
				}
				else if (newX == WIDTH_BORDER) {
					newX = POSITION_HORIZONTAL_AXIS + 1; // Prevent going out of bounds
				}
			}
			UpdateSnakePosition (g_XPOS, g_YPOS, g_numSnake, newX, newY);
			DrawSnake (g_XPOS, g_YPOS, g_numSnake);
		}

		
		
		// Check if the snake head has reached the prey (the snake eats the prey)
		if (g_XPOS[HEAD_INDEX] == xPrey && g_YPOS[HEAD_INDEX] == yPrey) {
			// If the snake head reaches the prey, increase the snake length
			if (g_numSnake < MAX_DOT_OF_SNAKE) {
				g_numSnake++;
				g_score++; // Increase the score
				UpdateScore ();
			}
			// Generate new prey position
			xPrey = RandomNumber(WIDTH_BORDER - 1);
			yPrey = RandomNumber(HEIGHT_BORDER - 1);
			DrawPrey(xPrey, yPrey); // Draw the new prey
		}

		g_gameOver = CheckGameOver (); // game over if head of snake touches its body
		if (g_gameOver) {
			CheckPressAnyKey(); // Wait for user input press any key before exiting
			break; // Exit the game loop if the game is over
		}
	}

	std::cout << std::endl;
	setNonBlocking(false); // Set terminal to blocking mode
	clear_console();
	std::cout << "Thank you for playing!" << std::endl;
	std::cout << "Your final score is: " << g_score << std::endl;
	return 0;
}


/*****************CONTROL SYSTEM CONSOLE **************************/
void gotoXY (int x, int y) {
	//move cusor position x, y
	printf("%c[%d;%df", 0x1B, y, x);


}

void clear_console(void)
{
    system("clear");
}

int RandomNumber (int max_number) {
	// Generate a random number between 0 and 100
	int randomNum = rand() % max_number;
	return randomNum;
}

// setNonBlocking(true): đọc phím real-time, không cần Enter, không block, không echo
// setNonBlocking(false): khôi phục nhập liệu bình thường (chờ Enter, có echo, blocking)
void setNonBlocking(bool enable) {
    struct termios ttystate;
    tcgetattr(STDIN_FILENO, &ttystate);

    if (enable) {
        ttystate.c_lflag &= ~ICANON; // tắt chế độ nhập dòng
        ttystate.c_lflag &= ~ECHO;   // tắt echo ký tự
        tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
    } else {
        ttystate.c_lflag |= ICANON;
        ttystate.c_lflag |= ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
        fcntl(STDIN_FILENO, F_SETFL, 0);
    }
}

char _getch_keyboard_press()
{
	int bytesRead = 1;
	char ch;
	if (read(STDIN_FILENO, &ch, bytesRead) > 0) {
		return ch;
	}
	return NULL; // Return NULL if no character was read
}

long long getCurrentTimeInMilliseconds() {
	struct timeval currentTime;
	gettimeofday(&currentTime, NULL);
	return (currentTime.tv_sec * 1000LL) + (currentTime.tv_usec / 1000LL); // convert seconds to milliseconds and add microseconds converted to milliseconds
	//time be count from 1970-01-01 00:00:00 UTC
}

/*****************************************************************/

/***************** DRAW ON CONSOLE **************************/
void SelectLevel (int &level){
	switch (level) {
		case 1:
			level = LEVEL_1;
			break;
		case 2:
			level = LEVEL_2;
			break;
		case 3:
			level = LEVEL_3;
			break;
		case 4:
			level = LEVEL_4;
			break;
		case 5:
			level = LEVEL_5;
			break;
		default:
			level = LEVEL_1; // Default level if input is invalid
			break;
	}
}

/**
 * @brief Draw height console
 * 
 * @param height: Height of the bar to draw
 * @param x : the first column to draw
 * @param y : the first row to draw
 */
void DrawHeight (int height, int x, int y)
{
	int yPos = y;
	for (int i=0; i<height; i++) {
		gotoXY(x, yPos);
		std::cout << "*" <<std::endl;

		yPos++;
	}
	std::cout.flush();
}

/**
 * @brief Draw height with char on console
 * 
 * @param height: Height of the bar to draw
 * @param x : the first column to draw
 * @param y : the first row to draw
 */
void DrawHeight (char c, int height, int x, int y)
{
	int yPos = y;
	for (int i=0; i<height; i++) {
		gotoXY(x, yPos);
		std::cout << c <<std::endl;

		yPos++;
	}
	std::cout.flush();
}

/**
 * @brief Draw width console
 * 
 * @param height: Width of the bar to draw
 * @param x : the first column to draw
 * @param y : the first row to draw
 */
void DrawWidth (int width, int x, int y)
{
	gotoXY(x, y);
	for (int i=0; i<width; i++) {
		std::cout << "*";
	}
	std::cout.flush();
}

/**
 * @brief Draw width with char on console
 * 
 * @param height: Width of the bar to draw
 * @param x : the first column to draw
 * @param y : the first row to draw
 */
void DrawWidth (char c, int width, int x, int y)
{
	gotoXY(x, y);
	for (int i=0; i<width; i++) {
		std::cout << c;
	}
	std::cout.flush();
}


void DrawBorderInit (void)
{
	int width = WIDTH_BORDER;
	int height = HEIGHT_BORDER;

	DrawHeight(height, 0, 0);
	DrawHeight(height, width, 0);

	DrawWidth(width, 0, 0);
	DrawWidth(width, 0, height);

	
	std::cout.flush();
}

void UpdateScore () {
	gotoXY(0, HEIGHT_BORDER+1); // Move cursor to the top-left corner
	std::cout << "Your Score: " << g_score << std::endl;
	std::cout.flush();
}


void DrawSnakeInit (void)
{
	int xPos = XPOS_ORIGINAL; // Initial X position of the snake head
	int yPos = YPOS_ORIGINAL;
	int numSnake = NUM_SNAKE; // Number of segments in the snake

	g_numSnake = numSnake; // Store the number of snake segments globally

	for (int i=0; i<numSnake; i++){
		gotoXY (xPos-i, yPos);
		if (i == 0)
			std::cout << CHARACTER_HEAD_SNAKE;
		else
			std::cout << CHARACTER_BODY_SNAKE;
		g_XPOS[i] = xPos - i; // Store the X position of the snake segment
		g_YPOS[i] = yPos; // Store the Y position of the snake
	}

	std::cout.flush();
}

void DrawSnake (int *xarray, int *yarray, int numSnake)
{
	for (int i=0; i<numSnake; i++){
		gotoXY (xarray[i], yarray[i]);
		if (i == 0)
			std::cout << CHARACTER_HEAD_SNAKE; // Head of the snake
		else
			std::cout << CHARACTER_BODY_SNAKE; // Body of the snake
	}

	std::cout.flush();
}

void UpdateSnakePosition (int *xArray, int *yArray, int numSnake, int newX, int newY)
{
	if (newX == xArray[HEAD_INDEX] && newY == yArray[HEAD_INDEX]) {
		return; // No movement, do not update
	}

	for (int i = numSnake-1; i>0; i--){
		gotoXY(xArray[i], yArray[i]); // Move the body segments to their new positions
		std::cout << " "; // clear the previous position of the body segment
		std::cout.flush();

		xArray[i] = xArray[i-1]; // Shift the body segments
		yArray[i] = yArray[i-1];		
	}
	
	xArray[HEAD_INDEX] = newX; // Update the head position
	yArray[HEAD_INDEX] = newY; // Update the head position

}

bool CheckGameOver (){ // tương lai sử dụng binary search để kiểm tra vì theo thứ tự thì tọa độ x và y sẽ tăng hoặc giảm dần trong mảng nên binary search sẽ nhanh hơn

	for (int i = 1; i < g_numSnake; i++) {
		if (g_XPOS[HEAD_INDEX] == g_XPOS[i] && g_YPOS[HEAD_INDEX] == g_YPOS[i]) {

			gotoXY((XPOS_ORIGINAL-4), YPOS_ORIGINAL); 
			std::cout << "Game Over!" << std::endl;
			gotoXY((XPOS_ORIGINAL-4), YPOS_ORIGINAL+1); 
			std::cout << "SCORE: " << g_score << std::endl;

			return true; // Game over condition met
		}
	}
	return false; // Game is not over
}

void DrawPrey (int x, int y) // vẽ con mồi
{
	gotoXY(x, y);
	std::cout << CHARACTER_BODY_SNAKE;
	std::cout.flush();
}
/*************************************************************/

