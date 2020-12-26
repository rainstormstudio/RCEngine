/**
 * @file RCEngine.hpp
 * @author Daniel Hongyu Ding
 * @brief 
 * @version 0.1
 * @date 2020-12-25
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once
#ifndef RC_ENGINE_HPP
#define RC_ENGINE_HPP

#ifdef __linux__
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_ttf.h"
#elif _WIN32
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#endif

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

class CellTexture {
    SDL_Texture* texture;    // texture of the cell
    Uint8 ch;
    int numSrcRows;     // number of rows in src texture
    int numSrcCols;     // number of columns in src texture
    SDL_Rect srcRect;
    SDL_Rect destRect;
    SDL_Color foreColor;
    SDL_Color backColor;

public:
    /**
     * @brief Construct a new Cell 
     * 
     * @param texture the texture created from the tileset
     * @param numSrcRows number of rows in the tileset
     * @param numSrcCols number of columns in the tileset
     * @param srcCellWidth the width of the character in the tileset
     * @param srcCellHeight the height of the character in the tileset
     * @param destCellWidth the width of the cell
     * @param destCellHeight the height of the cell
     */
    CellTexture(SDL_Texture* texture, int numSrcRows, int numSrcCols, 
            int srcCellWidth, int srcCellHeight, int destCellWidth, int destCellHeight)
    : texture{texture}, numSrcRows{numSrcRows}, numSrcCols{numSrcCols} {
        srcRect = {0, 0, srcCellWidth, srcCellHeight};
        destRect = {0, 0, destCellWidth, destCellHeight};
        foreColor = {255, 255, 255, 255};
        backColor = {0, 0, 0, 255};
        ch = 0;
    }

    /**
     * @brief Set the character of the cell
     * 
     * @param ch 
     */
    inline void setCh(Uint8 ch) {
        this->ch = ch;
        srcRect.x = (ch % numSrcCols) * srcRect.w;
        srcRect.y = (ch / numSrcCols) * srcRect.h;
    }

    /**
     * @brief Set the coordinate of the destRect
     * 
     * @param x x-coordinate
     * @param y y-coordinate
     */
    inline void setDestPosition(int x, int y) {
        destRect.x = x;
        destRect.y = y;
    }

    /**
     * @brief Set the coordinate of the srcRect
     * 
     * @param x 
     * @param y 
     */
    inline void setSrcPosition(int x, int y) {
        srcRect.x = x;
        srcRect.y = y;
    }

    /**
     * @brief Set the Fore Color of the cell
     * 
     * @param r red
     * @param g green
     * @param b blue
     * @param a alpha
     */
    inline void setForeColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
        foreColor = {r, g, b, a};
    }

    /**
     * @brief Set the Fore Color of the cell
     * 
     * @param color (r, g, b, a)
     */
    inline void setForeColor(SDL_Color color) {
        foreColor = color;
    }

    /**
     * @brief Set the Back Color of the cell
     * 
     * @param r red
     * @param g green
     * @param b blue
     * @param a alpha
     */
    inline void setBackColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
        backColor = {r, g, b, a};
    }

    /**
     * @brief Set the Back Color of the cell
     * 
     * @param color (r, g, b, a)
     */
    inline void setBackColor(SDL_Color color) {
        backColor = color;
    }

    /**
     * @brief Get the charactor of the cell
     * 
     * @return Uint8 
     */
    Uint8 getCh() const {
        return ch;
    }

    /**
     * @brief Get the Fore Color of the cell
     * 
     * @return SDL_Color 
     */
    SDL_Color getForeColor() const {
        return foreColor;
    }

    /**
     * @brief Get the Back Color of the cell
     * 
     * @return SDL_Color 
     */
    SDL_Color getBackColor() const {
        return backColor;
    }

    /**
     * @brief renders the cell to the screen
     * 
     * @param renderer 
     */
    void render(SDL_Renderer* renderer) {
        SDL_SetTextureColorMod(texture, backColor.r, backColor.g, backColor.b);
        SDL_SetTextureAlphaMod(texture, backColor.a);
        SDL_Rect backSrcRect = {static_cast<int>((219 % numSrcCols) * srcRect.w), static_cast<int>((219 / numSrcCols) * srcRect.h), srcRect.w, srcRect.h};
        SDL_RenderCopyEx(renderer, texture, &backSrcRect, &destRect, 0.0, nullptr, SDL_FLIP_NONE);
        SDL_SetTextureColorMod(texture, foreColor.r, foreColor.g, foreColor.b);
        SDL_SetTextureAlphaMod(texture, foreColor.a);
        SDL_RenderCopyEx(renderer, texture, &srcRect, &destRect, 0.0, nullptr, SDL_FLIP_NONE);
    }
};

