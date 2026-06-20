/*
 * File:   main.c
 * Author: omkar
 *
 * Created on 19 April, 2026, 9:29 AM
 */


#include <xc.h>
#include "clcd.h"
#include "matrix_keypad.h"
#include "timers.h"

#pragma config WDTE = OFF

#define MODE_SELECT 1
#define SHOW_POWER  2
#define SET_TIME    3
#define RUN         4
#define PAUSE       5
#define COMPLETE    6
#define SET_TEMP    7
#define PREHEAT     8
#define DONE_MSG    9



volatile unsigned char state=MODE_SELECT;
unsigned char prev_state=255;
unsigned char mode=1;

unsigned char t1=0,t2=0,t3=0,t4=0;
unsigned char pos=0;

unsigned char c1=0,c2=0,c3=1;
unsigned char tpos=0;
unsigned int conv_temp=1;

volatile unsigned int minutes=0;
volatile unsigned int seconds=0;
volatile unsigned int preheat_sec=60;

volatile unsigned int t2_ticks=0;
volatile unsigned char one_second_flag=0;

void __interrupt() isr(void)
{
 if(TMR2IF)
 {
   TMR2IF=0;
   //t2_ticks++;
   if(t2_ticks++>=1000)
   {
      t2_ticks=0;
      one_second_flag=1;
   }
 }
}

void clear_all(void)
{
clcd_print("                ",LINE1(0));
clcd_print("                ",LINE2(0));
clcd_print("                ",LINE3(0));
clcd_print("                ",LINE4(0));
}

void reset_time(void)
{
t1=t2=t3=t4=0;
pos=0;
minutes=0;
seconds=0;
t2_ticks=0;
one_second_flag=0;
}

void startup_screen(void)
{
int i;
clear_all();
clcd_print("Powering On",LINE2(2));
clcd_print("Microwave Oven",LINE3(1));
for(i=0;i<16;i++)
{
clcd_putch(255,LINE1(i));
clcd_putch(255,LINE4(i));
__delay_ms(150);
}
__delay_ms(1000);
}

