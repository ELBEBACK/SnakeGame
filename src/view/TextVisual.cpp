#include "view/TextVisual.hpp"

#include <iostream>
#include <atomic>
#include <csignal>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <unistd.h>
#include <string>



namespace {
    constexpr int kReset            =  0;
    constexpr int kBlack            = 30;
    constexpr int kRed              = 31;
    constexpr int kGreen            = 32;
    constexpr int kYellow           = 33;
    constexpr int kBlue             = 34;
    constexpr int kMagenta          = 35;
    constexpr int kCyan             = 36;
    constexpr int kWhite            = 37;

    constexpr int kBrightBlack      = 90;
    constexpr int kBrightRed        = 91;
    constexpr int kBrightGreen      = 92;
    constexpr int kBrightYellow     = 93;
    constexpr int kBrightBlue       = 94;
    constexpr int kBrightMagenta    = 95;
    constexpr int kBrightCyan       = 96;
    constexpr int kBrightWhite      = 97;

    constexpr int kBGBlack          = 40;
    constexpr int kBGRed            = 41;
    constexpr int kBGGreen          = 42;
    constexpr int kBGYellow         = 43;
    constexpr int kBGBlue           = 44;
    constexpr int kBGMagenta        = 45;
    constexpr int kBGCyan           = 46;
    constexpr int kBGWhite          = 47;
}

constexpr int BORDER_COLOR          = kBrightGreen;
constexpr int BORDER_TXT_COLOR      = kBrightMagenta;
constexpr int BORDER_TXT_BG_COLOR   = kBGGreen;
constexpr int GECKO_COLOR           = kBrightMagenta;

constexpr const char* GECKO = "♥";


static std::atomic<bool> g_screen_needs_update{false};

void handle_winch(int) {
    g_screen_needs_update.store(true, std::memory_order_relaxed);
}


TextVisual::TextVisual(const Model& model) {
    border_x_ = model.getColBorder();
    border_y_ = model.getRowBorder();

    if (tcgetattr(STDIN_FILENO, &saved_attr_) == -1) {
        perror("tcgetattr");
        return;
    }
    struct termios raw = saved_attr_;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == -1) {
        perror("tcsetattr");
    }

    g_screen_needs_update.store(true);
}

TextVisual::~TextVisual() {
    buf_.clear();
    showCursor();
    resetColor();
    std::cout << buf_ << std::flush;
    buf_.clear();
    tcsetattr(STDIN_FILENO, TCSANOW, &saved_attr_);
}

void TextVisual::gotoxy(int x, int y) {
    int sx = (x < 1) ? 1 : x;
    int sy = (y < 1) ? 1 : y;
    buf_ += "\033[" + std::to_string(sy) + ";" + std::to_string(sx) + "H";
}

void TextVisual::setColor(int color) {
    buf_ += "\033[" + std::to_string(color) + "m";
}

void TextVisual::resetColor() {
    buf_ += "\033[0m";
}

void TextVisual::hideCursor() {
    buf_ += "\033[?25l";
}

void TextVisual::showCursor() {
    buf_ += "\033[?25h";
}

void TextVisual::clearScreen() {
    buf_ += "\033[2J\033[3J\033[H";
}

void TextVisual::clearField(const Model& model) {
    int x1 = model.getColBorder() + 1;
    int x2 = model.getWidth()  - model.getColBorder();
    int y1 = model.getRowBorder() + 1;
    int y2 = model.getHeight() - model.getRowBorder();

    std::string blank(static_cast<size_t>(x2 - x1 + 1), ' ');
    for (int y = y1; y <= y2; y++) {
        gotoxy(x1, y);
        buf_ += blank;
    }
}

void TextVisual::drawBorder(const Model& model) {
    int w  = model.getWidth();
    int h  = model.getHeight();
    int bx = border_x_;
    int by = border_y_;

    int field_w = w - 2 * bx;
    int field_h = h - 2 * by;

    setColor(BORDER_COLOR);

    gotoxy(bx, by);
    buf_ += "╔";
    for (int i = 0; i < field_w; i++) buf_ += "═";
    buf_ += "╗";

    for (int row = 0; row < field_h; row++) {
        gotoxy(bx,                by + 1 + row);
        buf_ += "║";
        gotoxy(bx + field_w + 1, by + 1 + row);
        buf_ += "║";
    }

    gotoxy(bx, by + field_h + 1);
    buf_ += "╚";
    for (int i = 0; i < field_w; i++) buf_ += "═";
    buf_ += "╝";

    const std::string title = " SNAKES ";
    int title_x = bx + (field_w / 2) - static_cast<int>(title.size() / 2);
    gotoxy(title_x, by);
    setColor(BORDER_COLOR);
    buf_ += "╣";
    
    setColor(BORDER_TXT_COLOR);
    setColor(BORDER_TXT_BG_COLOR);
    buf_ += title;
    resetColor();

    setColor(BORDER_COLOR);
    buf_ += "╠";

    resetColor();
}


