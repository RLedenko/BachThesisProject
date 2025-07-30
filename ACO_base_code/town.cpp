// stdlib
#include <iostream>

#include "headers/town.hpp"

/// @brief Konstruktor klase koja sadrži sve potrebno za predstavljanje grada (tj. vrha grafa)
/// @param x_pos x koordinata
/// @param y_pos y koordinata
Town::Town(float x_pos, float y_pos)
{
    id = town_ctr++;
    town_sprite = sf::CircleShape(6.f);
    town_sprite.setOrigin(town_sprite.getRadius(), town_sprite.getRadius());
    town_sprite.setPosition(x_pos, y_pos);
}

/// @brief Vraća SFML sprite - koristi se za crtanje grada na prozoru  
sf::CircleShape Town::sprite()
{
    return town_sprite;
}

/// @brief Vraća poziciju grada 
sf::Vector2f Town::get_position()
{
    return town_sprite.getPosition();
}

/// @brief Funkcija za širenje/približavanje grada 
/// @param scale koliko proširiti/približiti
void Town::reposition(float scale)
{
    town_sprite.setOrigin(town_sprite.getRadius(), town_sprite.getRadius());
    town_sprite.setPosition(scale * town_sprite.getPosition());
}

size_t Town::town_ctr = 0;