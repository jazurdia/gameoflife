#include <SDL.h>

// Define the size of the framebuffer
const int FRAMEBUFFER_WIDTH = 100;
const int FRAMEBUFFER_HEIGHT = 100;
const int FRAMEBUFFER_SIZE = FRAMEBUFFER_WIDTH * FRAMEBUFFER_HEIGHT;
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

// Define a Color struct to hold the RGB values of a pixel
struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

// Declare the framebuffer as a global variable
Color framebuffer[FRAMEBUFFER_SIZE];

// Declare a global clearColor of type Color
Color clearColor = {0, 0, 0, 255}; // Initially set to black

// Declare a global currentColor of type Color
Color currentColor = {255, 255, 255, 255}; // Initially set to white

// Function to clear the framebuffer with the clearColor
void clear() {
    for (int i = 0; i < FRAMEBUFFER_SIZE; i++) {
        framebuffer[i] = clearColor;
    }
}

// Function to set a specific pixel in the framebuffer to the currentColor
void point(int x, int y) {
    if (x >= 0 && x < FRAMEBUFFER_WIDTH && y >= 0 && y < FRAMEBUFFER_HEIGHT) {
        framebuffer[y * FRAMEBUFFER_WIDTH + x] = currentColor;
    }
}

// Function to draw the initial pattern (Glider)
void drawRPentomino() {
    int midX = FRAMEBUFFER_WIDTH / 2;
    int midY = FRAMEBUFFER_HEIGHT / 2;

    point(midX, midY);
    point(midX + 1, midY);
    point(midX, midY + 1);
    point(midX - 1, midY + 1);
    point(midX, midY + 2);
}




void renderBuffer(SDL_Renderer* renderer) {
    // Create a texture
    SDL_Texture* texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ABGR8888,
            SDL_TEXTUREACCESS_STREAMING,
            FRAMEBUFFER_WIDTH,
            FRAMEBUFFER_HEIGHT
    );

    // Update the texture with the pixel data from the framebuffer
    SDL_UpdateTexture(
            texture,
            NULL,
            framebuffer,
            FRAMEBUFFER_WIDTH * sizeof(Color)
    );

    // Copy the texture to the renderer
    SDL_RenderCopy(renderer, texture, NULL, NULL);

    // Destroy the texture
    SDL_DestroyTexture(texture);
}

void render(SDL_Renderer* renderer) {

    // Create a temporary framebuffer for the next frame
    Color nextFrame[FRAMEBUFFER_SIZE];

    // Apply the rules of Conway's Game of Life
    for (int y = 0; y < FRAMEBUFFER_HEIGHT; y++) {
        for (int x = 0; x < FRAMEBUFFER_WIDTH; x++) {
            int aliveNeighbors = 0;

            // Check the eight neighbors of the current cell
            for (int ny = -1; ny <= 1; ny++) {
                for (int nx = -1; nx <= 1; nx++) {
                    // Skip the current cell
                    if (ny == 0 && nx == 0) {
                        continue;
                    }

                    int neighborX = x + nx;
                    int neighborY = y + ny;

                    // Check if the neighbor is within the bounds of the framebuffer
                    if (neighborX >= 0 && neighborX < FRAMEBUFFER_WIDTH && neighborY >= 0 && neighborY < FRAMEBUFFER_HEIGHT) {
                        // Check if the neighbor is alive
                        if (framebuffer[neighborY * FRAMEBUFFER_WIDTH + neighborX].r > 0) {
                            aliveNeighbors++;
                        }
                    }
                }
            }

            // Apply the rules of the game to determine the state of the cell in the next frame
            if (framebuffer[y * FRAMEBUFFER_WIDTH + x].r > 0) { // Cell is alive
                if (aliveNeighbors < 2 || aliveNeighbors > 3) {
                    nextFrame[y * FRAMEBUFFER_WIDTH + x] = clearColor; // Cell dies due to underpopulation or overpopulation
                } else {
                    nextFrame[y * FRAMEBUFFER_WIDTH + x] = currentColor; // Cell survives
                }
            } else { // Cell is dead
                if (aliveNeighbors == 3) {
                    nextFrame[y * FRAMEBUFFER_WIDTH + x] = currentColor; // Cell comes to life
                } else {
                    nextFrame[y * FRAMEBUFFER_WIDTH + x] = clearColor; // Cell remains dead
                }
            }
        }
    }

    // Update the framebuffer with the next frame
    for (int i = 0; i < FRAMEBUFFER_SIZE; i++) {
        framebuffer[i] = nextFrame[i];
    }

    // Render the framebuffer to the screen
    renderBuffer(renderer);
}


// Main function
int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("Conway's Game of Life", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Clear the framebuffer
    clear();

    // Draw the initial pattern
    drawRPentomino();

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Call our render function
        render(renderer);

        // Present the framebuffer to the screen
        SDL_RenderPresent(renderer);

        // Delay to limit the frame rate
        SDL_Delay(100); // Adjust the delay as needed for desired frame rate
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

