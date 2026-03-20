#ifndef SNAKES_CONTROLLER_HEADER
#define SNAKES_CONTROLLER_HEADER


namespace snakes {

class Model;
class View;

class Controller {
    Model& model_;
    View& view_;

    bool paused_;
    bool running_;

public:
    Controller(Model& model, View& view);

    void run();

    void pause();
    void exit();

private:
    void processInput();

};

}

#endif