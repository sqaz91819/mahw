#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <random>
#include <vector>
#include <chrono>
#include <cmath>
#include <ctime>

using namespace std;

namespace kmeans{
    struct iris {
        double sepal_length; // sepal = flower 2
        double sepal_width;
        double petal_length; // petal = flower 0.5
        double petal_width;
        short correct_type;  // just for caculate accuracy
        short current_type;
    };

    struct core {
        double sepal_length = 0; // sepal = flower 2
        double sepal_width = 0;
        double petal_length = 0; // petal = flower 0.5
        double petal_width = 0;
        short type;
        int total = 0;
        vector<iris> menmber;
    };

    enum Type {
        setosa = 1, versicolor = 2, virginica = 3
    };

    // *data set range for generate core

    // 1. set k group
    // 2. random k type group core
    // 3. caculate distance and assign to one group core
    // 4. using data in one group core update new group core
    // 5. repeat 3.-5. until end condition or convergence

    int k_group = 3;

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator = default_random_engine(seed);
    uniform_int_distribution<int> distribution1 = uniform_int_distribution<int>(1, k_group);
    uniform_real_distribution<double> distribution2 = uniform_real_distribution<double>(0, 8);
    uniform_int_distribution<int> distribution3 = uniform_int_distribution<int>(0, 149);

    auto kmeans_main(vector<iris> &iris_set, int iteration);
    void set_group(vector<iris> &iris_set, vector<iris> virtual_core);
    void re_core(vector<iris> iris_set, vector<iris> &virtual_core);
    void zero(iris &irs);
    void random_pos(core &irs);
    void clear_core(vector<core> &virtual_core);

    void clear_core(vector<core> &virtual_core) {
        for(auto & c : virtual_core)
            c.menmber.clear();
    }

    void zero(iris &irs) {
        irs.sepal_length = 0;
        irs.sepal_width = 0;
        irs.petal_length = 0;
        irs.petal_width = 0;
    }

    void random_pos(core &core) {
        core.sepal_length = distribution2(generator);
        core.sepal_width = distribution2(generator);
        core.petal_length = distribution2(generator);
        core.petal_width = distribution2(generator);
    }

    void set_group(vector<iris> &iris_set, vector<core> &virtual_core) {
        for(auto &irs : iris_set) {
            double distance = 9999;
            for(auto &core: virtual_core) {
                double d_distance = sqrt( pow(irs.sepal_length - core.sepal_length, 2) +
                                       pow(irs.sepal_width - core.sepal_width, 2) +
                                       pow(irs.petal_length - core.petal_length, 2) +
                                       pow(irs.petal_width - core.petal_width, 2) );

                if(d_distance < distance) {
                    distance = d_distance;
                    irs.current_type = core.type;
                }
            }

            virtual_core[irs.current_type-1].menmber.push_back(irs);
        }
    }

    void re_core(vector<core> &virtual_core, vector<iris> iris_set) {
        for (auto &corea : virtual_core)
        {
            if (corea.menmber.size() == 0)
            {
                corea.menmber.push_back(iris_set[distribution3(generator)]);
            }

            core temp;
            for (auto irs : corea.menmber)
            {
                temp.petal_length += irs.petal_length;
                temp.petal_width += irs.petal_width;
                temp.sepal_length += irs.sepal_length;
                temp.sepal_width += irs.sepal_width;
            }
            corea.petal_length = temp.petal_length / corea.menmber.size();
            corea.petal_width = temp.petal_width / corea.menmber.size();
            corea.sepal_length = temp.sepal_length / corea.menmber.size();
            corea.sepal_width = temp.sepal_width / corea.menmber.size();
        }

        for (int i = 0; i < k_group - 1; i++)
        {
            for (int j = i+1; j < k_group; j++)
            {
                if (virtual_core[i].sepal_length > virtual_core[j].sepal_length)
                {
                    core temp = virtual_core[i];
                    virtual_core[i] = virtual_core[j];
                    virtual_core[j] = temp;
                }
            }
        }

        for(int i = 0; i < k_group; i++)
            virtual_core[i].type = i+1;
    }

    auto kmeans_main(vector<iris> &iris_set, int iteration) {
        for(auto &irs : iris_set)
            irs.current_type = distribution1(generator);

        vector<core> virtaul_core;


        for(int i = 0; i < k_group; i++) {
            core core;
            random_pos(core);
            core.type = i + 1;
            virtaul_core.push_back(core);
        }

        for(int i = 0 ; i < iteration; i++) {
            set_group(iris_set, virtaul_core);
            re_core(virtaul_core, iris_set);
            clear_core(virtaul_core);
        }
    }

}


namespace iris_RW {

    vector<kmeans::iris> read_iris() {
        vector<kmeans::iris> iris_set;

        string filename = "iris.data";
        ifstream file;
        file.open(filename);
        char pass;

        for (int i = 0; i < 150; i++)
        {
            kmeans::iris temp;
            string str;
            file >> temp.petal_length >> pass >> temp.petal_width >> pass >> temp.sepal_length >> pass >> temp.sepal_width >> pass >> str;
            if (str.compare("Iris-setosa") == 0)
                temp.correct_type = 1;
            else if (str.compare("Iris-versicolor") == 0)
                temp.correct_type = 2;
            else if (str.compare("Iris-virginica") == 0)
                temp.correct_type = 3;

            iris_set.push_back(temp);
        }

        return  iris_set;
    }
}

namespace iris{
    struct chromo
    {
        vector<kmeans::iris> iris_set;
        double SSE;
    };
}
/*
int main() {
    double total1 = 0;
    for (int r = 0; r < 30; r++)
    {
        vector<kmeans::iris> iris_set = iris_RW::read_iris();

        kmeans::kmeans_main(iris_set, 3);

        double total = 0;
        for (auto i : iris_set)
        {
            //cout.precision(2);
            //cout << fixed << i.petal_length << " " << fixed << i.petal_width << " ";
            //cout << fixed << i.sepal_length << " " << fixed << i.sepal_width << " " << fixed << i.correct_type << " " << fixed << i.current_type << endl;
            if (i.current_type == i.correct_type)
                total++;
        }

        cout << "iteration accuracy : "<< total / 150 << endl;
        total1 += total / 150;
    }

    cout << "average accuracy : " << total1 / 30 << endl;
}
*/