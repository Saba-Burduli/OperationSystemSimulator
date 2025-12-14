#include "Kernel.h"

#include <iomanip>
#include <iostream>

using std::cin;
using std::cout;
using std::left;
using std::setw;
using std::string;
using std::vector;

vector<Process> Kernel::readProcesses() {
    int n = 0;
    cout << "How many processes? ";
    cin >> n;

    if (n <= 0) {
        return {};
    }

    vector<Process> processes;
    processes.reserve(n);

    for (int i = 0; i < n; ++i) {
        Process p;
        p.pid = i + 1;

        cout << "Process " << p.pid << " arrival time: ";
        cin >> p.arrival;

        cout << "Process " << p.pid << " CPU burst time: ";
        cin >> p.burst;

        if (p.arrival < 0 || p.burst <= 0) {
            cout << "Arrival must be >= 0 and burst must be > 0. Please re-enter this process.\n";
            --i;
            continue;
        }

        processes.push_back(p);
    }

    return processes;
}

Algorithm Kernel::chooseAlgorithm() const {
    cout << "\nChoose scheduling algorithm:\n";
    cout << " 1) First-Come First-Served (FCFS)\n";
    cout << " 2) Shortest Job First (non-preemptive)\n";
    cout << " 3) Shortest Remaining Time First (preemptive)\n";
    cout << " 4) Round Robin\n";
    cout << "Selection: ";

    int choice = 1;
    cin >> choice;
    if (choice < 1 || choice > 4) {
        cout << "Invalid choice. Defaulting to FCFS.\n";
        choice = 1;
    }

    return static_cast<Algorithm>(choice);
}

int Kernel::chooseTimeQuantum(Algorithm algo) const {
    if (algo != Algorithm::RoundRobin) {
        return 0;
    }

    int timeQuantum = 2;
    cout << "Time quantum (>=1): ";
    cin >> timeQuantum;
    if (timeQuantum <= 0) {
        cout << "Invalid quantum. Using default = 2.\n";
        timeQuantum = 2;
    }

    return timeQuantum;
}

void Kernel::printTimeline(const vector<ExecutionStep>& timeline) const {
    cout << "\n--- CPU Timeline ---\n";
    for (const auto& step : timeline) {
        if (step.idle) {
            cout << "Time " << step.time << ": CPU idle\n";
        } else {
            cout << "Time " << step.time << ": Running P" << step.pid << "\n";
        }
    }
}

void Kernel::printTable(const SimulationResult& result) const {
    cout << "\n------ Results ------\n";
    cout << left << setw(6) << "PID"
         << setw(10) << "Arrival"
         << setw(8) << "Burst"
         << setw(8) << "Start"
         << setw(8) << "Finish"
         << setw(12) << "Turnaround"
         << setw(10) << "Waiting"
         << setw(10) << "Response"
         << "\n";

    for (const auto& s : result.finalStates) {
        int turnaround = s.finish - s.spec.arrival;
        int waiting = turnaround - s.spec.burst;
        int response = s.start - s.spec.arrival;

        cout << left << setw(6) << ("P" + std::to_string(s.spec.pid))
             << setw(10) << s.spec.arrival
             << setw(8) << s.spec.burst
             << setw(8) << s.start
             << setw(8) << s.finish
             << setw(12) << turnaround
             << setw(10) << waiting
             << setw(10) << response
             << "\n";
    }

    cout << "\nAverages: Turnaround = " << std::fixed << std::setprecision(2) << result.summary.avgTurnaround
         << ", Waiting = " << result.summary.avgWaiting
         << ", Response = " << result.summary.avgResponse
         << ", CPU Utilization = " << result.summary.cpuUtilization << "%\n";
}

void Kernel::run() {
    cout << "CPU Scheduling Simulator\n";
    cout << "------------------------\n";

    auto processes = readProcesses();
    if (processes.empty()) {
        cout << "No processes to schedule.\n";
        return;
    }

    auto algo = chooseAlgorithm();
    int timeQuantum = chooseTimeQuantum(algo);

    Scheduler scheduler;
    auto result = scheduler.simulate(processes, algo, timeQuantum);

    cout << "\n--- Simulation (" << Scheduler::algorithmName(algo) << ") ---\n";
    printTimeline(result.timeline);
    printTable(result);
}
