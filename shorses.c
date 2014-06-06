/*
 * This program is a simple shooter game built using the ncurses library. 
 * Copyright (C) 2014 Kate Barr
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>

const int MAX_HEALTH = 100;
const char TIME_MAX = 10;
const char WIDTH = 50;
const char HEIGHT = 20;

struct gameData
{
	int display;
	int health;
	char type;
	char color;
};

int main(void)
{	
	int i = 1, ch = 0, horz,vert,charX = WIDTH / 2, score = 0, laser = 0;
	char direction[HEIGHT / 2];
	struct gameData gameBoard[WIDTH][HEIGHT];
	struct gameData storage, null, beam;

	initscr();
	
	// intro screen
	printw("\n\n\n\tShorses - A Shooter game for Curses\n\n");
	printw("\tUse arrow keys to move and space to shoot. \n\tpress q at any time to quit. \n\tpress any key to begin");
	refresh();
	getch();
	
	//initialize null
	null.display = ' ';
	null.health = 0;
	null.type = 0;	
	null.color = 1;	
	
	//initialize board & place enemies
	for (horz = 0;horz < WIDTH; horz++)
	{	
		for (vert = 0;vert < HEIGHT; vert++)
		{
			if(!((horz + vert ) % 6) && vert < HEIGHT / 2 && vert > 0)
			{
				gameBoard[horz][vert].display = ACS_PLUS;
				gameBoard[horz][vert].health = MAX_HEALTH;
				gameBoard[horz][vert].type = 1;
				gameBoard[horz][vert].color = 1;
			}
			else
			{
				gameBoard[horz][vert] = null;		
			}
		}
		
	}

	//initialize player & beam
	gameBoard[charX][HEIGHT-1].display = ACS_BTEE;
	gameBoard[charX][HEIGHT-1].health = MAX_HEALTH;
	gameBoard[charX][HEIGHT-1].type = 2;
	gameBoard[charX][HEIGHT-1].color = 2;
	
	beam.display = ACS_CKBOARD;
	beam.health = 0;
	beam.type = 4;
	beam.color = 3;
		
	//curses initializations
	halfdelay(1);
	keypad(stdscr, TRUE);
	noecho();
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_CYAN, COLOR_BLACK);
	init_pair(4, COLOR_YELLOW, COLOR_BLACK);
	
	//Main game loop
	while (ch != 'q')
	{
		//counter
		if (i < TIME_MAX)
		{
			i++;
		}
		else
		{
			i = 1;
		}
		
		//print gameboard
		for (horz = 0;horz < WIDTH; horz++)
		{	
			for (vert = 0;vert < HEIGHT; vert++)
			{
				move(vert,horz);
				addch(gameBoard[horz][vert].display | COLOR_PAIR(gameBoard[horz][vert].color));
			}
		}

		//enemy health check-up
		
		for (horz = 0;horz < WIDTH; horz++)
		{	
			for (vert = 0;vert < HEIGHT; vert++)
			{
				if (gameBoard[horz][vert].health <= 66 && gameBoard[horz][vert].type == 1)
				{
					gameBoard[horz][vert].color = 4;
				}
				if (gameBoard[horz][vert].health <= 33 && gameBoard[horz][vert].type == 1)
				{
					gameBoard[horz][vert].color = 2;
				}
				if (gameBoard[horz][vert].health <= 0 && gameBoard[horz][vert].type == 1)
				{
					gameBoard[horz][vert] = null;
					score += 50;
					laser += 100;
				}
			}
		}

		//bullet movement & collision
		for (horz = 0;horz < WIDTH; horz++)
		{	
			for (vert = 0;vert < HEIGHT; vert++)
			{
				if(gameBoard[horz][vert].type == 3)
				{
					if(vert > 0 && gameBoard[horz][vert-1].type == 0)
					{
						storage = gameBoard[horz][vert-1];
						gameBoard[horz][vert-1] = gameBoard[horz][vert];
						gameBoard[horz][vert] = storage;
					}
					else if(gameBoard[horz][vert-1].type == 1)
					{
						gameBoard[horz][vert-1].health -= gameBoard[horz][vert].health;
						gameBoard[horz][vert] = null;
					}
					else
					{
						gameBoard[horz][vert]= null;
					}
				}
			}	
		}


		//character movement
		switch(ch)
		{
			case KEY_LEFT:
				if(charX > 0)
				{
					 storage = gameBoard[charX-1][HEIGHT-1];
					 gameBoard[charX-1][HEIGHT-1] = gameBoard[charX][HEIGHT - 1];
					 gameBoard[charX][HEIGHT-1] = storage;
					 charX--;
				}
				break;
			case KEY_RIGHT:
				if(charX < WIDTH-1)
				{
					 storage = gameBoard[charX+1][HEIGHT-1];
					 gameBoard[charX+1][HEIGHT-1] = gameBoard[charX][HEIGHT - 1];
					 gameBoard[charX][HEIGHT-1] = storage;
					 charX++;
				}
				break;
			case ' ':
				gameBoard[charX][HEIGHT-2].display = ACS_BULLET;
				gameBoard[charX][HEIGHT-2].health = MAX_HEALTH / 6;
				gameBoard[charX][HEIGHT-2].type = 3;
				gameBoard[charX][HEIGHT-2].color = 3;
				break;
			case 'z':
			case 'Z':
				for (vert = 0; vert < HEIGHT-1; vert++)
				{
					if (gameBoard[charX][vert].type == 1)
					{
						gameBoard[charX][vert] = beam;
						score +=50;
					}
					else
					{
						gameBoard[charX][vert] = beam;
					}
				}
				laser -= 500;
				break;
		}
		
		//enemy movement
		if (i == TIME_MAX)
		{
			for (horz = 0;horz < WIDTH; horz++)
			{	
				for (vert = 0;vert < HEIGHT; vert++)
				{
					if ( gameBoard[horz][vert].type == 4)
					{
						gameBoard[horz][vert] = null;
					}
				}
			}
			for (vert = 0;vert < HEIGHT / 2; vert++)
			{
				
				if (gameBoard[0][vert].type == 1)
				{
					direction[vert] = 1;
				}
				else if (gameBoard[WIDTH - 1][vert].type == 1)
				{
					direction[vert] = 0;
				}
			}
			
			for (vert = 1;vert < HEIGHT / 2; vert++)
			{
				if (direction[vert] == 0)
				{
					for (horz = 0;horz < WIDTH-1; horz++)
					{
						gameBoard[horz][vert] = gameBoard[horz+1][vert];
					}
					gameBoard[WIDTH - 1][vert] = null;
				}
				else if (direction[vert] == 1)
				{
					
					for (horz = WIDTH-1;horz > 0; horz--)
					{
						gameBoard[horz][vert] = gameBoard[horz-1][vert];

					}
					gameBoard[0][vert] = null;
				}
			}
		}
		
		move(0,0);
		printw("Score: %i ", score);
		 if (laser >= 500)
		 {
			 addch(ACS_DIAMOND | COLOR_PAIR(3));
			 printw(" Press Z to use laser");
		 }
		//Refresh board
		refresh();
		
		//get input
		ch = '0';
		ch = getch();

	}
	endwin();
	
	
	return 0;
}
