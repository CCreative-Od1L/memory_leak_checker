#include <iostream>
#include "mleak_test.h"

using namespace std;
int main() {
    int *p1 = new int;
    *p1 = 20;

    // delete p1;

    if(checkLeaks() == 0) {
        cout << "Safe" << endl;
    } else {
        cout << "Not Safe" << endl;
    }
    return 0;
}