#include <iostream>
#include <fstream>

#include "flow.h"
using namespace std;



int main(int argc, char* argv[])
{   
    if (argc > 1) 
    {
    Network n(argv[1]);

    Flow * f = n.push_relabel(0,1);
    f->print();
    delete f;
    }

}
