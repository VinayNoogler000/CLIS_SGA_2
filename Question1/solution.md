# Solution of Question 1: Student Submission Deduplication and Backup System

<details>
<summary>
<h3 style="display: inline;">Action 1: Setup directory structure and sample files</h3>
</summary>
<br>

**Command:**
```bash
mkdir -p submissions && echo "Assignment 1 code" > submissions/student1.py && echo "Assignment 1 code" > submissions/student2.py && echo "Unique code solution" > submissions/student3.py
```
<br>

**Expected Output:**
```text
(Silent execution on success)
```
<br>

**Explanation:**  

This command creates the submissions directory and populates it with sample submission files. `student1.py` and `student2.py` share identical content to test duplicate detection, while `student3.py` contains unique code.
<br>

**Screenshot:**  

![Command Execution & Output](https://github.com/VinayNoogler000/CLIS_SGA_2/blob/main/Question1/Screenshots/Action%201.png?raw=true)

</details>




<br/> <br/>
<details>
<summary>
<h3 style="display: inline;">Action 2: Make the submission processing script executable</h3>
</summary>
<br>

**Command:**

```bash
chmod +x process_submissions.sh
```
<br>

**Expected Output:**

```text
(Silent execution on success)
```
<br>

**Explanation:**  

This command uses `chmod +x` to grant execution permissions to the shell script `process_submissions.sh` I observed that the file mode updated successfully to allow execution.
<br>

**Screenshot:**  

![Command Execution & Output](https://github.com/VinayNoogler000/CLIS_SGA_2/blob/main/Question1/Screenshots/Action%202.png?raw=true)

</details>




<br/> <br/>
<details>
<summary>
<h3 style="display: inline;">Action 3: Execute the submission processing script</h3>
</summary>
<br>

**Command:**

```bash
./process_submissions.sh
```
<br>

**Expected Output:**

```text
[BACKUP] student1.py successfully backed up.
[DUPLICATE] student2.py matches content of student1.py
[BACKUP] student3.py successfully backed up.
==========================================
     SUBMISSION PROCESSING REPORT        
==========================================
Total files processed : 3
Duplicate files found : 1
Unique files backed up: 2
==========================================
```
<br>

**Explanation:**  

This command runs the shell script which calculates MD5 hashes to identify `student2.py` as a duplicate of `student1.py`. It copies unique submissions to `./backup` and prints the final report.
<br>

**Screenshot:**  

![Command Execution & Output](https://github.com/VinayNoogler000/CLIS_SGA_2/blob/main/Question1/Screenshots/Action%203.png?raw=true)

</details>




<br/> <br/>
<details>
<summary>
<h3 style="display: inline;">Action 4: Verify backed-up unique files</h3>
</summary>
<br>

**Command:**

```bash
ls -l backup/
```
<br>

**Expected Output:**

```text
total 8
-rw-r--r-- 1 user user 18 Jul 24 16:40 student1.py
-rw-r--r-- 1 user user 21 Jul 24 16:40 student3.py
```
<br>

**Explanation:**  

This command lists the contents of the `./backup directory`. I verified that only the unique files (`student1.py` and `student3.py`) were copied over while the duplicate was excluded.
<br>

**Screenshot:**  

![Command Execution & Output](https://github.com/VinayNoogler000/CLIS_SGA_2/blob/main/Question1/Screenshots/Action%204.png?raw=true)

</details>




<br/> <br/>
<details>
<summary>
<h3 style="display: inline;">Action 5: Inspect generated report and error log files</h3>
</summary>
<br>

**Command:**

```bash
cat report.txt && cat error.log
```
<br>

**Expected Output:**

```text
==========================================
     SUBMISSION PROCESSING REPORT        
==========================================
Total files processed : 3
Duplicate files found : 1
Unique files backed up: 2
==========================================
```
<br>

**Explanation:** 

This command displays the contents of `report.txt` and `error.log`. I verified that summary metrics were saved in `report.txt` and that `error.log` remained empty due to zero runtime errors.
<br>

**Screenshot:**  

![Command Execution & Output](https://github.com/VinayNoogler000/CLIS_SGA_2/blob/main/Question1/Screenshots/Action%205.png?raw=true)

</details>




<br/> <br/>
## Technical Justification
<details>
<summary>
<h3 style="display: inline;"> Linux Commands Justification </h3>
</summary>

* `md5sum`: Generates a 128-bit cryptographic hash for each file. Hashing file content guarantees exact duplicate detection based on actual data rather than filenames or file sizes.  

* `declare -A seen_hashes`: Instantiates a Bash associative array, enabling $O(1)$ constant-time lookup to check if a file hash has been encountered previously.

* `mkdir -p`: Safely creates target directories if they do not already exist without throwing errors.

* `cp`: Copies verified unique files into the designated backup folder.

* `tee`: Writes the final statistical summary simultaneously to stdout and report.txt.

</details>  

<br>

<details>
<summary>
<h3 style="display: inline;"> Redirection Operators Justification </h3>
</summary>

* `2>> "$ERROR_LOG"`: Redirects Standard Error (file descriptor 2) in append mode to error.log, isolating operational errors from normal console output.  

* `exec 2>> ...`: Permanently redirects stderr for all subsequent commands executed within the script to the log file.  

* `> "$FILE"`: Truncates target files prior to execution to clear residual data from previous script runs.  

* `>&2`: Directs custom script error messages directly to Standard Error.  


</details>

<br>

<details>
<summary>
<h3 style="display: inline;"> File-Handling Techniques Justification </h3>
</summary>

* **Content-Based Deduplication**: Employs cryptographic hashes instead of names or timestamps to eliminate false positives during duplicate identification.

* **Isolated Logging**: Separates diagnostic error tracking (error.log) from operational metrics (report.txt) for streamlined auditing.

* **Defensive File Checking**: Uses `[ -f "$file" ]` to filter out non-regular files and subdirectories, preventing script crashes.

</details>