#include <iostream>
#include <sstream>
#include <string>

#include "classes.h"

using namespace std;

int main()
{

    PCounter k;

    cout << k.prediction() << endl;
    k.update_state(false);
    cout << k.present_state() << endl;

    return 0;
}

    
