/*  RGB Pong Clock - Andrew Holmes @pongclock
**  Inspired by, and shamelessly derived from 
**      Nick's LED Projects
**  https://123led.wordpress.com/about/
**  
**  Videos of the clock in action:
**  https://vine.co/v/hwML6OJrBPw
**  https://vine.co/v/hgKWh1KzEU0
**  https://vine.co/v/hgKz5V0jrFn
**  I run this on a Mega 2560, your milage on other chips may vary,
**  Can definately free up some memory if the bitmaps are shrunk down to size.
**  Uses an Adafruit 16x32 RGB matrix availble from here:
**  http://www.phenoptix.com/collections/leds/products/16x32-rgb-led-matrix-panel-by-adafruit
**  This microphone:
**  http://www.phenoptix.com/collections/adafruit/products/electret-microphone-amplifier-max4466-with-adjustable-gain-by-adafruit-1063
**  a DS1307 RTC chip (not sure where I got that from - was a spare)
**  and an Ethernet Shield
**  http://hobbycomponents.com/index.php/dvbd/dvbd-ardu/ardu-shields/2012-ethernet-w5100-network-shield-for-arduino-uno-mega-2560-1280-328.html
** 
*/

#include "led-matrix.h"
#include "graphics.h"

#include <signal.h>
#include <unistd.h>
#include <iomanip>
#include <thread>
#include <random>

#include <iostream>

using namespace std;
using namespace rgb_matrix;
using std::chrono::system_clock;

#define BAT1_X 2                         // Pong left bat x pos (this is where the ball collision occurs, the bat is drawn 1 behind these coords)
#define BAT2_X 28

char *time_format = "%I:%M:%S";
bool animNet = true;
/*Color paddleColor = Color(0, 0, 146);
Color netColor = Color(0, 146, 0);
Color scoreColor = Color(36, 36, 36);
Color ballColor = Color(146, 0, 0);*/
Color paddleColor = Color(0, 0, 255);
Color netColor = Color(0, 255, 0);
Color scoreColor = Color(63, 63, 63);
Color ballColor = Color(255, 0, 0);
  
RGBMatrix *matrix;
FrameCanvas *offscreen;

int random(int start, int end) {
  std::random_device rd;
  std::default_random_engine generator (rd());
  std::uniform_int_distribution<int> distribution(start,end-1);
  return distribution(generator);
}

void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, Color color)
{
  for (int16_t x_pos = x; x_pos < x + w; x_pos++)
  {
    for (int16_t y_pos = y; y_pos < y + h; y_pos++)
    {
      offscreen->SetPixel(x_pos, y_pos, color.r, color.g, color.b);
    }
  }
}

