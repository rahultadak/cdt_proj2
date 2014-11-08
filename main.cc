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
            cout << "Not enough arguments, ./sim hybrid <K> <M1> <N> <M2> <BTB size>\
<BTB assoc> <tracefile>" << endl;
            exit(EXIT_FAILURE);
        }

        //Another class for hybrid?
        p_bm = new predictor(atoi(argv[2]),atoi(argv[3]));
        type = 3;
        trace.open(argv[8]); 

    } 

    if (Debug) cout << p_gs->gbhr_s() << endl; 
    if (!trace)
    {
        cerr << "File not found, please check" << endl;
        exit(1);
    }

    int tran_cnt = 1;
    Transaction InTran;
    string strIn;
    
    //getting the first line
    getline(trace, strIn);
    while(!trace.eof())
    {
        if (Debug) cout << dec<<  tran_cnt << ". PC: ";

        InTran.setAddr(strIn);
        //Setting the type of transaction
        if (strIn[7] == 't')
        {
            InTran.setType(true);
            if (Debug) cout << hex << InTran.retAddr() << " t" << endl;
        }
        else if (strIn[7] == 'n')
        {
            InTran.setType(false);
            if (Debug) cout << hex << InTran.retAddr() << " n"  << endl;
        }

        if (type == 1) 
        {
            if (Debug) cout << "BIMODAL index: ";
            p_bm->predict(InTran.retAddr(), InTran.tranType());
            if (Debug) cout << endl;
        }
        else if (type == 2) 
        {
            if (Debug) cout << "GSHARE index: ";
            p_gs->predict(InTran.retAddr(), InTran.tranType());
            if (Debug) cout << endl;
        }
        tran_cnt++;

        //getting data from next line
        getline(trace, strIn);
        
        //breaking from the while loop if this is the last line of the file.
        //Because the eof state may not get set until after a read is attempted 
        //past the end of file. In our case we are reading line i+1 in iteration i. 
        //Thus when we reach the last line, the next line is read and the eofbit is set
        //before attempting the while loop condition.
    }

    //Printing outputs
    cout << "COMMAND" << endl;
    for (int i = 0; i<argc; i++)
    {
        cout << argv[i] << " ";
    }
    float tmp;
    switch (type)
    {
        case 1:
            p_bm->print_op();
            break;

        case 2:
            p_gs->print_op();
            break;
    }

    return 0;
}

    
