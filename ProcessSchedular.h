#ifndef PROCESSSCHEDULAR_H
#define PROCESSSCHEDULAR_H

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
using namespace std;

enum ProcessState{PREREADY, READY, RUNNING, BLOCKED, TERMINATED, WAITING};

struct Command
{
    string command;
    int duration;
    Command(string _command, int _duration){
        command = _command;
        duration = _duration;
    }
};

struct Process
{
    // Commands that holds all the info
    vector<Command> commands;
    ProcessState state;
    int currIndex;
    int processStartTime;
    int completionTime;
    bool waiting;
    int doorOwned;

    Process(int startTime){
        state = PREREADY;
        currIndex = 0;
        processStartTime = startTime;
        completionTime = 0;
        waiting = false;
        doorOwned = -1;
    }

    bool endOfProcess(){
        if(currIndex >= commands.size()){
            return true;
        }
        return false;
    }

};

class ProcessSchedular
{

    public:
        //Constructors, Deconstructors, and function to help the default constructor
        ProcessSchedular();
        ~ProcessSchedular();
        void setCommandsToProcess(string _input);

        //useful info for methods
        int busyCores(){return NCORES - nCores;}
        bool freeCore(){return (nCores > 0 && nCores <= NCORES);};
        bool freeSSD(){return ssd;};
        bool allTerminated();

        //request from events
        void requestCore(int pIndex, int eTime);
        void requestSSD(int pIndex, int eTime);
        void requestIO(int pIndex, int eTime);
        void requestLock(int pIndex, int doorNum);
        void requestUnlock(int pIndex, int doorNum);

        //completion time is met functions
        void completeCPU(int pIndex, int cIndex, string event, int eTime);
        void completeSSD(int pIndex, int cIndex, string event, int eTime);
        void completeIO(int pIndex, int eTime);

        //Finds the next completion time event and updates globalTime
        int nextCompletionEvent();
        void completeEvent(int &pIndex, int &cmdIndex, string &event, int &eTime, ProcessState &state);


        void setTerminated(int pIndex);   //set terminated if cmd index is = to Cmd size;
        void processEvent();
        //void startExecutions();
        int findNextEvent();
        void setArgs(int i, int& currIndexCommand, string& event, int& timeEvent, ProcessState& state);
       
        //Prints for outputs
        void printProcessess();
        void printStart(int currProcess, int eventTime);
        void printTerminate(int currProcess);
        void printProcessTable();

    private:
        int globalTime;     //tracks the total time of schedualing events
        int NCORES;         //total cores we started with, KEEP CONSTANT!!!
        int nCores;         //variable to use when we need to subtract one 
        static const int DOOR_SIZE = 64;        //total doors program has
        int doorsArr[DOOR_SIZE];        //0 = we door isnt owned, 1 = door is lock/owned
        bool ssd;           //ssd free = false, not free = true

        queue<int> cpuQueue;        //queue for when we dont have enough cores for processors
        queue<int> ssdQueue;        //queue for when the ssd isn't free, we add the processors to queue
        queue<int>* doorQueue;      //creates an array of size 64, if door is taken we add processor to queue

        vector< Process> processess;        //holds the processors and represented by index

        
};

#endif