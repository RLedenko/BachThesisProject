#pragma once

void init_matrix();
void center_graph(std::vector<std::pair<double, double>> &tsp);
void initial_guess();
void resize_graph(float scale);

void split_char(std::string in, std::vector<std::string> &res);
std::vector<std::pair<double, double>> parse_tsp_data(std::string file_dir);
std::vector<std::pair<double, double>> parse_tsp_data(std::string file_dir, float *max_x, float *max_y);
double distance(Town A, Town B);
double normalize_distances();
double pheromone_strength(uint64_t i, uint64_t j);
void distance(uint64_t i, uint64_t j, double value);
double distance(uint64_t i, uint64_t j);
void pheromone_strength(uint64_t i, uint64_t j, double value);
void add_pheromone_strength(uint64_t i, uint64_t j, double value);
void evaporate();
void evaporate(double pwr);
template <typename T> uint64_t weighted_rand(std::vector<uint64_t>& index_space, std::vector<T>& probabilities, double sum);
template <typename T> uint64_t weighted_rand(std::vector<uint64_t>& index_space, std::vector<T>& probabilities);
void calculate_neighbourhoods(double thresh);
double grade_path(std::vector<size_t>& path);
void boost_phero(std::vector<uint64_t>& path, double phero);
std::vector<uint64_t> _2opt(const std::vector<uint64_t>& path, uint64_t _i, uint64_t _j);
std::vector<uint64_t> _3opt(const std::vector<uint64_t>& path, uint64_t _i, uint64_t _j, uint64_t _k, int case_id);
int d2_opt();
std::vector<uint64_t> d2_opt(std::vector<uint64_t>& path, double& path_len);
int d3_opt();
int d3_opt(uint64_t region);
std::vector<uint64_t> d3_opt(std::vector<uint64_t>& path, double& path_len);
std::vector<uint64_t> d3_opt(std::vector<uint64_t>& path, double& path_len, uint64_t region);