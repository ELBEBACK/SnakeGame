#ifndef SNAKES_VIEW_HEADER
#define SNAKES_VIEW_HEADER

namespace snakes {

class Model;

class View {

public: 
    View(const Model& model);
    ~View();   

    void renderAll(/*const Model& */);
    
    void getEvent();
    void processEvent();

    void drawBorder();

    void drawSnake();
    void drawRabbit();

    void drawGameover();
    void drawScore();

private:
    //override override override

    void cls();
    void gotoxy(int x, int y);
    void setColor(int color);
    void hideCursor();
    void showCursor();

    const Model& model_;

};

}


#endif