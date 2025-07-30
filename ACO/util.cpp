// stdlib
#include <bits/stdc++.h>

// SFML
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Config.hpp>

// Custom
#include "headers/town.hpp"
#include "headers/thickline.hpp"
#include "headers/globals.hpp"

#include "headers/util.hpp"

/// @brief Inicijalizira zajedničku matricu udaljenosti i feremonskih tragova
void init_matrix()
{
    for(uint64_t i = 0; i < towns.size(); i++)
        for(uint64_t j = i; j < towns.size(); j++)
        {
            if(i == j) 
            {
                data_table[i][j] = -1.;
                continue;
            }
            distance(i, j, distance(towns[i], towns[j]));
            pheromone_strength(i, j, init_pheromone_pwr);
        }
}

/// @brief Centrira graf oko točke (0, 0)
/// @param tsp skup točaka grafa
void center_graph(std::vector<std::pair<double, double>> &tsp)
{
    sf::Vector2f centroid = {0.f, 0.f};
    for(uint64_t i = 0; i < tsp.size(); i++)
        centroid.x += tsp[i].first, centroid.y += tsp[i].second;
    centroid *= 1.f/tsp.size();
    for(uint64_t i = 0; i < tsp.size(); i++)
        tsp[i].first -= centroid.x, tsp[i].second -= centroid.y;
}

/// @brief Postavlja inicijalno pogađanje optimalnog puta
void initial_guess()
{
    for(uint64_t i = 0; i < towns.size(); i++)
    {
        best_path.push_back(i);
        if(i)
            best_path_len += distance(best_path[i - 1], best_path[i]);
    }
    best_path_len += distance(best_path[0], best_path[towns.size() - 1]);
}

/// @brief Rasprši ili približi točke grafa
/// @param scale faktor raspršenja/približenja
void resize_graph(float scale)
{
    for(Town& t : towns)
    {
        t.reposition(scale);
        t.sprite().setRadius(6.f * ((scale < 1) ? std::pow(0.75f, std::abs(scale_level)) : std::pow(1.33f, std::abs(scale_level))));
    }

    lines.clear();
    for(uint64_t i = 0; i < best_path.size() - 1; i++)
        lines.push_back(ThickLine(towns[best_path[i]].get_position(), towns[best_path[i+1]].get_position(), 3.f * (scale_level >= 0 ? scale_level + 1 : 1), sf::Color(255, 255, 255, 255)));
}

////

void split_char(std::string in, std::vector<std::string> &res)
{
    std::stringstream ss(in);
    std::string temp;

    while(std::getline(ss, temp, ' '))
        res.push_back(temp);
}


/// @brief Parser za .tsp datoteke
/// @param file_dir putanja do datoteke
/// @return vektor parova koji predstavljaju koordinate u 2D
std::vector<std::pair<double, double>> parse_tsp_data(std::string file_dir)
{
    return parse_tsp_data(file_dir, nullptr, nullptr);
}

/// @brief Parser za .tsp datoteke
/// @param file_dir putanja do datoteke
/// @param max_x pokazivač na varijablu kojom se vraća maksimalna x vrijednost
/// @param max_y pokazivač na varijablu kojom se vraća maksimalna y vrijednost
/// @return vektor parova koji predstavljaju koordinate u 2D
std::vector<std::pair<double, double>> parse_tsp_data(std::string file_dir, float *max_x, float *max_y)
{
    std::ifstream tspf(file_dir);
    std::vector<std::pair<double, double>> coords;
    std::string line;
    float maxX = -100000.f, maxY = -100000.f, minX = 100000.f, minY = 100000.f;
    while(std::getline(tspf, line))
    {
        if(!isdigit(line[0]))
            continue;
            
        std::vector<std::string> temp;
        split_char(line, temp);
        float tX = std::stof(temp[1]), tY = std::stof(temp[2]);
        coords.push_back({tX, tY});

        maxX = std::max(maxX, tX);
        maxY = std::max(maxY, tY);

        minX = std::min(minX, tX);
        minY = std::min(minY, tY);
    }

    for(uint64_t i = 0; i < coords.size(); i++)
    {
        coords[i].first -= minX;
        coords[i].second -= minY;
    }
    
    if(max_x != nullptr && max_y != nullptr)
    {
        *max_x = maxX - minX;
        *max_y = maxY - minY;
    }

    std::sort(coords.begin(), coords.end());
    coords.erase(std::unique(coords.begin(), coords.end()), coords.end());

    return coords;
}

