#include "ProcessSchedular.h"
using namespace std;

ProcessSchedular::ProcessSchedular()
{
    globalTime = -1;
    ssd = true;
    //initilize doors
    doorQueue = new queue<int>[DOOR_SIZE];
    for(int i = 0; i < DOOR_SIZE; i++){
        doorsArr[i] = -1;

    }

    string input;
    cin >> input;
    if(input == "NCORES"){
        cin >> nCores;
        NCORES = nCores;
        //cout << input << " " << nCores << endl;

        cin >> input;
    }
    else{
        nCores = 1;
    }

    //cout << input << endl;

    //function
    while(input == "START")
    {
        //fucntion read string, check if string is end, then return.
        //inside: read for the Process, add the command struct to Process
        ProcessSchedular::setCommandsToProcess(input);
        //cout << "Back in constructor, about to reloop or exit loop." << endl;
        if(!(cin >> input)){
            break;
        }
        //cout << "your input is: " << input << endl;
    }

}

ProcessSchedular::~ProcessSchedular(){
    delete[] doorQueue;
}

void ProcessSchedular::setCommandsToProcess(string _input)
{

    //finish reading "START" line
    string _command = _input;
    int _duration;
    cin >> _duration;

    //create structures
    Command _commandStruct = Command(_command, _duration);
    Process _processStruct = Process(_duration);

    //if startTime is -1, hence we are on the very first start command, updated the startTime;
    if(globalTime == -1){
        globalTime = 0;
    }
    
    //add first Command struct into the Process Struct inside the vector
    _processStruct.commands.push_back(_commandStruct);

    //read rest of instructions till you see "END"
    cin >> _command;
    if(_command == "END"){
       
    }
    else{
        //read data, add data, and exit loop when "END" appears
        while(_command != "END"){
            cin >> _duration;
            _commandStruct = Command(_command, _duration);
            _processStruct.commands.push_back(_commandStruct);

            //cout << _command << " " << _duration << endl;

            cin >> _command;
        }
    }

    //add process struct that we created to the processor vector
    processess.push_back(_processStruct);
    //cout << "Exiting and return to constructor" << endl;
}

void ProcessSchedular::printProcessess()
{
    cout << "\nPRINTING THE PROCESSESS AND ELEMENTS INSIDE\n" << endl;
    for(int i = 0; i < processess.size(); i++){
        for(int j = 0; j < processess.at(i).commands.size(); j++)
        cout << processess.at(i).commands.at(j).command << " " << processess.at(i).commands.at(j).duration << endl;

    }
    cout << "\nDONE PRINTING\n" << endl;
}

