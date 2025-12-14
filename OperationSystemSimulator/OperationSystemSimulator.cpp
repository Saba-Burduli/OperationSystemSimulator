// OperationSystemSimulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
using namespace std;

struct Process {

    int pid;
    int arrrival;
    int burst;
    int remaining;

    int start;
    int finish;
};

int main()
{
    int n;
    cout << "How many Processes?";
    cin >> n;

    vector<Process> processes;
    processes.reserve(n);

    for (int i = 0; i < n; i++) {
        Process p;
        p.pid = i + 1;

        cout << "Process " << p.pid << " arrival time: ";
        cin >> p.arrrival;

        cout << "Process " << p.pid << "CPU burst time: ";
        cin >> p.burst;

        p.remaining = p.burst;
        p.start = -1;
        p.finish = -1;

        processes.push_back(p);

    }

    vector<int> readyQueue;

    int time = 0;
    int finishedCount = 0;
    int runningIndex = -1;

    cout << "\n--- Simulation (FCFS) ---\n";

    while (finishedCount < n)
    {
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrrival == time && processes[i].finish == -1)
            {
                readyQueue.push_back(i);
            }
        }

        if (runningIndex == -1 && !readyQueue.empty())
        {
            runningIndex = readyQueue.front();

            readyQueue.erase(readyQueue.begin());

            if (processes[runningIndex].start == -1)
            {
                processes[runningIndex].start = time;
            }
        }

        if (runningIndex == -1)
        {
            cout << " Time : " << time << " CPU idle\n";
        }
        else {
            cout << "Time " << time << ": Running P" << processes[runningIndex].pid << "\n";

            processes[runningIndex].remaining--;

            if (processes[runningIndex].remaining == 0)
            {
                processes[runningIndex].finish = time + 1;
                finishedCount++;
                runningIndex = -1;
            }
        }
        time++;

    }
    //Stats
    cout << "\n------ Results ------\n";
    cout << "PID Arrival Burst Start Finish Turnarount \n";
    for (int i = 0; i < n; i++)
    {
        int turnaround = processes[i].finish - processes[i].arrrival;
        cout << "p" << processes[i].pid << "     "
            << processes[i].arrrival << "     "
            << processes[i].burst << "     "
            << processes[i].start << "     "
            << processes[i].finish << "       "
            << turnaround << "\n";
    }
    return 0;
}