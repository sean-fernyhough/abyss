#include <ncurses.h>
#include "game.h"

int main(void){
	initscr();
	noecho();
	curs_set(0);

	if(has_colors()){
		start_color();
		init_pair(VISIBLE_COLOR, 7, COLOR_BLACK);
		init_pair(SEEN_COLOR, 8, COLOR_BLACK);
	}


	menuloop();
	endwin();
}