/// @brief Računa udaljenost između dva grada
/// @param A prvi grad
/// @param B drugi grad
/// @return udaljenost
double distance(Town A, Town B)
{
    float dy = A.get_position().y - B.get_position().y,
          dx = A.get_position().x - B.get_position().x;
    return sqrt(dy*dy + dx*dx);
}

/// @brief Normalizira udaljenosti na raspon (0, 10)
/// @return faktor za koji se sve udaljenosti skaliraju dolje
double normalize_distances()
{
    double scaling = 0., avg = 0.; uint64_t ctr = 0;
    for(uint64_t i = 0; i < towns.size(); i++)
        for(uint64_t j = i + 1; j < towns.size(); j++)
        {
            scaling = std::max(scaling, distance(i, j));
            avg += distance(i, j);
            ctr++;
        }
    avg /= ctr;
    std::cout << "max: " << scaling << "\nscaling: " << pow(10., std::floor(std::log10(scaling)));
    scaling = pow(10., std::floor(std::log10(scaling)));
    for(uint64_t i = 0; i < towns.size(); i++)
        for(uint64_t j = i + 1; j < towns.size(); j++)
            distance(i, j, distance(i, j) / scaling);
    
    pheromone_intensity = avg / scaling / ctr * towns.size();
    density_thresh = 0.5 * pheromone_intensity * avg;
    std::cout << "\npstrength: " << pheromone_intensity << "\navg: " << avg << "\ndensity thresh: " << density_thresh << "\n";
    

    return scaling;
}

/// @brief Vraća udaljenost gradova i i j iz glavne matrice
double distance(uint64_t i, uint64_t j)
{
    uint64_t x = std::min(i, j), y = std::max(i, j);
    return data_table[x][y];
}

/// @brief Postavlja udaljenost gradova i i j u glavnoj matrici
void distance(uint64_t i, uint64_t j, double value)
{
    uint64_t x = std::min(i, j), y = std::max(i, j);
    data_table[x][y] = value;
}

/// @brief Vraća jačinu feremonskog traga na bridu i-j iz glavne matrice
double pheromone_strength(uint64_t i, uint64_t j)
{
    uint64_t x = std::max(i, j), y = std::min(i, j);
    return data_table[x][y];
}

/// @brief Postavlja jačinu feremonskog traga na bridu i-j u glavnoj matrici
void pheromone_strength(uint64_t i, uint64_t j, double value)
{
    uint64_t x = std::max(i, j), y = std::min(i, j);
    data_table[x][y] = value;
}

/// @brief Dodaje jačinu feremonskog traga na bridu i-j u glavnoj matrici
void add_pheromone_strength(uint64_t i, uint64_t j, double value)
{
    uint64_t x = std::max(i, j), y = std::min(i, j);
    data_table[x][y] += value;
}

/// @brief "Ispari" dio feremonskih tragova na cijelom grafu
void evaporate()
{
    for(uint64_t i = 0; i < towns.size(); i++)
        for(uint64_t j = i + 1; j < towns.size(); j++)
            pheromone_strength(i, j, (1.-evaporation_pwr) * pheromone_strength(i, j));
}

void evaporate(double pwr)
{
    for(uint64_t i = 0; i < towns.size(); i++)
        for(uint64_t j = i + 1; j < towns.size(); j++)
            pheromone_strength(i, j, (1.-pwr) * pheromone_strength(i, j));
}

/// @brief Vraća nasumično, težinski, izabran element iz index_space
template <typename T>
uint64_t weighted_rand(std::vector<uint64_t>& index_space, std::vector<T>& probabilities, double sum)
{
    double r = dist(e2);
    double c_sum = 0.;

    size_t i = 0;
    for(i = 0; i < index_space.size(); i++)
    {
        c_sum += static_cast<double>(probabilities[index_space[i]]) / sum;
        if(c_sum > r)
            break;
    }

    return index_space[i];
}

