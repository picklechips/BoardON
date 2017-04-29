# BoardON
A 2D endless scrolling snowboarding game developed using C++ and SDL2 available on both windows and linux

![Screenshot](http://www.ryan-martin.ca/ressources/supesnow-forest.png)

## How to play:
- Use A/D or Left/Right keys to move side to side
- Use W/S or Up/Down keys to speed up or slow down
- Score increases automatically
- Jump over obstacles or off cliffs to increase your score multiplier
- Once you crash 3 times the game is over and you can enter your highscore

## Installation:
#### Windows:
A compiled version is available.
Download the zip file here: http://bit.ly/1t6ddIl

Extract it, go into sources and run the .exe

#### Linux:
SDL2 and SDL2-ttf are required.

Open up a terminal and type

`apt-get install libsdl2-dev`

then

`apt-get install libsdl2-ttf-dev`

Once that's done, download this repository and cd into the `src` folder.

To compile it, type

`make all`

then to run it either type 
`./BoardON`
or use a file explorer to navigate to the `src` folder and manually run `BoardON`
