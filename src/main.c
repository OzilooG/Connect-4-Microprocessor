#include <stm32f031x6.h>
#include "display.h"
#include "serial.h"
#include "musical_notes.h"
#include "sound.h"
#include <stdint.h>
#include <stdlib.h>
#include <time.h> 

#define NAMESIZE 13

void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);
void draw_grid(void);
void displaySpinningCoin(uint16_t x, uint16_t y, uint8_t player_turn);
int check_condition (uint32_t [][7]);
void initSound(void);
uint32_t prbs(void);
uint32_t random1(uint32_t lower,uint32_t upper)
{
    return (prbs()%(upper-lower))+lower;
}
uint32_t shift_register=1234;
uint32_t prbs()
{
	// This is an unverified 31 bit PRBS generator
	// It should be maximum length but this has not been verified 
	unsigned long new_bit=0;
	static int busy=0; // need to prevent re-entrancy here
	if (!busy)
	{
		busy=1;
		new_bit= ((shift_register & (1<<27))>>27) ^ ((shift_register & (1<<30))>>30);
		new_bit= ~new_bit;
		new_bit = new_bit & 1;
		shift_register=shift_register << 1;
		shift_register=shift_register | (new_bit);
		busy=0;
	}
	return shift_register & 0x7fffffff; // return 31 LSB's 
}
void redOn(void);
void redOff(void);
void yellowOn(void);
void yellowOff(void);

volatile uint32_t milliseconds = 0;
uint8_t animation_flag = 1;

const uint16_t grid[]={
	65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,
	65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,
	65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,
	65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,
	65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,
	65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,
	65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,
	65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,
	65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,
	65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,
	65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,
	65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,
	65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,
	65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,
	65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,
	65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,
	65535,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,65535,
	65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535,65535
};

