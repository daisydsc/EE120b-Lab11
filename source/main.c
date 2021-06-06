/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
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

enum 1p_states { 1pDisplay };
int 1pdisplay(int state) {
	unsigned char column = 0x7F;
	unsigned char pattern;
	unsigned char sequence [8] = {0x00, 0x00, 0x7E, 0x00, 0x7E, 0x48, 0x48, 0x30};
	static unsigned char count = 0;
	switch(state) {
		case 1pDisplay:
			break;
		default:
			state = 1pDisplay;
			break;
	}
	switch(state) {
		for(count = 0; count < 8; count++) {
			



enum Drop_states { DropStart, DropWait, DropRight, DropLeft, DropEnter, DropPressed };
int Drop(int state){
	static unsigned char column = 0x7F;
	static unsigned char down = 0x80;
	switch(state) {
		case DropStart:
			state = DropWait;
			break;
		case DropWait:
			if(~PINA & 0x01) {
				state = DropRight;
			}
			else if(~PINA & 0x04) {
				state = DropLeft;
			}
			else if(~PINA & 0x02) {
				state = DropEnter;
			}
			else {
				state = DropWait;
			}
			break;
		case DropRight:
			if(~PINA & 0x01) {
				state = DropPressed;
			}
			else {
				state = DropWait;
			}
			break;
		case DropLeft:
			if(~PINA & 0x04) {
				state = DropPressed;
			}
			else {
				state = DropWait;
			}
			break;
		case DropEnter:
			if(~PINA & 0x04) {
				state = DropEnter;
			}
			else {
				state = DropWait;
			}
			break;
		case DropPressed:
			if(!(~PINA & 0x01) & !(~PINA & 0x04) & !(~PINA & 0x02)) {
				state = DropWait;
			}
			else {
				state = DropPressed;
			}
			break;
	}
	switch(state){
		case DropStart:
			break;
		case DropWait:
			break;
		case DropRight:
			if(column != 0xFE) {
				column = (column >> 1) | 0x80;
			}
			break;
		case DropLeft:
			if(column != 0x7F) {
				column = (column << 1) | 0x01;
			}
			break;
		case DropEnter:
			if(down != 0x01) {
				down >>= 1;
			}
			break;
		case DropPressed:
			break;
	}
	PORTC = down;
	PORTD = column;
	return state;
}




int main(){
    DDRA = 0x00; PORTA = 0xFF;
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    static task task1;
    task *tasks[] = {&task1};
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    const char start = -1;

    task1.state = DropStart;
    task1.period = 100;
    task1.elapsedTime = task1.period;
    task1.TickFct = &Drop;

    TimerSet(100);
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
