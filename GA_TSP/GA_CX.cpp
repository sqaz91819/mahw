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
        file.close();
        
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
        Trail n_a(coordination.size(), -1);
        Trail n_b(coordination.size(), -1);
        uniform_int_distribution<int> distribution = uniform_int_distribution<int>(0, coordination.size()-1); // 1~49
        int c1 = distribution(generator);

        int i = c1;
        while(n_a[i] == -1) {
            n_a[i] = a[i];
            i = b[i];
        }

        for(i = 0; i < coordination.size(); i++)
            if(n_a[i] == -1) n_a[i] = b[i];

        i = c1;

        while(n_b[i] == -1) {
            n_b[i] = b[i];
            i = n_a[i];
        }

        for(i = 0; i < coordination.size(); i++)
            if(n_b[i] == -1) n_b[i] = n_a[i];

        Pa.first = n_a;
        Pb.first = n_b;
        Pa.second = fitness(n_a);
        Pb.second = fitness(n_b);
        return pair<Solution, Solution>(Pa, Pb);
    }

    void mutation(Solution &sol) {
        uniform_real_distribution<double> distribution = uniform_real_distribution<double>(0, 1);
        uniform_int_distribution<int> distribution_i = uniform_int_distribution<int>(0, coordination.size()-1);
        if(distribution(generator) < mutation_rate) {
            int c1 = distribution_i(generator);
            int c2 = distribution_i(generator);
            swap(sol.first[c1], sol.first[c2]);
            sol.second = fitness(sol.first);
        }

    }

    /*
      Select the best chromosome from the random choose individual.
    */
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

    /*
      Sort the first popsize of elements by the fitness order, these elements
      will remain to next generation.
    */
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

    vector<double> CX_main(int iteration=2000, int popsize=50) throw(){
        double best = 99999;
        vector<double> best_record(iteration, 0);
        GA_PMX::popsize = popsize;
        Solutions population = init_population();
        for(int i = 0; i < iteration; i++) {
            for(int j = 0; j < (popsize / 2); j++){
                Solution Pa = Tournament_Selection(population, 2);
                Solution Pb = Tournament_Selection(population, 2);
                pair<Solution, Solution> temp = cross_over(Pa, Pb);
                mutation(temp.first);
                mutation(temp.second);
                population.push_back(temp.first);
                population.push_back(temp.second);
            }
            kth_sort(population, popsize);
            if(population[0].second < best)
                best = population[0].second;
            best_record[i] = best;
            population = Solutions(population.begin(), population.begin()+popsize);
            // cout << "Best fitness : " << best << endl;
        }
        return best_record;
    }

    void print_sol(Trail trail) {
        for(auto t : trail) {
            if(t < 10 and t >= 0) cout << " " << t << " ";
            else cout << t << " ";
        }
        cout << endl;
    }

    void run_output(string file_name, vector<double> record) {
        ofstream file;
        file.open(file_name);
        for(int i = 0; i < record.size(); i++)
            file << i * popsize << " " << record[i] << endl;
    }
}; // namespace GA_PMX

int main(int argc, char *argv[]){
    int runtimes = 30;
    int iteration = 2000;
    int popsize = 50;
    GA_PMX::read_coordination();
    GA_PMX::cal_dismatrix();
    vector<double> run_record(iteration, 0);
    for(int i = 0; i < runtimes; i++) {
        vector<double> record = GA_PMX::CX_main(iteration, popsize);
        for(int j = 0; j < iteration; j++)
            run_record[j] += record[j];
        cout << "runtimes " << i << endl;
    }

    for(int i = 0; i < iteration; i++) {
        run_record[i] = run_record[i] / runtimes;
        cout << run_record[i] << endl;
    }
    
    GA_PMX::run_output( "CX.txt", run_record);
    return 0;
} // main