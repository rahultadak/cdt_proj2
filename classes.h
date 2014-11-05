
using namespace std;

class PCounter{
    public:

        PCounter()
        {
            state = 1;
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


