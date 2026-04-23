#include "Model.hpp"
#include "TextVisual.hpp"
#include "Controller.hpp"

#include <csignal>
#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <unistd.h>



static void print_usage(const char* name) {
    std::cerr << "Usage: " << name
              << " [-u humans] [-s smart_bots] [-l silly_bots]\n"
              << "  -u / --human  N   human players (max 2, default 1)\n"
              << "  -s / --smart  N   smart bots    (default 1)\n"
              << "  -l / --silly  N   silly bots    (default 1)\n"
              << "  -h / --help       print manual             \n";
}


static void print_manual() {
    std::cerr << "How to play: \n"
              << " P1 uses \"W\" \"A\" SD to\n"
              << "  -u / --human  N   human players (max 2, default 1)\n"
              << "  -s / --smart  N   smart bots    (default 1)\n"
              << "  -l / --silly  N   silly bots    (default 1)\n";
}


int main(int argc, char* argv[]) {
    int num_humans     = 1;
    int num_smart_bots = 1;
    int num_silly_bots = 1;

    static const struct option long_opts[] = {
        {"human", required_argument, nullptr, 'u'},
        {"smart", required_argument, nullptr, 's'},
        {"silly", required_argument, nullptr, 'l'},
        {"help",  no_argument,       nullptr, 'h'},
        {nullptr, 0,                 nullptr,  0 },
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "u:s:l:h", long_opts, nullptr)) != -1) {
        switch (opt) {
            case 'u': num_humans     = std::atoi(optarg); break;
            case 's': num_smart_bots = std::atoi(optarg); break;
            case 'l': num_silly_bots = std::atoi(optarg); break;
            case 'h': print_manual();       return 0;
            default:  print_usage(argv[0]); return 1;
        }
    }

    
    std::signal(SIGWINCH, handle_winch);

    
    Model model(80, 24, 150);

    try {
        TextVisual view(model);
        Controller ctrl(model, view);
        ctrl.run(num_silly_bots, num_smart_bots, num_humans);
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
