#include <iostream>
#include <cmath>
#include <ctime>
#include <vector>
#include <random>
#include <string>
#include <chrono>
#include <fstream>
#include <algorithm>

using namespace std;
namespace GA_PMX{
    int popsize = 20;
    vector<vector<double>> distance_matrix;
    vector<vector<double>> reciprocal_distance;
    vector<vector<int>> coordination;

    void cal_dismatrix();
    void read_coordination();
    vector<int> random_trail();

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator = default_random_engine(seed);

    /*
      Read the coordination from eil file and store the x,y to the
      coordination matrix.
    */
    void read_coordination()
    {
        ifstream file;
        file.open("eil51.tsp.txt");
        string temp;
        // erase header info
        for (int i = 0; i < 6; i++)
            getline(file, temp);

        int x, y;
        // read file until end of file(EOF)
        while (file >> x >> x >> y)
        {
            vector<int> temp;
            temp.push_back(x);
            temp.push_back(y);
            GA_PMX::coordination.push_back(temp);
        }
        
    }

    /*
      Calculate the distance between cities, and store the result by the C*C matrix format.
      C is the number of cities.
    */
    void cal_dismatrix()
    {
        for (int i = 0; i < GA_PMX::coordination.size(); i++)
        {
            vector<double> temp_row(coordination.size(), 0);
            vector<double> temp_inverse_row(coordination.size(), 0);
            for (int j = 0; j < GA_PMX::coordination.size(); j++)
            {
                double distance = sqrt(
                    pow(GA_PMX::coordination[j][0] - GA_PMX::coordination[i][0], 2) + pow(GA_PMX::coordination[j][1] - GA_PMX::coordination[i][1], 2));
                // self connection distance is infinite.
                if(!distance) distance = 999999;
                temp_row[j] = distance;
                temp_inverse_row[j] = 1 / distance;
            }
            GA_PMX::distance_matrix.push_back(temp_row);
            GA_PMX::reciprocal_distance.push_back(temp_inverse_row);
        }
    }

    /*
      Generate a random shuffle trail
    */
    vector<int> random_trail()
    {
        vector<int> cities;
        for (int i = 0; i < coordination.size(); i++)
        {
            cities.push_back(i);
        }

        random_shuffle(cities.begin(), cities.end());
        return cities;
    }

    double fitness(vector<int> trail){
        double distance = 0;
        for(int i = 0; i < trail.size() - 1; i++) {
            distance += distance_matrix[trail[i]][trail[i+1]];
        }
        distance += distance_matrix[trail[trail.size()-1]][trail[0]];
        return distance;
    }



    void PMX_main(int popsize){
        
    }
}; // namespace GA_PMX

int main(){
    GA_PMX::PMX_main();
    return 0;
}