// stdlib
#include <cmath>

#include "headers/thickline.hpp"

/// @brief Konstruktor "ceste" grafa
/// @param start_point početna točka dužine
/// @param end_point krajnja točka dužine
/// @param width širina ceste
/// @param color boja ceste
ThickLine::ThickLine(const sf::Vector2f& start_point, const sf::Vector2f& end_point, const float width, sf::Color color) : end_point (end_point), width (width)
{
    setPosition(start_point);
    setFillColor(color);
    update();
}

/// @brief Postavlja početnu točku
/// @param point točka koja postaje nova početna
void ThickLine::set_start_point(const sf::Vector2f& point)
{
    setPosition(point);
    update();
}

/// @brief Postavlja krajnju točku
/// @param point točka koja postaje nova krajnja
void ThickLine::set_end_point(const sf::Vector2f& point)
{
    end_point = point;
    update();
}

/// @brief Postavlja širinu
/// @param width nova širina
void ThickLine::set_width(const float width)
{
    this->width = width;
    update();
}

/// @brief Interna funkcija SFML-a, koristi se pri crtanju
/// @return konstantni broj 4 - pravokutnik
size_t ThickLine::getPointCount() const
{
    return 4;
}

/// @brief Funkcija crtanja
/// @param index točka po redu koja se crta
/// @return poziciju točke sa indeksom index
sf::Vector2f ThickLine::getPoint(size_t index) const
{
    sf::Vector2f dir = end_point - getPosition();
    sf::Vector2f off = {-dir.y, dir.x}; 
    off *= 0.5f * width / std::sqrt(dir.x * dir.x + dir.y * dir.y);

    switch(index)
    {
        case 0:
            return off;
        case 1:
            return dir + off;
        case 2:
            return dir - off;
        case 3:
            return -off;
    }
    return sf::Vector2f(NAN, NAN);
}