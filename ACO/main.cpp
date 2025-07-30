// stdlib
#include <bits/stdc++.h>

// SFML
#include <SFML/Graphics.hpp>
#include <SFML/Config.hpp>
#include <SFML/Main.hpp>
#include <SFML/System.hpp>

// Custom
#include "headers/thickline.hpp"
#include "headers/town.hpp"
#include "headers/globals.hpp"
#include "headers/util.hpp"
#include "headers/ant.hpp"

// Odkomentirati za MMAS
//#define BESTANT

int main(int argc, char **argv)
{
    srand(time(NULL));

    // Procesiranje putanje do .tsp datoteke
    std::string file_path;
    if(argc > 1)
        file_path = argv[1];
    else
    {
        std::cout << "Path: ";
        std::cin >> file_path;
    }

    // Učitavanje skupa točaka
    std::vector<std::pair<double, double>> tsp = parse_tsp_data(file_path.c_str());

    // Centriranje grafa
    center_graph(tsp);

    // Inicijalizacija grafa
    for(size_t i = 0; i < tsp.size(); i++)
        towns.push_back(Town(tsp[i].first, tsp[i].second));

    // Inicijalizacija glavne matrice
    data_table = new double*[tsp.size()];
    for(size_t i = 0; i < tsp.size(); i++)
        data_table[i] = new double[tsp.size()];

    // Kalkulacija vrijednosti matrice udaljenosti i feremonskih tragova
    init_matrix();

    // Normalizacija udaljenosti na raspon [0, 10)
    scale = normalize_distances();

    // Izračun "gustoće" u okolici pojedinog grada na grafu
    calculate_neighbourhoods(density_thresh);

    // Inicijalizacija mrava
    std::vector<Ant> ants;
        for(size_t i = 0; i < ant_num; i++)
            ants.push_back(Ant(i % towns.size()));

    // Inicijalizacija početnog pogađanja najboljeg puta
    initial_guess();

    // Izvođenje iteracija
    Ant* global_best = &ants[0], * local_best = &ants[0];
    int stagnation_counter = 0;
    while(itter_counter <= itter_num)
    {
        Ant* global_prev = global_best, * local_prev = local_best;

#ifndef BESTANT
        double _prevbest = best_path_len;
#endif

        for(Ant& a : ants)
            a.update();
        
        evaporate();

        double l_min = __DBL_MAX__;
        for(Ant& a : ants)
        {
            double _res = a.finish();
            if(_res <= l_min)
            {
                l_min = _res;
                local_best = &a;
            }
            if(_res == best_path_len)
                global_best = &a;
        }

#ifdef BESTANT
        if(local_best == local_prev && global_best == global_prev)
            stagnation_counter++;
        else
            stagnation_counter = 0;
#else
        if(best_path_len == _prevbest)
            stagnation_counter++;
#endif
        if(stagnation_counter == 100)
        {
#ifdef STAGNATION_RESETALL
            evaporate(0.99);
            for(size_t i = 0; i < towns.size(); i++)
                for(size_t j = i + 1; j < towns.size(); j++)
                    pheromone_strength(i, j, pheromone_strength(i, j) + init_pheromone_pwr);
#else
            for(size_t i = 0; i < towns.size(); i++)
                for(size_t j = i + 1; j < towns.size(); j++)
                    if(!(close_neighbour_count[i] + close_neighbour_count[j] < 2 * density_thresh))
                        pheromone_strength(i, j, init_pheromone_pwr);

#endif
        }

#ifdef BESTANT
        local_best->spread_phero();
        if(local_best != global_best) 
            global_best->spread_phero();
#else
        for(Ant& a : ants)
            a.spread_phero();
#endif

        for(Ant& a : ants)
            a.reset();

        itter_counter++;
    }
    // Odkomentirati za globalne optimizacije
    //while(d2_opt());
    //while(d3_opt());

    // Ispis određenog puta
    std::cout << "best len: " << best_path_len * scale << "\n... - ";
    for(auto x : best_path)
        std::cout << x << " - ";
    std::cout << "...\n";

    // Spajanje dobivenog puta
    for(size_t i = 0; i < best_path.size() - 1; i++)
        lines.push_back(ThickLine(towns[best_path[i]].get_position(), towns[best_path[i+1]].get_position(), 3.f, sf::Color(255, 255, 255, 255)));

    // Vizualizacija
    sf::ContextSettings cs(/*depth*/ 0, /*stencil*/ 0, /*antialiasing*/ 3, /*major*/ 2, /*minor*/ 0);

    sf::RenderWindow window;
    window.create(sf::VideoMode(1200, 1000), "Test", sf::Style::Default, cs);

    sf::Vector2f mPos_old = {0.f, 0.f};
    
    sf::View camera(sf::FloatRect(0, 0, 1200, 1000)), gui(sf::FloatRect(0, 0, 1200, 1000));
    camera.setCenter(mPos_old);
    gui.setCenter(mPos_old);
    
    window.setView(camera);
    
    sf::Font font;

#ifdef _WIN32
    font.loadFromFile("C:\\Windows\\Fonts\\Arial.ttf");
#elif __linux__
    std::string font_path;
    std::cout << "Path to any font: ";
    std::cin >> font_path;
    font.loadFromFile(font_path.c_str());
#endif

    sf::Text gui_text;
    std::stringstream sstream;

    sstream << "Length: " << std::fixed << std::setprecision(2) << best_path_len * scale << "\tScaling level: " << scale_level;
    gui_text.setString(sstream.str());

    gui_text.setFont(font);
    gui_text.setCharacterSize(36);

    gui_text.setPosition(8.f - static_cast<float>(window.getSize().x) * 0.5f, static_cast<float>(window.getSize().y) * 0.5f - gui_text.getGlobalBounds().height - 16.f);

    int moving = 0;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
				window.close();
            if(event.type == sf::Event::MouseButtonPressed)
            {
                if(event.mouseButton.button == sf::Mouse::Left)
                {
                    moving = 1;
                    mPos_old = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                }
            }
            else if(event.type == sf::Event::MouseButtonReleased)
            {
                moving = 0;
            }
            else if(event.type == sf::Event::MouseMoved)
            {
                if(!moving)
                    break;
                sf::Vector2f mPos = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
                sf::Vector2f deltaPos = mPos_old - mPos;
                camera.setCenter(camera.getCenter() + deltaPos);
                window.setView(camera);
                mPos_old = window.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            }
            else if(event.type == sf::Event::MouseWheelMoved)
            {
                float zoom = 1 + event.mouseWheel.delta * -0.05;
                sf::Vector2i mPos = sf::Mouse::getPosition(window);
                sf::Vector2f mPos_before = window.mapPixelToCoords(mPos, camera);
                camera.zoom(zoom);
                sf::Vector2f mPos_after = window.mapPixelToCoords(mPos, camera);
                camera.setCenter(camera.getCenter() + (mPos_before - mPos_after));
                window.setView(camera);
            }
            else if(event.type == sf::Event::Resized) 
            {
                sf::Vector2f updated_resolution = {static_cast<float>(event.size.width), static_cast<float>(event.size.height)};
                camera.setSize(updated_resolution);
                gui.setSize(updated_resolution);

                gui_text.setPosition(8.f - static_cast<float>(window.getSize().x) * 0.5f, static_cast<float>(window.getSize().y) * 0.5f - gui_text.getGlobalBounds().height - 16.f);
                window.setView(camera);
            }
            else if(event.type == sf::Event::KeyPressed)
            {
                if(event.key.code == sf::Keyboard::Up)
                {
                    resize_graph(5.f);
                    scale_level++;
                    sstream.str(std::string());
                    sstream << "Length: " << std::fixed << std::setprecision(2) << best_path_len * scale << "\tScaling level: " << scale_level;
                    gui_text.setString(sstream.str());
                }
                else if(event.key.code == sf::Keyboard::Down)
                {
                    resize_graph(0.2f);
                    scale_level--;
                    sstream.str(std::string());
                    sstream << "Length: " << std::fixed << std::setprecision(2) << best_path_len * scale << "\tScaling level: " << scale_level;
                    gui_text.setString(sstream.str());
                }
            }
        }

        window.clear();
        
        for(size_t i = 0; i < lines.size(); i++)
            window.draw(lines[i]);

        for(size_t i = 0; i < towns.size(); i++)
            window.draw(towns[i].sprite());

        window.setView(gui);
        window.draw(gui_text);
        window.setView(camera);

        window.display();
    }

    // Čišćenje memorije
    for(size_t i = 0; i < tsp.size(); i++)
        delete[] data_table[i];  
    delete[] data_table;

    return 0;
}