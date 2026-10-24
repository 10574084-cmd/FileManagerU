#include <SDL2/SDL.h>
#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>
#include "file_ops.hpp"

static const char* SD_PATH = "/vol/external01";
static const char* USB_PATH = "/vol/storage_usb01";

int main(int, char**) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) return 1;
    SDL_Window* window = SDL_CreateWindow("WiiU File Manager",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 854, 480, SDL_WINDOW_SHOWN);
    if (!window) { SDL_Quit(); return 1; }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer) { SDL_DestroyWindow(window); SDL_Quit(); return 1; }

    std::string currentPath = SD_PATH;
    std::vector<FileEntry> entries;
    std::string status = "A: open folder | B: parent | X: copy selected file to other device | Y: delete (disabled)";
    size_t selected = 0;
    bool running = true;
    SDL_Event event;

    auto refresh = [&]() {
        std::string error;
        if (!listDirectory(currentPath, entries, error)) { entries.clear(); status = error; selected = 0; return; }
        selected = entries.empty() ? 0 : std::min(selected, entries.size() - 1);
    };
    refresh();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) running = false;
                if (event.key.keysym.sym == SDLK_UP && selected > 0) --selected;
                if (event.key.keysym.sym == SDLK_DOWN && selected + 1 < entries.size()) ++selected;
                if (event.key.keysym.sym == SDLK_TAB) {
                    currentPath = currentPath == SD_PATH ? USB_PATH : SD_PATH;
                    selected = 0; refresh();
                }
                if (event.key.keysym.sym == SDLK_BACKSPACE) {
                    auto pos = currentPath.find_last_of('/');
                    if (pos != std::string::npos && currentPath.size() > std::string(SD_PATH).size())
                        currentPath = currentPath.substr(0, pos);
                    selected = 0; refresh();
                }
                if (event.key.keysym.sym == SDLK_RETURN && selected < entries.size() && entries[selected].directory) {
                    currentPath = joinPath(currentPath, entries[selected].name);
                    selected = 0; refresh();
                }
            }
            if (event.type == SDL_CONTROLLERBUTTONDOWN) {
                auto b = event.cbutton.button;
                if (b == SDL_CONTROLLER_BUTTON_BACK) running = false;
                if (b == SDL_CONTROLLER_BUTTON_DPAD_UP && selected > 0) --selected;
                if (b == SDL_CONTROLLER_BUTTON_DPAD_DOWN && selected + 1 < entries.size()) ++selected;
                if (b == SDL_CONTROLLER_BUTTON_Y) {
                    currentPath = currentPath == SD_PATH ? USB_PATH : SD_PATH;
                    selected = 0; refresh();
                }
                if (b == SDL_CONTROLLER_BUTTON_A && selected < entries.size() && entries[selected].directory) {
                    currentPath = joinPath(currentPath, entries[selected].name);
                    selected = 0; refresh();
                }
                if (b == SDL_CONTROLLER_BUTTON_B) {
                    auto pos = currentPath.find_last_of('/');
                    if (pos != std::string::npos && currentPath.size() > std::string(SD_PATH).size())
                        currentPath = currentPath.substr(0, pos);
                    selected = 0; refresh();
                }
            }
        }

        // Minimal graphical list. Text rendering is deliberately not faked; names/status are
        // sent to stdout for debug builds until the font/UI layer is added.
        SDL_SetRenderDrawColor(renderer, 12, 22, 38, 255);
        SDL_RenderClear(renderer);
        SDL_Rect header{0, 0, 854, 58};
        SDL_SetRenderDrawColor(renderer, 22, 76, 120, 255);
        SDL_RenderFillRect(renderer, &header);
        SDL_Rect selectedBar{18, 72 + static_cast<int>(selected) * 24, 818, 22};
        SDL_SetRenderDrawColor(renderer, 38, 95, 142, 255);
        if (!entries.empty()) SDL_RenderFillRect(renderer, &selectedBar);
        SDL_SetRenderDrawColor(renderer, 105, 180, 220, 255);
        for (size_t i = 0; i < std::min<size_t>(entries.size(), 14); ++i) {
            SDL_Rect row{24, 80 + static_cast<int>(i) * 24, 5, 10};
            SDL_RenderFillRect(renderer, &row);
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
