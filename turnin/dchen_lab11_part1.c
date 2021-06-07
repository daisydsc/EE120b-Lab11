/*	Author: Daisy Cheng dchen213@ucr.edu
 *	Lab Section: 021
 *	Assignment: Lab # 11 Exercise # N/A
 *	Exercise Description: 
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "timer.h"
#include "scheduler.h"
#include "bit.h"
#include "keypad.h"
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

//=====================================================================
	
// 		DEMO CODE TO TEST LED MATRIX
		
//=====================================================================
/*
enum Demo_States {shift};
int Demo_Tick(int state){
        static unsigned char pattern = 0x80;
        static unsigned char row = 0x7F;
        switch(state){
                case shift:
                        break;
                default:
                        state = shift;
                        break;
        }
        switch(state){
                case shift:
                        if(row == 0xFE && pattern == 0x01){
                                pattern = 0x80;
                                row = 0x7F;
                        }
                        else if(pattern == 0x01){
                                pattern = 0x80;
                                row = (row >> 1) | 0x80;
                        }
                        else{
                                pattern >>= 1;
                        }
                        break;
                default:
                        break;
        }
        PORTC = pattern;
        PORTD = row;
        return state;
}
*/	

//=====================================================================
	
// 		GLOBAL VARIABLES
		
//=====================================================================
unsigned char columnCount[8] = {7, 7, 7, 7, 7, 7, 7, 7};
unsigned char columnPattern[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char columnNum = 0;
unsigned char column = 0x7F;
unsigned char down = 0x80;
unsigned char i = 0;
unsigned char tempD1;
unsigned char tempD2;
unsigned char tempC1;
unsigned char tempC2;
unsigned char Winner;

//=====================================================================
	
// 		ALLOWS GAME TO BE RESET WHEN SOMEONE WINS
		
//=====================================================================
// enum Game_states { GameStart, GamePlaying, GameRestart };
// int Game(int state) {
// 	switch(state) {
// 		case GameStart:
// 			state = GamePlaying;
// 			break;
// 		case GamePlaying:
// 			if(Winner != 1) {
// 				state = GamePlaying;
// 			}
// 			else {
// 				state = GameRestart;
// 			}
// 			break;
// 		case GameRestart:
// 			state = GameStart;
// 			break;
// 	}
// 	switch(state) {
// 		case GameStart:
// 			break;
// 		case GamePlaying:
// 			break;
// 		case GameRestart:
// 			for(i = 0; i < 8; i++) {
// 				columnCount[i] = 7;
// 				columnPattern[i] = 0x00;
// 			}
// 			columnNum = 0;
// 			column = 0x7F;
// 			down = 0x80;
// 	}
// 	return state;
// }

//=====================================================================
	
// 		GAMEBOARD: KEEPS TRACK OF PIECES PLAYED
		
//=====================================================================
/*
enum Board_states { Board };
int GameBoard(int state) {
	static unsigned char pattern;
        static unsigned char row = 0x7F;
	static unsigned char rowNum = 0;
	switch(state) {
		case Board:
			state = Board;
			break;
	}
	switch(state) {
		case Board:
			if(row == 0xFE){
                                row = 0x7F;
				rowNum = 0;
                        }
                        else{
                                row = (row >> 1) | 0x80;
				rowNum += 1;
                        }
                        break;
	}
	tempD2 = row;
	tempC2 = columnPattern[rowNum];
	return state;
}*/

//=====================================================================
	
// 		"CHIP" DROPS DOWN WHEN ENTER BUTON PRESSED
		
//=====================================================================
enum Drop_states { DropStart, DropWait, DropEnter };
int Drop(int state) {
	switch(state) {
		case DropStart:
			state = DropWait;
			break;
		case DropWait:
			if(~PINA & 0x02) {
				state = DropEnter;
			}
			else {
				state = DropWait;
			}
			break;
		case DropEnter:
			if(i < columnCount[columnNum]) {
				state = DropEnter;
				i++;
			}
			else {
				state = DropWait;
				i = 0;
				columnCount[columnNum] -= 1;
				columnPattern[columnNum] = (columnPattern[columnNum] << 1) | 0x01;
				columnNum = 0;
				column = 0x7F;
			}
			break;
	}
	switch(state) {
		case DropStart:
			break;
		case DropWait:
			down = 0x80;
			break;
		case DropEnter:
			down >>= 1;
			break;
	}
	PORTC = down;
	return state;
}
			
				
//=====================================================================
	
// 		COLUMN SELECTION WITH BUTTONS
		
//=====================================================================
enum Column_states { ColumnStart, ColumnWait, ColumnRight, ColumnLeft, ColumnPressed };
int ColumnSelect(int state){
	switch(state) {
		case ColumnStart:
			state = ColumnWait;
			break;
		case ColumnWait:
			if(~PINA & 0x01) {
				state = ColumnRight;
			}
			else if(~PINA & 0x04) {
				state = ColumnLeft;
			}
			else {
				state = ColumnWait;
			}
			break;
		case ColumnRight:
			if(~PINA & 0x01) {
				state = ColumnPressed;
			}
			else {
				state = ColumnWait;
			}
			break;
		case ColumnLeft:
			if(~PINA & 0x04) {
				state = ColumnPressed;
			}
			else {
				state = ColumnWait;
			}
			break;
		case ColumnPressed:
			if(!(~PINA & 0x01) & !(~PINA & 0x04)) {
				state = ColumnWait;
			}
			else {
				state = ColumnPressed;
			}
			break;
	}
	switch(state){
		case ColumnStart:
			break;
		case ColumnWait:
			break;
		case ColumnRight:
			if(column != 0xFE) {
				column = (column >> 1) | 0x80;
				columnNum += 1;
			}
			break;
		case ColumnLeft:
			if(column != 0x7F) {
				column = (column << 1) | 0x01;
				columnNum -= 1;
			}
			break;
		case ColumnPressed:
			break;
	}
	PORTD = column;
	return state;
}

//=====================================================================
	
// 		CHECK FOR WIN CONDITIONS
		
//=====================================================================
// enum Win_States { WinRow, WinColumn }
// int CheckWin(int state) {
// 	static char j = 0;
// 	switch(state) {
// 		case WinRow:
// 			state = WinColumn;
// 			break;
// 		case WinColumn:
// 			state = WinRow;
// 			break;
// 	}
// 	switch(state) {
// 		case WinRow:
// 			unsigned char test = 0x01;
// 			for(i = 0; i < 8; i++) {
// 				for(j = 0; j < 5; j++) {
// 					if(((ColumnPattern[j] & test) == test) && ((ColumnPattern[j + 1] & test) == test) && ((ColumnPattern[j + 2] & test) == test) && ((ColumnPattern[j + 3] & test) == test)) {
// 						Winner = 1;
// 					}
// 				}
// 				test <<= 1;
// 			}
// 			break;
// 		case WinColumn:
// 			for(j = 0; j < 4; j++) {
// 				if(((ColumnPattern[j] & 0x0F) == 0x0F) ||  ((ColumnPattern[j] & 0x1E) == 0x1E) ||  ((ColumnPattern[j] & 0x3C) == 0x3C) ||  ((ColumnPattern[j] & 0x78) == 0x78)) {
// 					Winner = 1;
// 				}
// 			}
// 			break;
// 	}
// 	return state;
// }


//=====================================================================
	
// 		COMBINES PREVIOUS SM'S TO DISPLAY ALL PARTS
		
//=====================================================================
// enum Display_states { DisplayLoop };
// int Display(int state) {
// 	switch (state) {
// 		case DisplayLoop:
// 			state = DisplayLoop;
// 			break;
// 	}
// 	switch (state) {
// 		case DisplayLoop:
// 			PORTD = tempD1 & tempD2;
// 			PORTC = tempC1 | tempC2;
// 			break;
// 	}
// 	return state;
// }




int main(){
    DDRA = 0x00; PORTA = 0xFF;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    static task task1, task2, task3, task4, task5, task6;
    task *tasks[] = {&task1, &task2, &task3, &task4, &task5, &task6};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    const char start = -1;

    task1.state = DropStart;
    task1.period = 100;
    task1.elapsedTime = task1.period;
    task1.TickFct = &Drop;
	
    task2.state = ColumnStart;
    task2.period = 100;
    task2.elapsedTime = task2.period;
    task2.TickFct = &ColumnSelect;
	
//     task3.state = Board;
//     task3.period = 1;
//     task3.elapsedTime = task3.period;
//     task3.TickFct = &GameBoard;
	
//     task4.state = DisplayLoop;
//     task4.period = 1;
//     task4.elapsedTime = task4.period;
//     task4.TickFct = &Display;
	
//     task5.state = GameStart;
//     task5.period = 50;
//     task5.elapsedTime = task5.period;
//     task5.TickFct = &Game;
	
//     task6.state = WinRow;
//     task6.period = 500;
//     task6.elapsedTime = task6.period;
//     task6.TickFct = &CheckWin;
	
    unsigned long GCD = tasks[0]->period;
	for (i = 1; i < numTasks; i++) {
		GCD = findGCD(GCD, tasks[i]->period);
	}

    TimerSet(GCD);
    TimerOn();

    unsigned short i;
    while(1){
        for(i = 0; i < numTasks; i++){
            if(tasks[i]->elapsedTime == tasks[i]->period){
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += 50;
        }
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 0;
}
