# Solution of Question 5: Evaluation of vi/vim File Recovery Mechanisms

<details>
<summary>
<h3 style="display: inline;"> Action 1: Create baseline configuration file and simulate system crash during editing </h3>
</summary>
<br>

**Command:**

```bash
echo -e "PORT=8080\nHOST=127.0.0.1" > config.sys && python3 -c "import pty, os, time; pid, fd = pty.fork(); os.execvp('vim', ['vim', '-u', 'NONE', '-c', 'set swapfile', '-c', 'normal oMAX_CONN=500', '-c', 'preserve', 'config.sys']) if pid == 0 else (time.sleep(1), os.kill(pid, 9))"
```
<br>

**Expected Output:**

```text
(Silent execution on success)
```
<br>

**Explanation:**  
This command creates `config.sys` and spawns `vim` in a pseudo-terminal. It enables swap files, appends an unsaved change `(MAX_CONN=500)`, executes `:preserve` to write buffer states to disk, and immediately sends a `SIGKILL (kill -9)` to simulate an abrupt system crash before Vim can perform exit cleanup.
<br>

**Screenshot:**  

![Command Execution & Output](https://github.com/VinayNoogler000/CLIS_SGA_2/blob/main/Question5/Screenshots/Action%201.png?raw=true)

</details>




<br/> <br/>
<details>
<summary>
<h3 style="display: inline;">Action 2: Recover the unsaved buffer from the swap file</h3>
</summary>
<br>

**Command:**

```bash
vim -r config.sys -c "w" -c "q!" 2>/dev/null && cat config.sys
```
<br>

**Expected Output:**

```text
PORT=8080
HOST=127.0.0.1
MAX_CONN=500
```
<br>

**Explanation:**  

This command triggers Vim's swap file recovery mechanism using `vim -r config.sys`. It recovers the unsaved edits (`MAX_CONN=500`) directly from the hidden `.config.sys.swp` file, saves the recovered buffer state back to `config.sys`, and prints the file content to confirm that no data was lost.
<br>

**Screenshot:**  

![Command Execution & Output](https://github.com/VinayNoogler000/CLIS_SGA_2/blob/main/Question5/Screenshots/Action%202.png?raw=true)

</details>




<br/> <br/>
<details>
<summary>
<h3 style="display: inline;">Action 3: Safely clean up the orphan swap file</h3>
</summary>
<br>

**Command:**

```bash
rm -f .config.sys.swp && ls -la .config.sys.swp 2>&1
```
<br>

**Expected Output:**

```text
ls: cannot access '.config.sys.swp': No such file or directory
```
<br>

**Explanation:**  
After successfully recovering and saving the file contents in Action 2, the orphan `.config.sys.swp` swap file must be deleted manually. Removing it prevents Vim from throwing recurring swap file warnings whenever `config.sys` is opened in the future.
<br>

**Screenshot:**  

![Command Execution & Output](https://github.com/VinayNoogler000/CLIS_SGA_2/blob/main/Question5/Screenshots/Action%203.png?raw=true)

</details>




<br/> <br/>
## Technical Justification and Conceptual Explanation
<details>
<summary>
<h3 style="display: inline;"> Evaluation of vi/vim Recovery Mechanisms </h3>
</summary>
<br>

| Mechanism | Strengths | Limitations / Behavior During Crash |
| :--- | :--- | :--- |
| **Swap Files (`.swp`)** | Continuously logs unsaved buffer deltas directly to disk during editing (`set swapfile`). | Primary recovery mechanism. Captures unwritten edits even if process terminates abruptly. |
| **Undo History (`u`)** | Enables granular step-by-step edit reversals during an active session. | Entire in-memory undo tree is lost on crash unless `set undofile` (persistent undo) is explicitly enabled prior to crash. |
| **Registers** | Holds cut, copied, or deleted text selections in named memory buffers. | In-memory data is completely wiped out during a hard crash unless preserved via `.viminfo` / `.shada`. |
| **Backup Files** | Retains a copy of the original file prior to overwriting (`set backup`). | Only created at the moment of explicit file saving (`:w`). Useless if system crashes *before* saving. |
| **Auto-Recovery (`vim -r`)** | Reads and reconstructs the last known buffer state using the swap file. | Requires manual deletion of orphan `.swp` file post-recovery to suppress warning prompts. |

</details>

<br>

<details>
<summary>
<h3 style="display: inline;"> Most Reliable Recovery Strategy & Justification </h3>
</summary>
<br>

**Proposed Strategy:**  
Combine Swap File Auto-Recovery (vim -r) with Persistent Undo (set undofile) and periodic buffer flushes (:preserve).

**Justification:**
1. **Uncommitted Data Protection:** Standard backups and undo trees only save state upon writing to disk. Swap files (`.swp`) capture in-flight, uncommitted modifications directly from kernel buffers into permanent storage.

2. **Post-Crash Reconstruction**: `vim -r <filename>` reconstructs the exact memory state of the buffer right before the crash occurred.

3. **Completeness:** Coupling swap recovery with persistent undo ensures that not only are unsaved edits restored, but the full editing history remains intact after crash recovery.

</details>