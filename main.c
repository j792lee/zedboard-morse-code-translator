#include <stdio.h>
#include <stdlib.h>
#include "platform.h"
#include "xparameters.h"
#include "xgpio.h"
#include "xstatus.h"
#include "xil_printf.h"
#include "ZedboardOLED.h"
#include <string.h>
#include <math.h>
#include <assert.h>
#include "sstack.h"

/* Definitions */
#define LED_DEVICE_ID  XPAR_AXI_GPIO_1_DEVICE_ID       /* GPIO device that LEDs are connected to */
#define BUTTON_DEVICE_ID  XPAR_AXI_GPIO_0_DEVICE_ID    /* GPIO device that buttons are connected to */
#define LED 0xFF									   /* Initial LED value - XXXXXXXX */
#define LED_DELAY 10000000							   /* Software delay length */
#define LED_CHANNEL 1                                  /* GPIO port for LEDS */
#define BUTTON_CHANNEL 1                               /* GPIO port for buttons */
#define printf xil_printf							   /* smaller, optimized printf */
#define BOUNCE 200000						           /* bounce of the button */
#define TIME_UNIT 900000							   /* length of one dot */
#define MIDDLE_BUTTON 0b00001						   /* code for middle button */
#define LEFT_BUTTON 0b00100							   /* code for left button */
#define RIGHT_BUTTON 0b01000						   /* code for right button */
#define TOP_BUTTON 0b00010							   /* code for top button */
#define BOTTOM_BUTTON 0b10000						   /* code for bottom button */
#define HASH_DOT 7							           /* converts dots to 7 for hash function */
#define HASH_DASH 3							           /* converts dashes to 3 for hash function */

// hash table for decoding
const char lookup[77] = {'D', '?', '?', 'I', 'T', '?', '?', '?', 'E', 'L', '6', '?', '?', 'Y', '?', '?', 'U',
		'C', '8', '?', 'S', '?', '?', '?', '?', 'V', '?', '?', '?', 'H', '4', '?', 'O', 'Q', '5', 'M', 'G', 'Z',
		'7', 'N', '?', 'J', '?', '?', '?', 'P', '2', '?', '?', '?', '0', '?', 'W', 'X', '9', '?', 'R', 'B', '?',
		'?', '?', '?', '?', '?', '?', 'F', '3', '?', '?', '?', '?', '?', 'K', '?', '1', 'A'};

const int encode_letter[44] = {33333, 33337, 33377, 33777, 37777, 77777, 77773, 77733, 77333, 73333, 0, 0, 0, 0, 0, 0, 0,
							37, 7773, 7373, 773, 7, 7377, 733, 7777, 77, 3337, 373, 7737, 33, 73, 333, 7337, 3733, 737, 777,
							3, 377, 3777, 337, 3773, 3373, 7733};

const char alphabet[37] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
							'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
							' '};