void display(unsigned char key)
{
if(state!=prev_state)
{
clear_all();
prev_state=state;
}

switch(state)
{
case MODE_SELECT:
RC2=0;
RC3=0;
clcd_print("1 Micro Mode    ",LINE1(0));
clcd_print("2 Grill Mode    ",LINE2(0));
clcd_print("3 Conv Mode     ",LINE3(0));
clcd_print("4 Start Mode  ",LINE4(0));
if(key==1){mode=1;state=SHOW_POWER;}
if(key==2){mode=2;state=SHOW_POWER;}
if(key==3){mode=3;state=SHOW_POWER;}
if(key==4){mode=4;state=SHOW_POWER;}
break;

case SHOW_POWER:
if(mode==1) clcd_print("Power = 900W",LINE2(0));
if(mode==2) clcd_print("Power = 700W",LINE2(0));
if(mode==3) clcd_print("Power =1200W",LINE2(0));
if(mode==4) clcd_print("Power = 900W",LINE2(0));
__delay_ms(2000);
if(mode==3) state=SET_TEMP;
else if(mode==4)
{
minutes=0;
seconds=30;
t2_ticks=0;
one_second_flag=0;
TMR2ON=1;
state=RUN;
}
else
{
TMR2ON=0;   /* STOP PREHEAT TIMER */
state=SET_TIME;
}
break;

case SET_TEMP:
RC2=0;
RC3=0;
clcd_print("SET TEMP (*C)",LINE1(0));
clcd_print("TEMP = ",LINE2(0));
clcd_putch(c1+'0',LINE2(7));
clcd_putch(c2+'0',LINE2(8));
clcd_putch(c3+'0',LINE2(9));
clcd_print("*:CLEAR",LINE4(0));
clcd_print("#:ENTER",LINE4(9));

if(key<=9)
{
if(tpos==0){c1=key;tpos++;}
else if(tpos==1){c2=key;tpos++;}
else if(tpos==2){c3=key;}
}
if(key=='*')
{c1=0;c2=0;c3=1;tpos=0;}
if(key=='#')
{
conv_temp=(c1*100)+(c2*10)+c3;
preheat_sec=60;
t2_ticks=0;
one_second_flag=0;
TMR2ON=1;   /* START TIMER FOR PREHEAT */
state=PREHEAT;
}
break;

case PREHEAT:
RC2=1;
RC3=1;
clcd_print("Pre-heating",LINE1(0));
clcd_print("Time left:",LINE2(0));
clcd_putch((preheat_sec/10)+'0',LINE2(10));
clcd_putch((preheat_sec%10)+'0',LINE2(11));
clcd_print("s",LINE2(12));

if(one_second_flag)
{
one_second_flag=0;
if(preheat_sec>0) preheat_sec--;
else state=SET_TIME;
}
break;

case SET_TIME:
RC2=0;
RC3=0;
clcd_print("SET TIME (MM:SS)",LINE1(0));
clcd_print("TIME:",LINE2(0));
clcd_putch(t1+'0',LINE2(5));
clcd_putch(t2+'0',LINE2(6));
clcd_putch(':',LINE2(7));
clcd_putch(t3+'0',LINE2(8));
clcd_putch(t4+'0',LINE2(9));
clcd_print("*:CLEAR",LINE4(0));
clcd_print("#:ENTER",LINE4(9));

if(key<=9)
{
if(pos==0){t1=key;pos++;}
else if(pos==1){t2=key;pos++;}
else if(pos==2){t3=key;pos++;}
else if(pos==3){t4=key;}
}
if(key=='*'){t1=t2=t3=t4=0;pos=0;}
if(key=='#')
{
minutes=(t1*10)+t2;
seconds=(t3*10)+t4;
t2_ticks=0;
one_second_flag=0;
TMR2ON=1;
state=RUN;
}
break;

case RUN:
RC2=1;
RC3=1;
RC2=1; RC3=1;
if(mode==1) clcd_print("MICRO RUNNING",LINE1(0));
if(mode==2) clcd_print("GRILL RUNNING",LINE1(0));
if(mode==3) clcd_print("CONV RUNNING",LINE1(0));
if(mode==4) clcd_print("START RUNNING",LINE1(0));

clcd_print("TIME:",LINE2(0));
clcd_putch((minutes/10)+'0',LINE2(5));
clcd_putch((minutes%10)+'0',LINE2(6));
clcd_putch(':',LINE2(7));
clcd_putch((seconds/10)+'0',LINE2(8));
clcd_putch((seconds%10)+'0',LINE2(9));
clcd_print("* Pause",LINE3(0));
clcd_print("0 Stop",LINE4(0));

if(key=='*')
{
   TMR2ON=0;
   state=PAUSE;
}

if(key==0)
{
   TMR2ON=0;
   reset_time();
   state=MODE_SELECT;
}

if(one_second_flag)
{
one_second_flag=0;
if(seconds>0) seconds--;
else
{
if(minutes>0){minutes--;seconds=59;}
else
{
if(mode==3) state=DONE_MSG;
else state=COMPLETE;
}
}
}
break;

case COMPLETE:
RC2=0;
RC3=0;
{
int i;
TMR2ON=0;
clcd_print("COOK COMPLETE",LINE2(0));
for(i=0;i<5;i++)
{
RC1=1; __delay_ms(500);
RC1=0; __delay_ms(500);
}
reset_time();
state=MODE_SELECT;
}
break;

case DONE_MSG:
RC2=0;
RC3=0;
{
int i;
TMR2ON=0;
clcd_print("TIME UP!!!",LINE2(2));
clcd_print("Enjoy your Meal",LINE3(0));
for(i=0;i<5;i++)
{
RC1=1; __delay_ms(500);
RC1=0; __delay_ms(500);
}
reset_time();
c1=0;c2=0;c3=1;tpos=0;
state=MODE_SELECT;
}
break;
case PAUSE:

RC2=0;
RC3=0;

clcd_print("COOK PAUSED",LINE2(0));

clcd_print("# Resume",LINE3(0));

clcd_print("0 Stop",LINE4(0));

if(key=='#')
{
   TMR2ON=1;
   state=RUN;
}

if(key==0)
{
   TMR2ON=0;
   reset_time();
   state=MODE_SELECT;
}

break;
}
}

void init_config(void)
{
init_clcd();
init_matrix_keypad();
init_timer2();
TMR2ON=0;
PEIE=1;
GIE=1;
TRISC1=0;
TRISC2=0;
TRISC3=0;
startup_screen();
}

void main(void)
{
unsigned char key;

init_config();

while(1)
{
key=read_matrix_keypad(LEVEL);

/* simple debounce */
key=read_matrix_keypad(LEVEL);

if(key!=ALL_RELEASED)
{
__delay_ms(150);
while(read_matrix_keypad(LEVEL)!=ALL_RELEASED);
}

display(key);
}
}