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
