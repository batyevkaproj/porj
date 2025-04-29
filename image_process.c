#include <emscripten.h>
#include <stdint.h> // For uint8_t

// imageDataBuffer points to the pixel data (RGBA format) in Wasm memory
// width and height are the dimensions of the image data
EMSCRIPTEN_KEEPALIVE
void apply_grayscale(uint8_t* imageDataBuffer, int width, int height) {
    int numPixels = width * height;
    for (int i = 0; i < numPixels * 4; i += 4) {
        // Get pixel components
        uint8_t r = imageDataBuffer[i];
        uint8_t g = imageDataBuffer[i + 1];
        uint8_t b = imageDataBuffer[i + 2];
        // Alpha (imageDataBuffer[i + 3]) is left unchanged

        // Calculate simple average grayscale value
        uint8_t gray = (r + g + b) / 3;

        // Set R, G, and B components to the grayscale value
        imageDataBuffer[i] = gray;
        imageDataBuffer[i + 1] = gray;
        imageDataBuffer[i + 2] = gray;
    }
}