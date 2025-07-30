// stdlib
#include <bits/stdc++.h>

// Custom
#include "headers/town.hpp"
#include "headers/thickline.hpp"
#include "headers/globals.hpp"
#include "headers/util.hpp"

#include "headers/ant.hpp"

/// @brief Funkcija koja vraća heurističku komponentu vjerojatnosti prijelaza
/// @param town grad za koji se računa prijelaz
/// @return heuristički dio vjerojatnosti prijelaza
double Ant::heuristic(size_t town)
{
    return pow(1.f/distance(current, town), dist_pwr);
}

/// @brief Funkcija koja vraća feremonsku komponentu vjerojatnosti prijelaza
/// @param town grad za koji se računa prijelaz
/// @return feremonski dio vjerojatnost prijelaza
double Ant::pheromone(size_t town)
{
    double _phero = pheromone_strength(current, town);
    return pow(_phero, pheromone_pwr);
}

/// @brief Funkcija koja normalizira vjerojatnosti - da im zbroj bude 1
/// @param len broj gradova
/// @param probabilities vektor vjerojatnosti za pojedine gradove
/// @param unvisited neposjećeni vrhovi
/// @param include_pheremone hoće li ignorirati feremonski trag - prevencija crashanja programa za situaciju kad suma ispadne 0 (program tada bira samo po udaljenosti)
/// @return suma vjerojatnosti - za normalizaciju
double Ant::calculate_weights(size_t len, std::vector<double> &probabilities, std::vector<size_t> &unvisited, int include_pheremone)
{
    double sum = 0.;
    for(size_t i = 0; i < len; i++)
    {
        if(std::find(visited.begin(), visited.end(), towns[i].id) != visited.end())
        {
            probabilities[i] = 0.;
        }
        else
        {
            probabilities[towns[i].id] = heuristic(towns[i].id) * (include_pheremone ? pheromone(towns[i].id) : 1);
            sum += probabilities[towns[i].id];
            unvisited.push_back(towns[i].id);
        }
    }
    return sum;
}

/// @brief Konstruktor mrava
/// @param start indeks početnog grada
Ant::Ant(size_t start)
{
    init = start;
    reset();
}

/// @brief Reset mrava za iduću iteraciju
void Ant::reset()
{
    init = rand() % towns.size();

    current = init;
    path_length = 0.f;
    visited.clear();
    visited.push_back(current);
}

/// @brief Izvođenje iteracije
void Ant::update()
{
    size_t len = towns.size();
    std::vector<double> probabilities(len);
    while(visited.size() != len)
    {
        std::vector<size_t> unvisited;
        double sum = calculate_weights(len, probabilities, unvisited, 1);

        if(sum < DBL_EPSILON)
            sum = calculate_weights(len, probabilities, unvisited, 0);
        
        uint64_t i = weighted_rand(unvisited, probabilities, sum);

        path_length += distance(current, i);
        visited.push_back(i);
        current = i;
    }
    path_length += distance(init, current);
    visited.push_back(init);

    // Odkomentirati za lokalni 2-opt
    //optimize_path(static_cast<std::vector<std::size_t>(*)(std::vector<std::size_t>&, double&)>(d2_opt));
}

/// @brief Funkcija za širenje feremona po zadnjem prođenom putu
void Ant::spread_phero()
{
    double pheremone_contribution = pheromone_intensity / path_length;
    for(size_t i = 0; i < visited.size() - 1; i++)
        add_pheromone_strength(visited[i], visited[i+1], pheremone_contribution);
}

/// @brief 2-opt poziv (konkretna optimizacija)
void Ant::optimize_path_2opt()
{
    std::vector<std::size_t> v_hold;
    while(v_hold != visited)
    {
        v_hold = visited;
        visited = d2_opt(visited, path_length);
    }
}

/// @brief 3-opt poziv (konkretna optimizacija)
void Ant::optimize_path_3opt()
{
    std::vector<std::size_t> v_hold;
    while(v_hold != visited)
    {
        v_hold = visited;
        visited = d3_opt(visited, path_length);
    }
}

/// @brief Generalni optimizator puta
void Ant::optimize_path(std::function<std::vector<std::size_t>(std::vector<std::size_t>&, double&)> opt)
{
    std::vector<std::size_t> v_hold;
    while(v_hold != visited)
    {
        v_hold = visited;
        visited = opt(visited, path_length);
    }
}

/// @brief Provjera je li mrav našao najbolji dosadašnji put
/// @return udaljenost prijeđenog puta
double Ant::finish()
{
    if(path_length < best_path_len)
    {
        // Odkomentirati za lokalni 3-opt
        //optimize_path(static_cast<std::vector<std::size_t>(*)(std::vector<std::size_t>&, double&)>(d3_opt));
        std::cout << "New best path length: " << std::fixed << std::setprecision(2) << path_length * scale << " @ itteration: " << itter_counter << "\n";
        best_path = visited;
        best_path_len = path_length;
    }

    return path_length;
}