// math.c
#include <emscripten.h> // Include Emscripten header for EMSCRIPTEN_KEEPALIVE

// EMSCRIPTEN_KEEPALIVE prevents Emscripten from removing this function during optimization
// It also makes it easily callable from JavaScript
EMSCRIPTEN_KEEPALIVE
int add(int a, int b) {
  return a + b;
}

EMSCRIPTEN_KEEPALIVE
int subtract(int a, int b) {
    return a - b;
}