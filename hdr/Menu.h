//Edited by: Keenan Longair.
//Last update: 11:50AM February 24th, 2016.
//Purpose: To prototype the interface needed to create a spash screen
//and main menu.
//Version: 0.6

#ifndef MENU_H_
#define MENU_H_

//Libraries required:
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <GL/freeglut.h>
#include <cstdio>

//Required files:
#include "ImageLoader.h"

//Classes required:
class imageLoader;

//Class description below:
class Menu {
	
	private:

		//Contructor
		Menu() {
			
			m_width = 500;//Need a set width.			
			m_height = 500;//Need a set height.
			
		};
		
		//Menu and splashScreen height and width variables.
		int m_width;
		int m_height;

	public:
		
		//Required Functions:
		void init();//Prepares the various files.
		void update();//Handles updateing any menu information if needed.
		void splashScreen();//Provides the spashscreen for pausing the
		//game.
		void mainMenu();//Provides the main menu presented at the start
		//of the game.

		//Extra goals, functions for the future:
		//void inventorymenu();//Provides an inventory menu.
		//void winScreen();//Provides the screen presenting the win message.
		//void loseScreen();//Provides the screen presenting the lose message.
		
};

#endif /* MENU_H_ */