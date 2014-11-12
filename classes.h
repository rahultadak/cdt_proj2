#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;
int Debug = 0;

//Basic smith counter class
class PCounter{
    public:

        //Initial state
        void init_state(int init)
        {
           state = init;
        }
        
        //Read present state
        int present_state()
        {   return state;}
        
        //Update state on actual branch decision
        void update_state (bool taken)
        {
            if (taken && state < 3)
                state +=1;
            
            else if (!taken && state > 0)
                state -= 1;
        }

        //Return prediction
        bool prediction()
        {
            if (state>=2)
                return true;
            else
                return false;
        }
            
    private: 
        int state;
            
};

//Prediction structure, has output anf prediction data
struct predictor_op{
    int index;
    bool prediction;
};

//Predictor Class, made generic for BIMODAL and GSHARE
class predictor{
    private:
        int index_size, index_offset;
        int gbhr_size;
        int gbhr_offset;
        int gbhr;
        int num_counters;
        vector<PCounter> counters;
        int predicts,mispredicts;

    public:

        predictor(int in_size, int in_ghr)
        {
            index_size = in_size;
            index_offset = pow(2,index_size) - 1;
            gbhr_size = in_ghr;
            gbhr_offset = (pow(2,gbhr_size) - 1); 
            num_counters = pow(2,index_size);
            predicts = 0;
            mispredicts = 0;
            counters.resize(num_counters);
            for (int i=0;i<num_counters;i++)
            {
                counters.at(i).init_state(2);
            }
        }

        //Returns the size of the counter array
        int num_cntrs()
        {   return num_counters;}
       
        //Updates the total prediction count
        void update_preds()
        {   predicts += 1;  }

        //Returns number of successful predictions
        int num_preds()
        {   return predicts; }
        
        //Updates the misprediction count
        void update_mis()
        {   mispredicts += 1;  }

        //Returns number of mipredictions
        int num_mis()
        {   return mispredicts; }

        //Predictior function, returns the prediction structure object
        predictor_op predict(int addr)
        {
            //Updating number of predictions
            predicts += 1;
            if (Debug) cout << "predicts: " << predicts << endl; 
            addr = addr / 4; //Removing the last 2 bits

            //Index for BIMODAL
            int index = addr & index_offset;

            //Calculating the index for GSHARE
            if (gbhr_size!=0)
            {
                int tmp =  (int)pow(2,(index_size - gbhr_size));    
                index = ((gbhr ^ (index/tmp))* tmp) + (index & (tmp - 1));            
            }
            if (Debug) 
            {
                cout << dec << index;
                cout << " old value: " << counters.at(index).present_state();
                cout << " new value ";
            }
            
            predictor_op op;
            op.index = index;
            op.prediction = counters.at(index).prediction();
            return op;
        }

        //Misprediction counter update, predictor smith counter update
        void update_tot_cnts(bool taken, predictor_op p)
        {
            //Update number of mispredicts
            if (p.prediction != taken)
            {   
                mispredicts += 1;
            }

            //Update the prediction counter based on if the branch was taken.
            counters.at(p.index).update_state(taken);
            if (Debug) 
                cout << counters.at(p.index).present_state() << endl; 
        }

        //GBHR update
        void update_gbhr(bool taken)
        {
            //Update the value of the gbhr
            gbhr = (gbhr/2) | (int)taken * (int)pow(2,gbhr_size-1);

            if (Debug && gbhr_size != 0)
            {
                cout << "BHR UPDATED: " << gbhr << endl;
            }
        }

        //Print contents
        void print_op()
        {
            for (int i = 0;i<num_counters;i++)
            {
                cout << dec << i;
                cout << setw(7) << counters.at(i).present_state() << endl;
            }
        }
};

//Chooser class for the HYBRID predictor, loosely based on the predictor class
class chooser{
    private:
        int size, offset, num_counters;
        vector<PCounter> chooser_cntrs;

    public:
        chooser(int k)
        {
            size = k;
            num_counters = pow(2,size);
            chooser_cntrs.resize(num_counters);
            offset = num_counters - 1;
            for (int i=0;i<num_counters;i++)
            {
                chooser_cntrs.at(i).init_state(1);
            }
        }

        //Return the choice to follow
        int get_choice(int addr)
        {
            addr = addr/4;
            int index = addr & offset;
            return chooser_cntrs.at(index).present_state();
        }

        //Update the chooser counter 
        void update_cntr(int addr, bool inp)
        {
            addr = addr/4;
            int index = addr&offset;
            chooser_cntrs.at(index).update_state(inp);
        }

        //Print final contents
        int print_op()
        {
            cout << "FINAL CHOOSER CONTENTS" << endl;
            for (int i = 0;i<num_counters;i++)
            {
                cout << dec << i;
                cout << setw(7) << chooser_cntrs.at(i).present_state() << endl;
            }
        }
};

//Transaction class
class Transaction{
    private:
        int addr;
        bool type;
        
    public:
        void setType(bool x) { type = x;}
        //type is true if branch is actually taken
        //type is false if branch is actually not taken
        int tranType() { return type;}

        void setAddr(const string &x) 
        {
            istringstream adr_buf(x.substr(0,6));
            adr_buf>>hex>>addr;
        }
        int retAddr() { return addr; }

};


