
#include <ncurses.h>
#include "snake.h"
#include <string.h>
#include <chrono>
#include <random>
#include <string>
#include <thread>

Game::Game()
{
    error = false;
    error_msg = "";

    if (create_windows()) {
        error = true;
        error_msg = "create_windows() failed";
    }

    delete_tail = true;
    game_over = false;
    score = 0;
    snake_direction = RIGHT;
}

bool Game::create_windows()
{
    game_window = newwin(height + 2, width + 2, 0, 0);
    if (game_window == NULL)
        return true;

    score_window = newwin(4, width + 2, height + 2, 0);
    if (score_window == NULL)
        return true;

    return false;
}

bool Game::init_game()
{
    if (create_borders()) {
        error_msg = "create_borders() failed";
        return (error = true);
    }

    if (print(game_window, empty_space)) {
        error_msg = "print() failed";
        return (error = true);
    }

    if (init_snake()) {
        error_msg = "init_snake() failed";
        return (error = true);
    }

    if (create_food()) {
        error_msg = "create_food() failed";
        return (error = true);
    }

    if (write_score()) {
        error_msg = "print_score() failed";
        return (error = true);
    }

    if (update_score()) {
        error_msg = "update_score() failed";
        return (error = true);
    }

    return false;
}

bool Game::create_borders()
{
    if (wborder(
            game_window,
            border, border, border, border,
            border, border, border, border) == ERR)
        return true;

    if (wborder(
            score_window,
            border, border, ' ', border,
            border, border, border, border) == ERR)
        return true;

    return false;
}

bool Game::print(WINDOW* w, char c)
{
    for (int y = 1 ; y <= height ; y++)
        for (int x = 1 ; x <= width ; x++)
            if (mvwaddch(game_window, y, x, c) == ERR)
                return true;
    return false;
}

bool Game::init_snake()
{
    Position p = center_position();

    if (mvwaddch(game_window, p.y, 4, snake_head) == ERR)
        return true;

    if (mvwaddch(game_window, p.y, 3, snake_body) == ERR)
        return true;

    if (mvwaddch(game_window, p.y, 2, snake_body) == ERR)
        return true;

    snake_positions.push(Position{2, p.y});
    snake_positions.push(Position{3, p.y});
    snake_positions.push(Position{4, p.y});

    return false;
}

bool Game::create_food()
{
    int free_spaces = height * width - snake_positions.size();

    unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    std::uniform_int_distribution<int> uid(1, free_spaces);

    int r = uid(generator);

    for (int y = 1 ; y <= height ; y++) {
        for (int x = 1 ; x <= width ; x++) {
            if (char_at(game_window, y, x) == empty_space && r-- == 1) {
                if (mvwaddch(game_window, y, x, food) == ERR)
                    return true;
                else
                    return false;
            }
        }
    }

    return true;
}

bool Game::write_score()
{
    if (mvwaddstr(score_window, 1, 2, score_info) == ERR)
        return true;

    return false;
}

bool Game::start()
{
    if (refresh_windows()) {
        error_msg = "refresh_windows() failed";
        return (error = true);
    }

    while (true) {
        auto start_time = std::chrono::steady_clock::now();

        set_direction(getch());

        if (delete_tail) {
            if (move_tail()) {
                error_msg = "move_tail() failed";
                return (error = true);
            }
        }
        else
            delete_tail = true;

        check_next();

        if (game_over) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(2*T));
            return false;
        }

        if (move_head()) {
            error_msg = "move_head() failed";
            return (error = true);
        }

        if (!delete_tail) {
            if (create_food()) {
                error_msg = "create_food() failed";
                return (error = true);
            }

            if (update_score()) {
                error_msg = "update_score() failed";
                return (error = true);
            }
        }

        if (refresh_windows()) {
            error_msg = "refresh_windows() failed";
            return (error = true);
        }

        auto stop_time = std::chrono::steady_clock::now();

        auto elapsed_time =
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                stop_time - start_time
            ).count();

        std::this_thread::sleep_for(std::chrono::nanoseconds(T - elapsed_time));
    }

    return false;
}

bool Game::set_direction(int direction)
{
    switch (direction) {
        case KEY_LEFT:
            if (snake_direction == RIGHT)
                return true;
            snake_direction = LEFT;
            return false;

        case KEY_RIGHT:
            if (snake_direction == LEFT)
                return true;
            snake_direction = RIGHT;
            return false;

        case KEY_UP:
            if (snake_direction == DOWN)
                return true;
            snake_direction = UP;
            return false;

        case KEY_DOWN:
            if (snake_direction == UP)
                return true;
            snake_direction = DOWN;
            return false;

        default:
            return true;
    }
}

void Game::check_next()
{
    Position p = next_position();
    char c = char_at(game_window, p);

    if (c == food) {
        delete_tail = false;
        score++;
    }
    else if (c != empty_space) {
        game_over = true;
    }
}

bool Game::move_tail()
{
    Position tail = snake_positions.front();

    if (mvwaddch(game_window, tail.y, tail.x, empty_space) == ERR)
        return true;

    snake_positions.pop();

    return false;
}

bool Game::move_head()
{
    Position head = snake_positions.back();

    if (mvwaddch(game_window, head.y, head.x, snake_body) == ERR)
        return true;

    Position p = next_position();

    if (mvwaddch(game_window, p.y, p.x, snake_head) == ERR)
        return true;

    snake_positions.push(p);

    return false;
}

bool Game::update_score()
{
    std::string s = std::to_string(score);

    if (mvwaddstr(score_window, 1, strlen(score_info)+3, s.c_str()) == ERR)
        return true;

    return false;
}

Position Game::next_position()
{
    Position head = snake_positions.back();

    switch (snake_direction) {
        case LEFT:
            return Position(head.x - 1, head.y);
        case RIGHT:
            return Position(head.x + 1, head.y);
        case UP:
            return Position(head.x, head.y - 1);
        case DOWN:
            return Position(head.x, head.y + 1);
        default:
            return head;
    }
}

Position Game::center_position()
{
    return Position {(width + 1) / 2, (height + 1) / 2};
}

bool Game::refresh_windows()
{
    if (wrefresh(stdscr) == ERR)
        return true;

    if (wrefresh(game_window) == ERR)
        return true;

    if (wrefresh(score_window) == ERR)
        return true;

    return false;
}

char Game::char_at(WINDOW *w, int y, int x)
{
    return mvwinch(w, y, x) & A_CHARTEXT;
}

char Game::char_at(WINDOW *w, Position p)
{
    return mvwinch(w, p.y, p.x) & A_CHARTEXT;
}




