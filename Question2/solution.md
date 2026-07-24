# Solution of Question 2: Process Creation, Zombie Prevention, and Signal Handling

<details>
<summary>
<h3 style="display: inline;">Action 1: Create the process monitor source file (process_monitor.c)</h3>
</summary>

<br>

**Command:**

```bash
cat << 'EOF' > process_monitor.c
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
EOF
```
<br>

**Expected Output:**

```text
(Silent execution on success)
```

<br>

**Explanation:**  

This command creates the C source file `process_monitor.c`. The program installs a `SIGCHLD` signal handler with `waitpid(-1, NULL, WNOHANG)` to reap finished children asynchronously and prevent zombies, creates worker children via `fork()`, and uses `kill()` to send `SIGTERM/SIGKILL` signals to terminate unresponsive processes.


<br>

**Screenshot:**  

![Command Execution & Output]()

</details>



<br/> <br/>
<details>
<summary>
<h3 style="display: inline;">Action 2: Compile the process monitor C program</h3>
</summary>

<br>

**Command:**

```bash
gcc -Wall -o process_monitor process_monitor.c
```
<br>

**Expected Output:**

```text
(Silent execution on success)
```

<br>

**Explanation:**  

This command compiles `process_monitor.c` using `gc`c` with the `-Wall` flag enabled to inspect for any compiler warnings. I observed that the program compiled cleanly without any errors or warnings, producing the executable binary `process_monitor`.

<br>

**Screenshot:**  

![Command Execution & Output]()

</details>




<br/> <br/>
<details>
<summary>
<h3 style="display: inline;">Action 3: Execute the process monitor and observe process management</h3>
</summary>

<br>

**Command:**

```bash
./process_monitor
```
<br>

**Expected Output:**

```text
[Parent PID: 5432] Starting Web Server Process Monitor...
[Child 1 PID: 5433] Normal worker started execution.
[Child 2 PID: 5434] Unresponsive worker started (entering infinite loop)...
[Child 1 PID: 5433] Finished task normally. Exiting.
[Parent] Monitoring child process PID 5434...
[Parent] Child PID 5434 is unresponsive. Sending SIGTERM signal...
[Parent] Child PID 5434 successfully terminated.
[Parent] All child processes monitored and cleaned up. Exiting.
```
(Note: Process IDs (PIDs) like 5432, 5433, and 5434 will vary dynamically on execution).

<br>

**Explanation:**  

This command runs the compiled program. I observed the parent process spawn two children. Child 1 completed and was asynchronously reaped by the `SIGCHLD` handler (preventing a zombie state), while the parent successfully identified Child 2's unresponsive loop and terminated it via a `SIGTERM` signal.

<br>

**Screenshot:**  

![Command Execution & Output]()

</details>




<br/> <br/>
<details>
<summary>
<h3 style="display: inline;">Action 4: Verify zombie process prevention and process cleanup</h3>
</summary>

<br>

**Command:**

```bash
ps aux | grep '[p]rocess_monitor'
```
<br>

**Expected Output:**
```text
(No output returned, confirming all parent and child processes terminated cleanly)
```

<br>

**Explanation:**  

This command searches the system process table using `ps` and `grep` with single-quoted bracket notation `'[p]rocess_monitor'` to prevent shell filename globbing. The empty output confirms that neither zombie processes (`<defunct>`) nor orphaned child processes remain active on the system.

<br>

**Screenshot:**  

![Command Execution & Output]()

</details>




<br/> <br/>
## Technical Justification and Conceptual Explanation
<details>
<summary>
<h3 style="display: inline;"> How Process Creation, Waiting, and Signal Handling Work Together ? </h3>
</summary>

#### 1. &nbsp; Process Creation via `fork()`:

* The parent process spawns worker child processes using fork(). Each child process receives a duplicate copy of the parent's address space and executes concurrently.

* fork() returns 0 in the child process and the child's Process ID (PID) in the parent process, allowing differentiated execution logic.

#### 2. &nbsp; Asynchronous Waiting and Zombie Prevention via SIGCHLD & waitpid():

* When a child process terminates, Unix keeps its exit status in the process table until the parent collects it. If uncollected, the process becomes a "zombie" (`<defunct>`).

* To prevent zombies without forcing the parent process to block and wait synchronously, a signal handler for `SIGCHLD` is installed via `sigaction()`.

* Inside the handler, `waitpid(-1, NULL, WNOHANG)` is called in a `while` loop. The `-1` argument targets any terminated child process, and `WNOHANG` ensures non-blocking execution, immediately reaping terminated children.

#### 3. &nbsp; Monitoring and Terminating Unresponsive Processes via Signals:

*  The parent checks process vitality using `kill(pid, 0)`, which tests process existence without sending an actual interrupting signal.

* If a worker becomes unresponsive or caught in an unrecoverable state, the parent issues a `SIGTERM` (Signal 15) to request a graceful shutdown.

* If the process fails to stop after a timeout, the parent escalates to `SIGKILL` (Signal 9), which cannot be caught, blocked, or ignored, ensuring process removal and resource retrieval.


</details>