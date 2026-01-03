



# OperationSystemSimulator

A small console simulator for classic CPU scheduling algorithms. It supports:
- First-Come First-Served (FCFS)
- Shortest Job First (non-preemptive)
- Shortest Remaining Time First (preemptive SJF)
- Round Robin with a configurable time quantum

Run the built executable and provide:
1. The number of processes.
2. Arrival time and CPU burst for each process.
3. The scheduling algorithm (and quantum for Round Robin).

The simulator prints a step-by-step CPU timeline plus per-process metrics (turnaround, waiting, response) and average statistics including CPU utilization.

Structure:
- `Scheduler` handles the algorithm implementations and metrics.
- `Kernel` gathers input, invokes the scheduler, and prints the timeline/table.