const uint16_t Yellow_key1 [] ={
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,38170,56611,56611,56611,56611,37650,0,0,0,0,0,0,0,0,0,54554,7694,24327,24327,24327,24327,7701,54042,0,0,0,0,0,0,0,54554,7694,32519,24327,24327,65383,8055,16135,7437,54042,0,0,0,0,0,38170,7694,32519,24327,24327,24327,40743,8063,8055,24327,7437,54042,0,0,0,0,56611,24327,24327,24327,24327,24327,32519,40743,48959,24327,16142,47907,0,0,0,0,56611,24327,8055,40751,24327,24327,24327,24327,24327,24327,16142,47907,0,0,0,0,56611,24327,40911,57167,24327,24327,24327,24327,24327,24327,16142,47907,0,0,0,0,56611,24327,8055,8063,8047,8047,48951,24327,24327,24327,16142,47907,0,0,0,0,37650,7701,16135,8055,24487,49119,32687,24327,24327,57095,64789,53522,0,0,0,0,0,54042,7437,16135,24327,65391,32695,65391,57095,64789,61714,0,0,0,0,0,0,0,54042,7437,16142,16142,16142,16142,64789,61714,0,0,0,0,0,0,0,0,0,54042,47907,47907,47907,47907,53522,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const uint16_t Yellow_key2 [] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40732,65315,40732,24327,0,0,0,0,0,0,0,0,0,0,0,48932,40740,40732,24327,24327,24327,0,0,0,0,0,0,0,0,0,48932,57124,65315,24327,24327,24327,24327,24327,0,0,0,0,0,0,0,0,65315,24349,48932,24327,24327,24327,24327,24327,0,0,0,0,0,0,0,24349,48932,65315,24327,24327,24327,24327,24327,24327,24327,0,0,0,0,0,0,65315,48932,57124,24327,24327,24327,24327,24327,24327,24327,0,0,0,0,0,0,65315,65315,24327,24327,24479,24327,24327,24327,24327,24327,0,0,0,0,0,0,65315,65315,24327,24327,49119,24327,24327,24327,24327,24327,0,0,0,0,0,0,65315,57124,48932,24327,24327,24327,24327,24327,24327,24327,0,0,0,0,0,0,57124,48932,65315,24327,24327,24479,24327,24327,24327,24327,0,0,0,0,0,0,0,65315,48932,24349,24327,57327,40895,24327,24327,0,0,0,0,0,0,0,0,48932,24349,65315,24327,24327,24327,24327,24327,0,0,0,0,0,0,0,0,0,57124,7972,40732,24327,24327,24327,0,0,0,0,0,0,0,0,0,0,0,57124,65315,65315,40732,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const uint16_t Yellow_key3 [] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,24327,40732,65315,40732,0,0,0,0,0,0,0,0,0,0,0,24327,24327,24327,40732,40740,48932,0,0,0,0,0,0,0,0,0,24327,24327,24327,24327,24327,65315,57124,48932,0,0,0,0,0,0,0,0,24327,24327,24327,24327,24327,48932,24349,65315,0,0,0,0,0,0,0,24327,24327,24327,24327,24327,24327,24327,65315,48932,24349,0,0,0,0,0,0,24327,24327,24327,24327,24327,24327,24327,57124,48932,65315,0,0,0,0,0,0,24327,24327,24327,24327,24327,24479,24327,24327,65315,65315,0,0,0,0,0,0,24327,24327,24327,24327,24327,49119,24327,24327,65315,65315,0,0,0,0,0,0,24327,24327,24327,24327,24327,24327,24327,48932,57124,65315,0,0,0,0,0,0,24327,24327,24327,24327,24479,24327,24327,65315,48932,57124,0,0,0,0,0,0,0,24327,24327,40895,57327,24327,24349,48932,65315,0,0,0,0,0,0,0,0,24327,24327,24327,24327,24327,65315,24349,48932,0,0,0,0,0,0,0,0,0,24327,24327,24327,40732,7972,57124,0,0,0,0,0,0,0,0,0,0,0,40732,65315,65315,57124,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const uint16_t Red_key1 [] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8192,2313,44817,12049,60945,49664,0,0,0,0,0,0,0,0,0,49664,21786,16162,16169,24361,16161,7202,35337,0,0,0,0,0,0,0,33024,31522,57121,32288,15648,15648,64800,40481,32546,46362,16384,0,0,0,0,8192,13594,57121,56600,56608,40224,7456,23865,40563,56864,65314,26633,0,0,0,0,2313,40738,24104,16028,15673,64800,32032,56608,48722,15657,40737,37137,0,0,0,0,4113,40737,48433,32387,40233,15648,40224,32032,48408,23840,65056,56354,25088,0,0,0,20241,7969,7456,48400,15648,48416,40224,40224,40224,40224,48672,23841,512,0,0,0,36625,40745,48416,64800,7456,64792,48416,40224,40224,32032,32288,31778,49664,0,0,0,8960,23842,24096,23857,16020,32049,7456,40224,40224,48416,24361,28689,0,0,0,0,0,43529,32546,24088,7803,40571,7448,32032,48416,65312,32034,512,0,0,0,0,0,8192,46362,57122,16161,40489,40480,32288,16169,32034,27145,0,0,0,0,0,0,0,16384,26889,28690,23586,15394,31778,45073,512,0,0,0,0,0,0,0,0,0,0,0,25088,25088,41472,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const uint16_t Red_key2 [] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40732,65315,40732,40224,0,0,0,0,0,0,0,0,0,0,0,48932,40740,40732,40224,40224,40224,0,0,0,0,0,0,0,0,0,48932,57124,65315,40224,40224,40224,40224,40224,0,0,0,0,0,0,0,0,65315,24349,48932,40224,40224,40224,40224,40224,0,0,0,0,0,0,0,24349,48932,65315,40224,40224,40224,40224,40224,40224,40224,0,0,0,0,0,0,65315,48932,57124,40224,40224,40224,40224,40224,40224,40224,0,0,0,0,0,0,65315,65315,40224,57294,40918,40224,48416,40224,40224,40224,0,0,0,0,0,0,65315,65315,40224,40224,65535,40224,40224,40224,40224,40224,0,0,0,0,0,0,65315,57124,48932,40224,40224,40224,40224,40224,40224,40224,0,0,0,0,0,0,57124,48932,65315,40224,40224,15681,40224,40224,40224,40224,0,0,0,0,0,0,0,65315,48932,24349,40224,65535,57294,40224,40224,0,0,0,0,0,0,0,0,48932,24349,65315,40224,40224,40224,40224,40224,0,0,0,0,0,0,0,0,0,57124,7972,40732,40224,40224,40224,0,0,0,0,0,0,0,0,0,0,0,57124,65315,65315,40732,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const uint16_t Red_key3 [] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40224,40732,65315,40732,0,0,0,0,0,0,0,0,0,0,0,40224,40224,40224,40732,40740,48932,0,0,0,0,0,0,0,0,0,40224,40224,40224,40224,40224,65315,57124,48932,0,0,0,0,0,0,0,0,40224,40224,40224,40224,40224,48932,24349,65315,0,0,0,0,0,0,0,40224,40224,40224,40224,40224,40224,40224,65315,48932,24349,0,0,0,0,0,0,40224,40224,40224,40224,40224,40224,40224,57124,48932,65315,0,0,0,0,0,0,40224,40224,40224,48416,40224,40918,57294,40224,65315,65315,0,0,0,0,0,0,40224,40224,40224,40224,40224,65535,40224,40224,65315,65315,0,0,0,0,0,0,40224,40224,40224,40224,40224,40224,40224,48932,57124,65315,0,0,0,0,0,0,40224,40224,40224,40224,15681,40224,40224,65315,48932,57124,0,0,0,0,0,0,0,40224,40224,57294,65535,40224,24349,48932,65315,0,0,0,0,0,0,0,0,40224,40224,40224,40224,40224,65315,24349,48932,0,0,0,0,0,0,0,0,0,40224,40224,40224,40732,7972,57124,0,0,0,0,0,0,0,0,0,0,0,40732,65315,65315,57124,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

const uint16_t Middle_key [] = {
	0,0,0,0,0,0,0,59400,60169,0,0,0,0,0,0,0,0,0,0,0,0,0,58624,39971,56611,17928,0,0,0,0,0,0,0,0,0,0,0,0,12306,48940,48940,20498,0,0,0,0,0,0,0,0,0,0,0,0,4114,24364,24364,12306,0,0,0,0,0,0,0,0,0,0,0,0,61201,24364,24364,4114,0,0,0,0,0,0,0,0,0,0,0,0,61201,24364,24364,4114,0,0,0,0,0,0,0,0,0,0,0,0,61201,24364,24364,4114,0,0,0,0,0,0,0,0,0,0,0,0,61201,24364,24364,4114,0,0,0,0,0,0,0,0,0,0,0,0,61201,24364,24364,4114,0,0,0,0,0,0,0,0,0,0,0,0,61201,24364,24364,4114,0,0,0,0,0,0,0,0,0,0,0,0,61201,24364,24364,4114,0,0,0,0,0,0,0,0,0,0,0,0,61201,24364,24364,4114,0,0,0,0,0,0,0,0,0,0,0,0,61201,24364,24364,4114,0,0,0,0,0,0,0,0,0,0,0,0,28690,48940,48940,45330,0,0,0,0,0,0,0,0,0,0,0,0,59144,40483,7724,18441,0,0,0,0,0,0,0,0,0,0,0,0,0,19985,2569,0,0,0,0,0,0,0,
};


int main()
{
	uint16_t x = 55;
	uint16_t y = 30;
	uint16_t oldx = x;
	uint16_t oldy = y;
	uint8_t rgb_check; //to make the rgb animation play once in the start screen
	uint32_t coin_array[6][7]; //the coin array for logic, to keep track of where players made their move
	uint8_t win_checker; //check who and if someone has made a win
	uint8_t player_turn = 1; // keep track of player turns
	int player1_score = 0, player2_score = 0;
	char player1_name[NAMESIZE] = {"Player1"}; //max characters for name is 12, one left for character /0
	char player2_name[NAMESIZE] = {"Player2"}; //max characters for name is 12
	initClock();
	initSysTick();
	setupIO();
	initSerial();
	initSound();


	//Welcome screen, start of the program
	welcome_screen:

		eputs("Welcome Screen\n");
		fillRectangle(0,0,128,160,0); //clear any images before
		rgb_check = 1;
		//Reset player scores
		player1_score = 0;
		player2_score = 0;

		int intro =0;
		
		if(intro == 0){
		playNote(E7);
		delay(100);
		playNote(0);
		delay(100);
		playNote(E7);
		delay(100);
		playNote(0);
		delay(100);
		playNote(E7);
		delay(100);
		playNote(0);
		delay(100);
		playNote(C7);
		delay(400);
		playNote(E7);
		delay(300);
		playNote(G7);
		delay(400);
		playNote(G3);
		delay(300);
		playNote(0);

		intro = 1;
		}
		

		while (1)
		{
			printTextX2("Welcome",25,10,4,0);

			//Play once
			while (rgb_check){
			printTextX2("Connect 4",13,40,RGBToWord(255,0,0),0);
			delay(500);
			printTextX2("Connect 4",13,40,RGBToWord(255,128,0),0);
			delay(500);
			printTextX2("Connect 4",13,40,RGBToWord(0,255,255),0);
			delay(500);
			printTextX2("Connect 4",13,40,RGBToWord(255,0,255),0);
			delay(500);
			rgb_check = 0; //prevent from playing more times
			}
			
			printText("Press up to start!",2,70,4,0);

			//If up is pressed
			if((GPIOA->IDR & (1 << 8)) == 0){
				break;
			}
		}

		
		//Ask players to enter their name
		fillRectangle(0,0,128,160,0); //clear any images before
		printTextX2("Check the",2,40,RGBToWord(255,0,0),0);
		printTextX2("screen",2,60,RGBToWord(255,0,0),0);
		printText("(Serial monitor)",2, 80, 5, 0);
		// Prompt for Player 1 name
		eputs("Enter the name of the first player (max 12 characters)\n");
		readSerial(player1_name);

		// Prompt for Player 2 name
		eputs("Enter the name of the second player (max 12 characters)\n");
		readSerial(player2_name);

		//Generate random turn order for the players
	//decides which player goes first
	fillRectangle(0,0,128,160,0);

	shift_register += milliseconds;

	int player1,player2,temp = 0;
    
    /* generate random number */  
    
        player1 = prbs();  
        player2 = prbs();  
		
		printDecimal(player1);
		printDecimal(player2);

	
	while (temp ==0){
	if( player1 > player2){
		printTextX2(player1_name,20,20,RGBToWord(255,128,0),0);
		printTextX2("Goes",35,50,RGBToWord(255,128,0),0);
		printTextX2("First",30,80,RGBToWord(255,128,0),0);
		player_turn = 1;
	}
	if (player2 > player1){
		printTextX2(player2_name,20,20,RGBToWord(255,0,0),0);
		printTextX2("Goes",35,50,RGBToWord(255,0,0),0);
		printTextX2("First",30,80,RGBToWord(255,0,0),0);
		player_turn = 2;
	}
	delay(1500);
	temp =1;
	}

	//label in order to bring the program back here for rematches
	start_game:

	if (player_turn==1){
		yellowOn();
	}
	else {
		redOn();
	}

	fillRectangle(0,0,128,160,0); //clear any images before
	draw_grid(); //draw the grid (originally it was one big grid, but that takes too much flash space)

	//To prevent the milliseconds from dissapearing and making it unable for the coin to spin
	milliseconds = 0;

	//Clear the coin array just before the game starts
	for (int i = 0; i < 6; i++){
		for (int j = 0; j < 7; j++){
			coin_array[i][j] = 0;
		}
	}

	//Clear the win_checker before the game starts
	win_checker = 0;

	eputs("Start of Game\n");

	//MAIN GAME LOOP
	while(1)
	{

		if ((GPIOB->IDR & (1 << 5))==0) // left pressed
        {
			eputs("\nLeft pressed "); //serial output

            if (x > 18)
            {
				oldx = x; //for clearing
                x -= 18;  // Move the coin left
				fillRectangle(oldx, oldy,16,16,0); //clear the coin
				delay(200);
            }
        }
        else if ((GPIOB->IDR & (1 << 4))==0) // right pressed
        {
			eputs("\nRight pressed "); //serial output

            if (x < 92)
            {
				oldx=x; //for clearing
                x += 18;  // Move the coin right
				fillRectangle(oldx, oldy,16,16,0); //clear the coin
				delay(200);
            }
        }

		else if ((GPIOA->IDR & (1 << 11))==0) // down pressed
        {
			eputs("\nDown pressed ");

            if (coin_array[0][x/18]==0)
            {
				oldx = x; //for clearing
				fillRectangle(oldx, oldy,16,16,0); //clear the coin
				delay(200);
				
				for (int i = 5; i >= 0; i--){
					if (coin_array[i][x/18] == 0){

						coin_array[i][(x-1)/18] = player_turn;

						if (player_turn == 1){
							playNote(G3);
							delay(100);
							playNote(E2);
							delay(100);
							playNote(0);
							putImage(x, 143-(5-i)*18, 16, 16, Yellow_key1, 0, 0);
							player_turn = 2;
							eputs("\nPlayer turn 2");
							yellowOff();
							redOn();
						}
						
						else {
							playNote(G3);
							delay(100);
							playNote(E2);
							delay(100);
							playNote(0);
							putImage(x, 143-(5-i)*18, 16, 16, Red_key1, 0, 0);
							player_turn = 1;
							eputs("\nPlayer turn 1");
							redOff();
							yellowOn();
						}

						win_checker = check_condition(coin_array);

						break;
					}
				}
				x = 55;  // Move the coin to the start
            }
			
			milliseconds = 0;
        }

		//Check if anything has been completed
		if (win_checker!=0){
			delay(1000);
			break;
		}

		if (milliseconds > 400 && milliseconds < 1400){
			displaySpinningCoin(x, y, player_turn);
			milliseconds = 0;
		}
			

	}

	//Display winning or draw conditions

	//If player 1 won, display their score in their name in their colour (yellow) above grid
	if (win_checker == 1){
		redOff();
		printText(player1_name, 3, 10, RGBToWord(255,255,0), 0);
		printText("wins!", 70, 10, RGBToWord(255,255,0), 0);
		player1_score++;
		printText("Their Score:", 3, 30, 2, 0);
		printNumber(player1_score, 3, 40, 2, 0);
		playNote(G2);
		yellowOn();
		delay(100);
		yellowOff();
		playNote(0);
		delay(100);
		yellowOn();
		playNote(G2);
		delay(100);
		yellowOff();
		playNote(0);
		delay(100);
		yellowOn();
		playNote(E7);
		delay(100);
		yellowOff();
		playNote(0);
		
	}

	//If player 2 won, display their score and their name in their colour (red) above grid
	else if (win_checker == 2){
		yellowOff();
		printText(player2_name, 3, 10, RGBToWord(255,0,0), 0);
		printText("wins!", 70, 10, RGBToWord(255,0,0), 0);
		player2_score++;
		printText("Their Score:", 3, 30, 2, 0);
		printNumber(player2_score, 3, 40, 2, 0);
		playNote(G2);
		redOn();
		delay(100);
		redOff();
		playNote(0);
		delay(100);
		redOn();
		playNote(G2);
		delay(100);
		redOff();
		playNote(0);
		delay(100);
		redOn();
		playNote(E7);
		delay(100);
		redOff();
		playNote(0);
		
	}
	//If it's a draw, show draw
	else {
		printText("It's a draw", 20, 10, 5, 0);
		playNote(G2);
		yellowOn();
		redOn();
		delay(100);
		yellowOff();
		redOff();
		playNote(0);
		delay(100);
		yellowOn();
		redOn();
		playNote(G2);
		delay(100);
		yellowOff();
		redOff();
		playNote(0);
		delay(100);
		yellowOn();
		redOn();
		playNote(E7);
		delay(100);
		yellowOff();
		redOff();
		playNote(0);
	}

	//Give some time so the players can notice their win
	delay(2000);

	//Clear screen of the grid and all other text
	fillRectangle(0,0,128,160,0);


	//END SCREEN

	//Display the current player names and scores
	printText("Scores:",2,10,5,0);
	printText(player1_name,2,20,5,0);
	printNumber(player1_score,70,20,6,0);
	printText(player2_name,2,30,5,0);
	printNumber(player2_score,70,30,6,0);

	//Prompt the players. Down, up, L, R stands for the buttons that need to be pressed
	//The text could not be longer because of the lack of space on the screen
	printTextX2("Play", 40, 50,RGBToWord(0,0,255),0);
	printTextX2("Again?", 35, 70,RGBToWord(0,0,255),0);
	printText("Down to menu",2,100,5,0);
	printText("Up to rematch",2,110,5,0);
	printText("L or R to quit",2,120,5,0);

	//Check for inputs
	while (1){
		if ((GPIOA->IDR & (1 << 11))==0) //Down pressed
		{
			goto welcome_screen;
			break;
		}
		if ((GPIOA->IDR & (1 << 8))==0) //Up pressed
		{
			goto start_game;
			break;
		}
		else if (((GPIOB->IDR & (1 << 5))==0)||((GPIOB->IDR & (1 << 4))==0)) // left or right pressed
		{
			fillRectangle(0,0,128,160,0);
			break;
		}
		delay(50); //avoid overloading the system
	}
	

	return 0;

}

// Function to read a line from serial input
void readSerial(char *buffer) {
	
	int i = 0;
	char c;

	while (i < NAMESIZE - 1) {
		c = egetchar(); // Assuming getchar() reads from serial input
		if (c == '\n') {
			break;
		}
		buffer[i] = c;
		eputchar(buffer[i]);
		i++;
	}

	buffer[i-1] = '\0';
}

//Displays an animation of the coin
void displaySpinningCoin(uint16_t x, uint16_t y, uint8_t player_turn) {

	if (player_turn == 1){
		if (animation_flag == 1){
			putImage(x, y, 16, 16, Yellow_key1, 0, 0);  // Display frame 1
		}
		else if (animation_flag == 2){
			putImage(x, y, 16, 16, Yellow_key2, 0, 0);  // Display frame 2
		}
		else if (animation_flag == 3){
			putImage(x, y, 16, 16, Middle_key, 0, 0);  // Display frame 3
		}
		else {
			putImage(x, y, 16, 16, Yellow_key3, 0, 0);  // Display frame 4
			animation_flag = 1;
			return;
		}
	}

	else{
		if (animation_flag == 1){
			putImage(x, y, 16, 16, Red_key1, 0, 0);  // Display frame 1
		}
		else if (animation_flag == 2){
			putImage(x, y, 16, 16, Red_key2, 0, 0);  // Display frame 2
		}
		else if (animation_flag == 3){
			putImage(x, y, 16, 16, Middle_key, 0, 0);  // Display frame 3
		}
		else {
			putImage(x, y, 16, 16, Red_key3, 0, 0);  // Display frame 4
			animation_flag = 1;
			return;
		}
	}

	animation_flag++;
}


//Check the condition of the game: player 1 or 2 win, or draw.
int check_condition (uint32_t coin_array[][7]){

	uint8_t i, j;
	uint8_t checker = 0;

    //Check if it's a win horizontal
    for (i = 0; i < 6; i++){
        for (j = 0; j < 4; j++){
            if (coin_array[i][j]==coin_array[i][j+1]&&
            coin_array[i][j]==coin_array[i][j+2]&&
            coin_array[i][j]==coin_array[i][j+3]&&
            coin_array[i][j]!=0){
				eputs("\nHorizontal win\n");
                return coin_array[i][j];
            }
        }
    }
 
	//Check if it's a win diagonal (this way /)
    for (i = 5; i > 2; i--){
        for (j = 0; j < 4; j++){
            if (coin_array[i][j]==coin_array[i-1][j+1]&&
            coin_array[i-1][j+1]==coin_array[i-2][j+2]&&
            coin_array[i-2][j+2]==coin_array[i-3][j+3]&&
            coin_array[i][j]!=0){
				eputs("\nDiagonal win\n");
                return coin_array[i][j];
            }
        }
    }

    //Check if it's a win vertical
    for (i = 0; i <= 2; i++){
        for (j = 0; j <= 6; j++){
            if (coin_array[i][j]==coin_array[i+1][j]&&
            coin_array[i][j]==coin_array[i+2][j]&&
            coin_array[i][j]==coin_array[i+3][j]&&
            coin_array[i][j]!=0){
				eputs("\nVertical win\n");
                return coin_array[i][j];
            }
        }
    }

	//Check if its a win the other diagonal way  (this way \)
    for (i = 0; i < 3; i++){
        for (j = 0; j < 4; j++){
            if (coin_array[i][j]==coin_array[i+1][j+1]&&
            coin_array[i+1][j+1]==coin_array[i+2][j+2]&&
            coin_array[i+2][j+2]==coin_array[i+3][j+3]&&
            coin_array[i][j]!=0){
				eputs("\nDiagonal win\n");
                return coin_array[i][j];
            }
        }
    }

    //Check if the top rows are full
    for (i = 0; i < 7; i++){
        if (coin_array[0][i] == 0){
            checker = 1;
        }
    }

    //If it's still zero its a draw
    if (checker == 0){
        return 3;
    }

	return 0;
    
}

//Draws the grid for the main game. This is used to save flash space.
void draw_grid(){

	//Clear the top
	fillRectangle(0, 0, 128, 160, 0);

	//Draw the grid
	for (int i = 0; i < 126; i+=18){
		for (int j = 52; j < 160; j+=18){
			putImage(i,j, 18, 18, grid, 0, 0);
		}
	}

}

void redOn(void){
	GPIOB->ODR |= (1 << 3); 
}

void redOff(void){
	GPIOB->ODR &= ~(1 << 3);
}

void yellowOn(void){
	GPIOB->ODR |= (1 << 0);
}

void yellowOff(void){
	GPIOB->ODR &= ~(1 << 0);
}

void initSysTick(void)
{
	SysTick->LOAD = 48000;
	SysTick->CTRL = 7;
	SysTick->VAL = 10;
	__asm(" cpsie i "); // enable interrupts
}
void SysTick_Handler(void)
{
	milliseconds++;
}
void initClock(void)
{
// This is potentially a dangerous function as it could
// result in a system with an invalid clock signal - result: a stuck system
        // Set the PLL up
        // First ensure PLL is disabled
        RCC->CR &= ~(1u<<24);
        while( (RCC->CR & (1 <<25))); // wait for PLL ready to be cleared
        
// Warning here: if system clock is greater than 24MHz then wait-state(s) need to be
// inserted into Flash memory interface
				
        FLASH->ACR |= (1 << 0);
        FLASH->ACR &=~((1u << 2) | (1u<<1));
        // Turn on FLASH prefetch buffer
        FLASH->ACR |= (1 << 4);
        // set PLL multiplier to 12 (yielding 48MHz)
        RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
        RCC->CFGR |= ((1<<21) | (1<<19) ); 

        // Need to limit ADC clock to below 14MHz so will change ADC prescaler to 4
        RCC->CFGR |= (1<<14);

        // and turn the PLL back on again
        RCC->CR |= (1<<24);        
        // set PLL as system clock source 
        RCC->CFGR |= (1<<1);
}
void delay(volatile uint32_t dly)
{
	uint32_t end_time = dly + milliseconds;
	while(milliseconds != end_time)
		__asm(" wfi "); // sleep
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	/*
	*/
	uint32_t mode_value = Port->MODER;
	Mode = Mode << (2 * BitNumber);
	mode_value = mode_value & ~(3u << (BitNumber * 2));
	mode_value = mode_value | Mode;
	Port->MODER = mode_value;
}

void setupIO()
{
	RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
	display_begin();
	pinMode(GPIOB,4,0);
	pinMode(GPIOB,5,0);
	pinMode(GPIOA,8,0);
	pinMode(GPIOA,11,0);
	pinMode(GPIOB,0,1);
	pinMode(GPIOB,3,1);
	enablePullUp(GPIOB,4);
	enablePullUp(GPIOB,5);
	enablePullUp(GPIOA,11);
	enablePullUp(GPIOA,8);
}