//Draw number n, with x,y as top left corner, in chosen color, scaled in x and y.
//when scale_x, scale_y = 1 then intacter is 3x5
void vectorNumber(int n, int x, int y, Color color, float scale_x, float scale_y){

	switch (n){
	case 0:
		DrawLine(offscreen, x ,y , x , y+(4*scale_y) , color);
		DrawLine(offscreen, x , y+(4*scale_y) , x+(2*scale_x) , y+(4*scale_y), color);
		DrawLine(offscreen, x+(2*scale_x) , y , x+(2*scale_x) , y+(4*scale_y) , color);
		DrawLine(offscreen, x ,y , x+(2*scale_x) , y , color);
		break; 
	case 1: 
		DrawLine(offscreen,  x+(1*scale_x), y, x+(1*scale_x),y+(4*scale_y), color);  
		DrawLine(offscreen, x , y+4*scale_y , x+2*scale_x , y+4*scale_y,color);
		DrawLine(offscreen, x,y+scale_y, x+scale_x, y,color);
		break;
	case 2:
		DrawLine(offscreen, x ,y , x+2*scale_x , y , color);
		DrawLine(offscreen, x+2*scale_x , y , x+2*scale_x , y+2*scale_y , color);
		DrawLine(offscreen, x+2*scale_x , y+2*scale_y , x , y+2*scale_y, color);
		DrawLine(offscreen, x , y+2*scale_y, x , y+4*scale_y,color);
		DrawLine(offscreen, x , y+4*scale_y , x+2*scale_x , y+4*scale_y,color);
		break; 
	case 3:
		DrawLine(offscreen, x ,y , x+2*scale_x , y , color);
		DrawLine(offscreen, x+2*scale_x , y , x+2*scale_x , y+4*scale_y , color);
		DrawLine(offscreen, x+2*scale_x , y+2*scale_y , x+scale_x , y+2*scale_y, color);
		DrawLine(offscreen, x , y+4*scale_y , x+2*scale_x , y+4*scale_y,color);
		break;
	case 4:
		DrawLine(offscreen, x+2*scale_x , y , x+2*scale_x , y+4*scale_y , color);
		DrawLine(offscreen, x+2*scale_x , y+2*scale_y , x , y+2*scale_y, color);
		DrawLine(offscreen, x ,y , x , y+2*scale_y , color);
		break;
	case 5:
		DrawLine(offscreen, x ,y , x+2*scale_x , y , color);
		DrawLine(offscreen, x , y , x , y+2*scale_y , color);
		DrawLine(offscreen, x+2*scale_x , y+2*scale_y , x , y+2*scale_y, color);
		DrawLine(offscreen, x+2*scale_x , y+2*scale_y, x+2*scale_x , y+4*scale_y,color);
		DrawLine(offscreen,  x , y+4*scale_y , x+2*scale_x , y+4*scale_y,color);
		break; 
	case 6:
		DrawLine(offscreen, x ,y , x , y+(4*scale_y) , color);
		DrawLine(offscreen, x ,y , x+2*scale_x , y , color);
		DrawLine(offscreen, x+2*scale_x , y+2*scale_y , x , y+2*scale_y, color);
		DrawLine(offscreen, x+2*scale_x , y+2*scale_y, x+2*scale_x , y+4*scale_y,color);
		DrawLine(offscreen, x+2*scale_x , y+4*scale_y , x, y+(4*scale_y) , color);
		break;
	case 7:
		DrawLine(offscreen, x ,y , x+2*scale_x , y , color);
		DrawLine(offscreen,  x+2*scale_x, y, x+scale_x,y+(4*scale_y), color);
		break;
	case 8:
		DrawLine(offscreen, x ,y , x , y+(4*scale_y) , color);
		DrawLine(offscreen, x , y+(4*scale_y) , x+(2*scale_x) , y+(4*scale_y), color);
		DrawLine(offscreen, x+(2*scale_x) , y , x+(2*scale_x) , y+(4*scale_y) , color);
		DrawLine(offscreen, x ,y , x+(2*scale_x) , y , color);
		DrawLine(offscreen, x+2*scale_x , y+2*scale_y , x , y+2*scale_y, color);
		break;
	case 9:
		DrawLine(offscreen, x ,y , x , y+(2*scale_y) , color);
		DrawLine(offscreen, x , y+(4*scale_y) , x+(2*scale_x) , y+(4*scale_y), color);
		DrawLine(offscreen, x+(2*scale_x) , y , x+(2*scale_x) , y+(4*scale_y) , color);
		DrawLine(offscreen, x ,y , x+(2*scale_x) , y , color);
		DrawLine(offscreen, x+2*scale_x , y+2*scale_y , x , y+2*scale_y, color);
		break;    
	}
}

void cls(){
	offscreen->Clear();
}

int pong_get_ball_endpoint(int tempballpos_x, float  tempballpos_y, int  tempballvel_x, float tempballvel_y) {

	//run prediction until ball hits bat
	while (tempballpos_x > BAT1_X+1 && tempballpos_x < BAT2_X  ){
		tempballpos_x = tempballpos_x + tempballvel_x;
		tempballpos_y = tempballpos_y + tempballvel_y;
		//check for collisions with top / bottom
		if (tempballpos_y <= 0 || tempballpos_y >= 15){
			tempballvel_y = tempballvel_y * -1;
		}    
	}  
	return tempballpos_y; 
}