/// @brief Vraća nasumično, težinski, izabran element iz index_space
template <typename T>
uint64_t weighted_rand(std::vector<uint64_t>& index_space, std::vector<T>& probabilities)
{
    return weighted_rand(index_space, probabilities, std::reduce(probabilities.begin(), probabilities.end()));
}

/// Template instance
template uint64_t weighted_rand<double>(std::vector<uint64_t>&, std::vector<double>&, double);
template uint64_t weighted_rand<double>(std::vector<uint64_t>&, std::vector<double>&);

template uint64_t weighted_rand<uint64_t>(std::vector<uint64_t>&, std::vector<uint64_t>&, double);
template uint64_t weighted_rand<uint64_t>(std::vector<uint64_t>&, std::vector<uint64_t>&);

/// @brief Određuje bliske susjede
void calculate_neighbourhoods(double thresh)
{
    for(uint64_t i = 0; i < towns.size(); i++)
    {
        uint64_t ctr = 0;
        for(uint64_t j = 0; j < towns.size(); j++)
            if(i != j)
                if(distance(i, j) < thresh)
                    ctr++;
        close_neighbour_count.push_back(ctr);
    }
}

/// @brief Ocjenjuje put - ovdje je to samo duljina puta
double grade_path(std::vector<size_t>& path)
{
    double len = 0.;

    for(uint64_t i = 1; i < path.size(); i++)
        len += distance(path[i - 1], path[i]);
    len += distance(path[0], path[path.size() - 1]);
    return len;
}

/// @brief Širi feremonski trag po nekom putu
void boost_phero(std::vector<uint64_t>& path, double phero)
{
    for(size_t i = 0; i < path.size() - 1; i++)
        add_pheromone_strength(path[i], path[i+1], phero);
}

/// @brief Računa 2opt operaciju s čvorovima _i, _j
std::vector<uint64_t> _2opt(const std::vector<uint64_t>& path, uint64_t _i, uint64_t _j)
{
    size_t i = std::min(_i, _j);
    size_t j = std::max(_i, _j);
    
    std::vector<uint64_t> new_path;
    
    new_path.insert(new_path.end(), path.begin(), path.begin() + i);
    new_path.insert(new_path.end(), std::reverse_iterator(path.begin() + j + 1), std::reverse_iterator(path.begin() + i));
    new_path.insert(new_path.end(), path.begin() + j + 1, path.end());
    
    return new_path;
}

/// @brief Računa comb-tu 3opt operaciju s čvorovima _i, _j, _k
std::vector<uint64_t> _3opt(const std::vector<uint64_t>& path, uint64_t _i, uint64_t _j, uint64_t _k, int comb)
{  
    size_t i = std::min(_i, std::min(_j, _k));
    size_t k = std::max(_i, std::max(_j, _k));
    size_t j = _i;
    if(j == i || j == k)
    {
        j = _j;
        if(j == i || j == k)
            j = _k;
    }

    std::vector<uint64_t> new_path;

    auto A = path.begin();
    auto B = path.begin() + i + 1;
    auto C = path.begin() + j + 1;
    auto D = path.begin() + k + 1;

    switch (comb)
    {
        case 0: 
            new_path = path;
            break;
        case 1:
            new_path.insert(new_path.end(), A, B);
            new_path.insert(new_path.end(), std::reverse_iterator(C), std::reverse_iterator(B));
            new_path.insert(new_path.end(), C, D);
            new_path.insert(new_path.end(), D, path.end());
            break;
        case 2:
            new_path.insert(new_path.end(), A, B);
            new_path.insert(new_path.end(), B, C);
            new_path.insert(new_path.end(), std::reverse_iterator(D), std::reverse_iterator(C));
            new_path.insert(new_path.end(), D, path.end());
            break;
        case 3:
            new_path.insert(new_path.end(), A, B);
            new_path.insert(new_path.end(), std::reverse_iterator(C), std::reverse_iterator(B));
            new_path.insert(new_path.end(), std::reverse_iterator(D), std::reverse_iterator(C));
            new_path.insert(new_path.end(), D, path.end());
            break;
        case 4:
            new_path.insert(new_path.end(), A, B);
            new_path.insert(new_path.end(), std::reverse_iterator(D), std::reverse_iterator(B));
            new_path.insert(new_path.end(), D, path.end());
            break;
        case 5:
            new_path.insert(new_path.end(), A, B);
            new_path.insert(new_path.end(), C, D);
            new_path.insert(new_path.end(), B, C);
            new_path.insert(new_path.end(), D, path.end());
            break;
        case 6:
            new_path.insert(new_path.end(), A, B);
            new_path.insert(new_path.end(), C, D);
            new_path.insert(new_path.end(), std::reverse_iterator(C), std::reverse_iterator(B));
            new_path.insert(new_path.end(), D, path.end());
            break;
        default:
            new_path = path;
            break;
    }

    return new_path;
}

