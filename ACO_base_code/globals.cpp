// stdlib
#include <bits/stdc++.h>

// Custom
#include "headers/town.hpp"
#include "headers/thickline.hpp"

#include "headers/globals.hpp"

size_t itter_counter = 1;
int scale_level = 0;
double **data_table;
std::vector<Town> towns;

std::random_device rd;
std::mt19937 e2(rd());
std::uniform_real_distribution<> dist(0, 1);

double best_path_len = 0.;
double scale;
std::vector<size_t> best_path;
std::vector<size_t> close_neighbour_count;
std::vector<ThickLine> lines;

double density_thresh;

double pheromone_intensity;
const double dist_pwr = 5.;
const double pheromone_pwr = 1.;
const double init_pheromone_pwr = 1.;
const double evaporation_pwr = 0.3;
const unsigned long long ant_num = 30;
const unsigned long long itter_num = 50;