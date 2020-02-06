
#include "snake.h"
#include <stdlib.h>
#include <iostream>

int main()
{
    if (init_ncurses()) {
        endwin();
        std::cerr << "init_ncurses() failed" << std::flush;
        return EXIT_FAILURE;
    }

    Game game = Game();

    if (!game.error)
        game.init_game();

    if (!game.error)
        game.start();

    endwin();

    if (game.error) {
        std::cerr << game.error_msg << std::flush;
        return EXIT_FAILURE;
    }
    else {
        std::cout << "Score: " << game.score << std::endl;
        return EXIT_SUCCESS;
    }
}

bool init_ncurses()
{
    if (initscr() == NULL)
        return true;

    if (cbreak() == ERR)
        return true;

    if (noecho() == ERR)
        return true;

    if (keypad(stdscr, TRUE) == ERR)
        return true;

    if (nodelay(stdscr, TRUE) == ERR)
        return true;

    if (curs_set(0) == ERR)
        return true;

    return false;
}




