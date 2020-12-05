#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <random>
#include <vector>
#include <chrono>
#include <cmath>
#include <unordered_map>
#include <ctime>
#include "test.h"

#define SIZE 51

using namespace std;

namespace ANT
{
    double beta = 0.91;    // evaparation [best=0.12][0.9]
    double gamma = 0.001; // init pheromone
    double tao = 0.0002;    // best pheromone rate [best=0.0002]
    double alpha = 6.2;   // 5.87  5.9
    int init_ant = 20;

    double dis_matrix[SIZE][SIZE];
    double inverse_dis[SIZE][SIZE];
    double p[SIZE][SIZE] = {{gamma}};
    double pheXdis[SIZE][SIZE] = {{1}};
    vector<vector<int>> coordination;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator = default_random_engine(seed);

    double trail_dis(vector<int> trail);
    void two_opt(vector<int> &trail);
    auto opt2_swap(vector<int> trail, int i, int j);

    void read_coordination()
    {
        ifstream file;
        file.open("eil51.tsp.txt");
        string temp;
        // erase header info
        for (int i = 0; i < 6; i++)
            getline(file, temp);

        for (int i = 0; i < 51; i++)
        {
            int x, y;
            file >> x >> x >> y;
            vector<int> temp;
            temp.push_back(x);
            temp.push_back(y);
            ANT::coordination.push_back(temp);
        }
    }

    void cal_dismatrix()
    {
        for (int i = 0; i < ANT::coordination.size(); i++)
        {
            for (int j = 0; j < ANT::coordination.size(); j++)
            {
                double distance = sqrt(
                    pow(ANT::coordination[j][0] - ANT::coordination[i][0], 2) + pow(ANT::coordination[j][1] - ANT::coordination[i][1], 2));
                
                if(!distance) distance = 999999;
                ANT::dis_matrix[i][j] = distance;
                ANT::inverse_dis[i][j] = 1 / distance;
            }
        }
    }

    vector<int> random_trail()
    {
        vector<int> cities;
        for (int i = 0; i < SIZE; i++)
        {
            cities.push_back(i);
        }

        random_shuffle(cities.begin(), cities.end());
        return cities;
    }

    pair<vector<int>, double> construct_ant()
    {
        uniform_int_distribution<int> distribution = uniform_int_distribution<int>(0, 50);
        unordered_map<int, int> unused(51);
        vector<int> s;

        // construct untraveled cities
        for(int i = SIZE-1; i >= 0; i--)
            unused.insert({i,i});

        // random pick first city
        int start = distribution(generator);
        s.push_back(start);
        unused.erase(start);

        for (int i = 0; i < SIZE - 1; i++)
        {
            double total = 0;
            // total pheromones * distance inverse ---!!!!
            for (auto &u : unused)
                total += pheXdis[start][u.second];

            // CDF[0] just for cumulate operartion never access or revise it(shift one index)
            vector<double> CDF(unused.size() + 1, 0);
            int index = 1;
            for (auto it = unused.begin(); it != unused.end(); ++it, ++index)
            {
                CDF[index] = (pheXdis[start][it->second] / total) + CDF[index - 1];
            }

            uniform_real_distribution<double> distribution2 = uniform_real_distribution<double>(0, CDF[CDF.size() - 1]);
            double select = distribution2(generator);

            // select next city from CDF table
            index = 1;
            bool select_b = false;
            for (auto it = unused.begin(); it != unused.end(); ++it, index++)
            {
                if (select <= CDF[index])
                {
                    select_b = true;
                    start = it->second;
                    s.push_back(start);
                    unused.erase(start);
                    break;
                }
            }
        } // for generating 50 cities

        // best 487.87
        /*
        if(trail_dis(s) < 487.87)
            two_opt(s);
        */

        return pair<vector<int>, double>(s, trail_dis(s));
    }

    double trail_dis(vector<int> trail)
    {
        double total = 0;
        for (int i = 0; i < SIZE - 1; i++)
        {
            total += dis_matrix[trail[i]][trail[i + 1]];
        }
        total += dis_matrix[trail[SIZE - 1]][trail[0]];
        return total;
    }

    void printer(vector<pair<vector<int>, double>> trails)
    {
        for (auto s : trails)
        {
            for (auto si : s.first)
            {
                cout << si << " ";
            }
            cout << "size  : " << s.first.size() << "  ";
            cout << "trail : " << s.second;
            cout << endl;
        }
    }