void pong(){
	int ballpos_x = 16;
	float ballpos_y = 8;
	int ballvel_x = 0;
	float ballvel_y = 0;
	int bat1_y = 5;  //bat starting y positions
	int bat2_y = 5;  
	int bat1_target_y = 5;  //bat targets for bats to move to
	int bat2_target_y = 5;
	bool bat1_update = true;  //flags - set to update bat position
	bool bat2_update = true;
	bool bat1miss = false;
	bool bat2miss = false; //flags set on the minute or hour that trigger the bats to miss the ball, thus upping the score to match the time.
	int restart = 1;   //game restart flag - set to 1 initially to setup 1st game
	bool holdTime = false;
	
	string time;
	
	while(true) {
	    std::chrono::seconds one_sec(1);
        std::time_t tt = system_clock::to_time_t(system_clock::now() + one_sec);
        struct std::tm *ptm = std::localtime(&tt);
	    if (!holdTime)
	    {
            std::stringstream ss;
            ss << std::put_time(ptm, time_format);
            time = ss.str();
        }
        
        //int hours = ptm->tm_hour;
        int mins = ptm->tm_min;
        int seconds = ptm->tm_sec;
        
		cls();
		//draw pitch centre line
		if (animNet)
		{
		    int adjust = 0;
		    if(seconds%2==0)adjust=1;
		    for (int i = 0; i <16; i++) {
			    if ( i % 2 == 0 ) { //plot point if an even number
				    offscreen->SetPixel(16,i+adjust,netColor.r,netColor.g,netColor.b);
			    }
		    } 
		}
		else
		{
		    for (int i = 0; i <16; i++) {
			    offscreen->SetPixel(16,i,netColor.r,netColor.g,netColor.b);
		    }
		}
		
		vectorNumber(time[0]-'0',8,1,scoreColor,1,1);
		vectorNumber(time[1]-'0',12,1,scoreColor,1,1);

		vectorNumber(time[3]-'0',18,1,scoreColor,1,1);
		vectorNumber(time[4]-'0',22,1,scoreColor,1,1);

		//if restart flag is 1, setup a new game
		if (restart > 0) {
			ballpos_x = 16;
		    if (restart == 1) {
			    //set ball start pos
			    ballpos_y = random (4,12);

			    //pick random ball direction
			    if (random(0,2) > 0) {
				    ballvel_x = 1; 
			    } 
			    else {
				    ballvel_x = -1;
			    }
			    if (random(0,2) > 0) {
				    ballvel_y = 0.5; 
			    } 
			    else {
				    ballvel_y = -0.5;
			    }
			}
			//draw bats in initial positions
			bat1miss = false; 
			bat2miss = false;
			holdTime = false;
			//reset game restart flag
			restart--;
		}
		
		if (seconds == 59){
			holdTime = true;
		}
		
		if (seconds == 0){
		    //if coming up to the minute: secs = 59 and mins < 59, flag bat 2 (right side) to miss the return so we inc the minutes score
		    if (mins > 0){
			    bat1miss = true;
			}
		    // if coming up to the hour: secs = 59  and mins = 59, flag bat 1 (left side) to miss the return, so we inc the hours score.
		    else{
			    bat2miss = true;
		    }
		}

		//AI - we run 2 sets of 'AI' for each bat to work out where to go to hit the ball back 
		//very basic AI...
		// For each bat, First just tell the bat to move to the height of the ball when we get to a random location.
		//for bat1
		if (ballpos_x == random(18,32)){
			bat1_target_y = ballpos_y;
		}
		//for bat2
		if (ballpos_x == random(4,16)){
			bat2_target_y = ballpos_y;
		}
		
		//when the ball is closer to the left bat, run the ball maths to find out where the ball will land
		if (ballpos_x == 15 && ballvel_x < 0) {

			int end_ball_y = pong_get_ball_endpoint(ballpos_x, ballpos_y, ballvel_x, ballvel_y);
			
			//if the miss flag is set,  then the bat needs to miss the ball when it gets to end_ball_y
			if (bat1miss){
				bat1miss = false;
				if ( end_ball_y > 8){
					bat1_target_y = random (0,3); 
				} 
				else {
					bat1_target_y = 8 + random (0,3);              
				}      
			} 
			//if the miss flag isn't set,  set bat target to ball end point with some randomness so its not always hitting top of bat
			else {
				bat1_target_y = end_ball_y - random (1, 5);        
				//check not less than 0
				if (bat1_target_y < 0){
					bat1_target_y = 0;
				}
				if (bat1_target_y > 10){
					bat1_target_y = 10;
				} 
			}
		}

		//right bat AI
		//if positive velocity then predict for right bat - first just match ball height
		//when the ball is closer to the right bat, run the ball maths to find out where it will land
		if (ballpos_x == 17 && ballvel_x > 0) {

			int end_ball_y = pong_get_ball_endpoint(ballpos_x, ballpos_y, ballvel_x, ballvel_y);

			//if flag set to miss, move bat out way of ball
			if (bat2miss){
				bat2miss = false;
				//if ball end point above 8 then move bat down, else move it up- so either way it misses
				if (end_ball_y > 8){
					bat2_target_y = random (0,3); 
				} 
				else {
					bat2_target_y = 8 + random (0,3);
				}      
			} 
			else {
				//set bat target to ball end point with some randomness 
				bat2_target_y =  end_ball_y - random (1,5);
				//ensure target between 0 and 15
				if (bat2_target_y < 0){
					bat2_target_y = 0;
				} 
				if (bat2_target_y > 10){
					bat2_target_y = 10;
				} 
			}
		}

		//move bat 1 towards target    
		//if bat y greater than target y move down until hit 0 (dont go any further or bat will move off screen)
		if (bat1_y > bat1_target_y && bat1_y > 0 ) {
			bat1_y--;
			bat1_update = true;
		}

		//if bat y less than target y move up until hit 10 (as bat is 6)
		if (bat1_y < bat1_target_y && bat1_y < 10) {
			bat1_y++;
			bat1_update = true;
		}

		//draw bat 1
		if (bat1_update){
			fillRect(BAT1_X-1,bat1_y,2,6,paddleColor);
		}

		//move bat 2 towards target (dont go any further or bat will move off screen)
		//if bat y greater than target y move down until hit 0
		if (bat2_y > bat2_target_y && bat2_y > 0 ) {
			bat2_y--;
			bat2_update = true;
		}

		//if bat y less than target y move up until hit max of 10 (as bat is 6)
		if (bat2_y < bat2_target_y && bat2_y < 10) {
			bat2_y++;
			bat2_update = true;
		}

		//draw bat2
		if (bat2_update){
			fillRect(BAT2_X+1,bat2_y,2,6,paddleColor);
		}

		//update the ball position using the velocity
		ballpos_x =  ballpos_x + ballvel_x;
		ballpos_y =  ballpos_y + ballvel_y;

		//check ball collision with top and bottom of screen and reverse the y velocity if either is hit
		if (ballpos_y <= 0 ){
			ballvel_y = ballvel_y * -1;
			ballpos_y = 0; //make sure value goes no less that 0
		}

		if (ballpos_y >= 15){
			ballvel_y = ballvel_y * -1;
			ballpos_y = 15; //make sure value goes no more than 15
		}

		//check for ball collision with bat1. check ballx is same as batx
		//and also check if bally lies within width of bat i.e. baty to baty + 6. We can use the exp if(a < b && b < c) 
		if ((int)ballpos_x == BAT1_X+1 && (bat1_y <= (int)ballpos_y && (int)ballpos_y <= bat1_y + 5) ) { 
		//if ((int)ballpos_x <= BAT1_X+1 && !bat1miss) { 

			//random if bat flicks ball to return it - and therefor changes ball velocity
			if(!random(0,3)) { //not true = no flick - just straight rebound and no change to ball y vel
				ballvel_x = ballvel_x * -1;
			} 
			else {
				bat1_update = true;
				int flick;  //0 = up, 1 = down.

				if (bat1_y > 1 || bat1_y < 8){
					flick = random(0,2);   //pick a random dir to flick - up or down
				}

				//if bat 1 or 2 away from top only flick down
				if (bat1_y <=1 ){
					flick = 0;   //move bat down 1 or 2 pixels 
				} 
				//if bat 1 or 2 away from bottom only flick up
				if (bat1_y >=  8 ){
					flick = 1;  //move bat up 1 or 2 pixels 
				}

				switch (flick) {
					//flick up
				case 0:
					bat1_target_y = bat1_target_y + random(1,3);
					ballvel_x = ballvel_x * -1;
					if (ballvel_y < 2) {
						ballvel_y = ballvel_y + 0.2;
					}
					break;

					//flick down
				case 1:   
					bat1_target_y = bat1_target_y - random(1,3);
					ballvel_x = ballvel_x * -1;
					if (ballvel_y > 0.2) {
						ballvel_y = ballvel_y - 0.2;
					}
					break;
				}
			}
		}

		//check for ball collision with bat2. check ballx is same as batx
		//and also check if bally lies within width of bat i.e. baty to baty + 6. We can use the exp if(a < b && b < c) 
		if ((int)ballpos_x == BAT2_X && (bat2_y <= (int)ballpos_y && (int)ballpos_y <= bat2_y + 5) ) { 
		//if ((int)ballpos_x >= BAT2_X && !bat2miss) { 

			//random if bat flicks ball to return it - and therefor changes ball velocity
			if(!random(0,3)) {
				ballvel_x = ballvel_x * -1;    //not true = no flick - just straight rebound and no change to ball y vel
			} 
			else {
				bat2_update = true;
				int flick;  //0 = up, 1 = down.

				if (bat2_y > 1 || bat2_y < 8){
					flick = random(0,2);   //pick a random dir to flick - up or down
				}
				//if bat 1 or 2 away from top only flick down
				if (bat2_y <= 1 ){
					flick = 0;  //move bat up 1 or 2 pixels 
				} 
				//if bat 1 or 2 away from bottom only flick up
				if (bat2_y >=  8 ){
					flick = 1;   //move bat down 1 or 2 pixels 
				}

				switch (flick) {
					//flick up
				case 0:
					bat2_target_y = bat2_target_y + random(1,3);
					ballvel_x = ballvel_x * -1;
					if (ballvel_y < 2) {
						ballvel_y = ballvel_y + 0.2;
					}
					break;

					//flick down
				case 1:   
					bat2_target_y = bat2_target_y - random(1,3);
					ballvel_x = ballvel_x * -1;
					if (ballvel_y > 0.2) {
						ballvel_y = ballvel_y - 0.2;
					}
					break;
				}
			}
		}
		
        if (restart == 0) {
		    //plot the ball on the screen
		    int plot_x = (int)(ballpos_x + 0.5f);
		    int plot_y = (int)(ballpos_y + 0.5f);

		    offscreen->SetPixel(plot_x,plot_y,ballColor.r,ballColor.g,ballColor.b);
		}

		//check if a bat missed the ball. if it did, reset the game.
		if ((int)ballpos_x < 0 ||(int) ballpos_x > 31){
			restart = 25;
			holdTime = false;
		}
		
		usleep(40000);
        matrix->SwapOnVSync(offscreen);
	} 
}

