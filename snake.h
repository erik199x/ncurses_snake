
#ifndef SNAKE_H
#define SNAKE_H

#include <ncurses.h>
#include <queue>

bool init_ncurses();

enum Direction
{
    UP, LEFT, DOWN, RIGHT
};

class Position
{
    public:
        int x;
        int y;
        Position() : x(1), y(1) {}
        Position(int x, int y) : x(x), y(y) {}
};

class Game
{
    public:
        std::queue<Position> snake_positions;
        Direction snake_direction;

        WINDOW* game_window;
        WINDOW* score_window;

        bool delete_tail;
        bool game_over;
        int score;

        bool error;
        const char* error_msg;

        const char* score_info = "Score:";
        const char snake_body = 'o';
        const char snake_head = '@';
        const char food = '+';
        const char border = '#';
        const char empty_space = '.';

        const int height = 8;
        const int width = 16;

        const int T = 271828183; // nanoseconds

        Game();
        bool create_windows();

        bool init_game();
        bool print(WINDOW* w, char c);
        bool create_borders();
        bool init_snake();
        bool create_food();
        bool write_score();

        bool start();
        bool move_tail();
        bool move_head();
        bool update_score();
        bool set_direction(int direction);
        void check_next();

        bool refresh_windows();
        char char_at(WINDOW *w, Position p);
        char char_at(WINDOW *w, int y, int x);

        Position center_position();
        Position next_position();
};

#endif