    /*
    initailize pheromoes and pheXdis matix
    !--abort random generating trail--!
    */
    void init_trail()
    {
        // initialize first random generation
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
                p[i][j] = gamma;
        }
        /*
        for (int i = 0; i < init_ant; i++)
        {
            vector<int> s = random_trail();
            double distance = trail_dis(s);
            distance = 1 / distance;
            for (int j = 0; j < SIZE - 1; j++)
            {
                p[s[j]][s[j + 1]] += distance;
            }
            p[s[SIZE - 1]][s[0]] += distance;
        }
        */
        // update pheromenes * inverse distance
        for (int k = 0; k < SIZE; k++)
        {
            for (int l = 0; l < SIZE; l++)
            {
                pheXdis[k][l] = p[k][l] * pow(inverse_dis[k][l], alpha);
            }
        }
    }

    void update_pheromones(vector<pair<vector<int>, double>> Population, pair<vector<int>, double> cur_best) {
        for(auto &i : p)
            for( auto &j : i)
                if(j >= 1.0e-290) j *= (1-beta);  // if statement to avoid double underflow -> NaN

        // update all ants trail
        for( auto i : Population) {
            double Lk = 1 / i.second;
            for (int j = 0; j < i.first.size() - 1; j++)
            {
                p[i.first[j]][i.first[j+1]] += beta * Lk;
                p[i.first[j+1]][i.first[j]] = p[i.first[j]][i.first[j+1]];
            }
            p[i.first[SIZE-1]][i.first[0]] += beta * Lk;
            p[i.first[0]][i.first[SIZE-1]] = p[i.first[SIZE-1]][i.first[0]];
        }

        for (auto &i : p)
            for (auto &j : i)
                if (j >= 1.0e-290) j *= (1 - tao); // if statement to avoid double underflow -> NaN

        // update the trail of current best ant
        double temp = cur_best.second;
        for(int i = 0; i < SIZE-1; i++) {
            p[cur_best.first[i]][cur_best.first[i+1]] += tao * temp;
            p[cur_best.first[i+1]][cur_best.first[i]] = p[cur_best.first[i]][cur_best.first[i+1]];
        }

        p[cur_best.first[SIZE-1]][cur_best.first[0]] += tao * temp;
        p[cur_best.first[0]][cur_best.first[SIZE-1]] = p[cur_best.first[SIZE-1]][cur_best.first[0]];

        // update temp for pheromenes * inverse distance
        for(int i = 0; i < SIZE; i++)
            for(int j = 0; j < SIZE; j++)
                pheXdis[i][j] = p[i][j] * pow(inverse_dis[i][j], alpha);

    }

    auto opt2_swap(vector<int> trail, int i, int j)
    {
        vector<int> new_trail(SIZE, 0);
        for (int a = 0; a <= i; a++)
        {
            new_trail[a] = trail[a];
        }
        for (int a = j-1; a > i; a--)
        {
            new_trail[i + j - a] = trail[a];
        }

        for (int a = j; a < SIZE; a++)
        {
            new_trail[a] = trail[a];
        }

        return new_trail;
    }

    void two_opt(vector<int> &trail)
    {
        double change = 0;
        do
        {
            for (int i = 0; i < SIZE - 3; i++)
            {
                for (int j = i + 3; j < SIZE - 1; j++)
                {
                    change = dis_matrix[trail[i]][trail[i + 1]] + dis_matrix[trail[j - 1]][trail[j]] - dis_matrix[trail[i]][trail[j - 1]] - dis_matrix[trail[i + 1]][trail[j]];
                    if (change > 0)
                    {
                        trail = opt2_swap(trail, i, j);
                    }
                }
            }
        } while (change > 0);
    }

    pair<vector<int>, double> greedy_ant() {

    }

    auto ACO_main(int runtimes, int iteration, int popsize)
    {
        read_coordination();
        cal_dismatrix();
        double total = 0;
        vector<pair<vector<int>, double>> best_update_record;
        pair<vector<int>, double> global_best({vector<int>(0),9999});
        vector<double> iteration_average(iteration, 0);
        for (int i = 0; i < runtimes; i++)
        {
            init_trail();
            pair<vector<int>, double> cur_best({vector<int>(0),9999});
            for (int j = 0; j < iteration; j++)
            {
                vector<pair<vector<int>, double>> Population;
                for (int k = 0; k < popsize; k++)
                {
                    pair<vector<int>, double> s = construct_ant();
                    if( s.second < cur_best.second ) {
                        cur_best = s;
                        best_update_record.push_back(cur_best);
                    }
                    Population.push_back(s);
                }
                // 497.98
                /*
                if(cur_best.second < 497.98) {
                    two_opt(cur_best.first);
                    cur_best.second = trail_dis(cur_best.first);
                }
                */
                iteration_average[j] += cur_best.second;
                update_pheromones(Population, cur_best);

                // record whole runtimes best
                if(cur_best.second < global_best.second)
                    global_best = cur_best;
            }
            best_update_record.push_back({vector<int>(SIZE, -1), -1.0});
            cout << i << ".Current Best : " << cur_best.second << endl;
            total += cur_best.second;
        }
        cout << "Average : " << total / runtimes << endl;
        return tuple<pair<vector<int>, double>, vector<double>, vector<pair<vector<int>, double>>>({global_best,iteration_average,best_update_record});
    }

}; // namespace ANT

