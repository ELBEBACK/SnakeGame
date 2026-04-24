#include "view/GraphicVisual.hpp"
#include "model/Snake.hpp"
#include "model/Gecko.hpp"

#include <algorithm>
#include <array>
#include <string>
#include <thread>
 
namespace {
 
constexpr unsigned int kDefaultWidth  = 800;
constexpr unsigned int kDefaultHeight = 600;
 
const std::array<const char*, 4> kFontPaths = {{
    "assets/font/DejaVuSans.ttf",
}};
 
}
 
GraphicVisual::GraphicVisual(int cell_size)
    : cell_(cell_size)
{
    window_.create(
        sf::VideoMode({kDefaultWidth, kDefaultHeight}),
        "Snakes"
    );
    window_.setFramerateLimit(60);

    sf::Image icon;
    if (icon.loadFromFile("assets/icon/Snakes_64x64.png")) {
        window_.setIcon(icon);
    } else {
        throw std::runtime_error("Failed to load window icon");
    }
 
    if(!font_.openFromFile("assets/font/Notable-Regular.ttf")) 
            throw std::runtime_error("Failed to load font");
}
 
sf::Color GraphicVisual::ansiToColor(int ansi) const {
    switch (ansi) {
        case 31: return sf::Color(220,  60,  60);
        case 32: return sf::Color( 80, 200,  80);
        case 33: return sf::Color(200, 200,  80);
        case 34: return sf::Color( 80, 100, 220);
        case 35: return sf::Color(200,  80, 200);
        case 36: return sf::Color( 80, 200, 200);
        default: return sf::Color(200, 200, 200);
    }
}
 
sf::Color GraphicVisual::brighten(sf::Color c, int amount) const {
    return sf::Color(
        static_cast<uint8_t>(std::min(255, static_cast<int>(c.r) + amount)),
        static_cast<uint8_t>(std::min(255, static_cast<int>(c.g) + amount)),
        static_cast<uint8_t>(std::min(255, static_cast<int>(c.b) + amount))
    );
}
 
sf::FloatRect GraphicVisual::cellRect(int x, int y) const {
    return sf::FloatRect(
        {static_cast<float>(x * cell_),       static_cast<float>(y * cell_)},
        {static_cast<float>(cell_ - 1),        static_cast<float>(cell_ - 1)}
    );
}
 
void GraphicVisual::drawBackground() {
    window_.clear(sf::Color(15, 15, 15));
}
 
void GraphicVisual::drawBorder(const Model& model) {
    int bx = model.getColBorder();
    int by = model.getRowBorder();
    int fw = model.getWidth()  - 4 * bx;
    int fh = model.getHeight() - 4 * by;
 
    sf::RectangleShape border(
        {static_cast<float>((fw + 2) * cell_),
         static_cast<float>((fh + 2) * cell_)}
    );
    border.setPosition(
        {static_cast<float>(bx * cell_),
         static_cast<float>(by * cell_)}
    );
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineColor(sf::Color(80, 160, 80));
    border.setOutlineThickness(2.f);
    window_.draw(border);
}
 
void GraphicVisual::drawGeckos(const Model& model) {
    sf::RectangleShape shape;
    shape.setFillColor(sf::Color(220, 60, 60));
 
    for (const auto& gecko : model.getGeckos()) {
        auto r = cellRect(gecko.getX(), gecko.getY());
        shape.setSize(r.size);
        shape.setPosition(r.position);
        window_.draw(shape);
    }
}
 
void GraphicVisual::drawSnakes(const Model& model) {
    for (const auto& snake : model.getSnakes()) {
        if (snake.getState() != SnakeStatus::ALIVE) continue;
 
        sf::Color body_color = ansiToColor(snake.getColor());
        sf::Color head_color = brighten(body_color, 70);
 
        sf::RectangleShape shape;
        bool is_head = true;
 
        for (const auto& seg : snake.getBody()) {
            auto r = cellRect(seg.x, seg.y);
            shape.setSize(r.size);
            shape.setPosition(r.position);
            shape.setFillColor(is_head ? head_color : body_color);
            window_.draw(shape);
            is_head = false;
        }
    }
}
 
void GraphicVisual::drawScore(const Model& model) {
 
    float x = static_cast<float>(model.getColBorder() * cell_);
    float y = static_cast<float>((model.getHeight() - model.getRowBorder() + 1) * cell_);
 
    std::string line = "Snakes:";
    for (const auto& snake : model.getSnakes()) {
        line += "  #" + std::to_string(snake.getID())
              + " len=" + std::to_string(snake.getLength());
    }
 
    sf::Text text(font_, line, static_cast<unsigned>(cell_ - 2));
    text.setPosition({x, y});
    text.setFillColor(sf::Color(200, 80, 180));
    window_.draw(text);
}
 
Event GraphicVisual::getEvent(long tick_ms) {
    auto deadline = std::chrono::steady_clock::now()
                  + std::chrono::milliseconds(tick_ms);
 
    while (std::chrono::steady_clock::now() < deadline) {
        while (auto event = window_.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                return Event(EventType::HALT);
 
            if (event->is<sf::Event::KeyPressed>()) {
                const auto* key = event->getIf<sf::Event::KeyPressed>();
                switch (key->scancode) {
                    case sf::Keyboard::Scancode::W:     return Event(EventType::UP_1);
                    case sf::Keyboard::Scancode::S:     return Event(EventType::DOWN_1);
                    case sf::Keyboard::Scancode::A:     return Event(EventType::LEFT_1);
                    case sf::Keyboard::Scancode::D:     return Event(EventType::RIGHT_1);
                    case sf::Keyboard::Scancode::Up:    return Event(EventType::UP_2);
                    case sf::Keyboard::Scancode::Down:  return Event(EventType::DOWN_2);
                    case sf::Keyboard::Scancode::Left:  return Event(EventType::LEFT_2);
                    case sf::Keyboard::Scancode::Right: return Event(EventType::RIGHT_2);
                    case sf::Keyboard::Scancode::P:     return Event(EventType::PAUSE);
                    case sf::Keyboard::Scancode::Q:
                    case sf::Keyboard::Scancode::Escape: return Event(EventType::HALT);
                    default: break;
                }
            }
 
            if (event->is<sf::Event::Resized>()) {
                const auto* r = event->getIf<sf::Event::Resized>();
                sf::FloatRect area({0.f, 0.f},
                    {static_cast<float>(r->size.x),
                     static_cast<float>(r->size.y)});
                window_.setView(sf::View(area));
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
 
    return Event();
}
 
void GraphicVisual::render(Model& model) {
    auto size = window_.getSize();
    int cols  = static_cast<int>(size.x) / cell_;
    int rows  = static_cast<int>(size.y) / cell_;
 
    if (cols != model.getWidth() || rows != model.getHeight()) {
        model.setWidth(cols);
        model.setHeight(rows);
    }
 
    drawBackground();
    drawBorder(model);
    drawGeckos(model);
    drawSnakes(model);
    drawScore(model);
    window_.display();
}