void ProcessSchedular::requestCore(int pIndex, int eTime)
{
    if(freeCore()){
        //give the core to processor for CPU and set completion time
        //cout << "Process " << pIndex << " gets a core at " << globalTime << " ms" << endl;
        processess.at(pIndex).completionTime = globalTime + eTime;
        //cout << "\t\tExpected completion time " << processess.at(pIndex).completionTime << " ms" << endl;
        //update the state of the processor
        processess.at(pIndex).state = RUNNING;
        //decrement a core
        nCores--;
    }
    else{   // no core avalible for CPU
        cpuQueue.push(pIndex);      //add the processor to the cpu queue
        processess.at(pIndex).state = WAITING;   //update process state
    }
}
void ProcessSchedular::requestSSD(int pIndex, int eTime)
{
    //if ssd is free, set up for completion
    if(freeSSD()){
        //cout << "Process " << pIndex << " access SSD at " << globalTime << " ms" << endl;
        processess.at(pIndex).completionTime = globalTime + eTime;
        //cout << "\t\tExpected completion time " << processess.at(pIndex).completionTime << " ms" << endl;
        
        processess.at(pIndex).state = BLOCKED;   //update process state
        ssd = false;    //ssd is no longer free
    }
    else{ // pass to ssd queue b/c another processor has access
        ssdQueue.push(pIndex);
        processess.at(pIndex).state = WAITING;   //update process state
    }
}
void ProcessSchedular::requestIO(int pIndex, int eTime)
{
    //cout << "Process " << pIndex << " request I/O at " << globalTime << " ms for " << eTime << endl;
    processess.at(pIndex).completionTime = globalTime + eTime;
    //cout << "\tI/O completion event will be " << processess.at(pIndex).completionTime << endl;
    processess.at(pIndex).state = BLOCKED;
}
void ProcessSchedular::requestLock(int pIndex, int doorNum)
{
    //cout << "Process " << pIndex << " request lock " << doorNum << " at time " << globalTime 
       // << " ms." << endl;

    if(doorsArr[doorNum] == -1 || doorsArr[doorNum] == 0){
        //process now owns that door
        processess.at(pIndex).doorOwned = doorNum;

        doorsArr[doorNum] = 1;
        //cout << "Process " << pIndex << " gets lock " << doorNum << " at time " << globalTime 
        //<< " ms." << endl;
        processess.at(pIndex).currIndex++;

        //requestCore(pIndex, processess.at(pIndex).commands.at(processess.at(pIndex).currIndex).duration);

    }
    else{
        //cout << "DOOR IS LOCK" << endl;
        doorQueue[doorNum].push(pIndex);
        processess.at(pIndex).state = WAITING;
    }

}
void ProcessSchedular::requestUnlock(int pIndex, int doorNum)
{
    // cout << "Process " << currProcess << " releases lock " << doorNum << " at time" <<     globalTime << " ms." << endl;
    processess.at(pIndex).doorOwned = -1;
    if(!doorQueue[doorNum].empty()){
        processess.at(doorQueue[doorNum].front()).currIndex++;
        processess.at(doorQueue[doorNum].front()).state = READY;    //change state to ready
        processess.at(doorQueue[doorNum].front()).doorOwned = doorNum;   //set new door owner
        requestCore(doorQueue[doorNum].front(), processess.at(doorQueue[doorNum].front()).commands.at(processess.at(doorQueue[doorNum].front()).currIndex).duration);
        doorQueue[doorNum].pop();   // dequeu door

        //cout << "Process " << pIndex << " releases lock " << doorNum << " at time " << globalTime << " ms." << endl;
    
    }
    else{
        //cout << "LOCK " << doorNum << " is now UNLOCKED" << endl;
        doorsArr[doorNum] = 0;
    }
    processess.at(pIndex).currIndex++;
}

void ProcessSchedular::completeCPU(int pIndex, int cIndex, string event, int eTime) //pIndex
{
    globalTime = processess.at(pIndex).completionTime;
    processess.at(pIndex).completionTime = -1;  //set back to default
    //cout << "Next event is a CPU completion event for Process " << pIndex << " at time " << globalTime << endl;

    if(!cpuQueue.empty()){      //Pop queue, then set up completion time
        int nextProccess = cpuQueue.front();
        cpuQueue.pop();
        processess.at(nextProccess).completionTime = globalTime 
            + processess.at(nextProccess).commands.at(processess.at(nextProccess).currIndex).duration;
        //cout << "Process " << nextProccess << " will release core at time " << processess.at(nextProccess).completionTime << endl;
        processess.at(nextProccess).state = RUNNING;
    }
    else{
        //increment cores that are free
        if(nCores < NCORES){
            nCores++;
        }
    }
    //increment the current inddex of command struct for the completed process
    processess.at(pIndex).currIndex++;
    processess.at(pIndex).state = READY;
}
void ProcessSchedular::completeSSD(int pIndex, int cIndex, string event, int eTime) //int pIndex
{
    globalTime = processess.at(pIndex).completionTime;
    processess.at(pIndex).completionTime = -1;   //set back to default
    //cout << "Next event is a SSD completion event for Process " << pIndex << " at time " << globalTime << endl;

    if(!ssdQueue.empty()){
        int nextProccess = ssdQueue.front();
        ssdQueue.pop();
        processess.at(nextProccess).completionTime = globalTime 
            + processess.at(nextProccess).commands.at(processess.at(nextProccess).currIndex).duration;
        //cout << "Process " << nextProccess << " will release SSD at time " << processess.at(nextProccess).completionTime << endl;
        processess.at(nextProccess).state = BLOCKED;
    }
    else{
        ssd = true;
    }
    //increment the current inddex of command struct for the completed process
    processess.at(pIndex).currIndex++;
    processess.at(pIndex).state = READY;
}
void ProcessSchedular::completeIO(int pIndex, int eTime)
{
    globalTime = processess.at(pIndex).completionTime;
    //cout << "Process " << pIndex << " completes I/O step at time " << globalTime << " ms" << endl; 

    processess.at(pIndex).currIndex++;
    processess.at(pIndex).state = READY;
}

