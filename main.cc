#include "model.hpp"
#include "view.hpp"
#include "controller.hpp"

#include <fcntl.h>
#include <sys/ioctl.h>

int main(void) {

    snakes::Model model(40, 60);
    snakes::View view(model);

    snakes::Controller controller(model, view);

    controller.run();

    return 0;
}
