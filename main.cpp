/*
    Austin Bernal
    Assignment #1
*/


#include <iostream>
#include <fstream>
#include <queue>
#include <vector>
#include "ProcessSchedular.h"
#include "ProcessSchedular.cpp"

using namespace std;



int main()
{

    //cout << "START PROGRAM\n" << endl;

    ProcessSchedular mySchedule;

    //mySchedule.printProcessess();
    // mySchedule.startExecutions();
    mySchedule.processEvent();
    
    //cout << "\nEND OF PROGRAM" << endl;
    return 0;
}

