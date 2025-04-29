#include <stdio.h>
#include <stdlib.h> // For malloc, free
#include <string.h> // For strlen
#include <emscripten.h>
#include "sha256.h" // Assuming your sha256 implementation is here

// Keep this function available to JS
EMSCRIPTEN_KEEPALIVE
// Takes a string from JS, returns a pointer to the hex-encoded hash string in Wasm memory
// NOTE: This simple example returns a pointer to a static buffer.
// A more robust solution would allocate memory for the result and require JS to free it.
char* calculate_sha256(const char* input_string) {
    static char hex_output[65]; // 64 hex chars + null terminator
    uint8_t hash[32];           // SHA-256 output is 32 bytes
    SHA256_CTX ctx;             // Context for the hashing algorithm

    sha256_init(&ctx);
    sha256_update(&ctx, (const uint8_t*)input_string, strlen(input_string));
    sha256_final(&ctx, hash);

    // Convert the 32-byte hash to a 64-char hex string
    for (int i = 0; i < 32; i++) {
        sprintf(hex_output + (i * 2), "%02x", hash[i]);
    }
    hex_output[64] = '\0'; // Null terminate

    return hex_output; // Return pointer to the static buffer in Wasm memory
}