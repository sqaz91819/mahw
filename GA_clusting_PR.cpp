#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <random>
#include <vector>
#include <chrono>
#include <cmath>
#include <ctime>
#include "kmeans.cpp"

using namespace std;

namespace GA_clusting {

    int runtimes = 1;
    int iteration = 2000;
    double popsize = 20;
    double crossover_rate = 0.5;
    double kmeans_rate = 0.9;
    double mutation_rate = 0.01;
    double bound = 0.8;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator = default_random_engine(seed);
    uniform_int_distribution<int> distribution1 = uniform_int_distribution<int>(1, 3);
    uniform_real_distribution<double> distribution2 = uniform_real_distribution<double>(0, 1);

    auto random_chromo();
    void crossover(iris::chromo &iris_set1, iris::chromo &iris_set2);
    void mutation(iris::chromo &iris_set, vector<int> M);
    auto GA_clusting_main();
    auto init_random(iris::chromo &iris_set);
    auto init_kmeans(iris::chromo &iris_set);
    
    auto init_kmeans(iris::chromo &iris_set) {
        for(auto &irs: iris_set.iris_set)
            if(distribution2(generator) > kmeans_rate)
                irs.current_type = distribution1(generator);
    }

    auto init_random(iris::chromo &iris_set) {
        for(auto &irs: iris_set.iris_set)
            irs.current_type = distribution1(generator);
    }

    void crossover(iris::chromo &iris_set1, iris::chromo &iris_set2) {
        for(int i = 0; i < iris_set1.iris_set.size(); i++) {
            if( distribution2(generator) < crossover_rate ) {
                short temp = iris_set1.iris_set[i].current_type;
                iris_set1.iris_set[i].current_type = iris_set2.iris_set[i].current_type;
                iris_set2.iris_set[i].current_type = temp;
            }
        }
    }

    void mutation(iris::chromo &iris_set, vector<int> M) {
        for(int i = 0; i < iris_set.iris_set.size(); i++)
            if(distribution2(generator) < mutation_rate and M[i] != 0) {
                short temp;
                do{
                    temp = distribution1(generator);
                }while(temp == iris_set.iris_set[i].current_type);

                iris_set.iris_set[i].current_type = temp;
            }
    }

    void cal_SSE(iris::chromo &iris_set, vector<int> M)
    {
        vector<kmeans::core> cores(3, kmeans::core());

        // summation of iris info by group
        int t1 = 0;
        for (auto irs : iris_set.iris_set)
        {
            if(M[t1] != 0 ) {
                cores[irs.current_type - 1].petal_length += irs.petal_length * M[t1];
                cores[irs.current_type - 1].petal_width  += irs.petal_width  * M[t1];
                cores[irs.current_type - 1].sepal_length += irs.sepal_length * M[t1];
                cores[irs.current_type - 1].sepal_width  += irs.sepal_width  * M[t1];
                cores[irs.current_type - 1].total        += M[t1] ;
            }
            t1++;
        }

        // find centroid position
        for (auto &c : cores)
        {
            c.petal_length = c.petal_length / c.total;
            c.petal_width  = c.petal_width  / c.total;
            c.sepal_length = c.sepal_length / c.total;
            c.sepal_width  = c.sepal_width  / c.total;
        }

        double SSE = 0;
        // cal SSE of this chromo
        t1 = 0;
        for (auto irs : iris_set.iris_set)
        {
            if(M[t1] != 0) {
                double d1 = pow(cores[irs.current_type - 1].petal_length - irs.petal_length, 2);
                double d2 = pow(cores[irs.current_type - 1].petal_width  - irs.petal_width,  2);
                double d3 = pow(cores[irs.current_type - 1].sepal_length - irs.sepal_length, 2);
                double d4 = pow(cores[irs.current_type - 1].sepal_width  - irs.sepal_width,  2);
                SSE = SSE + (d1 + d2 + d3 + d4) * M[t1];
            }

            t1++;
        }

        iris_set.SSE = SSE;
    }

    void kth_sort(vector<iris::chromo> &population, int k)
    {
        for (int i = 0; i < k; i++)
        {
            int minIndex = i;
            int minValue = population[i].SSE;
            for (int j = i + 1; j < popsize * 2; j++)
            {
                if (population[j].SSE < minValue)
                {
                    minIndex = j;
                    minValue = population[j].SSE;
                    iris::chromo temp = population[i];
                    population[i] = population[minIndex];
                    population[minIndex] = temp;
                }
            }
        }
    }

