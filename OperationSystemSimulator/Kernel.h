#pragma once

#include "Scheduler.h"

class Kernel {
public:
    void run();

private:
    std::vector<Process> readProcesses();
    Algorithm chooseAlgorithm() const;
    int chooseTimeQuantum(Algorithm algo) const;

    void printTimeline(const std::vector<ExecutionStep>& timeline) const;
    void printTable(const SimulationResult& result) const;
};
