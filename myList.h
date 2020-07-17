#pragma once
#include <vector>
#include <string>
using namespace std;

class myList
{
private:
    /* data */
public:
    vector<bool> bitlist;

    myList() {
        
    }

    myList(int maxbit){
        bitlist = vector<bool>(maxbit, false);
    }

    myList(vector<bool> bit) {
        bitlist = vector<bool>(bit.begin(), bit.end());
    }

    void flip(int pos) {
        bitlist[pos] = !bitlist[pos];
    }

    int count() {
        int counter = 0;
        for(bool a : bitlist) {
            if (a)
                counter++;
        }
        return counter;
    }

    // this to_string function is reverse order
    // since the vector order is different with bitset
    string to_string() {
        string temp = "";
        for (int i = bitlist.size()-1; i >= 0; i--)
        {
            if (bitlist[i])
                temp += "1";
            else 
                temp += "0";
        }
        return temp;
    }
    ~myList() {
    }
};