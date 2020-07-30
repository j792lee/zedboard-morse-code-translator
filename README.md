# zedboard-morse-code-translator

This program is a two way translator between morse code and ascii using the Vivado Zedboard. To make the OLED display functional, I used Texas A&M's ip, which can be found [here](https://github.com/ldolson/ZedboardOLED-v1.0-IP).

## How to load onto your ZedBoard

Before you start, make sure you have Vitis installed

1. Clone or Download the repository

`git clone https://github.com/j792lee/zedboard-morse-code-translator.git`

2. Make a new project in Vitis

![Vitis new pro](./images/new_pro.png?raw=true "Title")

3. When asked for the platform, click on the "Create a new platform from hardware" tab and click on the '+'

![XSA example](./images/xsa_show.png?raw=true "Title")

4. Upload OLED_LED_BUTTON_design_wrapper.xsa from the xsa folder

![XSA file](./images/files.png?raw=true "Title")
