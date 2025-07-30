#include <iostream>
#include <fstream>
#include <cmath>

void handle(const char* path, const char* title, const char* command);
inline std::string num_to_6bit_str(int n);
inline std::string num_to_command(int n);

int main(int argc, char** argv)
{
    for(int i = 62; i < 64; i++)
    {
        if((i & 0x03) == 0x03)
            continue;
        std::string path = ".\\data_best_ant\\" + num_to_6bit_str(i) + ".txt";
        std::string title = "Combination: " + std::to_string(i);
        std::string command = ".\\build\\main.exe .\\res\\point131.tsp ";
        command += num_to_command(i);
        command += " > a.txt";

        handle(path.c_str(), title.c_str(), command.c_str());
    }
    return 0;
}

void handle(const char* path, const char* title, const char* command)
{
    double avg = 0., max = 0., min = 10000.;
    std::ofstream reads(path);
    reads << title << "\n";
    reads.flush();
    std::cout << title << "; saving to " << path << "; command: " << command << "\n";
    double data[100];
    for(int i = 0; i < 100; i++)
    {
        system(command);
        std::ifstream read("a.txt");
        std::string _h;
        std::getline(read, _h);
        read.close();
        double _v = std::stod(_h);
        avg += _v;
        max = std::max(max, _v);
        min = std::min(min, _v);
        std::cout << i << " " << _v << "\n";
        reads << _v << "\n";
        data[i] = _v;
    }
    avg /= 100;
    double sigma = 0.;
    for(double d : data)
        sigma += (d - avg) * (d - avg);
    sigma /= 100;
    sigma = std::sqrt(sigma);

    std::cout << "Avg: " << avg << "\nMin: " << min << "\nMax: " << max << "\nSigma: " << sigma << "\n";
    reads << "Avg: " << avg << "\nMin: " << min << "\nMax: " << max << "\nSigma: " << sigma << "\n";
    reads.close();
}

inline std::string num_to_6bit_str(int n)
{
    return std::to_string(n & 0x20 ? 1 : 0) + std::to_string(n & 0x10 ? 1 : 0) + std::to_string(n & 0x08 ? 1 : 0) + std::to_string(n & 0x04 ? 1 : 0) + std::to_string(n & 0x02 ? 1 : 0) + std::to_string(n & 0x01 ? 1 : 0);
}

inline std::string num_to_command(int n)
{
    return std::to_string(n & 0x20 ? 1 : 0) + " " + std::to_string(n & 0x10 ? 1 : 0) + " " + std::to_string(n & 0x08 ? 1 : 0) + " " + std::to_string(n & 0x04 ? 1 : 0) + " " + std::to_string(n & 0x02 ? 1 : 0) + " " + std::to_string(n & 0x01 ? 1 : 0);
}