static void InterruptHandler(int signo) {
  matrix->Clear();
  delete matrix;
  
  printf("Received CTRL-C. Exiting.\n");
  exit(0);
}

static int usage(const char *progname, RGBMatrix::Options &matrix_options, rgb_matrix::RuntimeOptions &runtime_opt) {
  fprintf(stderr, "usage: %s [options]\n", progname);
  fprintf(stderr, "Displays the time using a game of Pong.\n");
  fprintf(stderr, "Options:\n");
  rgb_matrix::PrintMatrixFlags(stderr, matrix_options, runtime_opt);
  fprintf(stderr,
          "\t-t                : Use 24-hour clock.\n"
          "\t-a                : Don't animate the net.\n"
          "\t-p <r,g,b>        : Paddle color. Default 0,0,255\n"
          "\t-n <r,g,b>        : Net color. Default 0,255,0\n"
          "\t-s <r,g,b>        : Score color. Default 63,63,63\n"
          "\t-b <r,g,b>        : Ball color. Default 255,0,0\n"
          );
  return 1;
}

static bool parseColor(Color *c, const char *str) {
  return sscanf(str, "%hhu,%hhu,%hhu", &c->r, &c->g, &c->b) == 3;
}

int main(int argc, char *argv[]) {
  RGBMatrix::Options matrix_options;
  matrix_options.rows = 16;
  matrix_options.brightness = 50;
  rgb_matrix::RuntimeOptions runtime_opt;
  if (!rgb_matrix::ParseOptionsFromFlags(&argc, &argv, &matrix_options, &runtime_opt)) {
    return usage(argv[0], matrix_options, runtime_opt);
  }

  int opt;
  while ((opt = getopt(argc, argv, "tap:n:s:b:")) != -1) {
    switch (opt) {
      case 't':
        time_format = "%H:%M:%S";
        break;
      case 'a':
        animNet = false;
        break;
      case 'p':
        if (!parseColor(&paddleColor, optarg)) {
          fprintf(stderr, "Invalid color spec: %s\n", optarg);
          return usage(argv[0], matrix_options, runtime_opt);
        }
        break;
      case 'n':
        if (!parseColor(&netColor, optarg)) {
          fprintf(stderr, "Invalid color spec: %s\n", optarg);
          return usage(argv[0], matrix_options, runtime_opt);
        }
        break;
      case 's':
        if (!parseColor(&scoreColor, optarg)) {
          fprintf(stderr, "Invalid color spec: %s\n", optarg);
          return usage(argv[0], matrix_options, runtime_opt);
        }
        break;
      case 'b':
        if (!parseColor(&ballColor, optarg)) {
          fprintf(stderr, "Invalid color spec: %s\n", optarg);
          return usage(argv[0], matrix_options, runtime_opt);
        }
        break;
      default:
        return usage(argv[0], matrix_options, runtime_opt);
    }
  }

  matrix = rgb_matrix::CreateMatrixFromOptions(matrix_options, runtime_opt);
  if (matrix == NULL)
    return 1;

  printf("Size: %dx%d. Hardware gpio mapping: %s\n", matrix->width(), matrix->height(), matrix_options.hardware_mapping);
  
  signal(SIGTERM, InterruptHandler);
  signal(SIGINT, InterruptHandler);

  printf("Press <CTRL-C> to exit and reset LEDs\n");
    
  offscreen = matrix->CreateFrameCanvas();

  pong();
  return 0;
}