class RCEngine {
protected:
    // graphics info
    int cellRows;   // number of rows of cells
    int cellCols;   // number of columns of cells
    int cellWidth;  // the width of the cell
    int cellHeight; // the height of the cell
    int screenWidth;    // the width of the screen
    int screenHeight;   // the height of the screen
    std::string windowTitle; // the title of the window

    // inputs
    struct KeyState {
        bool pressed;
        bool released;
        bool hold;
    };
    std::vector<bool> keyInput;
    std::vector<bool> prevKeyInput;
    std::vector<KeyState> keyState;
    std::vector<bool> cursorInput;
    std::vector<bool> prevCursorInput;
    std::vector<KeyState> cursorState;
    int cursorPosX;
    int cursorPosY;

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* tileset;
    std::vector<std::vector<std::shared_ptr<CellTexture>>> buffer;

    // events info
    SDL_Event event;

    // inputs
    const int TOTAL_KEYS = 332;
    const int TOTAL_CURSOR_STATES = 5;

    // game info
    bool loop;

public:
    RCEngine() {
        cellRows = 0;
        cellCols = 0;
        screenWidth = 0;
        screenHeight = 0;
        windowTitle = "RCEngine";

        keyInput = std::vector<bool>(TOTAL_KEYS, false);
        prevKeyInput = std::vector<bool>(TOTAL_KEYS, false);
        keyState = std::vector<KeyState>(TOTAL_KEYS, {false, false, false});
        cursorInput = std::vector<bool>(TOTAL_CURSOR_STATES, false);
        prevCursorInput = std::vector<bool>(TOTAL_CURSOR_STATES, false);
        cursorState = std::vector<KeyState>(TOTAL_CURSOR_STATES, {false, false, false});
    }

    ~RCEngine() {}

