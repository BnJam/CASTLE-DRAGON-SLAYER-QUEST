//Edited by: Keenan Longair.
//Last update: 12:00PM March 12th, 2016.
//Purpose: Contains the body of the game file prototyped in the Game.h file.
//Version: 1.4
//**************************************************************************************
//TODO: Add a restart function ability to the code so we can restart the game from within.
//Not sure how to add this ability yet but I believe it would have to be part of the \
//win/lose code. So far only a win/lose screen from menu has been coded. 
//TODO: Test the key input during paused to ensure that the player cannot move, or do 
//anything except unpause the game. 

//Required Libraries:
#include <cstdio>
#include <stdlib.h>
#include <GL/glut.h>/*glut.h includes gl.h and glu.h*/
#include <math.h>
#include <time.h>
#include <GL/freeglut.h>

//Required Files:
#include "../hdr/Game.h"//Prototype of this file.
#include "../hdr/Jukebox.h"//Dependancy for Sound.
#include "../hdr/Obstacle.h"//Dependancy for Obstacles.
#include "../hdr/ImageLoader.h"//Dependancy for Images.
#include "../hdr/MapHandler.h"//Dependancy for the Map.
#include "../hdr/Player.h"//Dependancy for the Player.
#include "../hdr/ItemHandler.h"//Dependancy for the item handler.
#include "../hdr/HUDHandler.h" //HUD Dependency
#include "../hdr/ZombieHandler.h"//Zombie Handler dependancy.

//Main Variables:
bool Game::c_run = false;//Set the game to display the main menu. Once changed this
//moves the game along.
bool Game::c_running = false;//Set the running state to false to start the game
//with the splashscreen displayed.
bool* Game::keystates = new bool[256];
int Game::m_lastSong = 6000000;


/****Main Work Functions***************************************************************/
void Game::init()
//Initialization function.
{
	
	//TODO remove the next 2 lines if the random number function is not needed.
    //Set the seed for the random variable generator just in case we need it.
    //srandom(time(NULL));

    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);//Use double buffering for smoother images
    glutInitWindowSize(m_width+HUD_WIDTH, m_height);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Kitten Pirateer - Adventure of Zombie Island");

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
	
	//Alpha layer code:
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
    //gluOrtho2D(0, m_width+m_margine, 0, m_height+m_margine);
	
    glOrtho(0, m_width+HUD_WIDTH, 0, m_height, 0, 1000);

	//Set up the callbacks that will be taken care of in step 1:
    glutKeyboardFunc(Game::key);//Keyboard input
    glutDisplayFunc(Game::run);//Display frames
	glutKeyboardUpFunc(Game::keyUp);//Keyboard Movement input.
    //glutIdleFunc(Game::run);//Wait time between frames.
	
	//Load the default texture into the member variable:
	m_backgroundTexture = MapHandler::getInstance().getTile(START_TILE);
	
	//Place init here for the main GameObject (probably the PC character).
	Player::getInstance().init();
	ZombieHandler::getInstance();
	
	//Run the main glut loop for processing the game. 
    glutMainLoop(); //glutMainLoop enters the GLUT event processing loop. 
                    //This routine should be called at most once in a GLUT program. 
                    //Once called, this routine will never return. 
                    //It will call as necessary any callbacks that have been registered.

}

void Game::update()
//Function handling the update of the game.
{
	
	int now;
	int miliseconds;
	now = glutGet(GLUT_ELAPSED_TIME);
	miliseconds =  now - m_lastSong;
	
	if (miliseconds > 32600) {
		
		Jukebox::PlayBackground();
		m_lastSong = glutGet(GLUT_ELAPSED_TIME);
		
	}
	
	Game::getInstance().keyOperations();
	
	//Clear color and depth buffers:
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	//Clear the screen:
	glClearColor(1.0, 1.0, 1.0, 1.0); // Set the clear color to white
	glClear(GL_COLOR_BUFFER_BIT);     // clear the screen

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//Draw the background:
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_REPLACE);
	glBindTexture (GL_TEXTURE_2D, m_backgroundTexture);
	ImageLoader::rectangle(0,0, m_width, m_height);
	glDisable(GL_TEXTURE_2D);
	glFlush();
   
	//Update the HUD:
	HUDHandler::getInstance().displayHUD(); 

	if(!Game::c_running) {
		
	   	if(!Game::c_run) {

			//This should only call the mainMenu once at the start of the game.
			return m_menu.mainMenu();

			
		} else {
			
			//This will call the splash screen when ever the user pauses the game
			//using the space bar.
			return m_menu.splashScreen();

			
		}
	 
	}

	//Display and update the zombies:
	ZombieHandler::getInstance().display();
	ZombieHandler::getInstance().update();
	//Update the items:
	ItemHandler::getInstance().update();
	//Player display should be one of the very last, if not last.
	Player::getInstance().display();
	
}

void Game::run() 
{

    Game::getInstance().update();   
    //Flush the graphics pipele and display the objects we have specified
    //Since we are using double buffering this means we swap the buffer be just drew
    //on to the screen.
    glutSwapBuffers();
    Game::idle();
	
}

//Called by Game::idle this function sends a signal to update the screen
void Game::timer(int id)
{
	
	if(id == 513) {
		
		//Redisplay frame
		glutPostRedisplay();
		
	}
   
}