int ProcessSchedular::nextCompletionEvent()
{
    int min = INT_MAX;
    int pIndex = -1;
    for(int i = 0; i < processess.size(); i++){
        if(processess.at(i).completionTime != -1  && 
            processess.at(i).currIndex < processess.at(i).commands.size()){
            if(processess.at(i).completionTime < min){
                min = processess.at(i).completionTime;
                pIndex = i;
            }
        }
    }
    return pIndex;
}
void ProcessSchedular::completeEvent(int &pIndex, int &cmdIndex, string &event, int &eTime, ProcessState &state)
{
    if(event == "CPU" && state == RUNNING){
        completeCPU(pIndex, cmdIndex, event, eTime);    //pIndex
        //check to see if therminate;
    }
    else if(event == "SSD" && state == BLOCKED){
        completeSSD(pIndex, cmdIndex, event, eTime);    //pIndex, cmdIndex, event, eTime
        // setArgs(pIndex, cmdIndex, event, eTime, state);
        // requestCore(pIndex, eTime);
    }
    else if(event == "INPUT" || event == "OUTPUT"){
        completeIO(pIndex, eTime);
    }
    setTerminated(pIndex);
    //check if that process is terminated
}

void ProcessSchedular::processEvent()
{
    // ProcessState status;
    while(!allTerminated()){   
        int pIndex = ProcessSchedular::findNextEvent();
        ProcessState status;
        int commIndex;      //represents current command index inside the processor
        string event;       //START, CPU, SSD, TERMINATE, LOCK
        int timeEvent;      //requested event time


        if(pIndex != -1){
        //finds next event in processess
            setArgs(pIndex, commIndex, event, timeEvent, status);
            // cout << "At process index " << pIndex << "\n";
            //cout << "Event is " << event << " with requesting time of " << timeEvent;
            // cout << endl;

            if(event == "START"){
                printStart(pIndex, timeEvent);      //process starts/arriving
                setArgs(pIndex, commIndex, event, timeEvent, status);   //update variables
                requestCore(pIndex, timeEvent);     //then request core for cpu
            }
            else if(event == "CPU"){
                requestCore(pIndex, timeEvent);
            }
            else if(event == "SSD"){
                requestSSD(pIndex, timeEvent); //request access to ssd
            }
            else if(event == "LOCK"){
                requestLock(pIndex, timeEvent); //here timeEven would actually be a door

            }
            else if(event == "UNLOCK"){
                requestUnlock(pIndex, timeEvent);
                
            }
            else if(event == "INPUT" || event == "OUTPUT"){
                requestIO(pIndex, timeEvent);
                
            }
        }
        //Check to see if there is any completed events coming up
        pIndex = nextCompletionEvent();
        if(pIndex != -1){
            setArgs(pIndex, commIndex, event, timeEvent, status);   //update variables values
            completeEvent(pIndex, commIndex, event, timeEvent, status);     //call, to see which event needs to complete
        }
    }
}