    /**
     * @brief Creates the console window
     * 
     * @param tilesetPath the path to the tileset
     * @param rows number of rows of cells
     * @param cols number of columns of cells
     * @param fontWidth the width of the cell
     * @param fontHeight the height of the cell
     * @return true
     * @return false
     */
    bool createConsole(std::string tilesetPath = "./RCE_tileset.png", int rows = 30, int cols = 40, int fontWidth = 20, int fontHeight = 20) {
        cellRows = rows;
        cellCols = cols;
        cellWidth = fontWidth;
        cellHeight = fontHeight;
        int numSrcRows = 16;
        int numSrcCols = 16;
        screenWidth = cellCols * cellWidth;
        screenHeight = cellRows * cellHeight;

        window = nullptr;
        renderer = nullptr;
        tileset = nullptr;

        loop = false;

        int tileWidth = 0;
        int tileHeight = 0;

        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO) < 0) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            return false;
        } else {
            window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
            SDL_SetWindowFullscreen(window, 0);
            SDL_RaiseWindow(window);
            if (!window) {
                std::cerr << "Failed to create window: " << SDL_GetError() << std::endl;
                return false;
            } else {
                renderer = SDL_CreateRenderer(window, -1, 0);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
                    std::cerr << "Failed to load SDL_image: " << IMG_GetError() << std::endl;
                    return false;
                }
                if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
                    std::cerr << "Failed to load SDL_mixer: " << Mix_GetError() << std::endl;
                    return false;
                }
            }
        }

        SDL_Surface* surface = IMG_Load(tilesetPath.c_str());
        if (!surface) {
            std::cerr << "Error initializing SDL surface: " << IMG_GetError() << std::endl;
            return false;
        } else {
            SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 255, 0, 255));
            tileset = SDL_CreateTextureFromSurface(renderer, surface);
            if (tileset == nullptr) {
                std::cerr << "Error creating texture from " << tilesetPath << ": " << SDL_GetError() << std::endl;
                return false;
            } else {
                tileWidth = surface->w / numSrcCols;
                tileHeight = surface->h / numSrcRows;
            }
            SDL_FreeSurface(surface);
        }

        buffer = std::vector<std::vector<std::shared_ptr<CellTexture>>>(cellRows);
        for (int i = 0; i < cellRows; i ++) {
            buffer[i] = std::vector<std::shared_ptr<CellTexture>>(cellCols);
            for (int j = 0; j < cellCols; j ++) {
                buffer[i][j] = std::make_shared<CellTexture>(tileset, numSrcRows, numSrcCols, tileWidth, tileHeight, cellWidth, cellHeight);
                buffer[i][j]->setDestPosition(j * cellWidth, i * cellHeight);
            }
        }
        return true;
    }

    /**
     * @brief blends two colors
     * 
     * @param color1 (r, g, b, a)
     * @param color2 (r, g, b, a)
     * @return SDL_Color 
     */
    SDL_Color blendColor(SDL_Color color1, SDL_Color color2) {
        double red = color1.r;
        double green = color1.g;
        double blue = color1.b;
        double alpha = color1.a;
        red = red * (alpha / 255.0);
        green = green * (alpha / 255.0);
        blue = blue * (alpha / 255.0);
        alpha = 0.0;
        double newRed = color2.r;
        double newGreen = color2.g;
        double newBlue = color2.b;
        double newAlpha = color2.a;
        red = newRed * (newAlpha / 255.0) + red * (1.0 - newAlpha / 255.0);
        green = newGreen * (newAlpha / 255.0) + green * (1.0 - newAlpha / 255.0);
        blue = newBlue * (newAlpha / 255.0) + blue * (1.0 - newAlpha / 255.0);
        alpha = 255.0;
        SDL_Color blended = {0, 0, 0, 0};
        blended.r = static_cast<Uint8>(round(red));
        blended.g = static_cast<Uint8>(round(green));
        blended.b = static_cast<Uint8>(round(blue));
        blended.a = static_cast<Uint8>(round(alpha));
        return blended;
    }

    /**
     * @brief draws a charactor ch at (x, y) width foreColor and backColor
     * 
     * @param x x-coordinate (the index of column)
     * @param y y-coordinate (the index of row)
     * @param ch charactor
     * @param foreColor (r, g, b, a)
     * @param backColor (r, g, b, a)
     */
    void draw(int x, int y, Uint8 ch = ' ', SDL_Color foreColor = {255, 255, 255, 255}, SDL_Color backColor = {0, 0, 0, 255}) {
        if (0 <= x && x < cellCols && 0 <= y && y < cellRows) {
            buffer[y][x]->setCh(ch);
            buffer[y][x]->setForeColor(blendColor(buffer[y][x]->getForeColor(), foreColor));
            buffer[y][x]->setBackColor(blendColor(buffer[y][x]->getBackColor(), backColor));
        }
    }

    /**
     * @brief draws a line from (x1, y1) to (x2, y2)
     * 
     * @param x1 
     * @param y1 
     * @param x2 
     * @param y2 
     * @param ch 
     * @param foreColor 
     * @param backColor 
     */
    void drawLine(int x1, int y1, int x2, int y2, Uint8 ch = ' ', SDL_Color foreColor = {255, 255, 255, 255}, SDL_Color backColor = {0, 0, 0, 255}) {
        if (0 <= x1 && x1 < cellCols && 0 <= y1 && y1 < cellRows
            && 0 <= x2 && x2 < cellCols && 0 <= y2 && y2 < cellRows) {
            int dx = abs(x2 - x1);
            int dy = -abs(y2 - y1);

            // straight line optimization
            if (dx == 0) {  // vertical
                if (y1 > y2) {
                    std::swap(y1, y2);
                }
                for (int y = y1; y <= y2; y ++) {
                    draw(x1, y, ch, foreColor, backColor);
                }
            } else if (dy == 0) {   // horizontal
                if (x1 > x2) {
                    std::swap(x1, x2);
                }
                for (int x = x1; x <= x2; x ++) {
                    draw(x, y1, ch, foreColor, backColor);
                }
            } else {
                int sx = x1 < x2 ? 1 : -1;
                int sy = y1 < y2 ? 1 : -1;
                int error = dx + dy;
                while (1) {                    
                    draw(x1, y1, ch, foreColor, backColor);
                    if (x1 == x2 && y1 == y2) {
                        break;
                    }
                    int error2 = 2 * error;
                    if (error2 >= dy) {
                        error += dy;
                        x1 += sx;
                    }
                    if (error2 <= dx) {
                        error += dx;
                        y1 += sy;
                    }
                }
            }
        }
    }

    /**
     * @brief Get the character at (x, y)
     * 
     * @param x x-coordinate (the index of column)
     * @param y y-coordinate (the index of row)
     * @return Uint8 
     */
    Uint8 getCh(int x, int y) const {
        if (0 <= x && x < cellCols && 0 <= y && y < cellRows) {
            return buffer[y][x]->getCh();
        } else {
            return 0;
        }
    }

    /**
     * @brief Get the Fore Color at (x, y)
     * 
     * @param x x-coordinate (the index of column)
     * @param y y-coordinate (the index of row)
     * @return SDL_Color 
     */
    SDL_Color getForeColor(int x, int y) const {
        if (0 <= x && x < cellCols && 0 <= y && y < cellRows) {
            return buffer[y][x]->getForeColor();
        } else {
            return {0, 0, 0, 0};
        }
    }

    /**
     * @brief Get the Back Color at (x, y)
     * 
     * @param x x-coordinate (the index of column)
     * @param y y-coordinate (the index of row)
     * @return SDL_Color 
     */
    SDL_Color getBackColor(int x, int y) const {
        if (0 <= x && x < cellCols && 0 <= y && y < cellRows) {
            return buffer[y][x]->getBackColor();
        } else {
            return {0, 0, 0, 0};
        }
    }

    /**
     * @brief write a string to the screen starting at (x, y)
     * 
     * @param x x-coordinate (the index of column)
     * @param y y-coordinate (the index of row)
     * @param content 
     * @param foreColor (r, g, b, a)
     * @param backColor (r, g, b, a)
     */
    void write(int x, int y, std::string content, SDL_Color foreColor = {0, 0, 0, 0}, SDL_Color backColor = {0, 0, 0, 0}) {
        if (0 <= x && x < cellCols && 0 <= y && y < cellRows) {
            int len = content.length();
            for (int i = 0; i < len && x + i < cellCols; i ++) {
                if (content[i] == ' ') continue;
                buffer[y][x + i]->setCh(content[i]);
                buffer[y][x + i]->setForeColor(blendColor(buffer[y][x + i]->getForeColor(), foreColor));
                buffer[y][x + i]->setBackColor(blendColor(buffer[y][x + i]->getBackColor(), backColor));
            }
        }
    }

    /**
     * @brief fills a rectangle region
     * 
     * @param dest (x, y, w, h)
     * @param ch character
     * @param foreColor (r, g, b, a)
     * @param backColor (r, g, b, a)
     */
    void fill(SDL_Rect dest, Uint8 ch = ' ', SDL_Color foreColor = {0, 0, 0, 0}, SDL_Color backColor = {0, 0, 0, 0}) {
        if (0 <= dest.x && dest.x < cellCols && 0 <= dest.y && dest.y < cellRows) {
            for (int i = dest.y; i < dest.y + dest.h && i < cellCols; i ++) {
                for (int j = dest.x; j < dest.x + dest.w && j < cellRows; j ++) {
                    buffer[i][j]->setCh(ch);
                    buffer[i][j]->setForeColor(blendColor(buffer[i][j]->getForeColor(), foreColor));
                    buffer[i][j]->setBackColor(blendColor(buffer[i][j]->getBackColor(), backColor));
                }
            }
        }
    }

    /**
     * @brief start the game loop
     * 
     */
    void init() {
        loop = true;
        gameLoop();
    }

    /**
     * @brief clear screen and buffer
     * 
     */
    void clear() {
        for (int i = 0; i < cellRows; i ++) {
            for (int j = 0; j < cellCols; j ++) {
                buffer[i][j]->setCh(' ');
                buffer[i][j]->setForeColor(255, 255, 255, 255);
                buffer[i][j]->setBackColor(0, 0, 0, 255);
            }
        }
        SDL_RenderClear(renderer);
    }

    /**
     * @brief render to the screen
     * 
     */
    void render() {
        for (int i = 0; i < cellRows; i ++) {
            for (int j = 0; j < cellCols; j ++) {
                buffer[i][j]->render(renderer);
            }
        }
        SDL_RenderPresent(renderer);
    }
