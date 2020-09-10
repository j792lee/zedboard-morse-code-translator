# zedboard-morse-code-translator

This program is a two way translator between morse code and ascii using the Vivado Zedboard. To make the OLED display functional, I used Texas A&M's ip, which can be found [here](https://github.com/ldolson/ZedboardOLED-v1.0-IP).

## How to load onto your ZedBoard

Before you start, make sure you have Vitis installed and your Zedboard is turned on and connected to your computer.

1. Clone or Download the repository

`git clone https://github.com/j792lee/zedboard-morse-code-translator.git`

2. Make a new project in Vitis

![Vitis new pro](./images/new_pro.png?raw=true "Title")

3. When asked for the platform, click on the "Create a new platform from hardware" tab and click on the '+'

![XSA example](./images/xsa_show.png?raw=true "Title")

4. Upload OLED_LED_BUTTON_design_wrapper.xsa from the xsa folder

![XSA file](./images/files.png?raw=true "Title")

5. Finish creating the project then replace the default src folder with this project's src folder, then build the project

6. Plug in the Zedboard using the j-tag input and launch the application oh hardwate

![launch on hardware](./images/launch_on_hardware.png?raw=true "Title")

## Using the Morse code encoder

First, select the "Encode" option on the main menu using the Zedboard's up and down d-pad buttons to move the arrow, and the middle d-pad button to select. Then, input the message you would like to be translated in a similar method, only with the right d-pad button to go to the next letter. When you are done inputting your message, press the middle button and your message should be displayed in morse code using the LEDs.

## Using the Morse code decoder

Select the "Decode" option on the main menu using the Zedboard's up and down d-pad buttons to move the arrow, and the middle d-pad button to select. Then, communicate in morse code as you would with a morse code transmitter. As you tap each letter, the corresponding letter should appear on the OLED display.
