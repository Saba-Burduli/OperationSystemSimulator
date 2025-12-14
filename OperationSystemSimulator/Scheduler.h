#pragma once

#include <string>
#include <vector>

enum class Algorithm {
    FCFS = 1,
    SJF = 2,
    SRTF = 3,
    RoundRobin = 4
};

struct Process {
    int pid;
    int arrival;
    int burst;
};

struct ProcessState {
    Process spec;
    int remaining;
    int start;
    int finish;
};

struct ExecutionStep {
    int time;
    int pid;
    bool idle;
};

struct Summary {
    double avgTurnaround;
    double avgWaiting;
    double avgResponse;
    double cpuUtilization;
};

struct SimulationResult {
    std::vector<ProcessState> finalStates;
    std::vector<ExecutionStep> timeline;
    Summary summary;
};

class Scheduler final {
public:
    SimulationResult simulate(const std::vector<Process>& processes, Algorithm algo, int timeQuantum) const;
    static std::string algorithmName(Algorithm algo);

private:
    static int takeProcess(std::vector<int>& candidates, const std::vector<ProcessState>& states, bool useRemaining);
};