char *get_letters(XGpio *input) {
	struct sstack *to_decode = sstack_create();
	int Status;
	char printed[16] = {'\0'};
	int printed_len = 0;
	int button_data = 0;
	int curr_char = 0;

	Status = XGpio_Initialize(input, BUTTON_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XGpio_SetDataDirection(input, BUTTON_CHANNEL, 0xF);

	while (1) {
		button_data = XGpio_DiscreteRead(input, BUTTON_CHANNEL);	 /* get which button is pressed */

		if (button_data == BOTTOM_BUTTON){
			curr_char--;
			if (curr_char < 0) {
				curr_char = 36;
			}
			clear();
		} else if (button_data == TOP_BUTTON){
			curr_char++;
			if (curr_char > 36) {
				curr_char = 0;
			}
			clear();
		} else if (button_data == RIGHT_BUTTON){
			char new_letter = alphabet[curr_char];

			if (printed_len < 15) {
				printed[printed_len] = new_letter;
				printed_len++;
			} else {
				for (int i = 0; i < 14; i++) {
					printed[i] = printed[i+1];
				}
				printed[14] = new_letter;
			}

			char to_push[2] = {'\0'};
			to_push[0] = new_letter;

			sstack_push(to_push, to_decode);
			clear();
		} else if (button_data == MIDDLE_BUTTON){
			clear();
			print_message(printed, 0);
			break;
		}

		print_message(printed, 0);
		print_char(alphabet[curr_char], 0, printed_len);


		for (int delay = 0; delay < 10000000; delay++);
	}

	int num_chars = sstack_get_len(to_decode);
	char *inputted_str = malloc(sizeof(char) * (num_chars + 1));
	inputted_str[num_chars] = '\0';
	for (int i = num_chars - 1; i >= 0; i--) {
		char *temp = sstack_pop(to_decode);
		inputted_str[i] = temp[0];
		free(temp);
	}

	printf("%s", inputted_str);
	sstack_destroy(to_decode);

	return inputted_str;
}

int encode(XGpio *output, XGpio *input){
	for (int Delay = 0; Delay < LED_DELAY * 3; Delay++);
	int Status;
	char *to_encode = get_letters(input);

	/* GPIO driver initialization */
	Status = XGpio_Initialize(output, LED_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*Set the direction for the LEDs to output. */
	XGpio_SetDataDirection(output, LED_CHANNEL, 0x00);
	for (int i = 0; to_encode[i]; i++) {
		for (int Delay = 0; Delay < LED_DELAY * 3; Delay++);
		if (to_encode[i] == ' ') {
			XGpio_DiscreteWrite(output, LED_CHANNEL, ~LED);
			for (int Delay = 0; Delay < LED_DELAY * 4; Delay++);
		} else {
			int encoded_letter = encode_letter[to_encode[i] - '0'];
			while (encoded_letter) {
				int current_sym = encoded_letter % 10;
				if (current_sym == 7) {
					XGpio_DiscreteWrite(output, LED_CHANNEL, LED);
					for (int Delay = 0; Delay < LED_DELAY; Delay++);
				} else if (current_sym == 3) {
					XGpio_DiscreteWrite(output, LED_CHANNEL, LED);
					for (int Delay = 0; Delay < LED_DELAY * 3; Delay++);
				}
				XGpio_DiscreteWrite(output, LED_CHANNEL, ~LED);
				for (int Delay = 0; Delay < LED_DELAY; Delay++);
				encoded_letter /= 10;
			}
		}
	}
	free(to_encode);
	return XST_SUCCESS; /* Should be unreachable */
}


// lookup_letter(morse_letter, morse_len) returns the letter associated with the morse code letter, where dots are
// 		represented as HASH_DOT and dashes are represented as HASH_DASH
// requires: morse_letter is composed of only HASH_DOTs and HASH_DASHs [not asserted]
//			 morse_len >= 0
char lookup_letter(int morse_letter, int morse_len) {
	assert(morse_len >= 0);
	if (!morse_len) {
		return ' ';
	}
	int index = (morse_letter + morse_len) % 76;     		/* hash function to lookup letter */
	return lookup[index];
}


int decode(XGpio *input){
	assert(input);

	int Status;
	int button_data = 0;
	unsigned long hold_count = 0;							/* length of time user holds down a button for */
	unsigned long space_count = 0;							/* length of time no button is pressed */
	int morse_letter = 0;									/* the letter the user is typing in morse code*/
	int morse_len = 0;										/* length of morse_letter */
	char decode[17] = {'\0'};								/* decoded sentence */
	int decode_len = 0;										/* length of decoded sentence */

	Status = XGpio_Initialize(input, BUTTON_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XGpio_SetDataDirection(input, BUTTON_CHANNEL, 0xF);

	while(1){
	   button_data = XGpio_DiscreteRead(input, BUTTON_CHANNEL);	 /* get which button is pressed */

	   if (button_data == RIGHT_BUTTON) {							/* quit button */
		   return 0;
	   }

	   if (button_data == LEFT_BUTTON) {                			/* clear button */
	   		   clear();
	   		   for (int i = 0; i < 17; i++){
	   			   decode[i] = '\0';
	   		   }
	   		   decode_len = 0;
	   		   morse_letter = 0;
	   		   morse_len = 0;
	   		   clear();
	   	   }

	   while (!button_data){										/* no button pressed */
		   button_data = XGpio_DiscreteRead(input, BUTTON_CHANNEL);
		   space_count++;

	   }

	   if (space_count <= ((TIME_UNIT * 5) - TIME_UNIT/2) && space_count >= TIME_UNIT * 2) {   /* end of letter */
		   clear();
		   char new_letter = lookup_letter(morse_letter, morse_len);
		   if (decode_len >= 16) {								/* scrolling of decoded message */
			   for (int i = 0; i < 15; i++) {
				   decode[i] = decode[i+1];
			   }
			   decode[15] = new_letter;
		   } else {
			   decode[decode_len] = new_letter;
			   decode_len++;
		   }
		   print_message(decode, 2);
		   space_count = 0;
		   morse_letter = 0;
		   morse_len = 0;
		   continue;
	   } else if (space_count >= (TIME_UNIT * 5) - TIME_UNIT/2) {	/* letter is a space*/
		   clear();
		   if (decode_len >= 16) {									/* scrolling of decoded message */
			   for (int i = 0; i < 15; i++) {
				   decode[i] = decode[i+1];
			   }
			   decode[15] = ' ';
		   } else {
			   decode[decode_len] = ' ';
			   decode_len++;
		   }
		   print_message(decode, 2);
		   space_count = 0;
		   morse_letter = 0;
		   morse_len = 0;
		   continue;
	   }
	   space_count = 0;


	   while (button_data == MIDDLE_BUTTON) {					/* middle button pressed */
		   button_data = XGpio_DiscreteRead(input, BUTTON_CHANNEL);
		   hold_count++;
	   }

	   if (hold_count <= TIME_UNIT && hold_count >= BOUNCE) {	/* user held down to signal a dot */
		   print_char('.', 0, morse_len);
		   morse_letter = morse_letter * 10 + HASH_DOT;         /* Adding a dot to the letter being sent */
		   morse_len++;
	   } else if (hold_count >= TIME_UNIT) {					/* user held down to signal a dash */
		   print_char('-', 0, morse_len);
		   morse_letter = morse_letter * 10 + HASH_DASH;  		/* Adding a dash to the letter being sent */
		   morse_len++;
	   }
	   hold_count = 0;

	}
	return 0;
}


/* Main function. */
 int main(void){
	int Status;
	XGpio input, output;									/* GPIO channels */
	int mode = 0;											/* counter to see if the user wants to encode or decode */
	int up_down = 0;										/* button data */

	init_platform();

	Status = XGpio_Initialize(&input, BUTTON_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XGpio_SetDataDirection(&input, BUTTON_CHANNEL, 0xF);

	while(1) {
		print_message("  SELECT MODE   ", 0);
		up_down = XGpio_DiscreteRead(&input, BUTTON_CHANNEL);

		if (up_down == TOP_BUTTON) {
			mode++;
			clear();
		} else if (up_down == BOTTOM_BUTTON) {
			mode--;
			clear();
		} else if (up_down == MIDDLE_BUTTON) {
			clear();
			if (mode % 2 == 0) {
				encode(&output, &input);
				clear();
			} else {
				decode(&output);
				clear();
			}
		}

		if (mode % 2 == 0) {									/* switching the arrow */
			print_message("  -> ENCODE     ", 2);
			print_message("     DECODE     ", 3);
		} else {
			print_message("     ENCODE     ", 2);
			print_message("  -> DECODE     ", 3);
		}
	}
	clear();

	cleanup_platform();
	return 0;
}
