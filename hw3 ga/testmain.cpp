#include "GA.cpp"

int main(int argc, char *argv[]) {
    if (string("GA_r").compare(argv[1]) == 0)
    {
        // maxbit, iteration, runtimes, popsize
        GA::GA ga(std::stoi(argv[2]),std::stoi(argv[3]),std::stoi(argv[4]),std::stoi(argv[5]));
        ga.GA_run_r();
    }
    else if (string("GA_t").compare(argv[1]) == 0)
    {
        // maxbit, iteration, runtimes, popsize
        GA::GA ga(std::stoi(argv[2]),std::stoi(argv[3]),std::stoi(argv[4]),std::stoi(argv[5]));
        ga.GA_run_t();
    }
    else if (string("GA_e").compare(argv[1]) == 0)
    {
        // maxbit, iteration, runtimes, popsize
        GA::GA ga(std::stoi(argv[2]),std::stoi(argv[3]),std::stoi(argv[4]),std::stoi(argv[5]));
        ga.GA_run_e();
    }
    else {
        cout << "Wrong command or parameters" << endl;
        return 0;
    }
}