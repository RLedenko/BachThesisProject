#pragma once

// SFML
#include <SFML/Graphics.hpp>

/// @brief Klasa koja prestavlja "grad"
class Town
{
private:
    static size_t town_ctr;
    sf::CircleShape town_sprite;

public:
    size_t id;
    Town(float x_pos, float y_pos);
    sf::CircleShape sprite();
    sf::Vector2f get_position();
    void reposition(float scale);
};