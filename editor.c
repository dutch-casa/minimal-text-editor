#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 4096

// Function to edit a specific line in the buffer
void edit_line(char* buffer, int line_to_edit) {
    char* start = buffer;

    // Navigate to the desired line
    for (int i = 0; i < line_to_edit; i++) {
        start = strchr(start, '\n');
        if (!start) {
            printf("Error: Line %d does not exist.\n", line_to_edit);
            return;
        }
        start++;
    }

    char* line_end = strchr(start, '\n');
    if (!line_end) {
        line_end = start + strlen(start); // Handle last line without newline
    }

    // Temporarily store the rest of the content after the line
    char saved[BUFFER_SIZE] = { 0 };
    strcpy(saved, line_end);

    // Prompt user for the new content
    printf("Enter new content for line %d: ", line_to_edit + 1);
    if (!fgets(start, BUFFER_SIZE - (start - buffer), stdin)) {
        printf("Error reading input. Exiting.\n");
        return;
    }

    start[strcspn(start, "\n")] = '\0'; // Remove trailing newline from input

    // Append the saved content back to the buffer
    strcat(start, saved);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* f = fopen(argv[1], "r");
    if (!f) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    char buffer[BUFFER_SIZE] = {0};
    size_t bytes_read = fread(buffer, 1, BUFFER_SIZE - 1, f);
    fclose(f);

    if (bytes_read == 0) {
        printf("The file is empty or could not be read.\n");
        return EXIT_FAILURE;
    }

    // Display file contents with line numbers
    printf("Current file contents:\n");
    char* line = buffer;
    int line_number = 0;
    while (line) {
        char* next_line = strchr(line, '\n');
        if (next_line) *next_line = '\0'; // Temporarily null-terminate
        printf("%d: %s\n", line_number++, line);
        if (next_line) *next_line = '\n'; // Restore newline
        line = next_line ? next_line + 1 : NULL;
    }

    // Prompt user to edit a line
    printf("Enter the line number you want to edit (0-%d): ", line_number - 1);
    int line_to_edit;
    if (scanf("%d", &line_to_edit) != 1 || line_to_edit < 0 || line_to_edit >= line_number) {
        printf("Invalid line number. Exiting.\n");
        return EXIT_FAILURE;
    }

    getchar(); // Clear newline left in input buffer
    edit_line(buffer, line_to_edit);

    // Confirm before writing
    printf("Do you want to save the changes? (y/n): ");
    char confirm;
    if (scanf(" %c", &confirm) != 1 || (confirm != 'y' && confirm != 'Y')) {
        printf("Changes discarded.\n");
        return EXIT_SUCCESS;
    }

    // Write the updated content back to the file
    f = fopen(argv[1], "w");
    if (!f) {
        perror("Error opening file for writing");
        return EXIT_FAILURE;
    }

    fwrite(buffer, 1, strlen(buffer), f);
    fclose(f);

    printf("File updated successfully!\n");
    return EXIT_SUCCESS;
}
