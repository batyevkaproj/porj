#include <emscripten.h>
#include <stdint.h> // For uint8_t
#include <pthread.h> // Include Pthreads header
#include <stdlib.h>  // For malloc/free
#include <stdio.h>   // For basic printf debugging

// Structure to pass data to each thread
typedef struct {
    uint8_t* imageDataBuffer; // Pointer to the start of the pixel data
    int width;
    int start_row;
    int end_row; // Process rows [start_row, end_row)
} thread_args_t;

// --- FORWARD DECLARATION ---
// Declare the core logic function before it's used.
void process_rows(uint8_t* imageDataBuffer, int width, int start_row, int end_row);


// --- CORE LOGIC ---
// Helper function containing the actual grayscale logic for a range of rows
void process_rows(uint8_t* imageDataBuffer, int width, int start_row, int end_row) {
    int row_stride = width * 4; // 4 bytes per pixel (RGBA)

    // Loop through assigned rows
    for (int y = start_row; y < end_row; ++y) {
        // Calculate starting position for this row
        uint8_t* row_start = imageDataBuffer + (y * row_stride);
        // Loop through pixels in the row
        for (int i = 0; i < row_stride; i += 4) {
            uint8_t r = row_start[i];
            uint8_t g = row_start[i + 1];
            uint8_t b = row_start[i + 2];
            // Simple average grayscale
            uint8_t gray = (r + g + b) / 3;
            row_start[i] = gray;
            row_start[i + 1] = gray;
            row_start[i + 2] = gray;
            // Alpha (row_start[i + 3]) remains unchanged
        }
    }
}

// --- PTHREAD WORKER ---
// The function each thread will execute
void* grayscale_worker(void* args) {
    thread_args_t* data = (thread_args_t*)args;
    // Call the core logic function
    process_rows(data->imageDataBuffer, data->width, data->start_row, data->end_row);
    // Optional: free(data) if you allocated dynamically per thread
    return NULL; // Pthread functions should return void*
}


// --- MAIN PTHREAD FUNCTION (Called from JS) ---
EMSCRIPTEN_KEEPALIVE
void apply_grayscale_pthread(uint8_t* imageDataBuffer, int width, int height, int num_threads) {
    if (num_threads <= 0) {
        num_threads = 1; // Sensible default or error
    }
    // Ensure num_threads doesn't exceed height (or maybe width for very thin images?)
    if (height > 0 && num_threads > height) {
        num_threads = height; // Cannot have more threads than rows
    }
    if (num_threads == 0 && height == 0) { // Handle 0xN images
         num_threads = 1;
    }


    printf("Starting grayscale processing with %d threads.\n", num_threads);

    // Handle trivial case: single thread (or 0 height) -> no need for pthread overhead
    if (num_threads == 1 || height == 0) {
        printf("Processing with a single call.\n");
        process_rows(imageDataBuffer, width, 0, height);
        printf("Single call finished.\n");
        return;
    }


    pthread_t* threads = malloc(sizeof(pthread_t) * num_threads);
    thread_args_t* thread_args = malloc(sizeof(thread_args_t) * num_threads);

    // Allocation failure fallback
    if (!threads || !thread_args) {
        fprintf(stderr, "Failed to allocate memory for threads/args. Falling back to single call.\n");
        if (threads) free(threads);
        if (thread_args) free(thread_args);
        process_rows(imageDataBuffer, width, 0, height); // Call helper directly
        return;
    }

    // Distribute rows
    int rows_per_thread = height / num_threads;
    int remaining_rows = height % num_threads;
    int current_start_row = 0;
    int created_threads_count = 0; // Keep track of successfully created threads

    // Create and launch threads
    for (int i = 0; i < num_threads; ++i) {
        thread_args[i].imageDataBuffer = imageDataBuffer;
        thread_args[i].width = width;
        thread_args[i].start_row = current_start_row;
        int rows_for_this_thread = rows_per_thread + (i < remaining_rows ? 1 : 0);
        thread_args[i].end_row = current_start_row + rows_for_this_thread;
        threads[i] = 0; // Initialize thread ID to 0 (or invalid value)

        // Skip creating thread if no rows assigned (can happen if num_threads > height initially)
        if (rows_for_this_thread <= 0) continue;

        // printf("Launching thread %d for rows %d to %d\n", i, thread_args[i].start_row, thread_args[i].end_row);
        int rc = pthread_create(&threads[i], NULL, grayscale_worker, &thread_args[i]);
        if (rc) {
            fprintf(stderr, "ERROR; return code from pthread_create() for thread %d is %d\n", i, rc);
            threads[i] = 0; // Mark as not created successfully
        } else {
            created_threads_count++;
        }
        current_start_row = thread_args[i].end_row;
    }

     printf("Launched %d threads successfully.\n", created_threads_count);

    // Wait for all successfully created threads to complete
    int joined_threads_count = 0;
    for (int i = 0; i < num_threads; ++i) {
         if (threads[i] != 0) { // Only join threads that were successfully created
              pthread_join(threads[i], NULL);
              joined_threads_count++;
         }
    }

    printf("Joined %d threads.\n", joined_threads_count);

    // Clean up allocated memory
    free(threads);
    free(thread_args);
}


// --- SINGLE THREADED WRAPPER (Called from JS) ---
// Keep the single-threaded version for comparison or fallback (optional)
EMSCRIPTEN_KEEPALIVE
void apply_grayscale_single(uint8_t* imageDataBuffer, int width, int height) {
     printf("Processing with single-threaded wrapper.\n");
     // Call the core logic function directly
     process_rows(imageDataBuffer, width, 0, height);
     printf("Single-threaded wrapper finished.\n");
}