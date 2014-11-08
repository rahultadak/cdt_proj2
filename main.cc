#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>

#include "classes.h"

using namespace std;

int main(int argc, char *argv[])
{
    int type = 0;
    ifstream trace;
    std::string input (argv[1]), bimodal("bimodal"), gshare("gshare"), hybrid("hybrid");
    
    predictor *p_bm, *p_gs ;
    chooser *c_hy;

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

        c_hy = new chooser(atoi(argv[2]));
        p_bm = new predictor(atoi(argv[5]),0);
        p_gs = new predictor(atoi(argv[3]),atoi(argv[4]));
        type = 3;
        trace.open(argv[8]); 
    } 

    if (Debug) cout << p_gs->gbhr_s() << endl; 
    if (!trace)
    {
        cerr << "File not found, please check" << endl;
        exit(1);
    }

    int tran_cnt = 0;
    Transaction InTran;
    string strIn;
    
    //getting the first line
    getline(trace, strIn);
    predictor_op p1, p2;
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
            p1 = p_bm->predict(InTran.retAddr());
            p_bm->update_tot_cnts(InTran.tranType(), p1);
            if (Debug) cout << endl;
        }
        else if (type == 2) 
        {
            if (Debug) cout << "GSHARE index: ";
            p1 = p_gs->predict(InTran.retAddr());
            p_gs->update_tot_cnts(InTran.tranType(), p1);
            p_gs->update_gbhr(InTran.tranType());
            if (Debug) cout << endl;
        }
        else if (type == 3)
        {
            //Making predictions
            p1 = p_bm->predict(InTran.retAddr());
            p2 = p_gs->predict(InTran.retAddr());
            
            //Getting choice from chooser
            int choice = c_hy->get_choice(InTran.retAddr());
            
            //Picking prediction based on chooser
            if (choice<2)
            {
                //Choose BIMODAL, and update mispredict based on that
                p_bm->update_tot_cnts(InTran.tranType(), p1);
            }
            else
            {
                p_gs->update_tot_cnts(InTran.tranType(), p2);
            }
            
            p_gs->update_gbhr(InTran.tranType());

            //Updating Chooser counter
            //BIMODAL correct and GSHARE incorrect
            if (p1.prediction == InTran.tranType() &&
                    p2.prediction != InTran.tranType())
            {
                c_hy->update_cntr(InTran.retAddr(),false);
            }
            //BIMODAL incorrect and GSHARE correct
            else if (p1.prediction != InTran.tranType() &&
                    p2.prediction == InTran.tranType())
            {
                c_hy->update_cntr(InTran.retAddr(),true);
            }
        }

        //Updating number of completed transactions
        tran_cnt++;

        //getting data from next line
        getline(trace, strIn);
        
        //breaking from the while loop if this is the last line of the file.
        //Because the eof state may not get set until after a read is attempted 
        //past the end of file. In our case we are reading line i+1 in iteration i. 
        //Thus when we reach the last line, the next line is read and the eofbit is set
        //before attempting the while loop condition.
    }

    float mispred_rate;
    //Printing outputs
    cout << "COMMAND" << endl;
    for (int i = 0; i<argc; i++)
    {
        cout << argv[i] << " ";
    }
    switch (type)
    {
        case 1:
            cout << endl << "OUTPUT" << endl;
            cout << "number of predictions:  " << p_bm->num_preds() << endl;
            cout << "number of mispredictions:  "<< p_bm->num_mis() << endl;
            mispred_rate = ((float)p_bm->num_mis()/p_bm->num_preds())*100;
            cout << "misprediction rate: " << setprecision(2) << fixed 
                << mispred_rate << "%" << endl;
            cout << "FINAL BIMODAL CONTENTS" << endl;
            p_bm->print_op();
            break;

        case 2:
            cout << endl << "OUTPUT" << endl;
            cout << "number of predictions:  " << p_gs->num_preds() << endl;
            cout << "number of mispredictions:  "<< p_gs->num_mis() << endl;
            mispred_rate = ((float)p_gs->num_mis()/p_gs->num_preds())*100;
            cout << "misprediction rate: " << setprecision(2) << fixed 
                << mispred_rate << "%" << endl;
            cout << "FINAL GSHARE CONTENTS" << endl;
            p_gs->print_op();
            break;

        case 3:
            cout << endl << "OUTPUT" << endl;
            cout << "number of predictions:  " << tran_cnt << endl;
            cout << "number of mispredictions:  "<< p_bm->num_mis() + p_gs->num_mis() << endl;
            mispred_rate = ((float)(p_bm->num_mis() + p_gs->num_mis())
                    /tran_cnt)*100;
            cout << "misprediction rate: " << setprecision(2) << fixed 
                << mispred_rate << "%" << endl;
            c_hy->print_op();
            cout << "FINAL GSHARE CONTENTS" << endl;
            p_gs->print_op();
            cout << "FINAL BIMODAL CONTENTS" << endl;
            p_bm->print_op();

    }

    return 0;
}

    