    double output_acc(iris::chromo iris_set) {
        double total = 0;
        for (auto i : iris_set.iris_set)
        {
            //cout.precision(2);
            //cout << fixed << i.petal_length << " " << fixed << i.petal_width << " ";
            //cout << fixed << i.sepal_length << " " << fixed << i.sepal_width << " " << fixed << i.correct_type << " " << fixed << i.current_type << endl;
            if (i.current_type == i.correct_type)
                total++;
        }

        //cout << "acc : " << total / 150 << endl;
        return total / 150;
    }

    auto GA_clusting_main()
    {
        vector<kmeans::iris> iris_set1 = iris_RW::read_iris();
        vector<kmeans::iris> iris_backup(iris_set1);            // backup the original dataset
        vector<vector<double>> record;

        double total_acc = 0;
        for (int r = 0; r < runtimes; r++)
        {
            vector<int> M(150,1);    // record the number of compresstion data points
            kmeans::kmeans_main(iris_set1, 3);
            vector<iris::chromo> population;
            for (int i = 0; i < popsize; i++)
            {
                iris::chromo iris_temp; // !--need standard reference--!
                iris_temp.iris_set = iris_set1;
                //init_kmeans(iris_temp);
                init_random(iris_temp);
                population.push_back(iris_temp);
            }

            vector<double> itr_record;
            for (int z = 0; z < iteration; z++)
            {

                for (int i = 0; i < popsize; i++)
                {
                    vector<kmeans::core> cores(3, kmeans::core());

                    // summation of iris info by group
                    int t1 = 0;
                    for (auto irs : population[i].iris_set)
                    {
                        if(M[t1] != 0 ) {
                            cores[irs.current_type - 1].petal_length += irs.petal_length * M[t1];
                            cores[irs.current_type - 1].petal_width  += irs.petal_width  * M[t1];
                            cores[irs.current_type - 1].sepal_length += irs.sepal_length * M[t1];
                            cores[irs.current_type - 1].sepal_width  += irs.sepal_width  * M[t1];
                            cores[irs.current_type - 1].total        += M[t1] ;
                        }
                        t1++;
                    }

                    // find centroid position
                    for (auto &c : cores)
                    {
                        c.petal_length = c.petal_length / c.total;
                        c.petal_width  = c.petal_width  / c.total;
                        c.sepal_length = c.sepal_length / c.total;
                        c.sepal_width  = c.sepal_width  / c.total;
                    }

                    // !!! performance boom boom !!!!!!!!!!!!!!!!!! need fix***
                    for (int k = 0; k < 3 - 1; k++)
                    {
                        for (int j = k + 1; j < 3; j++)
                        {
                            if (cores[k].sepal_length > cores[j].sepal_length)
                            {
                                kmeans::core temp = cores[k];
                                cores[k] = cores[j];
                                cores[j] = temp;
                                for (auto &irs : population[i].iris_set)
                                    if (irs.current_type == k + 1)
                                        irs.current_type == j + 1;
                                    else if (irs.current_type == j + 1)
                                        irs.current_type = k + 1;
                            }
                        }
                    }

                    double SSE = 0;
                    // cal SSE of this chromo
                    t1 = 0;
                    for (auto irs : population[i].iris_set)
                    {
                        if(M[t1] != 0) {
                            double d1 = pow(cores[irs.current_type - 1].petal_length - irs.petal_length, 2);
                            double d2 = pow(cores[irs.current_type - 1].petal_width  - irs.petal_width,  2);
                            double d3 = pow(cores[irs.current_type - 1].sepal_length - irs.sepal_length, 2);
                            double d4 = pow(cores[irs.current_type - 1].sepal_width  - irs.sepal_width,  2);
                            SSE = SSE + (d1 + d2 + d3 + d4) * M[t1];
                        }

                        t1++;
                    }

                    population[i].SSE = SSE;
                    //cout << "SSE : " << population[i].SSE << endl;
                }

                vector<double> CDF(popsize, 0);
                CDF[0] = 1 / population[0].SSE;
                for (int i = 1; i < popsize; i++)
                    CDF[i] = CDF[i - 1] + (1 / population[i].SSE);

                for (int i = 0; i < popsize; i++)
                    CDF[i] = CDF[i] / CDF[popsize - 1];

                // generate offstring
                for (int f = 0; f < popsize / 2; f++)
                {
                    int pa, pb;
                    for (int i = 0; i < popsize; i++)
                    {
                        if (distribution2(generator) < CDF[i])
                        {
                            pa = i;
                            break;
                        }
                    }

                    for (int i = 0; i < popsize; i++)
                    {
                        if (distribution2(generator) < CDF[i])
                        {
                            pb = i;
                            break;
                        }
                    }

                    iris::chromo Pa = population[pa];
                    iris::chromo Pb = population[pb];

                    crossover(Pa, Pb);
                    mutation(Pa, M);
                    mutation(Pb, M);
                    cal_SSE(Pa, M);
                    cal_SSE(Pb, M);
                    population.push_back(Pa);
                    population.push_back(Pb);
                }

                // maintain next generation popsize
                // select popsize from 2 * popsize
                kth_sort(population, popsize);
                vector<iris::chromo> p_temp;
                for (int i = 0; i < popsize; i++)
                    p_temp.push_back(population[i]);

                population = p_temp;

                // compression and remove part
                int total_zero = 0;
                for(int i = 0; i < 150; i++) {
                    if(M[i] == 0)
                        total_zero++;
                }

/*
                for(auto pi : population) {
                    for(auto it : pi.iris_set)
                        cout << it.current_type;

                    cout << endl;
                }
                */

                if(total_zero < bound * 150 and z > 150) {
                    vector<kmeans::iris> central(3, kmeans::iris());
                    vector<int> temp_m(3, 0);
                    for( int i = 0; i < 150; i++) {
                        bool compression = true;
                        if(M[i] != 0) {
                            for(int j = 0; j < popsize; j++) {
                                if(population[0].iris_set[i].current_type != population[j].iris_set[i].current_type) {
                                    compression = false;
                                    break;
                                }
                            }

                            if(compression) {
                                /*
                                for(int b = 0; b < popsize; b++) {
                                    cout << population[b].iris_set[i].current_type;
                                }
                                */
                                cout << endl;
                                int index = population[0].iris_set[i].current_type - 1;
                                central[index].petal_length += iris_backup[i].petal_length * M[i];
                                central[index].petal_width  += iris_backup[i].petal_width  * M[i];
                                central[index].sepal_length += iris_backup[i].sepal_length * M[i];
                                central[index].sepal_width  += iris_backup[i].sepal_width  * M[i];
                                temp_m[index] += M[i];
                                M[i] = 0;
                            }
                        }
                    }

                    for(int i = 0; i < 3; i++) {
                        central[i].petal_length /= temp_m[i];
                        central[i].petal_width  /= temp_m[i];
                        central[i].sepal_length /= temp_m[i];
                        central[i].sepal_width  /= temp_m[i];
                        for(int j = 0; j < 150; j++) {
                            if(iris_set1[j].current_type == i + 1 and M[j] == 0) {
                                iris_set1[j].petal_length = central[i].petal_length;
                                iris_set1[j].petal_width  = central[i].petal_width;
                                iris_set1[j].sepal_length = central[i].sepal_length;
                                iris_set1[j].sepal_width  = central[i].sepal_width;
                                for(auto &pi : population) {
                                    pi.iris_set[j].petal_length = central[i].petal_length;
                                    pi.iris_set[j].petal_width  = central[i].petal_width;
                                    pi.iris_set[j].sepal_length = central[i].sepal_length;
                                    pi.iris_set[j].sepal_width  = central[i].sepal_width;
                                }
                                M[j] = temp_m[i];
                                break;
                            }
                        }
                    }
                }


                // accuracy recorder
                double best = 0;
                for (int i = 0; i < popsize; i++)
                {
                    double temp = output_acc(population[i]);
                    if (temp > best)
                        best = temp;
                }

                itr_record.push_back(best);
            }

            record.push_back(itr_record);

            double best = 0;
            for (int i = 0; i < popsize; i++)
            {
                double temp = output_acc(population[i]);
                if (temp > best)
                    best = temp;
            }

            cout << best << endl;
            total_acc += best;
        }

        cout << "average acc : " << total_acc / runtimes << endl;
        return record;
    }
}

int main() {
    vector<vector<double>> temp;
    clock_t c1 = clock();
    temp = GA_clusting::GA_clusting_main();
    cout << "Time : " << clock() - c1 << endl;
    vector<double> output(GA_clusting::iteration, 0);
    for(int i = 0; i < GA_clusting::runtimes; i++) {
        for(int j = 0; j < GA_clusting::iteration; j++) {
            output[j] += temp[i][j];
        }
    }

    ofstream plotfile;
    plotfile.open("kmeans_ga_clusting.txt");

    for(int i = 0; i < GA_clusting::iteration; i++) {
        //cout << i+1 << " " << output[i] / GA_clusting::runtimes << endl;
        plotfile << i+1 << " " << output[i] / GA_clusting::runtimes << endl;
    }

    plotfile.close();

}