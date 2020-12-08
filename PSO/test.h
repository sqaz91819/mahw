#pragma once
#include <bitset>
#include "myList.h"
#define MAX_BITS 100
#define RUNTIMES 1000
using namespace std;

//testing version no use
void bit_neg(bitset<MAX_BITS> &test)
{
    bool carry = false;
    if (!test[0])
        carry = true;
    for (int i = 0; i < test.size(); i++)
    {
        if (!test[i] && carry)
        {
            test.flip(i);
            carry = true;
        }
        else
        {
            test.flip(i);
            break;
        }
    }
}

void bit_neg(myList &test)
{
    bool carry = false;
    if (!test.bitlist[0])
        carry = true;
    for (int i = 0; i < test.bitlist.size(); i++)
    {
        if (!test.bitlist[i] && carry)
        {
            test.flip(i);
            carry = true;
        }
        else
        {
            test.flip(i);
            break;
        }
    }
}

// testing version no use
void bit_add(bitset<MAX_BITS> &test)
{
    bool carry = false;
    if (test[0])
        carry = true;
    for (int i = 0; i < test.size(); i++)
    {
        if (test[i] && carry)
        {
            test.flip(i);
            carry = true;
        }
        else
        {
            test.flip(i);
            break;
        }
    }
}

void bit_add(myList &test)
{
    bool carry = false;
    if (test.bitlist[0])
        carry = true;
    for (int i = 0; i < test.bitlist.size(); i++)
    {
        if (test.bitlist[i] && carry)
        {
            test.flip(i);
            carry = true;
        }
        else
        {
            test.flip(i);
            break;
        }
    }
}

void output(int average_result, int average_iteration, string filename)
{
    ofstream myFile;
    myFile.open(filename);
    myFile << average_iteration << endl;
    myFile << average_result << endl;
    myFile.close();
}

void mySystemPause() {
    cout << "Press enter to exit...";
    cin.ignore();
    while (cin.get() != '\n') {
        cout << "Press enter to exit...";
    }
}