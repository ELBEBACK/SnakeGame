#include "view.hpp"
#include "model.hpp"

#include <iostream>


namespace snakes {

View::View(const Model& model) : model_(model) {
    hideCursor();
};

View::~View() {
    showCursor();
    cls();
};


void View::renderAll() {
    cls();
    drawBorder();

    std::cout.flush();
}



void View::cls() {
    std::cout << "\033[2J\033[H";
}

void View::gotoxy(int x, int y) {
    std::cout << "\033[" << y + 1 << ";" << x + 1 << "H";
}

void View::setColor(int color) {
    std::cout << "\033[" << color << "m";
}

void View::hideCursor() {
    std::cout << "\033[?25l";
}

void View::showCursor() {
    std::cout << "\033[?25h";
}



void View::drawBorder() {
    int w = 60; 
    int h = 40; 

    for (int x = 0; x < w + 2; ++x) {
        gotoxy(x, 0);      std::cout << "\033[47;30mo\033[0m";
        gotoxy(x, h + 1);  std::cout << "\033[47;30mo\033[0m";
    }

    for (int y = 0; y < h + 2; ++y) {
        gotoxy(0, y);      std::cout << "\033[47;30mo\033[0m";
        gotoxy(w + 1, y);  std::cout << "\033[47;30mo\033[0m";
    }
}


}