private:


    /**
     * @brief game loop
     * 
     */
    void gameLoop() {
        if (!start()) {
            loop = false;
        }

        auto time_a = std::chrono::high_resolution_clock::now();
        auto time_b = std::chrono::high_resolution_clock::now();

        while (loop) {
            while (loop) {
                time_b = std::chrono::high_resolution_clock::now();
                double deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(time_b - time_a).count() / 1000000.0f;
                time_a = time_b;

                while (SDL_PollEvent(&event)) {
                    switch (event.type) {
                        case SDL_QUIT: {
                            loop = false;
                            break;
                        }
                        case SDL_KEYDOWN: {
                            keyInput[event.key.keysym.sym] = true;
                            break;
                        }
                        case SDL_KEYUP: {
                            keyInput[event.key.keysym.sym] = false;
                            break;
                        }
                        case SDL_MOUSEMOTION: {
                            cursorPosX = event.motion.x / cellWidth;
                            cursorPosY = event.motion.y / cellHeight;
                            break;
                        }
                        case SDL_MOUSEBUTTONDOWN: {
                            switch (event.button.button) {
                                case SDL_BUTTON_LEFT: {
                                    cursorInput[0] = true;
                                    break;
                                }
                                case SDL_BUTTON_RIGHT: {
                                    cursorInput[1] = true;
                                    break;
                                }
                                case SDL_BUTTON_MIDDLE: {
                                    cursorInput[2] = true;
                                    break;
                                }
                                case SDL_BUTTON_X1: {
                                    cursorInput[3] = true;
                                    break;
                                }
                                case SDL_BUTTON_X2: {
                                    cursorInput[4] = true;
                                    break;
                                }
                            }
                            break;
                        }
                        case SDL_MOUSEBUTTONUP: {
                            switch (event.button.button) {
                                case SDL_BUTTON_LEFT: {
                                    cursorInput[0] = false;
                                    break;
                                }
                                case SDL_BUTTON_RIGHT: {
                                    cursorInput[1] = false;
                                    break;
                                }
                                case SDL_BUTTON_MIDDLE: {
                                    cursorInput[2] = false;
                                    break;
                                }
                                case SDL_BUTTON_X1: {
                                    cursorInput[3] = false;
                                    break;
                                }
                                case SDL_BUTTON_X2: {
                                    cursorInput[4] = false;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
                
                for (int i = 0; i < TOTAL_KEYS; i ++) {
                    keyState[i].pressed = false;
                    keyState[i].released = false;
                    if (keyInput[i] != prevKeyInput[i]) {
                        if (keyInput[i]) {
                            keyState[i].pressed = !keyState[i].hold;
                            keyState[i].hold = true;
                        } else {
                            keyState[i].released = true;
                            keyState[i].hold = false;
                        }
                    }
                    prevKeyInput[i] = keyInput[i];
                }

                for (int i = 0; i < TOTAL_CURSOR_STATES; i ++) {
                    cursorState[i].pressed = false;
                    cursorState[i].released = false;
                    if (cursorInput[i] != prevCursorInput[i]) {
                        if (cursorInput[i]) {
                            cursorState[i].pressed = true;
                            cursorState[i].hold = true;
                        } else {
                            cursorState[i].released = true;
                            cursorState[i].hold = false;
                        }
                    }
                    prevCursorInput[i] = cursorInput[i];
                }

                clear();

                if (!update(deltaTime)) {
                    loop = false;
                }
                std::string title = windowTitle + " - FPS: " + std::to_string(1.0f / deltaTime);
                SDL_SetWindowTitle(window, title.c_str());

                render();
            }

            if (destroy()) {
                if (!tileset) {
                    SDL_DestroyTexture(tileset);
                    tileset = nullptr;
                }
                SDL_DestroyWindow(window);
                SDL_DestroyRenderer(renderer);
                Mix_Quit();
                IMG_Quit();
                SDL_Quit();
            } else {
                loop = true;
            }
        }
    }

public:
    virtual bool start() = 0;
    virtual bool update(double deltaTime) = 0;
    virtual bool destroy() {
        return true;
    }

public:

    /**
     * @brief Get the x coordinate of the cursor
     * 
     * @return int 
     */
    int getCursorX() const {
        return cursorPosX;
    }

    /**
     * @brief Get the y coordinate of the cursor
     * 
     * @return int 
     */
    int getCursorY() const {
        return cursorPosY;
    }

    /**
     * @brief Get the Key State of key
     * 
     * @param key 
     * @return KeyState 
     */
    KeyState getKeyState(int key) const {
        return keyState[key];
    }

    /**
     * @brief Get the Cursor State of cursor
     * 
     * @param cursor 
     * @return KeyState 
     */
    KeyState getCursorState(int cursor) const {
        return cursorState[cursor];
    }

protected:
    /**
     * @brief write debug message to standard error
     * 
     * @param msg 
     * @param level 
     */
    void debugMsg(std::string msg, int level = 0) {
        for (int i = 0; i < level; i ++) {
            std::cerr << "    ";
        }
        std::cerr << "| " << msg << std::endl;
    }

    /**
     * @brief write a line to standard error
     * 
     * @param level 
     */
    void debugLine(int level = 0) {
        for (int i = 0; i < level; i ++) {
            std::cerr << "    ";
        }
        for (int i = 0; i < 50; i ++) {
            std::cerr << "-";
        }
        std::cerr << std::endl;
    }
};

#endif
