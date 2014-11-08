#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;
int Debug = 0;

class PCounter{
    public:

        PCounter()
        {
            state = 2;
        }
        
        int present_state()
        {   return state;}
        
        void update_state (bool taken)
        {
            if (taken && state < 3)
                state +=1;
            
            else if (!taken && state > 0)
                state -= 1;
        }

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

struct predictor_op{
    int index;
    bool prediction;
};

class predictor{
    private:
        int index_size, index_offset;
        int gbhr_size;
        int gbhr_offset;
        int gbhr;
        int num_counters;
        vector<PCounter> counters;
        int predicts,mispredicts;
        float mispred_rate;

    public:
        predictor(int in_size, int in_ghr)
        {
            index_size = in_size;
            index_offset = pow(2,index_size) - 1;
            gbhr_size = in_ghr;
            gbhr_offset = (pow(2,gbhr_size) - 1); 
            num_counters = pow(2,index_size);
            counters.resize(num_counters);
            predicts = 0;
            mispredicts = 0;
        }

        int num_cntrs()
        {   return num_counters;}
       
        int num_preds()
        {   return predicts; }
        
        int num_mis()
        {   return mispredicts; }

        int gbhr_off()
        {   return gbhr_offset; }

        int gbhr_s()
        {   return gbhr_size; }

        predictor_op predict(int addr)
        {
            //Updating number of predictions
            predicts += 1;
            if (Debug) cout << "predicts: " << predicts << endl; 
            addr = addr / 4; //Removing the last 2 bits
            int index = addr & index_offset;
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

        void update_gbhr(bool taken)
        {
            //Update the value of the gbhr
            gbhr = (gbhr/2) | (int)taken * (int)pow(2,gbhr_size-1);

            if (Debug && gbhr_size != 0)
            {
                cout << "BHR UPDATED: " << gbhr << endl;
            }
        }

        void print_op()
        {
            cout << endl << "OUTPUT" << endl;
            cout << "number of predictions:  " << predicts << endl;
            cout << "number of mispredictions:  "<< mispredicts << endl;
            mispred_rate = ((float)mispredicts/predicts)*100;
            cout << "misprediction rate: " << setprecision(4) << mispred_rate << "%" << endl;
            cout << "FINAL ";
            
            if(!gbhr_size)
                cout << "BIMODAL";
            else
                cout << "GSHARE";
            cout << "  CONTENTS" << endl;

            for (int i = 0;i<num_counters;i++)
            {
                cout << dec << i;
                cout << setw(7) << counters.at(i).present_state() << endl;
            }
        }
};

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


