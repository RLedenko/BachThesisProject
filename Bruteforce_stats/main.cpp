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

#define BESTANT

int ant_2opt = 0, ant_3opt = 0, global_2opt = 0, global_3opt = 0, reset_all = 0, reset_density = 0;

int main(int argc, char **argv)
{
    srand(time(NULL));
    std::string file_path;
    file_path = argv[1];
    ant_2opt = !strcmp(argv[2], "1");
    ant_3opt = !strcmp(argv[3], "1");
    global_2opt = !strcmp(argv[4], "1");
    global_3opt = !strcmp(argv[5], "1");
    reset_all = !strcmp(argv[6], "1");
    reset_density = !strcmp(argv[7], "1");
    std::vector<std::pair<double, double>> tsp = parse_tsp_data(file_path.c_str());
    center_graph(tsp);
    for(size_t i = 0; i < tsp.size(); i++)
        towns.push_back(Town(tsp[i].first, tsp[i].second));
    data_table = new double*[tsp.size()];
    for(size_t i = 0; i < tsp.size(); i++)
        data_table[i] = new double[tsp.size()];
    init_matrix();
    scale = normalize_distances();
    calculate_neighbourhoods(density_thresh);
    std::vector<Ant> ants;
        for(size_t i = 0; i < ant_num; i++)
            ants.push_back(Ant(i % towns.size()));
    initial_guess();


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
            if(reset_all)
            {
                for(size_t i = 0; i < towns.size(); i++)
                    for(size_t j = i + 1; j < towns.size(); j++)
                        pheromone_strength(i, j, init_pheromone_pwr);
            }
            else if(reset_density)
            {
                for(size_t i = 0; i < towns.size(); i++)
                    for(size_t j = i + 1; j < towns.size(); j++)
                        if(!(close_neighbour_count[i] + close_neighbour_count[j] < 2 * density_thresh))
                            pheromone_strength(i, j, init_pheromone_pwr);
            }
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
    while(global_2opt && d2_opt());
    while(global_3opt && d3_opt());

    std::cout << best_path_len * scale << "\n";

    for(size_t i = 0; i < tsp.size(); i++)
        delete[] data_table[i];  
    delete[] data_table;

    return 0;
}