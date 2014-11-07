#include <vector>
#include <cmath>

using namespace std;
int Debug = 1;

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

class predictor{
    private:
        int index_size, index_offset;
        int gbhr_size;
        int gbhr_offset;
        int gbhr;
        vector<PCounter> counters;
        int mispredict;

    public:
        predictor(int in_size, int in_ghr)
        {
            index_size = in_size;
            index_offset = pow(2,index_size) - 1;
            gbhr_size = in_ghr;
            gbhr_offset = (pow(2,gbhr_size) - 1); //*pow(2,(index_size - gbhr_size));  

            counters.resize(pow(2,index_size));
        }

        int num_counters()
        {   return counters.size();}
       
        int gbhr_off()
        {   return gbhr_offset; }

        int gbhr_s()
        {   return gbhr_size; }

        void predict(int addr, bool taken)
        {
            addr = addr / 4; //Removing the last 2 bits
            int index = addr & index_offset;
            if (gbhr_size!=0)
            {
                int tmp =  (int)pow(2,(index_size - gbhr_size));    
                int index = ((gbhr ^ (index/tmp))* tmp) + (index & (tmp - 1));            
            }
            //Need to add misprediction counter
            //need to add update of prediction counters
            //Update of GBHR
            //
            if (Debug) cout << hex << index;
        }

};

class Transaction{
    private:
        int type,addr;
        
    public:
        void setType(int x) { type = x;}
        int tranType() { return type;}

        void setAddr(const string &x) 
        {
            istringstream adr_buf(x.substr(0,6));
            adr_buf>>hex>>addr;
        }
        int retAddr() { return addr; }

};


