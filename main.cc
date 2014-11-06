#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>

#include "classes.h"

using namespace std;

int main(int argc, char *argv[])
{
    int type;
    ifstream trace;
    std::string input (argv[1]), bimodal("bimodal"), gshare("gshare"), hybrid("hybrid");
    
    predictor *p_bm, *p_gs ;

    if (!input.compare(bimodal))
    {
        if (argc != 6)
        {
            cout << "Not enough arguments, ./sim bimodal <M2> <BTB size> <BTB assoc> \
<tracefile>" << endl;
            exit(EXIT_FAILURE);
        }

        p_bm = new predictor(atoi(argv[2]),0);
        type = 1;
        trace.open(argv[5]); 
    }
    else if (!input.compare(gshare))
    {   
        if (argc != 7)
        {
            cout << "Not enough arguments, ./sim gshare <M1> <N> <BTB size> <BTB assoc> \
<tracefile>" << endl;
            exit(EXIT_FAILURE);
        }

        p_gs = new predictor(atoi(argv[2]),atoi(argv[3]));
        type = 2;
        trace.open(argv[6]); 

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
        p_bm = new predictor(atoi(argv[2]),atoi(argv[3]));
        type = 3;
        trace.open(argv[8]); 

    } 

    if (!trace)
    {
        cerr << "File not found, please check" << endl;
        exit(1);
    }

    int tran_cnt = 0;
    Transaction InTran;
    while(trace)
    {
        tran_cnt++;
        if (Debug) cout << dec<<  tran_cnt << ". PC: ";
        //taking input
        string strIn;
        //getting the line
        getline(trace, strIn);
        //breaking from the while loop if this is the last line of the file.
        if (trace.eof()) break;

        InTran.setAddr(strIn);
        //Setting the type of transaction
        if (strIn[7] == 't')
        {
            InTran.setType(1);
            if (Debug) cout << hex << InTran.retAddr() << " t" << endl;
        }
        else if (strIn[7] == 'n')
        {
            InTran.setType(0);
            if (Debug) cout << hex << InTran.retAddr() << " n"  << endl;
        }

        if (type == 1) 
        {
            if (Debug) cout << "BIMODAL index: ";
            p_bm->predict(InTran.retAddr());
            if (Debug) cout << endl;
        }
    }

    return 0;
}

    
