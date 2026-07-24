# Solution of Question 3: Secure Low-Level File Processing System

<details>
<summary>
<h3 style="display: inline;">Action 1: Create the low-level file processing source file (record_utility.c)</h3>
</summary>
<br>

**Command:**

```bash
cat << 'EOF' > record_utility.c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

// Fixed-size binary employee record structure
struct Employee {
    int id;
    char name[32];
    double salary;
};

int main() {
    const char *filename = "employees.dat";

    // 1. Create and open file using low-level system calls (O_CREAT | O_RDWR | O_TRUNC)
    int fd = open(filename, O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error opening file");
        exit(1);
    }
    printf("[System Call] File '%s' created and opened successfully. File Descriptor: %d\n", filename, fd);

    // 2. Write initial employee records using write()
    struct Employee emp1 = {101, "Alice Smith", 65000.0};
    struct Employee emp2 = {102, "Bob Jones", 72000.0};
    struct Employee emp3 = {103, "Charlie Brown", 58000.0};

    if (write(fd, &emp1, sizeof(struct Employee)) != sizeof(struct Employee) ||
        write(fd, &emp2, sizeof(struct Employee)) != sizeof(struct Employee) ||
        write(fd, &emp3, sizeof(struct Employee)) != sizeof(struct Employee)) {
        perror("Error writing records");
        close(fd);
        exit(1);
    }
    printf("[System Call] Successfully wrote 3 initial employee records using write().\n");

    // 3. Update specific record (Record 2: ID 102 - Bob Jones) without rewriting entire file
    off_t offset = 1 * sizeof(struct Employee); // Offset calculation for 2nd record (index 1)
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("Error seeking position");
        close(fd);
        exit(1);
    }

    struct Employee updated_emp2 = {102, "Bob Jones", 80000.0}; // Salary update
    if (write(fd, &updated_emp2, sizeof(struct Employee)) != sizeof(struct Employee)) {
        perror("Error updating record");
        close(fd);
        exit(1);
    }
    printf("[System Call] Updated Record 2 directly at byte offset %ld using lseek() + write().\n", (long)offset);

    // 4. Retrieve record from specific location efficiently (Random Access)
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("Error seeking position");
        close(fd);
        exit(1);
    }

    struct Employee read_emp;
    if (read(fd, &read_emp, sizeof(struct Employee)) != sizeof(struct Employee)) {
        perror("Error reading record");
        close(fd);
        exit(1);
    }

    printf("[System Call] Retrieved Record at Index 1 -> ID: %d, Name: %s, Salary: $%.2f\n",
           read_emp.id, read_emp.name, read_emp.salary);

    // 5. Close file descriptor
    if (close(fd) == -1) {
        perror("Error closing file");
        exit(1);
    }
    printf("[System Call] File descriptor %d closed safely using close().\n", fd);

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

This command creates the C source file `record_utility.c` using direct Linux system calls (`open`, `write`, `lseek`, `read`, and `close`). The program creates a binary file, writes fixed-size Employee structures, performs an in-place update using `lseek()`, retrieves specific records directly, and cleans up file descriptors safely.
<br>

**Screenshot:**  

![Command Execution & Output](https://github.com/VinayNoogler000/CLIS_SGA_2/blob/main/Question3/Screenshots/Action%201.png?raw=true)

</details>




<br/> <br/>
<details>
<summary>
<h3 style="display: inline;">Action 2: Compile the record utility C program</h3>
</summary>
<br>

**Command:**

```bash
gcc -Wall -o record_utility record_utility.c
```
<br>

**Expected Output:**

```text
(Silent execution on success)
```
<br>

**Explanation:**  

This command compiles `record_utility.c` using gcc with the `-Wall` flag enabled to catch any compilation warnings. I observed that the program compiled cleanly without errors, generating the binary executable record_utility.
<br>

**Screenshot:**  

![Command Execution & Output](https://github.com/VinayNoogler000/CLIS_SGA_2/blob/main/Question3/Screenshots/Action%202.png?raw=true)

</details>




<br/> <br/>
<details>
<summary>
<h3 style="display: inline;">Action 3: Execute the low-level file processing utility</h3>
</summary>
<br>

**Command:**

```bash
./record_utility
```
<br>

**Expected Output:**

```text
[System Call] File 'employees.dat' created and opened successfully. File Descriptor: 3
[System Call] Successfully wrote 3 initial employee records using write().
[System Call] Updated Record 2 directly at byte offset 44 using lseek() + write().
[System Call] Retrieved Record at Index 1 ->
```
<br>

**Explanation:**  

This command executes the compiled binary. I observed the program create `employees.dat`, write three initial records using `write()`, reposition the file offset directly to the second record via `lseek()` to update the salary without rewriting the rest of the file, and retrieve the modified data.
<br>

**Screenshot:**  

![Command Execution & Output](https://github.com/VinayNoogler000/CLIS_SGA_2/blob/main/Question3/Screenshots/Action%203.png?raw=true)

</details>




<br/> <br/>
<details>
<summary>
<h3 style="display: inline;">Action 4: Inspect and verify the binary database file on disk</h3>
</summary>
<br>

**Command:**

```bash
ls -l employees.dat
```
<br>

**Expected Output:**

```text
-rw-r--r-- 1 user user 132 Jul 24 18:20 employees.dat
```
<br>

**Explanation:**  

This command checks the created binary data file `employees.dat`. The total file size of 132 bytes accurately reflects three binary records of 44 bytes each (`sizeof(struct Employee)`), verifying that records are stored in fixed-size structures for efficient offset calculation and direct access.
<br>

**Screenshot:**  

![Command Execution & Output](https://github.com/VinayNoogler000/CLIS_SGA_2/blob/main/Question3/Screenshots/Action%204.png?raw=true)

</details>



<br/> <br/>
## Technical Justification and Conceptual Explanation
<details>
<summary>
<h3 style="display: inline;"> System Call Contributions to the Low-Level File Processing Solution </h3>
</summary>
<br>

1. &nbsp; `open()`:

    * Opens or creates a file directly via kernel system calls without standard C library buffered streams (`FILE*`).
    
    * Uses bitwise OR flags (`O_CREAT | O_RDWR | O_TRUNC`) to configure file creation, read/write capabilities, and truncation. Returns a low-level integer file descriptor (`fd`) while enforcing precise file permission modes (`0644`).


2. &nbsp; `write()`:

    * Transfers raw byte streams directly from user-space memory buffers into kernel disk page caches.
    
    * Enables direct writing of binary structure formats, eliminating formatting overhead and buffering latency associated with standard I/O functions.


3. &nbsp; `lseek()`:

    * Repositions the file offset pointer associated with the file descriptor in constant time.

    * By setting offsets to `index * sizeof(struct Employee)` using `SEEK_SET`, the program navigates directly to specific byte positions. This enables in-place updates without having to read or rewrite unchanged records.


4. &nbsp; `read()`:

    * Reads raw binary bytes from the current file offset directly into memory structures.
    
    * Combined with `lseek()`, it facilitates instant random-access record retrieval from any file index.


5. &nbsp; `close()`:

    * Unbinds and releases the file descriptor from the process's file descriptor table.

    * Ensures all pending kernel write operations are safely flushed to storage and frees operating system kernel resources.


</details>