/*
parameter 1. runtimes 2. iteration 3. popsize
          4. beta(evaparation1-beta) 5. gamma(pheromone)
          6. tao(best pheromone evaparation 1-tao)
          7. alpha(pow of distance inverse)
*/
int main(int argc, char *argv[]) {
    int runtimes = 30;
    int iteration = 350;
    int popsize = 50;
    if(argc > 3) {
        runtimes = std::stoi(argv[1]);
        iteration = std::stoi(argv[2]);
        popsize = std::stoi(argv[3]);
    }
    if(argc > 7) {
        ANT::beta = std::stoi(argv[4]);
        ANT::gamma = std::stoi(argv[5]);
        ANT::tao = std::stoi(argv[6]);
        ANT::alpha = std::stoi(argv[7]);
    }
    tuple<pair<vector<int>, double>, vector<double>, vector<pair<vector<int>, double>>> pack_record;
    clock_t t = clock();
    // runtimes, iteration, ants
    pack_record = ANT::ACO_main(runtimes, iteration, popsize); 
    t = clock() - t;
    cout << float(t) / CLOCKS_PER_SEC  << "sec" << endl;

    // convergence = [0-1633][50-506][100-430]...
    vector<double> temp0 = get<1>(pack_record);
    ofstream plotfile;
    plotfile.open("ant_convergence.txt");
    plotfile << 0 << " " << 1423 << endl;
    for(int i = 0; i < temp0.size(); i++) {
        plotfile << (i + 1) * popsize << " " << temp0[i] / runtimes << endl;
    }
    plotfile.close();

    // best trail coordination a[x][y] b[x][y] c[x][y]
    plotfile.open("best_trail.txt");
    pair<vector<int>, double> temp = get<0>(pack_record);
    for(int i = 0; i < SIZE; i++){
        plotfile << ANT::coordination[temp.first[i]][0] << " " << ANT::coordination[temp.first[i]][1] << endl;
    }
    plotfile << ANT::coordination[temp.first[0]][0] << " " << ANT::coordination[temp.first[0]][1] << endl;
    plotfile.close();

    // detail for every iteration update current best + condition info
    plotfile.open("ant_detail.txt");
    vector<pair<vector<int>, double>> temp2 = get<2>(pack_record);
    plotfile << "Runtimes       : " << runtimes << endl;
    plotfile << "Iteration      : " << iteration << endl;
    plotfile << "Ants           : " << popsize << endl;
    plotfile << "Beta           : " << ANT::beta << endl;
    plotfile << "Gamma          : " << ANT::gamma << endl;
    plotfile << "Tao            : " << ANT::tao << endl;
    plotfile << "Alpha          : " << ANT::alpha << endl;
    plotfile << "Execution time : " << float(t) / CLOCKS_PER_SEC  << " secs" << endl;
    plotfile << "Average result : " << temp0[temp0.size()-1] / runtimes << endl;
    plotfile << "Best result    : " << temp.second << endl << endl;

    for(auto i : temp2) {
        for(auto j : i.first)
            plotfile << j << " ";
        plotfile << "distance : " << i.second << endl;
    }
    plotfile << endl;
    plotfile << "My best trail : " << endl;
    for(auto i : temp.first)
        plotfile << i << " ";
    plotfile << "distance : " << temp.second << endl;

    plotfile.close();
}