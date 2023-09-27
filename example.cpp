#include "mleak_plugin.h"
#include "mleak_define.h"

#include <iostream>
using namespace std;

int main() {
    int *p1 = new int;
    *p1 = 20;

    // delete p1;
    int check_cnt = checkLeaks();
    if(check_cnt == 0) {
        cout << "Safe" << endl;
    } else {
        cout << "Not Safe. "<< check_cnt <<" place(s) will result in memory leak." << endl;
    }
    return 0;
}