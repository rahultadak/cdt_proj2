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
        int index_bits, index_offset;
        int ghr_size;
        vector<PCounter> counters;

    public:
        predictor(int in_size, int in_ghr)
        {
            index_bits = in_size;
            index_offset = pow(2,index_bits) - 1;
            ghr_size = in_ghr;

            counters.resize(pow(2,index_bits));
        }

        int num_counters()
        {   return counters.size();}
        
        void predict(int addr)
        {
            addr = addr / 4; //Removing the last 2 bits
            int index = addr & index_offset;
            //For gshare, we need to add the GHR component
            //Update the counters
            //Update the GHR
            if (Debug) cout << dec << index;
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


