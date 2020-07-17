#include <bitset>
#define MAX_BITS 100
#define RUNTIMES 1000
using namespace std;

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

void mySystemPause() {
    cout << "Press enter to exit...";
    cin.ignore();
    while (cin.get() != '\n') {
        cout << "Press enter to exit...";
    }
}