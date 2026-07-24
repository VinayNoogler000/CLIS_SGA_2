#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

// Signal handler to reap child processes and prevent zombies
void handle_sigchld(int sig) {
    int saved_errno = errno;
    // Reap all pending terminated children asynchronously
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

int main() {
    // Configure SIGCHLD signal handler
    struct sigaction sa;
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    printf("[Parent PID: %d] Starting Web Server Process Monitor...\n", getpid());

    // 1. Create a responsive child worker
    pid_t pid1 = fork();
    if (pid1 == 0) {
        printf("[Child 1 PID: %d] Normal worker started execution.\n", getpid());
        sleep(1);
        printf("[Child 1 PID: %d] Finished task normally. Exiting.\n", getpid());
        exit(0);
    }

    // 2. Create an unresponsive (hung) child worker
    pid_t pid2 = fork();
    if (pid2 == 0) {
        printf("[Child 2 PID: %d] Unresponsive worker started (entering infinite loop)...\n", getpid());
        while (1) {
            sleep(1); // Simulates hung/unresponsive state
        }
    }

    // Allow time for execution monitoring
    sleep(3);

    // 3. Monitor and terminate unresponsive child (Child 2)
    printf("[Parent] Monitoring child process PID %d...\n", pid2);
    if (kill(pid2, 0) == 0) {
        printf("[Parent] Child PID %d is unresponsive. Sending SIGTERM signal...\n", pid2);
        kill(pid2, SIGTERM);
        sleep(1);
        
        // If child is still alive, send SIGKILL
        if (kill(pid2, 0) == 0) {
            printf("[Parent] Child PID %d ignored SIGTERM. Sending SIGKILL...\n", pid2);
            kill(pid2, SIGKILL);
        }
        printf("[Parent] Child PID %d successfully terminated.\n", pid2);
    }

    sleep(1);
    printf("[Parent] All child processes monitored and cleaned up. Exiting.\n");
    return 0;
}
