#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;


class mylib
{
private:
    string _file;
    int _scale;
    vector<int> _data;
    
public:
    mylib(/* args */);
    mylib(string filename, int scale) {
        _file = filename;
        _scale = scale;
    }
    ~mylib();

    void adddata( int runtimes, int counter) {
        if (runtimes % _scale == 0)
        {
            _data.push_back( counter );
        }
    }

    void outputdata() {
        ofstream myFile;
        myFile.open(".\\data and plot\\" + _file);
        myFile << _scale << endl;
        for( int d : _data ) {
            myFile << d << endl;
        }
        myFile.close();
    }
    
};

mylib::mylib(/* args */)
{
}

mylib::~mylib()
{
}