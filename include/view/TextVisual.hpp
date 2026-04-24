#pragma once

#include <termios.h>
#include <string>

#include "view/View.hpp"



void handle_winch(int sig);


class TextVisual : public View {
    int            border_x_;
    int            border_y_;
    std::string    buf_;
    struct termios saved_attr_;

public:
    explicit TextVisual(const Model& model);
    ~TextVisual();

    Event getEvent(long tick_ms) override;
    void  render(Model& model) override;
    void showScoreboard(const Scoreboard& scoreboard) override;

private:
    void gotoxy(int x, int y);
    void setColor(int color);
    void resetColor();
    void hideCursor();
    void showCursor();
    void clearScreen();
    void clearField(const Model& model);

    void drawBorder(const Model& model);
    void drawSnake(const Snake& snake);
    void drawGecko(const Gecko& gecko);
    void drawDroppedHead(const DroppedHead& dh);
    void drawScore(const Model& model);
};
