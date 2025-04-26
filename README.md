https://aistudio.google.com/prompts/1l3l-PAdrnG6Yt5jhCgsqyBDnoV-DrNjZ

Emscripten SDK: This is the most common toolchain for compiling C/C++ code to WebAssembly. Installation instructions are on the official Emscripten site: https://emscripten.org/docs/getting_started/downloads.html (Installation can sometimes be tricky, follow their steps carefully).
Steps:
Install Emscripten: Follow the instructions for your OS from the link above. Make sure you can run emcc in your terminal/command prompt after installation.

emcc math.c -o math.js -s EXPORTED_FUNCTIONS="['_add', '_subtract']" -s EXPORTED_RUNTIME_METHODS="['cwrap']"