#!/bin/bash

# Define working directory paths and log files
SUBMISSIONS_DIR="./submissions"
BACKUP_DIR="./backup"
REPORT_FILE="report.txt"
ERROR_LOG="error.log"

# Clear previous error log and report files if they exist
> "$ERROR_LOG"
> "$REPORT_FILE"

# Redirect all Standard Error (stderr) output to the error log file
exec 2>> "$ERROR_LOG"

# Create backup directory if it does not already exist
mkdir -p "$BACKUP_DIR"

# Initialize counters for reporting
processed_count=0
duplicate_count=0
backup_count=0

# Declare an associative array to store unique file hashes
declare -A seen_hashes

# Verify that the submissions directory exists
if [ ! -d "$SUBMISSIONS_DIR" ]; then
    echo "Error: Directory '$SUBMISSIONS_DIR' does not exist." >&2
    exit 1
fi

# Iterate over all files in the submissions directory
for file in "$SUBMISSIONS_DIR"/*; do
    # Process only regular files
    if [ -f "$file" ]; then
        ((processed_count++))
        
        # Calculate MD5 checksum hash of the file content
        file_hash=$(md5sum "$file" 2>> "$ERROR_LOG" | awk '{print $1}')
        filename=$(basename "$file")
        
        # Check if hash has already been encountered
        if [ -n "${seen_hashes[$file_hash]}" ]; then
            ((duplicate_count++))
            echo "[DUPLICATE] $filename matches content of ${seen_hashes[$file_hash]}"
        else
            # Store hash, back up unique file, and increment count
            seen_hashes[$file_hash]="$filename"
            cp "$file" "$BACKUP_DIR/" 2>> "$ERROR_LOG"
            ((backup_count++))
            echo "[BACKUP] $filename successfully backed up."
        fi
    fi
done

# Generate summary report displayed on stdout and saved to report.txt
{
    echo "=========================================="
    echo "     SUBMISSION PROCESSING REPORT        "
    echo "=========================================="
    echo "Total files processed : $processed_count"
    echo "Duplicate files found : $duplicate_count"
    echo "Unique files backed up: $backup_count"
    echo "=========================================="
} | tee "$REPORT_FILE"