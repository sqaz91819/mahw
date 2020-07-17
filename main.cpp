#include "HC.cpp"
#include "ES.cpp"
#include "SA.cpp"
#include "TS.cpp"

int main(int argc, char *argv[]) {
    if (string("ES").compare(argv[1]) == 0)
    {
        // maxbit, iteration, timelimit
        ES::ES(std::stoi(argv[2]), std::stoi(argv[3]), std::stoi(argv[4]));
    }
    else if (string("HC").compare(argv[1]) == 0)
    {
        // maxbit, iteration, runtimes
        HC::HC(std::stoi(argv[2]),std::stoi(argv[3]),std::stoi(argv[4]));
    }
    else if (string("SA").compare(argv[1]) == 0)
    {
        // maxbit, iteration, runtimes, temprature
        SA::SA sa(std::stoi(argv[2]),std::stoi(argv[3]),std::stoi(argv[4]),std::stoi(argv[5]));
        sa.SA_run();
        // SA_class::SA(std::stoi(argv[2]),std::stoi(argv[3]),std::stoi(argv[4]),std::stoi(argv[5]));
    }
    else if (string("TS").compare(argv[1]) == 0)
    {
        // maxbit, iteration, runtimes, length
        TS::TS ts(std::stoi(argv[2]),std::stoi(argv[3]),std::stoi(argv[4]),std::stoi(argv[5]));
        ts.TS_run();
    }
    else
    {
        cout << "No file" << endl;
        return 0;
    }
}