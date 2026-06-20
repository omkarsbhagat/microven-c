MICROWAVE OVEN SIMULATION

Description:
A microwave oven simulation developed using PIC16F877A microcontroller, CLCD, matrix keypad, Timer2 interrupts, buzzer, and LEDs. The project provides multiple cooking modes with time and temperature control similar to a real microwave oven.

Features:
• Microwave Mode
• Grill Mode
• Convection Mode
• Quick Start Mode
• Temperature Setting
• Preheating Function
• Cooking Time Setting
• Pause / Resume Operation
• Buzzer Indication on Completion
• CLCD User Interface
• Matrix Keypad Input
• Timer2 Interrupt Based Countdown

Files:
• main.c             - Application logic and state machine
• clcd.c/.h          - CLCD driver
• matrix_keypad.c/.h - Keypad driver
• timers.c/.h        - Timer2 configuration
• Makefile           - Build configuration

Hardware Used:
• PIC16F877A
• 20MHz Crystal Oscillator
• 16x4 CLCD
• Matrix Keypad
• Buzzer
• LEDs

Project Flow:
Startup → Mode Select → (Set Temperature & Preheat for Convection) → Set Time → Run → Pause/Resume → Complete → Mode Select

Compiler:
XC8 Compiler

IDE:
MPLAB X IDE
