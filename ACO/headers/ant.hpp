#pragma once

/// @brief Klasa koja predstavlja mrava koji pretražuje graf
class Ant
{
private:
    size_t current, init;
    std::vector<size_t> visited;
    double path_length;
    double heuristic(size_t town);
    double pheromone(size_t town);
    double calculate_weights(size_t len, std::vector<double> &probabilities, std::vector<size_t> &unvisited, int include_pheremone);

public:
    Ant(size_t start);
    void reset();
    void update();
    void spread_phero();
    void optimize_path_2opt();
    void optimize_path_3opt();
    void optimize_path(std::function<std::vector<std::size_t>(std::vector<std::size_t>&, double&)> opt);
    double finish();
};

