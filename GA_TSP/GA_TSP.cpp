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
    typedef vector<int> Trail;
    typedef pair<Trail, double> Solution;
    typedef vector<Solution> Solutions;
    int popsize = 20;
    double mutation_rate = 0.5;
    vector<vector<double>> distance_matrix;
    vector<vector<double>> reciprocal_distance;
    vector<vector<int>> coordination;

    void cal_dismatrix();
    void read_coordination();
    vector<int> random_trail();
    void print_sol(Trail trail);


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
            cities.push_back(i);

        random_shuffle(cities.begin(), cities.end());
        return cities;
    }


    /*
      Calculate the distance of input trail
    */
    double fitness(vector<int> trail){
        double distance = 0;
        for(int i = 0; i < trail.size() - 1; i++) {
            distance += distance_matrix[trail[i]][trail[i+1]];
        }

        distance += distance_matrix[trail[trail.size()-1]][trail[0]];
        return distance;
    }



    /*
      Initialize the popsize of population, each solution contain one trail and
      its fitness value.
    */
    Solutions init_population(){
        vector<pair<Trail, double>> population;
        for(int i = 0; i < popsize; i++){
            Trail trail = random_trail();
            double fit = fitness(trail);
            population.push_back(Solution(trail, fit));
        }
        
        return population;
    }

    pair<Solution, Solution> cross_over(Solution Pa, Solution Pb){
        Trail a = Pa.first;
        Trail b = Pb.first;
        uniform_int_distribution<int> distribution = uniform_int_distribution<int>(1, coordination.size()-2); // 1~49
        int c1 = distribution(generator);
        int c2 = distribution(generator);
        if(c1 > c2)
            swap(c1, c2);

        // cross the segment which is greater equal than c1 and less equal than c2.
        for(int i = c1; i <= c2; i++)
            swap(a[i], b[i]);

        bool conflict = false;
        for(int i = 0; i < coordination.size(); i++){
            if(i < c1 or i > c2){
                int j = c1, k = c1;
                while(j <= c2){
                    if(a[i] == a[j]){  
                        // if the unchanged part conflict with cross part, set the same element as the opposite
                        // cross part, if conflict again repeat this process.
                        a[i] = b[j];
                        j = c1;
                    }
                    else j++;
                }
                while(k <= c2){
                    if(b[i] == b[k]) {
                        b[i] = a[k];
                        k = c1;
                    }
                    else k++;
                }
            }
        }
        Pa.first = a;
        Pb.first = b;
        Pa.second = fitness(a);
        Pb.second = fitness(b);
        return pair<Solution, Solution>(Pa, Pb);
    }

    void mutation(Solution &sol) {
        uniform_real_distribution<double> distribution = uniform_real_distribution<double>(0, 1);
        uniform_int_distribution<int> distribution_i = uniform_int_distribution<int>(0, coordination.size()-1);
        if(distribution(generator) < mutation_rate) {
            int c1 = distribution_i(generator);
            int c2 = distribution_i(generator);
            int temp = sol.first[c1];
            sol.first[c1] = sol.first[c2];
            sol.first[c2] = temp;
            sol.second = fitness(sol.first);
        }

    }

    Solution Tournament_Selection(Solutions population, int size=2) {
        uniform_int_distribution<int> distribution(0, population.size()-1);
        int best = distribution(generator);
        for(int i = 0; i < size; i++){
            int next = distribution(generator);
            if(population[best].second > population[next].second)
                best = next;
        }
        return population[best];
    }

    void kth_sort(Solutions &population, int k)
    {
        for (int i = 0; i < k; i++)
        {
            int minIndex = i;
            int minValue = population[i].second;
            for (int j = i + 1; j < popsize * 2; j++)
            {
                if (population[j].second < minValue)
                {
                    minIndex = j;
                    minValue = population[j].second;
                    Solution temp = population[i];
                    population[i] = population[minIndex];
                    population[minIndex] = temp;
                }
            }
        }
    }

    void PMX_main(int iteration, int popsize) throw(){
        GA_PMX::popsize = popsize;
        read_coordination();
        cal_dismatrix();
        Solutions population = init_population();
        int best = 99999;
        for(int i = 0; i < iteration; i++) {
            for(int j = 0; j < (popsize / 2); j++){
                Solution Pa = Tournament_Selection(population);
                Solution Pb = Tournament_Selection(population);
                pair<Solution, Solution> temp = cross_over(Pa, Pb);
                mutation(temp.first);
                mutation(temp.second);
                population.push_back(temp.first);
                population.push_back(temp.second);
            }
            kth_sort(population, popsize);
            if(population[0].second < best)
                best = population[0].second;
            population = Solutions(population.begin(), population.begin()+popsize);
            cout << "Best fitness : " << best << endl;
        }
    
    }

    void print_sol(Trail trail) {
        for(auto t : trail) {
            cout << t << " ";
        }
        cout << endl;
    }
}; // namespace GA_PMX

int main(int argc, char *argv[]){
    GA_PMX::PMX_main(2000, 50);
    return 0;
}