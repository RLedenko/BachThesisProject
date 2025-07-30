#pragma once

extern size_t itter_counter;
extern int scale_level;
extern double **data_table;
extern std::vector<Town> towns;

extern std::random_device rd;
extern std::mt19937 e2;
extern std::uniform_real_distribution<> dist;

extern double best_path_len;
extern double scale;
extern std::vector<size_t> best_path;
extern std::vector<size_t> close_neighbour_count;
// extern std::vector<std::vector<size_t>> close_neighbours;
extern std::vector<ThickLine> lines;

extern double density_thresh;

extern double pheromone_intensity;
extern const double dist_pwr;
extern const double pheromone_pwr;
extern const double init_pheromone_pwr;
extern const double evaporation_pwr;
extern const unsigned long long ant_num;
extern const unsigned long long itter_num;