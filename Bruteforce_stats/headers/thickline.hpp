#pragma once

// SFML
#include <SFML/Graphics.hpp>

/// @brief Klasa koja predstavlja "cestu" koja spaja dva grada
class ThickLine : public sf::Shape
{
private :
    sf::Vector2f end_point;
    float width;
public:
    ThickLine(const sf::Vector2f& start_point = sf::Vector2f(0.f, 0.f), const sf::Vector2f& end_point = sf::Vector2f(0.f, 0.f), const float width = 1.f, sf::Color color = sf::Color(255, 255, 255, 0));

    void set_start_point(const sf::Vector2f& point);
    void set_end_point(const sf::Vector2f& point);
    void set_width(const float width);

    virtual size_t getPointCount() const;
    virtual sf::Vector2f getPoint(size_t index) const;
};