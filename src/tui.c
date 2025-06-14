#include "tui.h"
#include <ncurses.h>

void tui_init(void)
{
    initscr();
    start_color();
    use_default_colors();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE); // nonblocking getch
}

void tui_destroy(void)
{
    endwin();
}