static std::string_view bodyChar(const Segment& seg) {
    bool straight = (seg.direction_head == opposite(seg.direction_tail));

    if (straight) {
        bool horizontal = seg.direction_head == Direction::LEFT
                       || seg.direction_head == Direction::RIGHT;
        return horizontal ? "═" : "║";
    }

    auto connects = [&](Direction a, Direction b) {
        return (seg.direction_head == a && seg.direction_tail == b)
            || (seg.direction_head == b && seg.direction_tail == a);
    };

    if (connects(Direction::RIGHT, Direction::DOWN)) return "╔";
    if (connects(Direction::LEFT,  Direction::DOWN)) return "╗";
    if (connects(Direction::RIGHT, Direction::UP))   return "╚";
    if (connects(Direction::LEFT,  Direction::UP))   return "╝";

    return "o";
}

void TextVisual::drawSnake(const Snake& snake) {
    const auto& body = snake.getBody();
    if (body.empty()) return;

    setColor(snake.getColor());

    auto it = body.begin();
    std::string_view head_char;
    switch (snake.getDirection()) {
        case Direction::UP:    head_char = "▲"; break;
        case Direction::DOWN:  head_char = "▼"; break;
        case Direction::LEFT:  head_char = "◄"; break;
        case Direction::RIGHT: head_char = "►"; break;
        default:               head_char = "O"; break;
    }
    gotoxy(it->x, it->y);
    buf_ += head_char;
    ++it;

    for (; it != body.end(); ++it) {
        gotoxy(it->x, it->y);
        buf_ += bodyChar(*it);
    }

    resetColor();
}

void TextVisual::drawGecko(const Gecko& gecko) {
    setColor(kRed);
    gotoxy(gecko.getX(), gecko.getY());
    buf_ += GECKO;
    resetColor();
}

void TextVisual::drawScore(const Model& model) {
    int y = model.getHeight() - model.getRowBorder() + 1;
    int x = model.getColBorder() + 1;

    gotoxy(x, y);
    setColor(BORDER_COLOR);
    buf_ += "╣ ";
    setColor(BORDER_TXT_COLOR);
    buf_ += "Snakes: ";

    for (const auto& snake : model.getSnakes()) {
        setColor(snake.getColor());
        buf_ += "#" + std::to_string(snake.getID())
              + " len=" + std::to_string(snake.getLength()) + "  ";
    }

    setColor(BORDER_COLOR);
    buf_ += "╠";
    for (int i = 0; i < 19; i++) buf_ += "═";

    resetColor();
}

void TextVisual::render(Model& model) {
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    int cur_w = (ws.ws_col > 0) ? ws.ws_col : model.getWidth();
    int cur_h = (ws.ws_row > 0) ? ws.ws_row : model.getHeight();

    bool size_changed = (cur_w != model.getWidth() || cur_h != model.getHeight());
    if (size_changed) {
        model.setWidth(cur_w);
        model.setHeight(cur_h);
    }

    bool full_redraw = g_screen_needs_update.exchange(false) || size_changed;

    buf_.clear();
    hideCursor();

    if (full_redraw) {
        clearScreen();
        drawBorder(model);
    }

    clearField(model);

    for (const auto& gecko : model.getGeckos()) drawGecko(gecko);
    for (const auto& snake : model.getSnakes()) {
        if (snake.getState() == SnakeStatus::ALIVE) drawSnake(snake);
    }

    drawScore(model);
    std::cout << buf_ << std::flush;
}

Event TextVisual::getEvent(long tick_ms) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);

    struct timeval timeout;
    timeout.tv_sec  = 0;
    timeout.tv_usec = tick_ms * 1000;

    int ret = select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &timeout);
    if (ret <= 0) return Event();

    char buf[8] = {};
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
    if (n <= 0) return Event();

    if (n == 3 && buf[0] == '\033' && buf[1] == '[') {
        switch (buf[2]) {
            case 'A': return Event(EventType::UP_2);
            case 'B': return Event(EventType::DOWN_2);
            case 'C': return Event(EventType::RIGHT_2);
            case 'D': return Event(EventType::LEFT_2);
        }
        return Event();
    }

    if (n == 1) {
        switch (buf[0]) {
            case 'w': case 'W': return Event(EventType::UP_1);
            case 's': case 'S': return Event(EventType::DOWN_1);
            case 'a': case 'A': return Event(EventType::LEFT_1);
            case 'd': case 'D': return Event(EventType::RIGHT_1);
            case 'p': case 'P': return Event(EventType::PAUSE);
            case 'q': case 'Q':
            case '\033':        return Event(EventType::HALT);
            default:            return Event();
        }
    }

    return Event();
}