#pragma once

#include <SFML/Graphics.hpp>
#include <chrono>

#include "view/View.hpp"



class GraphicVisual : public View {
    sf::RenderWindow window_;
    sf::Font         font_;
    int              cell_;

public:
    explicit GraphicVisual(int cell_size = 25);
 
    Event getEvent(long tick_ms) override;
    void  render(Model& model)   override;
    void  showScoreboard(const Scoreboard& scoreboard) override;
private:
    sf::Color      ansiToColor(int ansi_code)  const;
    sf::Color      brighten(sf::Color c, int amount) const;
    sf::FloatRect  cellRect(int x, int y)      const;
 
    void drawBackground();
    void drawDroppedHeads(const Model& model);
    void drawBorder(const Model& model);
    void drawGeckos(const Model& model);
    void drawSnakes(const Model& model);
    void drawScore(const Model& model);
};
