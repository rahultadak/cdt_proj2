#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

#include "classes.h"

using namespace std;

int main(int argc, char *argv[])
{
    int index_size, ghr_size;
    std::string input (argv[1]), bimodal("bimodal"), gshare("gshare"), hybrid("hybrid");
    
    if (!input.compare(bimodal))
    {
        if (argc != 6)
        {
            cout << "Not enough arguments, ./sim bimodal <M2> <BTB size> <BTB assoc> \
<tracefile>" << endl;
            exit(EXIT_FAILURE);
        }

        predictor* p = new predictor(atoi(argv[2]),0);
    }
    else if (!input.compare(gshare))
    {   
        if (argc != 7)
        {
            cout << "Not enough arguments, ./sim gshare <M1> <N> <BTB size> <BTB assoc> \
<tracefile>" << endl;
            exit(EXIT_FAILURE);
        }

        predictor* p = new predictor(atoi(argv[2]),atoi(argv[3]));
    }
    else if (!input.compare(hybrid))
    {   
        if (argc != 9)
        {
            cout << "Not enough arguments, ./sim gshare <K> <M1> <N> <M2> <BTB size>\
<BTB assoc> <tracefile>" << endl;
            exit(EXIT_FAILURE);
        }

        //Another class for hybrid?
        predictor* p = new predictor(atoi(argv[2]),atoi(argv[3]));
    }
    return 0;
}

    
