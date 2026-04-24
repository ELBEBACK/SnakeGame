#include "model/Model.hpp"
#include "view/TextVisual.hpp"
#include "view/GraphicVisual.hpp"
#include "controller/Controller.hpp"

#include <csignal>
#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <unistd.h>
#include <memory>
#include <sys/ioctl.h>



static void print_usage(const char* name) {
    std::cerr << "Usage: " << name
              << " [-u humans] [-s smart_bots] [-l silly_bots]\n"
              << "  -u / --human  N   human players (max 2, default 1)\n"
              << "  -s / --smart  N   smart bots    (default 1)\n"
              << "  -l / --silly  N   silly bots    (default 1)\n"
              << "  -g / --graphics   use SFML graphics\n"
              << "  -c / --cell   N   cell size in pixels for SFML (default 25)\n"
              << "  -h / --help       print manual             \n";
}


static void print_manual() {
    std::cerr << "\nC O N T R O L S : \n\n"
              << "  [ W ] or [ ↑ ] : Move UP\n"
              << "  [ A ] or [ ← ] : Move LEFT)\n"
              << "  [ S ] or [ ↓ ] : Move DOWN\n"
              << "  [ D ] or [ → ] : Move RIGHT\n\n"
              << "  [ P ]          : Pause / Unpause\n"
              << "  [ Q ]          : Quit Game\n\n";
}


int main(int argc, const char** argv) {
    int num_humans      = 1;
    int num_smart_bots  = 1;
    int num_silly_bots  = 1;
    bool use_graphics   = false;
    int cell_size       = 25;

    static const struct option long_opts[] = {
        {"human",       required_argument, nullptr, 'u'},
        {"smart",       required_argument, nullptr, 's'},
        {"silly",       required_argument, nullptr, 'l'},
        {"graphics",    no_argument,       nullptr, 'g'},
        {"cell",        required_argument, nullptr, 'c'},
        {"help",        no_argument,       nullptr, 'h'},
        {nullptr,       0,                 nullptr,  0 },
    };

    int opt;
    while ((opt = getopt_long(argc, const_cast<char**>(argv), "u:s:l:gc:h", long_opts, nullptr)) != -1) {
        switch (opt) {
            case 'u': num_humans     = std::atoi(optarg); break;
            case 's': num_smart_bots = std::atoi(optarg); break;
            case 'l': num_silly_bots = std::atoi(optarg); break;
            case 'g': use_graphics   = true;              break;
            case 'c': cell_size      = std::atoi(optarg); break;
            case 'h': print_manual();       return 0;
            default:  print_usage(argv[0]); return 1;
        }
    }

    std::unique_ptr<Model> model;
    std::unique_ptr<View>  view;

    if (use_graphics) {
        
        auto* gv = new GraphicVisual(cell_size);
        view.reset(gv);
        model = std::make_unique<Model>(800 / cell_size, 600 / cell_size, 150);
    
    } else {
        
        std::signal(SIGWINCH, handle_winch);
        struct winsize ws;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
        int w = (ws.ws_col > 0) ? ws.ws_col : 80;
        int h = (ws.ws_row > 0) ? ws.ws_row : 24;
        model = std::make_unique<Model>(w, h, 150);
        view  = std::make_unique<TextVisual>(*model);

    }


    try {
        Controller ctrl(*model, *view);
        ctrl.run(num_silly_bots, num_smart_bots, num_humans);
    } catch (const std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }


    return 0;
}
