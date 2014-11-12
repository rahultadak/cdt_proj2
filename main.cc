#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>

#include "classes.h"
#include "cache_classes.h"

using namespace std;

int main(int argc, char *argv[])
{
    int type = 0;
    ifstream trace;
    std::string input(argv[1]), bimodal("bimodal"), gshare("gshare"), hybrid("hybrid");
   
    //Instantiating cache object pointer
    Cache *btb;

    //Instantiating the predictor and chooser pointers as required
    predictor *p_bm, *p_gs ;
    chooser *c_hy;

    if (!input.compare(bimodal))
    {
        //BIMODAL simulation
        if (argc != 6)
        {
            cout << "Not enough arguments, ./sim bimodal <M2> <BTB size> <BTB assoc> \
<tracefile>" << endl;
            exit(EXIT_FAILURE);
        }

        //Setting up the BIMODAL predictor
        p_bm = new predictor(atoi(argv[2]),0);

        //With BTB
        if (atoi(argv[3]))
        {
            btb = new Cache(4,atoi(argv[3]),atoi(argv[4]),0,0,NULL);
        }
        else
            btb = NULL;

        type = 1;
        trace.open(argv[5]); 
    }
    else if (!input.compare(gshare))
    {   
        //GSHARE Simulation
        if (argc != 7)
        {
            cout << "Not enough arguments, ./sim gshare <M1> <N> <BTB size> <BTB assoc> \
<tracefile>" << endl;
            exit(EXIT_FAILURE);
        }

        //Setting up the GSHARE predictor
        p_gs = new predictor(atoi(argv[2]),atoi(argv[3]));

        //With BTB
        if (atoi(argv[4]))
        {
            btb = new Cache(4,atoi(argv[4]),atoi(argv[5]),0,0,NULL);
        }
        else
            btb = NULL;
        type = 2;
        trace.open(argv[6]); 

    }
    else if (!input.compare(hybrid))
    {   
        //HYBRID simulation
        if (argc != 9)
        {
            cout << "Not enough arguments, ./sim hybrid <K> <M1> <N> <M2> <BTB size>\
<BTB assoc> <tracefile>" << endl;
            exit(EXIT_FAILURE);
        }

        //Setting up Chooser
        c_hy = new chooser(atoi(argv[2]));

        //Setting up the BIMODAL and GSHARE predictors
        p_bm = new predictor(atoi(argv[5]),0);
        p_gs = new predictor(atoi(argv[3]),atoi(argv[4]));

        //With BTB
        if (atoi(argv[6]))
        {
            btb = new Cache(4,atoi(argv[6]),atoi(argv[7]),0,0,NULL);
        }
        else 
            btb = NULL;
        type = 3;
        trace.open(argv[8]); 
    } 

    if (!trace)
    {
        cerr << "File not found, please check" << endl;
        exit(1);
    }

    int tran_cnt = 0;

    //Transaction object
    Transaction InTran;
    string strIn;
    bool btb_hit;
    int btb_mispreds = 0;
    
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

        //BTB lookup
        if (btb!=NULL)
        {
            //BTB access will be a read access only
            btb_hit = btb->request(InTran.retAddr(),0);
            if (!btb_hit)
            {
                if (InTran.tranType())
                    btb_mispreds += 1;
                goto contin;
            }
        }

        //Choosing the predictor based in the type selected
        if (type == 1) 
        {
            if (Debug) cout << "BIMODAL index: ";

            //Making predictions
            p1 = p_bm->predict(InTran.retAddr());
            p_bm->update_tot_cnts(InTran.tranType(), p1);
            if (Debug) cout << endl;
        }
        else if (type == 2) 
        {
            if (Debug) cout << "GSHARE index: ";
            
            //Making predictions
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

contin:
        //Updating number of completed transactions
        tran_cnt++;

        //getting data from next line
        getline(trace, strIn);
        
    }

    float mispred_rate;
    //Printing outputs
    cout << "COMMAND" << endl;
    for (int i = 0; i<argc; i++)
    {
        cout << argv[i] << " ";
    }

    if (btb == NULL)
    {
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
    }

    else
    {
        switch (type)
        {
            case 1:
                cout << endl << "OUTPUT" << endl;
                cout << "size of BTB:  " << btb->getSize() << endl;
                cout << "number of branches:  " << tran_cnt << endl; 
                cout << "number of predictions from branch predictor:  " 
                    << p_bm->num_preds() << endl;
                cout << "number of mispredictions from branch predictor:  "
                    << p_bm->num_mis() << endl;
                cout << "number of branches miss in BTB and taken:  " 
                    << btb_mispreds << endl;
                cout << "total mispredictions: " << p_bm->num_mis() + btb_mispreds << endl;
                mispred_rate = ((float)(p_bm->num_mis() + btb_mispreds)/tran_cnt)*100;
                cout << "misprediction rate: " << setprecision(2) << fixed 
                    << mispred_rate << "%" << endl << endl;
                cout << "FINAL BTB CONTENTS" << endl;
                btb->print_contents();
                cout << endl << "FINAL BIMODAL CONTENTS" << endl;
                p_bm->print_op();
                break;

            case 2:
                cout << endl << "OUTPUT" << endl;
                cout << "size of BTB:  " << btb->getSize() << endl;
                cout << "number of branches:  " << tran_cnt << endl; 
                cout << "number of predictions from branch predictor:  " 
                    << p_gs->num_preds() << endl;
                cout << "number of mispredictions from branch predictor:  "
                    << p_gs->num_mis() << endl;
                cout << "number of branches miss in BTB and taken:  " 
                    << btb_mispreds << endl;
                cout << "total mispredictions: " << p_gs->num_mis() + btb_mispreds << endl;
                mispred_rate = ((float)(p_gs->num_mis() + btb_mispreds)/tran_cnt)*100;
                cout << "misprediction rate: " << setprecision(2) << fixed 
                    << mispred_rate << "%" << endl << endl;
                cout << "FINAL BTB CONTENTS" << endl;
                btb->print_contents();
                cout << endl << "FINAL GSHARE CONTENTS" << endl;
                p_gs->print_op();
        }
    }


    return 0;
}

    
