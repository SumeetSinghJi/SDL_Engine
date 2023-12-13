#include <SDL.h>
#include <SDL_image.h>
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
int offsetX = 0;
int offsetY = 0;
const int IMAGE_WIDTH = 1920;
const int IMAGE_HEIGHT = 1080;
SDL_Rect imageRect = {(SCREEN_WIDTH - IMAGE_WIDTH) / 2, (SCREEN_HEIGHT - IMAGE_HEIGHT) / 2, IMAGE_WIDTH, IMAGE_HEIGHT};

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* backgroundImage = nullptr;

SDL_Texture* load_texture(const char* path, const char* name) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path);
    if (!texture) {
        std::cerr << "Error: Failed to load " << name << ": " << IMG_GetError() << std::endl;
    } else {
        std::cout << "Successfully loaded texture: " << name << std::endl;
    }
    return texture;
}

void load_textures() {
    backgroundImage = load_texture("assets/graphics/backgrounds/rome-day.jpg", "Rome Day Background");
}

void start_SDL() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cerr << "Error: Failed to initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }

    window = SDL_CreateWindow("sdl_engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Error: Failed to create SDL Window: " << SDL_GetError() << std::endl;
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Error: Failed to create SDL Renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        exit(1);
    }

    load_textures();
}

void renderBackground() {
    SDL_Rect renderQuad = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    // Calculate the zoomed-out size
    int zoomFactor = 20;  // higher number means zoomed out
    int zoomedWidth = (IMAGE_WIDTH * zoomFactor);
    int zoomedHeight = (IMAGE_HEIGHT * zoomFactor);

    // Calculate the source rectangle considering the zoom level and mouse position
    SDL_Rect sourceRect = {
    static_cast<int>(offsetX),
    static_cast<int>(offsetY),
    zoomedWidth,
    zoomedHeight
};

    // Rendering the background image
    SDL_RenderCopy(renderer, backgroundImage, &sourceRect, &renderQuad);
}

void run_SDL() {
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEMOTION) {
                int mouseX = e.motion.x;
                int mouseY = e.motion.y;

                // Adjust offsetX and offsetY based on mouse position to simulate scrolling
                if (mouseX < 0) {
                    offsetX += 10;  // Scroll left
                } else if (mouseX > SCREEN_WIDTH) {
                    offsetX -= 10;  // Scroll right
                }

                if (mouseY < 0) {
                    offsetY += 10;  // Scroll up
                } else if (mouseY > SCREEN_HEIGHT) {
                    offsetY -= 10;  // Scroll down
                }
            }
        }

        SDL_RenderClear(renderer);
        renderBackground();
        SDL_RenderPresent(renderer);
    }
}


void closeSDL() {
    SDL_DestroyTexture(backgroundImage);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    start_SDL();
    run_SDL();
    closeSDL();
    return 0;
}
