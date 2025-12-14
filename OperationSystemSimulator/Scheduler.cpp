#include "Scheduler.h"

#include <algorithm>
#include <numeric>

using std::string;
using std::vector;

string Scheduler::algorithmName(Algorithm algo) {
    switch (algo) {
    case Algorithm::FCFS:
        return "First-Come First-Served";
    case Algorithm::SJF:
        return "Shortest Job First (non-preemptive)";
    case Algorithm::SRTF:
        return "Shortest Remaining Time First (preemptive)";
    case Algorithm::RoundRobin:
        return "Round Robin";
    default:
        return "Unknown";
    }
}

int Scheduler::takeProcess(vector<int>& candidates, const vector<ProcessState>& states, bool useRemaining) {
    if (candidates.empty()) {
        return -1;
    }

    size_t bestPos = 0;
    for (size_t i = 1; i < candidates.size(); ++i) {
        int currentIdx = candidates[i];
        int bestIdx = candidates[bestPos];

        int currentMetric = useRemaining ? states[currentIdx].remaining : states[currentIdx].spec.burst;
        int bestMetric = useRemaining ? states[bestIdx].remaining : states[bestIdx].spec.burst;

        if (currentMetric < bestMetric || (currentMetric == bestMetric && states[currentIdx].spec.pid < states[bestIdx].spec.pid)) {
            bestPos = i;
        }
    }

    int chosen = candidates[bestPos];
    candidates.erase(candidates.begin() + static_cast<long long>(bestPos));
    return chosen;
}

SimulationResult Scheduler::simulate(const vector<Process>& processes, Algorithm algo, int timeQuantum) const {
    vector<ProcessState> states;
    states.reserve(processes.size());
    for (const auto& p : processes) {
        states.push_back({p, p.burst, -1, -1});
    }

    vector<ExecutionStep> timeline;
    vector<int> ready;

    int time = 0;
    int finished = 0;
    int running = -1;
    int quantumRemaining = timeQuantum;

    auto enqueueArrivals = [&](int currentTime) {
        for (size_t i = 0; i < states.size(); ++i) {
            if (states[i].spec.arrival == currentTime) {
                ready.push_back(static_cast<int>(i));
            }
        }
    };

    while (finished < static_cast<int>(states.size())) {
        enqueueArrivals(time);

        switch (algo) {
        case Algorithm::FCFS:
            if (running == -1 && !ready.empty()) {
                running = ready.front();
                ready.erase(ready.begin());
                if (states[running].start == -1) {
                    states[running].start = time;
                }
            }
            break;
        case Algorithm::SJF:
            if (running == -1 && !ready.empty()) {
                running = takeProcess(ready, states, false);
                if (states[running].start == -1) {
                    states[running].start = time;
                }
            }
            break;
        case Algorithm::SRTF: {
            int candidate = running;
            if (!ready.empty()) {
                int smallestFromReady = takeProcess(ready, states, true);
                if (candidate == -1 || states[smallestFromReady].remaining < states[candidate].remaining) {
                    if (candidate != -1) {
                        ready.push_back(candidate);
                    }
                    candidate = smallestFromReady;
                } else {
                    ready.push_back(smallestFromReady);
                }
            }
            if (candidate != -1 && candidate != running) {
                if (states[candidate].start == -1) {
                    states[candidate].start = time;
                }
                running = candidate;
            } else if (running == -1 && !ready.empty()) {
                running = takeProcess(ready, states, true);
                if (states[running].start == -1) {
                    states[running].start = time;
                }
            }
            break;
        }
        case Algorithm::RoundRobin:
            if (running == -1 && !ready.empty()) {
                running = ready.front();
                ready.erase(ready.begin());
                quantumRemaining = timeQuantum;
                if (states[running].start == -1) {
                    states[running].start = time;
                }
            }
            break;
        }

        if (running == -1) {
            timeline.push_back({time, -1, true});
            ++time;
            continue;
        }

        timeline.push_back({time, states[running].spec.pid, false});
        --states[running].remaining;

        if (algo == Algorithm::RoundRobin) {
            --quantumRemaining;
        }

        if (states[running].remaining == 0) {
            states[running].finish = time + 1;
            ++finished;
            running = -1;
            quantumRemaining = timeQuantum;
        } else if (algo == Algorithm::RoundRobin && quantumRemaining == 0) {
            ready.push_back(running);
            running = -1;
            quantumRemaining = timeQuantum;
        }

        ++time;
    }

    double totalTurnaround = 0.0;
    double totalWaiting = 0.0;
    double totalResponse = 0.0;
    int busySteps = 0;

    for (const auto& s : states) {
        int turnaround = s.finish - s.spec.arrival;
        int waiting = turnaround - s.spec.burst;
        int response = s.start - s.spec.arrival;

        totalTurnaround += turnaround;
        totalWaiting += waiting;
        totalResponse += response;
    }

    for (const auto& step : timeline) {
        if (!step.idle) {
            ++busySteps;
        }
    }

    int totalTime = timeline.empty() ? 0 : timeline.back().time + 1;
    Summary summary{
        totalTurnaround / states.size(),
        totalWaiting / states.size(),
        totalResponse / states.size(),
        totalTime == 0 ? 0.0 : (static_cast<double>(busySteps) / static_cast<double>(totalTime)) * 100.0};

    return {states, timeline, summary};
}