void ProcessSchedular::setTerminated(int pIndex)
{
    if(processess.at(pIndex).currIndex == processess.at(pIndex).commands.size()){
        processess.at(pIndex).state = TERMINATED;
        printTerminate(pIndex);
        cout << endl << endl;
    }
}
bool ProcessSchedular::allTerminated()
{
    for(int i = 0; i < processess.size(); i++){
        if(processess.at(i).state != TERMINATED){
            return false;
        }
    }
    return true;
}

int ProcessSchedular::findNextEvent()
{
    int min = INT_MAX;
    int processNum = -1;
    for(int i = 0; i < processess.size(); i++){
        if(processess.at(i).currIndex < processess.at(i).commands.size() 
            && (processess.at(i).state == PREREADY || processess.at(i).state == READY)) //(processess.at(i).state == READY || processess.at(i).state == PREREADY)
        {
            if(processess.at(i).commands.at(processess.at(i).currIndex).duration < min){    //processess.at(i).completionTime < min  && processess.at(i).completionTime != globalTime
                min = processess.at(i).commands.at(processess.at(i).currIndex).duration;
                processNum = i;
                //cout << "IN LOOP : THE LOWEST EVENT TIME IS: " << min << " AT PROCESS " << processNum << endl;
            }
        }
    }
    //cout << "THE LOWEST EVENT TIME IS: " << min << " AT PROCESS " << processNum << endl;
    if(processNum == -1){
        //cout << "END OF PROCESSESS" << endl;
    }
    return processNum;
}


void ProcessSchedular::setArgs(int i, int& currIndexCommand, string& event, int& timeEvent, ProcessState& state){
    //get current index location of command for the processes youre looking at.
    currIndexCommand = processess.at(i).currIndex;
    //assign current event of command for the processes youre looking at.
    event = processess.at(i).commands.at(currIndexCommand).command;
    //assign current event time of command for the processes youre looking at.
    timeEvent = processess.at(i).commands.at(currIndexCommand).duration;
    state = processess.at(i).state;
}

void ProcessSchedular::printStart(int currProcess, int eventTime)
{
    cout << "\n\nProcess " << currProcess << " starts at time " << eventTime << " ms" << endl;

    globalTime = eventTime;
    processess.at(currProcess).completionTime = -1;
    processess.at(currProcess).state = READY;
    //increment currrent command index
    processess.at(currProcess).currIndex++;

    //call functions to get amount of busy cores and process table status
    cout << "Current number of busy cores " << busyCores() << endl;
    ProcessSchedular::printProcessTable();
    cout << endl << endl;
    
}
void ProcessSchedular::printTerminate(int currProcess)
{
    cout << "\n\nProcess " << currProcess << " terminates at time " << globalTime << " ms." << endl;
    cout << "Current number of busy cores " << busyCores() << endl;
    ProcessSchedular::printProcessTable();
    processess.at(currProcess).currIndex++; //it will no longer account for in other functions now
}
void ProcessSchedular::printProcessTable()
{
    cout << "Process Table:" << endl;
    for(int i = 0; i < processess.size(); i++){

        if(processess.at(i).currIndex <= processess.at(i).commands.size()){

            if(processess.at(i).state == READY || processess.at(i).state == WAITING){
                cout << "Process " << i << " is READY." << endl;
            }
            else if(processess.at(i).state == RUNNING){
                cout << "Process " << i << " is RUNNING." << endl;
            }
            else if(processess.at(i).state == BLOCKED){
                cout << "Process " << i << " is BLOCKED." << endl;
            }
            else if(processess.at(i).state == TERMINATED){
                cout << "Process " << i << " is TERMINATED." << endl;
            }
        }
    }

    if(cpuQueue.empty()){   //ready queue
        cout << "Ready queue is empty." << endl;
    }
    else{
        cout << "Ready queue contains:" << endl;
        queue<int> temp = cpuQueue;
        for(int i = 0; i < temp.size(); i++){
            cout << "Process " << temp.front() << endl;
            temp.pop();
        }
    }

}