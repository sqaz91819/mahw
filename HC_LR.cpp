#include <bitset>
#include <iostream>
#include <random>
#include <chrono>
#include <string>
#include <fstream>
#include "mylib.h"
#include "test.h"
#define MAX_BITS 100
#define RUNTIMES 1000
#define RUN 30

using namespace std;

bitset<MAX_BITS> bitstring;
bitset<MAX_BITS> new_string;
int counter = 0 ;
int new_counter = 0;
bool direction = true;
// random setting for reverse position
unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
default_random_engine generator( seed );
uniform_int_distribution<int> distribution(0, MAX_BITS - 1 );

void random_bit(bitset<MAX_BITS> & bits) {
    uniform_int_distribution<int> distribution(0, 1 );
    for (int i = 0; i < bits.size(); i++)
    {
        bits[i] = distribution(generator);
    }
}

void initialization(void) {
    random_bit(bitstring);  // set bitstring
    counter = bitstring.count();            // recording number of 1
    cout << "Initial : " << bitstring << endl;
    cout << "Counter : " << counter << endl;
    cout << "---------------------------------------------" << endl;
}

void transition() {
    new_string = bitset<MAX_BITS> ( bitstring );
    if (direction)
    {
        bit_add(new_string);
    }
    else
    {
        bit_neg(new_string);
    }
    
    
    /*
    int pos = distribution(generator);  // select random position from 0~ (MAXBITS-1)
    new_string.flip( pos );             // reverse one bit

    */
    cout << "Test string : " << new_string << endl;
    cout << "Test counter : " << new_string.count() << endl;
    cout << "---------------------------------------------" << endl;
}

void evaluation() {
    new_counter = new_string.count();
}

void determination() {
    // new stirng is better
    if( new_counter >= counter ) {
        cout << "New : " << new_string << endl;
        cout << " Counter : " << new_counter << endl;
        cout << "Updated !!" << endl;
        cout << "---------------------------------------------" << endl;
        bitstring = new_string; // update it
        counter = new_counter;  // recording number of one
    }
    else
    {
        direction = !direction;
    }
    
}

void output() {
    ofstream myFile;
    myFile.open("HC_LR.txt");
    myFile << "Best string : " << bitstring << endl;
    myFile << "Counter     : " << counter << endl;
    myFile.close();
}

int main() {
    for (int j = 0; j < RUN; j++) {
        mylib mb("HC_LRout" + to_string(j), 10);
        initialization();
        for (int i = 0; i < RUNTIMES /*&& counter != MAX_BITS*/; i++) {
            transition();
            evaluation();
            determination();
            mb.adddata(i, counter);
        }
    mb.outputdata();
    }
    output();
    mySystemPause();
}