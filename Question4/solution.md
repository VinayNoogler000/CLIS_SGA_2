# Solution of Question 4: Real-Time Log Monitoring and Filtering Pipeline

<details>
<summary>
<h3 style="display: inline;"> Action 1: Create the target log file and initialize baseline entries </h3>
</summary>
<br>

**Command:**

```bash
touch sys.log && echo "[INFO] 2026-07-24 10:00:01 - System booted successfully" >> sys.log && echo "[ERROR] 2026-07-24 10:00:05 - Database connection failed" >> sys.log
```
<br>

**Expected Output:**

```text
(Silent execution on success)
```
<br>

**Explanation:**  
This command creates an active log file named `sys.log` and populates it with baseline sample log entries containing both informational (`[INFO]`) and error (`[ERROR]`) events to prepare for testing real-time pipeline monitoring.
<br>

**Screenshot:**  

![Command Execution & Output](https://github.com/VinayNoogler000/CLIS_SGA_2/blob/main/Question4/Screenshots/Action%201.png?raw=true)

</details>




<br/> <br/>
<details>
<summary>
<h3 style="display: inline;">Action 2: Start real-time log monitoring pipeline and simulate incoming events</h3>
</summary>
<br>

**Command:**

```bash
tail -f -n 0 sys.log 2>/dev/null | grep --line-buffered "ERROR" | tee -a error_report.log & PID=$! && sleep 1 && echo "[INFO] 2026-07-24 10:01:00 - User login success" >> sys.log && echo "[ERROR] 2026-07-24 10:01:05 - Disk quota exceeded" >> sys.log && echo "[WARNING] 2026-07-24 10:01:10 - High memory usage" >> sys.log && echo "[ERROR] 2026-07-24 10:01:15 - Service crashed" >> sys.log && sleep 1 && kill $PID
```
<br>

**Expected Output:**

```text
[ERROR] 2026-07-24 10:01:05 - Disk quota exceeded
[ERROR] 2026-07-24 10:01:15 - Service crashed
```
<br>

**Explanation:**  
This command executes the monitoring pipeline in the background using `tail -f -n 0` to track new additions in real time while redirecting `stderr` to `/dev/null`. It pipes raw logs to `grep --line-buffered "ERROR"` to extract errors immediately without I/O buffering delays, and outputs them to standard output while appending them to `error_report.log` using tee `-a`.
<br>

**Screenshot:**  

![Command Execution & Output](https://github.com/VinayNoogler000/CLIS_SGA_2/blob/main/Question4/Screenshots/Action%202.png?raw=true)

</details>




<br/> <br/>
<details>
<summary>
<h3 style="display: inline;">Action 3: Verify extracted error log report file</h3>
</summary>
<br>

**Command:**

```bash
cat error_report.log
```
<br>

**Expected Output:**

```text
[ERROR] 2026-07-24 10:01:05 - Disk quota exceeded
[ERROR] 2026-07-24 10:01:15 - Service crashed
```
<br>

**Explanation:**  
This command inspects `error_report.log` to verify that the background monitoring pipeline filtered incoming log entries accurately, maintaining only `[ERROR]` level messages in the separate report file.
<br>

**Screenshot:**  

![Command Execution & Output](https://github.com/VinayNoogler000/CLIS_SGA_2/blob/main/Question4/Screenshots/Action%203.png?raw=true)

</details>


<br/> <br/>
## Technical Justification and Conceptual Explanation:
<details>
<summary>
<h3 style="display: inline;"> Pipeline Architecture and Component Efficiency </h3>
</summary>
<br>

1. &nbsp; `tail -f -n 0`:

    * Actively streams newly appended lines in real time rather than re-reading the full log file continuously.

    * The `-n 0` flag skips existing historical entries, ensuring only fresh events are passed into the pipeline.

2. &nbsp; Pipes (`|`):

    * Connects stdout of one process directly to stdin of the next using kernel memory buffers.

    * Eliminates the overhead of writing temporary disk files between filtering stages, saving I/O cycles and memory.

3. &nbsp; `grep --line-buffered "ERROR"`:
    
    * Filters input to extract only lines containing `"ERROR"`.

    * The `--line-buffered` flag forces `grep` to flush its buffer immediately after every matched line, preventing delays in real-time monitoring.

4. &nbsp; Redirection & `tee -a error_report.log`:

    * `tee -a` duplicates the filtered output, displaying errors live on the screen (`stdout`) while simultaneously appending (`-a`) them to `error_report.log`.

5. &nbsp; `2>/dev/null`:

    * Redirects standard error (`stderr`, file descriptor 2) to the `/dev/null` bit bucket.

    * Discards unwanted warning or error messages (e.g., file rotation notices or permission warnings), maintaining a clean, clean terminal view.

</details>