/// @brief Globalni 2-opt, djeluje na globalni put
int d2_opt()
{
    std::vector<uint64_t> exp_path = best_path;
    exp_path.pop_back();
    double best_found = best_path_len;

    uint64_t s = exp_path.size();

    for (size_t i = 0; i < s - 2; i++)
        for (size_t j = i + 1; j < s - 1; j++)
        {
            std::vector<uint64_t> potential = exp_path;
            potential = _2opt(potential, i % s, j % s);
            double potential_len = grade_path(potential);

            if(potential_len < best_found)
            {
                exp_path = potential;
                best_found = potential_len;
                std::cout << "New best path length: " << std::fixed << std::setprecision(2) << best_found * scale << " @ 2-opt\n";
            }
        }
    exp_path.push_back(exp_path[0]);
    if(exp_path != best_path)
    {
        best_path = exp_path;
        best_path_len = best_found;
        return 1;
    }
    return 0;
}

/// @brief Lokalni 2opt na danom putu
std::vector<uint64_t> d2_opt(std::vector<uint64_t>& path, double& path_len)
{
    std::vector<uint64_t> exp_path = path;
    exp_path.pop_back();
    double best_found = path_len;

    uint64_t s = exp_path.size();

    for (size_t i = 0; i < s - 1; i++)
        for (size_t j = i + 1; j < s; j++)
        {
            std::vector<uint64_t> potential = exp_path;
            potential = _2opt(potential, i % s, j % s);
            double potential_len = grade_path(potential);
            if(potential_len < best_found)
            {
                exp_path = potential;
                best_found = potential_len;
            }
        }
    path_len = best_found;
    exp_path.push_back(exp_path[0]);
    return exp_path;
}

/// @brief Globalni 3-opt, djeluje na globalni put
int d3_opt()
{
    std::vector<uint64_t> exp_path = best_path;
    exp_path.pop_back();
    double best_found = best_path_len;

    uint64_t s = exp_path.size();

    for (size_t i = 0; i < s - 2; i++)
        for (size_t j = i + 1; j < s - 1; j++)
            for (size_t k = j + 1; k < s; k++)
                for(size_t c = 0; c < 7; c++)
                {
                    std::vector<uint64_t> potential = exp_path;
                    potential = _3opt(potential, i % s, j % s, k % s, c);
                    double potential_len = grade_path(potential);

                    if(potential_len < best_found)
                    {
                        exp_path = potential;
                        best_found = potential_len;
                        std::cout << "New best path length: " << std::fixed << std::setprecision(2) << best_found * scale << " @ 3-opt\n";
                    }
                }
    exp_path.push_back(exp_path[0]);
    if(exp_path != best_path)
    {
        best_path = exp_path;
        best_path_len = best_found;
        return 1;
    }
    return 0;
}

/// @brief Lokalni 3opt na danom putu
std::vector<uint64_t> d3_opt(std::vector<uint64_t>& path, double& path_len)
{
    std::vector<uint64_t> exp_path = path;
    exp_path.pop_back();
    double best_found = path_len;

    uint64_t s = exp_path.size();

    for (size_t i = 0; i < s - 2; i++)
        for (size_t j = i + 1; j < s - 1; j++)
            for (size_t k = j + 1; k < s; k++) 
                for(size_t cid = 0; cid < 7; cid++)
                {
                    std::vector<uint64_t> potential = exp_path;
                    potential = _3opt(potential, i % s, j % s, k % s, cid);
                    double potential_len = grade_path(potential);
                    if(potential_len < best_found)
                    {
                        exp_path = potential;
                        best_found = potential_len;
                    }
                }
    path_len = best_found;
    exp_path.push_back(exp_path[0]);
    return exp_path;
}