//This function handles the delay between the update signals for the screen.
void Game::idle() 
{
	
	glutTimerFunc(Game::c_interval, // in this many miliseconds
		Game::timer,      // call the funtion timer
		513               // with this parameter
	);
   
}

/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void Game::reshape(GLsizei newwidth, GLsizei newheight) 
{  

	//Set the viewport to cover the new window
    glViewport(0, 0, m_width=newwidth, m_height=newheight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, m_width, m_height, 0.0, 0.0, 100.0);
    glMatrixMode(GL_MODELVIEW); 
    glutPostRedisplay();
	
}

void Game::updateTile(GLuint x)
//Function which allows the background texture variable to be altered when required.
{
	
	m_backgroundTexture = x;
	ZombieHandler::getInstance().updateTile(x);
	
}
/****End of Main Work Functions*********************************************************/

/****Key Functions**********************************************************************/
void Game::key(unsigned char key, int x, int y)
//This function handles key input from the user for non-movement controls.
{
	
	keystates[key] = true;
	
    switch (key){
		
        case ' ' :
		
			//If the space bar is hit, the game first checks to see if the c_run
			//has changed from false yet. 
			if (Game::c_run == false) {
				
				//If c_run is still false, then we would start the game and set c_run
				//and c_running to true and allow the game to run.
				Game::c_run = !Game::c_run;
				Game::c_running = !Game::c_running;
				Jukebox::PlaySound("./sounds/Song.wav");
                m_lastSong = glutGet(GLUT_ELAPSED_TIME);
				
            } else {
				
				//If c_run has changed to true, then the game has been started and 
				//we simply deal with the c_running variable to decide upon displaying
				//the pause screen or allowing game play.
				Game::c_running = !Game::c_running;
				
            }
			break;
			
		//The following controls are not finite. They may be changed later on however 
		//this will allow us to test them ass we need them.
        case 'k':
			
			if (c_running) {
				
				//If the game is running, allow item use.
				ItemHandler::getInstance().iUse();
				//k will handle item use.
				break;
			
			} else {
				
				//Otherwise do nothing.
				break;
				
			}
			
		case 'i':
		
			if (c_running) {
				
				//If the game is running, allow item swapping.
				//i will handle item swap.
				ItemHandler::getInstance().iSwitch();
				break;
			
			} else {

				//Otherwise do nothing.
				break;
				
			}
				
		case 'j':
			if (c_running) {	

				//If the game is running allow attacks.
				//j uses the currently equipted weapon to attack or do something else.
				Player::getInstance().attack();
				break;
			
			} else {

				//Otherwise do nothing.
				break;
				
			}
			
		case 'u':
		
			if (c_running) {	

				//If the game is running allow weapon swapping.
				//u handles weapon swap.
				ItemHandler::getInstance().wSwitch();
				break;
			
			} else {

				//Otherwise do nothing.
				break;
				
			}
			
		//TODO fill in the restart command code.
		/*	
		case '':
			
			//This case is a prototype for the restart function during a menu. 
			if (!c_running) {
				
				//If the game is paused allow the user to indicate if they wish 
				//to restart the game. This will also be used for restarting from
				//a win or lose.
				//Code to confirm a restart and cause the restart either through
				//a function call or code to do so.
				break;
			
			} else {
				
				//Otherwise do nothing.
				break;
			
			}
		*/	
    }
	
    //glutPostRedisplay();
	
}

void Game::keyOperations()
//keyOpertations handles smooth movement from the control input.
{
	
	if (c_running) {
		
		//If the game is running, allow movement keys to be used. '
		if (keystates['a']) {
	   
			Player::getInstance().left();
			Player::getInstance().update();
			Player::getInstance().c_left = true;
	  
		} else {
	   
			Player::getInstance().c_left = false;
	
		}
   
		if (keystates['w']) {
	   
			Player::getInstance().up();
			Player::getInstance().update();
			Player::getInstance().c_up = true;
	  
		} else {
	   
			Player::getInstance().c_up = false;
	
		}
   
		if (keystates['d']) {
	   
			Player::getInstance().right();
			Player::getInstance().update();
			Player::getInstance().c_right = true;
	  
		} else {
	   
			Player::getInstance().c_right = false;
	
		}
   
		if (keystates['s']) {
	   
			Player::getInstance().down();
			Player::getInstance().update();
			Player::getInstance().c_down = true;
	  
		} else {
	   
			Player::getInstance().c_down = false;
	
		}

	}
	
	//Otherwise do nothing.
		
}

void Game::keyUp(unsigned char key, int x, int y)
//Sets the proper key state to be false.
{
	
   keystates[key] = false;

}
/****End of Key Functions**************************************************************/


/****Other Functions*******************************************************************/
//Possibly helpful functions.
//These functions are here to provide their possible use for the project.
//Anything not used by the end will be removed.
//TODO remove any functions in this location that are not used.

/*
//Function to return a random number if needed. May be removed if 
//not used before final presentation.
GLfloat Game::frand()
//Random number generating function.
{
	
    return random()/(GLfloat)RAND_MAX;

}
*/
/****End of Other Functions************************************************************/


/****Main Function*********************************************************************/
int main(int argc, char **argv) 
{
	
	//Start by trying to initialize Jukebox and then init glut and getInstance to that
	//the game will start (within getInstance).
    if(!Jukebox::init())
		exit;
    glutInit(&argc, argv);
    Game::getInstance().init();
	//Once the game is ended, close the audio.
    SDL_CloseAudio();
	
}
/****End Main Function*****************************************************************/
