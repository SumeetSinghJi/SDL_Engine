#include <iostream>
#include <string>
#include <vector>
#include <cstdlib> // Include the necessary header for rand() function
#include <thread>  // for timer
#include <chrono>  // for timer
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "headers/game_update_1.h"
#include "headers/game_update_1.h"

/*
    To DO - Tic Tac Toe
    * Tic Tac Toe - Keyboard selection in Game field. Important as template for every other game ttt_keyboard_on_screen_handle
    * For popup time - slider texture background + 1 for button then create drag logic
    * Keyboard selection fix with highlight
    * Fix Win history
    * Test update function - render text into a popup
    * Test CMAKE with libzip and curl
    * Fix lives function
    * test win animation
    * Readme read file
    * Sign.exe with relevant CA
    * Practice unit testing, then do it for next project senet
    * on Win draw line
    * Google Test (Unit and Integration + Automatic on build)
    * Parralex background for world map
    * Secret hidden clickable scene, appears only after unlocking all other scenes, is a space game
    * Save game file for unlocked scenes, and achievements
    * Leaderboard scene reads from leaderboard website
    * keyboard handles
    * gamepad handles
*/

// SDL
SDL_Window *window;
SDL_Renderer *renderer;
SDL_Event event;
SDL_GameController *controller;

// SDL Image
SDL_Texture *splashScreenTexture;

// SDL Font
TTF_Font *font_24;
TTF_Font *font_36;
TTF_Font *font_48;

// SDL Music
Mix_Music *music;
Mix_Chunk *sfx;
Mix_Chunk *winGameSound = nullptr;
Mix_Chunk *loseGameSound = nullptr;

// Backgrounds
SDL_Texture *romeDayBackgroundTexture = nullptr; // scene 32

// MUSIC SETTINGS VARIABLES
bool isMusicPlaying = NULL;
std::string currentSong = "";                                                 // for music
std::string songTitle = "assets/sounds/music/8 bit dream land - Pixabay.mp3"; // for music
std::string os_version = "";
int windowWidth = 1200;
int windowHeight = 800;
bool quit = false;
int fontSize = 24;
int rectWidth = (windowWidth / 22);       // Button - The image width inside button itself
int rectHeight = (windowHeight / 22);     // Button - The image height inside button itself
int buttonWidth = ((windowWidth / 10));   // Button - The literal button width
int buttonHeight = ((windowHeight / 12)); // Button - The literal button height
int buttonXOffset = 25;                   // Button - X Offset e.g. (windowWidth / 10) - 10) = ((windowWidth / 10) - buttonXOffset) - higher number box goes left
int buttonYOffset = 14;                   // Button - Y Offset e.g. (windowWidth / 10) - 15) = ((windowWidth / 10) - buttonXOffset) - higher number box goes up

// Timer
bool timerRunning = false;
bool countdownStarted = false;
int countdownSeconds = 20; // Initial countdown time

// Popup textures
SDL_Texture *humanTexture = nullptr;
SDL_Texture *computerTexture = nullptr;
SDL_Texture *buttonTexture = nullptr; // Popup - button texture

// Tic Tac Toe - Textures
SDL_Texture *xTexture;
SDL_Texture *oTexture;
SDL_Texture *lineTexture;

// Update button
SDL_Texture *updateTexture = nullptr;

// HUD textures
SDL_Texture *restartTexture = nullptr;   // In game - restart game button
SDL_Texture *timerTexture = nullptr;     // In game - timer box
SDL_Texture *frequencyTexture = nullptr; // In game - frequency box
SDL_Texture *helpTexture = nullptr;      // In game - frequency box
SDL_Texture *settingsTexture = nullptr;  // In game - frequency box
SDL_Texture *worldMapTexture = nullptr;  // In game - frequency box
SDL_Texture *heartTexture = nullptr;     // In game - Rounds
SDL_Texture *hearts2Texture = nullptr;   // In game - Rounds
SDL_Texture *hearts3Texture = nullptr;   // In game - Rounds
SDL_Texture *hearts4Texture = nullptr;   // In game - Rounds
SDL_Texture *hearts5Texture = nullptr;   // In game - Rounds
SDL_Texture *hearts10Texture = nullptr;  // In game - Rounds

// Tic Tac Toe - Global variables
int ttt_ROWS = 3;                                             // Core Logic - for drawing 9 grid array
int ttt_COLUMNS = 3;                                          // Core Logic - for drawing 9 grid array
int ttt_player_choice = 3;                                    // Core Logic - 0 = O, 1 = X, 3 = garbage value
int ttt_opponent_choice = 3;                                  // Core Logic - 0 = O, 1 = X, 3 = garbage value
std::vector<int> ttt_positions = {2, 2, 2, 2, 2, 2, 2, 2, 2}; // Core Logic - 9 grid positions 3 top, 3 middle, 3 bottom
int ttt_winner = 0;                                           // Core Logic - 1 = player, 2 = opponent, 3 = Draw
bool ttt_game_over = false;                                   // Core Logic - after any players turn check _update_win_logic() to see if game over true
bool ttt_opponentsTurn = false;                               // Core Logic - after players turn, this turns true;
bool ttt_showPopup = true;                                    // Popup - toggle close after selections below with mouse_handle
bool ttt_player_choose_x_or_o = false;                        // Popup - Choose X or O
int ttt_rounds = 0;                                           // Popup - choose rounds
bool ttt_choose_rounds = false;                               // Popup - Choose rounds
bool ttt_starting_player_chosen = false;                      // Popup - starting player
bool ttt_starting_player_is_x = false;                        // Popup - starting player
bool ttt_choose_human_or_computer = false;                    // Popup - play against human or computer
bool ttt_play_against_human = false;                          // Popup - play against human or computer
bool ttt_timer_set = false;                                   // Popup - Timer
std::vector<int> ttt_winner_history;                          // HUD - winners frequency rect
std::vector<int> ttt_winner_choice_history;                   // HUD - winners frequency rect

int selectedOption = 0;    // For Keyboard arrow key or Gamepad d-pad selection
int menuTotalOptions = 13; // For Keyboard arrow key or Gamepad d-pad selection

// FUNCTION PROTOTYPES
void ttt_load_textures(); // for Start_SDL() only in this Sandbox environment not Main branch
void ttt_SDL_cleanup();   // for keyboard_handle ESC key = quit
void exit_SDL();          // for keyboard_handle ESC key = quit

// main.cpp function
void load_music(std::string songTitle)
{
    if (currentSong != songTitle)
    {
        music = Mix_LoadMUS(songTitle.c_str());
        if (!music)
        {
            std::cerr << "Error: Failed to load music: " << Mix_GetError() << std::endl;
            music = nullptr;
        }
        else
        {
            Mix_PlayMusic(music, -1); // Set loops to -1 for infinite looping
            isMusicPlaying = true;
            currentSong = songTitle;
            std::cout << "Music playing is: " << songTitle << std::endl;
        }
    }
}
void load_sound()
{
    winGameSound = Mix_LoadWAV("assets/sounds/sound-effects/wavs/Success Fanfare Trumpets - Pixabay.wav");
    loseGameSound = Mix_LoadWAV("assets/sounds/sound-effects/wavs/Fail - UNIVERSFIELD.wav");

    if (!winGameSound || !loseGameSound)
    {
        std::cerr << "Error: Failed to load win/loss sound effects: " << Mix_GetError() << std::endl;
    }
}
void load_fonts()
{
    font_24 = TTF_OpenFont("assets/fonts/noto-sans/NotoSansJP-Regular.ttf", 24);
    font_36 = TTF_OpenFont("assets/fonts/noto-sans/NotoSansJP-Regular.ttf", 36);
    font_48 = TTF_OpenFont("assets/fonts/noto-sans/NotoSansJP-Regular.ttf", 48);
    if (!font_24 || !font_36 || !font_48)
    {
        std::cerr << "Error: Failed to load font size: " << TTF_GetError() << std::endl;
    }
}
void render_text(const std::string &text, int x, int y)
{
    TTF_Font *font = nullptr;
    if (fontSize == 24)
    {
        font = font_24;
        // std::cout << "Rendering Font at size: " << fontSize << std::endl; // for debugging text render size
    }
    else if (fontSize == 36)
    {
        font = font_36;
        // std::cout << "Rendering Font at size: " << fontSize << std::endl; // for debugging text render size
    }
    else if (fontSize == 48)
    {
        font = font_48;
        // std::cout << "Rendering Font at size: " << fontSize << std::endl; // for debugging text render size
    }

    if (font)
    {
        SDL_Color textColor = {0, 0, 0};
        SDL_Surface *textSurface = TTF_RenderUTF8_Solid(font, text.c_str(), textColor);

        if (textSurface)
        {
            // Calculate textWidth and textHeight using TTF_SizeText
            int textWidth, textHeight;
            TTF_SizeText(font, text.c_str(), &textWidth, &textHeight);

            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_FreeSurface(textSurface);

            if (textTexture)
            {
                SDL_Rect textRect = {x, y, textWidth, textHeight};
                SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
                SDL_DestroyTexture(textTexture);
            }
        }
    }
}
SDL_Texture *load_texture(const char *path, const char *name)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, path);
    if (!texture)
    {
        std::cerr << "Error: Failed to load " << name << ": " << IMG_GetError() << std::endl;
    }
    else
    {
        std::cout << "Successfully loaded texture: " << name << std::endl;
    }
    return texture;
}
void start_SDL()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "Error: Failed to initialize SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }

    if (TTF_Init() != 0)
    {
        std::cerr << "Error: Failed to initialize SDL Font: " << TTF_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
    {
        std::cerr << "Error: Failed to initialize SDL IMG: " << IMG_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    if (Mix_Init(MIX_INIT_MP3) != MIX_INIT_MP3)
    {
        std::cerr << "Error: Failed to initialize Audio: " << Mix_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
    {
        std::cerr << "Error: Failed to open audio channel: " << Mix_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    window = SDL_CreateWindow("sdl_engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cerr << "Error: Failed to create SDL Window: " << SDL_GetError() << std::endl;
        exit(1);
        return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << "Error: Failed to create SDL Renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        exit(1);
        return;
    }
    load_fonts();
    ttt_load_textures();
    ttt_load_textures();
    load_music(songTitle);
    load_sound();
}
void toggle_countdown()
{
    // toggle_countdown() started whenever the popup for game start variable is closed
    if (!countdownStarted)
    {
        countdownStarted = true;
        std::thread countdownThread([]()
                                    {
            timerRunning = true;
            for (; countdownSeconds > 0 && timerRunning; --countdownSeconds) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
            timerRunning = false;
            if (countdownSeconds <= 0) {
                if(!ttt_showPopup) {
                    ttt_game_over = true;
                    ttt_winner = 3; // draw
                }
                
            } });
        countdownThread.detach(); // Detach the thread to let it run independently
    }
    else
    {
        countdownStarted = false;
        timerRunning = false; // Stop the countdown when toggled off
    }
}
std::string find_os()
{
    const char *char_osVersion = SDL_GetPlatform();
    os_version = char_osVersion;
    if (char_osVersion != NULL)
    {
        std::cout << "Host operating system: " << char_osVersion << std::endl;
    }
    else
    {
        std::cerr << "Failed to detect the host operating system." << std::endl;
    }
    return os_version;
}


// main.cpp HUD Functions
void draw_timer(int countdownSeconds)
{
    /* draw_timer() is placed in standard draw()
    countdownSeconds can be any value from rect, but default value is global variable, countdownSeconds = 120;

    Calculate minutes and seconds
        if countdownSeconds = 75;
            75 seconds / 60 = 1 minute
            75 seconds / 60 = remainder 15 seconds
        render_text(1:15)
    */
    SDL_Rect timerRect = {static_cast<int>(windowWidth * 0.05), static_cast<int>(windowHeight * 0.05), (windowWidth / 4), (windowHeight / 8)};
    SDL_RenderCopy(renderer, timerTexture, nullptr, &timerRect);

    int minutes = countdownSeconds / 60;
    int seconds = countdownSeconds % 60;

    // If minutes or seconds < 10, it will add an 0 e.g. 120 seconds = 2. As 2 < 10, final output: 02:00
    std::string timerText = (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);

    render_text(timerText, timerRect.x + 150, timerRect.y + 30);
}
void draw_win_frequency(const std::vector<int> &winners, const std::vector<int> &winnersChoice)
{
    SDL_Rect frequencyRect = {static_cast<int>(windowWidth * 0.8), static_cast<int>(windowHeight * 0.3), (windowWidth / 6), (windowHeight / 6)};
    SDL_RenderCopy(renderer, frequencyTexture, nullptr, &frequencyRect);

    int frequencyRectyOffSet = 0; // Initialize offset for vertical spacing

    for (size_t i = 0; i < winners.size(); ++i)
    {
        std::string winnerString = "";
        std::string winnerChoiceString = "";

        int winner = winners[i];
        int winnerChoice = winnersChoice[i];

        if (winner == 1)
        {
            winnerString = "Player";
        }
        else if (winner == 2)
        {
            winnerString = "Opponent";
        }
        else if (winner == 3)
        {
            winnerString = "Draw";
        }

        if (winnerChoice == 0)
        {
            winnerChoiceString = "O";
        }
        else if (winnerChoice == 1)
        {
            winnerChoiceString = "X";
        }
        else if (winnerChoice == 2)
        {
            winnerChoiceString = "";
        }

        // Render winner and their choice at appropriate positions
        render_text(winnerString, static_cast<int>(windowWidth * 0.8), static_cast<int>(windowHeight * 0.15) + frequencyRect.y + frequencyRectyOffSet);
        render_text(winnerChoiceString, static_cast<int>(windowWidth * 0.92), static_cast<int>(windowHeight * 0.15) + frequencyRect.y + frequencyRectyOffSet);

        frequencyRectyOffSet += 40; // Move to the next vertical position for next winner
    }
}
void draw_lives(int lives)
{
    int initialHeartX = static_cast<int>(windowWidth * 0.37); // Initial x-position for the first heart
    int heartWidth = windowWidth / 26;                        // Width of each heart
    int xHeartSpacing = 5;                                    // Adjust this value for spacing between hearts

    SDL_Rect heartRect = {initialHeartX, static_cast<int>(windowHeight * 0.15), heartWidth, heartWidth};

    for (int i = 0; i < lives; i++)
    {
        heartRect.x = initialHeartX + (heartWidth + xHeartSpacing) * i;
        SDL_RenderCopy(renderer, heartTexture, nullptr, &heartRect);
    }
}

// Tic Tac Toe - Draws
void ttt_load_textures()
{
    xTexture = load_texture("assets/graphics/buttons/popup/x.png", "Tic Tac Toe X image");
    oTexture = load_texture("assets/graphics/buttons/popup/o.png", "Tic Tac Toe O image");
    lineTexture = load_texture("assets/graphics/hud/line.png", "Tic Tac Toe Line image");
    restartTexture = load_texture("assets/graphics/buttons/settings/restart-button.png", "Restart");
    buttonTexture = load_texture("assets/graphics/buttons/boxes/button.png", "Button");
    humanTexture = load_texture("assets/graphics/buttons/settings/human-button.png", "Human");
    computerTexture = load_texture("assets/graphics/buttons/settings/computer-button.png", "Computer");
    romeDayBackgroundTexture = load_texture("assets/graphics/backgrounds/rome-day.jpg", "Rome Day Background");
    heartTexture = load_texture("assets/graphics/HUD/heart.png", "Heart");
    hearts2Texture = load_texture("assets/graphics/HUD/2hearts.png", "2 Hearts");
    hearts3Texture = load_texture("assets/graphics/HUD/3hearts.png", "3 Hearts");
    hearts4Texture = load_texture("assets/graphics/HUD/4hearts.png", "4 Hearts");
    hearts5Texture = load_texture("assets/graphics/HUD/5hearts.png", "5 Hearts");
    hearts10Texture = load_texture("assets/graphics/HUD/10hearts.png", "10 Hearts");
}
void ttt_draw_field()
{
    // Play area
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Set the blend mode to enable transparency
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 64);       // RGB value: light grey, last digit (Alpha value = 256 / 4) (75% transparency)
    SDL_Rect ttt_rect = {(windowWidth / 4), (windowHeight / 4), (windowWidth / 2), (windowHeight / 2)};
    SDL_RenderFillRect(renderer, &ttt_rect);

    // Grid lines
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB value: Black

    // Calculate the number of squares inside the Tic Tac Toe rectangle
    int innerttt_COLUMNS = ttt_COLUMNS;
    int innerttt_ROWS = ttt_ROWS;

    // Calculate the size of each square
    int squareWidth = ttt_rect.w / ttt_COLUMNS;
    int squareHeight = ttt_rect.h / ttt_ROWS;

    // Draw vertical lines within the Tic Tac Toe rectangle
    for (int i = 1; i < innerttt_COLUMNS; ++i)
    {
        SDL_RenderDrawLine(renderer, ttt_rect.x + i * squareWidth, ttt_rect.y, ttt_rect.x + i * squareWidth, ttt_rect.y + ttt_rect.h);
    }

    // Draw horizontal lines within the Tic Tac Toe rectangle
    for (int i = 1; i < innerttt_ROWS; ++i)
    {
        SDL_RenderDrawLine(renderer, ttt_rect.x, ttt_rect.y + i * squareHeight, ttt_rect.x + ttt_rect.w, ttt_rect.y + i * squareHeight);
    }
}
void ttt_draw_setup_game_popup_window()
{
    // Draw popup black border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect borderRect = {(windowWidth / 4) - 2, (windowHeight / 6) - 2, (windowWidth / 2) + 4, static_cast<int>(windowHeight * 0.8) + 4};
    SDL_RenderFillRect(renderer, &borderRect);

    // draw popup
    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect popupRect = {(windowWidth / 4), (windowHeight / 6), (windowWidth / 2), static_cast<int>(windowHeight * 0.8)};
    SDL_RenderFillRect(renderer, &popupRect);

    // Draw a close button
    // close button - grey square background black border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect closeButtonBorderLineRect = {static_cast<int>(windowWidth * 0.68) - 4, static_cast<int>(windowHeight * 0.2) - 4, rectWidth + 8, rectHeight + 8};
    SDL_RenderFillRect(renderer, &closeButtonBorderLineRect);

    // close button - grey square background
    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect closeButtonBorderRect = {static_cast<int>(windowWidth * 0.68) - 2, static_cast<int>(windowHeight * 0.2) - 2, rectWidth + 4, rectHeight + 4};
    SDL_RenderFillRect(renderer, &closeButtonBorderRect);

    // close button
    SDL_Rect closeButtonRect = {static_cast<int>(windowWidth * 0.68), static_cast<int>(windowHeight * 0.2), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, xTexture, nullptr, &closeButtonRect);

    /*

        Choose X or O

    */
    if (!ttt_player_choose_x_or_o)
    {
        render_text("Choose: Crosses (X) or Naughts (0)", static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.2));
    }
    else // after picking choice advise player what they choose
    {
        if (ttt_player_choice == 0)
        {
            render_text("You choose: 0", static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.2));
            // highlight selected rect, and in if condition dont hide, so player can repick.
        }
        else
        {
            render_text("You choose: X", static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.2));
            // highlight selected rect, and in if condition dont hide, so player can repick.
        }
    }

    // Button - for choose X
    SDL_Rect ttt_XbuttonRect = {static_cast<int>(windowWidth * 0.26) - buttonXOffset, static_cast<int>(windowHeight * 0.26) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &ttt_XbuttonRect);

    SDL_Rect ttt_X_rect = {static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.26), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, xTexture, nullptr, &ttt_X_rect);

    // Button - for choose O
    SDL_Rect ttt_ObuttonRect = {static_cast<int>(windowWidth * 0.36) - buttonXOffset, static_cast<int>(windowHeight * 0.26) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &ttt_ObuttonRect);

    SDL_Rect ttt_O_rect = {static_cast<int>(windowWidth * 0.36), static_cast<int>(windowHeight * 0.26), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, oTexture, nullptr, &ttt_O_rect);

    /*

        Starting Player

    */

    if (!ttt_starting_player_chosen)
    {
        render_text("Who starts first: X or 0", static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.34));
    }
    else
    {
        if (!ttt_starting_player_is_x)
        {
            render_text("You choose: Starting player O", static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.34));
        }
        else if (ttt_starting_player_is_x)
        {
            render_text("You choose: Starting player X", static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.34));
        }
    }

    // player starts first
    SDL_Rect ttt_player_start_first_button_Rect = {static_cast<int>(windowWidth * 0.26) - buttonXOffset, static_cast<int>(windowHeight * 0.40) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &ttt_player_start_first_button_Rect);

    SDL_Rect ttt_player_start_first_rect = {static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.40), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, xTexture, nullptr, &ttt_player_start_first_rect);

    // Opponent starts first
    SDL_Rect ttt_opponent_start_first_button_Rect = {static_cast<int>(windowWidth * 0.36) - buttonXOffset, static_cast<int>(windowHeight * 0.40) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &ttt_opponent_start_first_button_Rect);

    SDL_Rect ttt_opponent_start_first_rect = {static_cast<int>(windowWidth * 0.36), static_cast<int>(windowHeight * 0.40), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, oTexture, nullptr, &ttt_opponent_start_first_rect);

    /*

        Play against

    */

    if (!ttt_choose_human_or_computer)
    {
        render_text("Play against: Human or Computer", static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.50));
    }
    else
    {
        if (ttt_play_against_human)
        {
            render_text("You choose: Play against Human", static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.50));
        }
        else
        {
            render_text("You choose: Play against Computer", static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.50));
        }
    }

    // player starts first
    SDL_Rect ttt_humanButtonRect = {static_cast<int>(windowWidth * 0.26) - buttonXOffset, static_cast<int>(windowHeight * 0.56) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &ttt_humanButtonRect);

    SDL_Rect humanRect = {static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.56), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, humanTexture, nullptr, &humanRect);

    // Opponent starts first
    SDL_Rect ttt_computerButtonRect = {static_cast<int>(windowWidth * 0.36) - buttonXOffset, static_cast<int>(windowHeight * 0.56) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &ttt_computerButtonRect);

    SDL_Rect computerRect = {static_cast<int>(windowWidth * 0.36), static_cast<int>(windowHeight * 0.56), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, computerTexture, nullptr, &computerRect);

    /*

        Rounds

    */

    if (!ttt_choose_rounds)
    {
        render_text("Rounds: 1, 3, 5, custom", static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.66));
    }
    else
    {
        std::string renderRounds = "You choose: " + std::to_string(ttt_rounds) + " rounds";
        render_text(renderRounds, static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.66));
    }

    // 1 Round
    SDL_Rect heartRounds1ButtonRect = {static_cast<int>(windowWidth * 0.26) - buttonXOffset, static_cast<int>(windowHeight * 0.72) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &heartRounds1ButtonRect);

    SDL_Rect heartRounds1Rect = {static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.72), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, heartTexture, nullptr, &heartRounds1Rect);

    // 3 Rounds
    SDL_Rect heartRounds3ButtonRect = {static_cast<int>(windowWidth * 0.36) - buttonXOffset, static_cast<int>(windowHeight * 0.72) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &heartRounds3ButtonRect);

    SDL_Rect heartRounds3Rect = {static_cast<int>(windowWidth * 0.36), static_cast<int>(windowHeight * 0.72), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, hearts3Texture, nullptr, &heartRounds3Rect);

    // 5 Rounds
    SDL_Rect heartRounds5ButtonRect = {static_cast<int>(windowWidth * 0.46) - buttonXOffset, static_cast<int>(windowHeight * 0.72) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &heartRounds5ButtonRect);

    SDL_Rect heartRounds5Rect = {static_cast<int>(windowWidth * 0.46), static_cast<int>(windowHeight * 0.72), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, hearts5Texture, nullptr, &heartRounds5Rect);

    // 10 Rounds
    SDL_Rect heartRounds10ButtonRect = {static_cast<int>(windowWidth * 0.56) - buttonXOffset, static_cast<int>(windowHeight * 0.72) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &heartRounds10ButtonRect);

    SDL_Rect heartRounds10Rect = {static_cast<int>(windowWidth * 0.56), static_cast<int>(windowHeight * 0.72), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, hearts10Texture, nullptr, &heartRounds10Rect);

    /*

        Timer

    */

    if (!ttt_timer_set)
    {
        render_text("Timer: 10s, 30s, 1m, 5m, 10m, 30m, 1hr, custom (s) ", static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.82));
    }
    else
    {
        int hours = countdownSeconds / 3600;
        int remainingSecondsAfterHours = countdownSeconds % 3600;
        int minutes = remainingSecondsAfterHours / 60;
        int seconds = remainingSecondsAfterHours % 60;
        std::string renderTimer = "You chose: " + std::to_string(hours) + " h, " +
            std::to_string(minutes) + " m, " +
            std::to_string(seconds) + " s";
        render_text(renderTimer, static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.82));
    }

    // 10 seconds
    SDL_Rect seconds10ButtonRect = {static_cast<int>(windowWidth * 0.26) - buttonXOffset, static_cast<int>(windowHeight * 0.88) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &seconds10ButtonRect);

    SDL_Rect seconds10Rect = {static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.88), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, oTexture, nullptr, &seconds10Rect);

    // 30 seconds
    SDL_Rect seconds30ButtonRect = {static_cast<int>(windowWidth * 0.31) - buttonXOffset, static_cast<int>(windowHeight * 0.88) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &seconds30ButtonRect);

    SDL_Rect seconds30Rect = {static_cast<int>(windowWidth * 0.31), static_cast<int>(windowHeight * 0.88), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, oTexture, nullptr, &seconds30Rect);

    // 1 minute
    SDL_Rect seconds60ButtonRect = {static_cast<int>(windowWidth * 0.36) - buttonXOffset, static_cast<int>(windowHeight * 0.88) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &seconds60ButtonRect);

    SDL_Rect seconds60Rect = {static_cast<int>(windowWidth * 0.36), static_cast<int>(windowHeight * 0.88), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, oTexture, nullptr, &seconds60Rect);

    // 5 minute
    SDL_Rect seconds300ButtonRect = {static_cast<int>(windowWidth * 0.41) - buttonXOffset, static_cast<int>(windowHeight * 0.88) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &seconds300ButtonRect);

    SDL_Rect seconds300Rect = {static_cast<int>(windowWidth * 0.41), static_cast<int>(windowHeight * 0.88), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, oTexture, nullptr, &seconds300Rect);

    // 10 minute
    SDL_Rect seconds600ButtonRect = {static_cast<int>(windowWidth * 0.46) - buttonXOffset, static_cast<int>(windowHeight * 0.88) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &seconds600ButtonRect);

    SDL_Rect seconds600Rect = {static_cast<int>(windowWidth * 0.46), static_cast<int>(windowHeight * 0.88), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, oTexture, nullptr, &seconds600Rect);

    // 30 minute
    SDL_Rect seconds1800ButtonRect = {static_cast<int>(windowWidth * 0.51) - buttonXOffset, static_cast<int>(windowHeight * 0.88) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &seconds1800ButtonRect);

    SDL_Rect seconds1800Rect = {static_cast<int>(windowWidth * 0.51), static_cast<int>(windowHeight * 0.88), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, oTexture, nullptr, &seconds1800Rect);

    // 1 hour
    SDL_Rect seconds3600ButtonRect = {static_cast<int>(windowWidth * 0.56) - buttonXOffset, static_cast<int>(windowHeight * 0.88) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_RenderCopy(renderer, buttonTexture, nullptr, &seconds3600ButtonRect);

    SDL_Rect seconds3600Rect = {static_cast<int>(windowWidth * 0.56), static_cast<int>(windowHeight * 0.88), rectWidth, rectHeight};
    SDL_RenderCopy(renderer, oTexture, nullptr, &seconds3600Rect);
}
void ttt_draw_X_or_O()
{
    std::vector<SDL_Rect> positionRects = {
        {static_cast<int>(windowWidth * 0.25), static_cast<int>(windowHeight * 0.25), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.45), static_cast<int>(windowHeight * 0.25), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.6), static_cast<int>(windowHeight * 0.25), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.25), static_cast<int>(windowHeight * 0.45), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.45), static_cast<int>(windowHeight * 0.45), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.6), static_cast<int>(windowHeight * 0.45), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.25), static_cast<int>(windowHeight * 0.6), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.45), static_cast<int>(windowHeight * 0.6), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.6), static_cast<int>(windowHeight * 0.6), (windowWidth / 8), (windowHeight / 8)}};
    for (size_t i = 0; i < ttt_positions.size(); ++i)
    {
        if (ttt_positions[i] == 0)
        {
            SDL_RenderCopy(renderer, oTexture, nullptr, &positionRects[i]);
        }
        else if (ttt_positions[i] == 1)
        {
            SDL_RenderCopy(renderer, xTexture, nullptr, &positionRects[i]);
        }
        else if (ttt_positions[i] == 2)
        {
            // Do nothing for position 2
        }
    }
}
void ttt_draw_settings_buttons()
{
    SDL_Rect helpRect = {static_cast<int>(windowWidth * 0.9), static_cast<int>(windowHeight * 0.3), rectWidth, rectHeight};
    SDL_Rect restartRect = {static_cast<int>(windowWidth * 0.9), static_cast<int>(windowHeight * 0.4), rectWidth, rectHeight};
    SDL_Rect settingsRect = {static_cast<int>(windowWidth * 0.9), static_cast<int>(windowHeight * 0.5), rectWidth, rectHeight};
    SDL_Rect worldMapRect = {static_cast<int>(windowWidth * 0.9), static_cast<int>(windowHeight * 0.6), rectWidth, rectHeight};

    SDL_RenderCopy(renderer, helpTexture, nullptr, &helpRect);
    SDL_RenderCopy(renderer, restartTexture, nullptr, &restartRect);
    SDL_RenderCopy(renderer, settingsTexture, nullptr, &settingsRect);
    SDL_RenderCopy(renderer, worldMapTexture, nullptr, &worldMapRect);
}

// Tic Tac Toe - Updates
std::vector<int> ttt_update_ai_check_available_moves(std::vector<int> &ttt_positions)
{
    /*
        A vector function that accepts the 9 int vector postitions
        finds any available spaces and returns them;
    */
    std::vector<int> moves;
    for (int i = 0; i < ttt_positions.size(); ++i)
    {
        if (ttt_positions[i] == 2)
        {
            moves.push_back(i);
        }
    }
    return moves;
}
int ttt_update_possible_winning_moves(std::vector<int> ttt_positions, int player)
{
    for (int i = 0; i < ttt_positions.size(); ++i)
    {
        if (ttt_positions[i] == 2)
        {
            std::vector<int> testBoard = ttt_positions;
            testBoard[i] = player; // pretend this free space on board is any player

            for (int j = 0; j < 3; ++j)
            {
                if ((testBoard[j * 3] == player && testBoard[j * 3 + 1] == player && testBoard[j * 3 + 2] == player) ||
                    (testBoard[j] == player && testBoard[j + 3] == player && testBoard[j + 6] == player))
                {
                    return i;
                }
            }
            if ((testBoard[0] == player && testBoard[4] == player && testBoard[8] == player) ||
                (testBoard[2] == player && testBoard[4] == player && testBoard[6] == player))
            {
                return i;
            }
        }
    }
    return -1;
}
int ttt_update_find_winning_moves(std::vector<int> ttt_positions, int ttt_opponent_choice, int ttt_player_choice)
{
    /*
        Check for winning moves for the AI and Player
        the priority of movement will be
        1. Winning move for AI
        2. Blocking player from winning
        3. Make a random move if neither above condition is true
    */

    int aiWinningMove = ttt_update_possible_winning_moves(ttt_positions, ttt_opponent_choice); // AI check winning moves
    if (aiWinningMove != -1)
    {
        return aiWinningMove;
    }

    int humanWinningMove = ttt_update_possible_winning_moves(ttt_positions, ttt_player_choice); // Human check winning moves
    if (humanWinningMove != -1)
    {
        return humanWinningMove;
    }

    std::vector<int> moves = ttt_update_ai_check_available_moves(ttt_positions);
    if (!moves.empty())
    {
        return moves[rand() % moves.size()];
    }

    return -1;
}
void ttt_update_winning_logic()
{
    /* Tic Tac Toe field positioning
    1 | 2 | 3
    _________
    4 | 5 | 6
    _________
    7 | 8 | 9 */

    // Player win condition
    if ((ttt_positions[0] == ttt_player_choice && ttt_positions[1] == ttt_player_choice && ttt_positions[2] == ttt_player_choice) || // Top row
        (ttt_positions[3] == ttt_player_choice && ttt_positions[4] == ttt_player_choice && ttt_positions[5] == ttt_player_choice) || // Middle row
        (ttt_positions[6] == ttt_player_choice && ttt_positions[7] == ttt_player_choice && ttt_positions[8] == ttt_player_choice) || // Bottom row
        (ttt_positions[0] == ttt_player_choice && ttt_positions[3] == ttt_player_choice && ttt_positions[6] == ttt_player_choice) || // Left column
        (ttt_positions[1] == ttt_player_choice && ttt_positions[4] == ttt_player_choice && ttt_positions[7] == ttt_player_choice) || // Middle column
        (ttt_positions[2] == ttt_player_choice && ttt_positions[5] == ttt_player_choice && ttt_positions[8] == ttt_player_choice) || // Right column
        (ttt_positions[0] == ttt_player_choice && ttt_positions[4] == ttt_player_choice && ttt_positions[8] == ttt_player_choice) || // Diagonal 1
        (ttt_positions[2] == ttt_player_choice && ttt_positions[4] == ttt_player_choice && ttt_positions[6] == ttt_player_choice))   // Diagonal 2
    {
        if (!ttt_game_over)
        {
            std::cout << "Player wins" << std::endl;
            toggle_countdown();
            ttt_game_over = true;
            ttt_winner = 1;
            ttt_opponentsTurn = false;
            Mix_PlayChannel(-1, winGameSound, 0);
        }
    }

    // Opponent win condition
    if ((ttt_positions[0] == ttt_opponent_choice && ttt_positions[1] == ttt_opponent_choice && ttt_positions[2] == ttt_opponent_choice) || // Top row
        (ttt_positions[3] == ttt_opponent_choice && ttt_positions[4] == ttt_opponent_choice && ttt_positions[5] == ttt_opponent_choice) || // Middle row
        (ttt_positions[6] == ttt_opponent_choice && ttt_positions[7] == ttt_opponent_choice && ttt_positions[8] == ttt_opponent_choice) || // Bottom row
        (ttt_positions[0] == ttt_opponent_choice && ttt_positions[3] == ttt_opponent_choice && ttt_positions[6] == ttt_opponent_choice) || // Left column
        (ttt_positions[1] == ttt_opponent_choice && ttt_positions[4] == ttt_opponent_choice && ttt_positions[7] == ttt_opponent_choice) || // Middle column
        (ttt_positions[2] == ttt_opponent_choice && ttt_positions[5] == ttt_opponent_choice && ttt_positions[8] == ttt_opponent_choice) || // Right column
        (ttt_positions[0] == ttt_opponent_choice && ttt_positions[4] == ttt_opponent_choice && ttt_positions[8] == ttt_opponent_choice) || // Diagonal 1
        (ttt_positions[2] == ttt_opponent_choice && ttt_positions[4] == ttt_opponent_choice && ttt_positions[6] == ttt_opponent_choice))   // Diagonal 2
    {
        if (!ttt_game_over)
        {
            std::cout << "Opponent wins" << std::endl;
            toggle_countdown();
            ttt_game_over = true;
            ttt_winner = 2;
            ttt_opponentsTurn = false;
            Mix_PlayChannel(-1, loseGameSound, 0);
        }
    }

    // Draw condition
    if (!ttt_showPopup && !ttt_game_over) // If game has started and not over (to not keep cout "it's a draw")
    {
        bool ttt_all_positions_used = true;
        for (int i = 0; i < ttt_positions.size(); ++i)
        {

            if (ttt_positions[i] == 2) // Check for any empty position
            {
                ttt_all_positions_used = false; // no empty positions
                break;
            }
        }

        if (ttt_all_positions_used && ttt_winner != 1 && ttt_winner != 2)
        {
            toggle_countdown();
            std::cout << "It's a Draw." << std::endl;
            ttt_opponentsTurn = false;
            ttt_game_over = true;
            ttt_winner = 3;
            Mix_PlayChannel(-1, loseGameSound, 0);
        }
    }

    if (ttt_game_over) // Add winner and combination to Frequency rect
    {
        ttt_winner_history.push_back(ttt_winner);

        if (ttt_winner == 1)
        {
            ttt_winner_choice_history.push_back(ttt_player_choice);
        }
        else if (ttt_winner == 2)
        {
            ttt_winner_choice_history.push_back(ttt_opponent_choice);
        }
        else if (ttt_winner == 3)
        {
            ttt_winner_choice_history.push_back(2); // Draw
        }
    }
}
void ttt_update_new_game_reset_variables()
{
    ttt_player_choice = 3;
    ttt_opponent_choice = 3;
    ttt_positions = {2, 2, 2, 2, 2, 2, 2, 2, 2};
    ttt_player_choose_x_or_o = false;
    ttt_winner = 0;
    ttt_game_over = false;
    ttt_opponentsTurn = false;
    ttt_showPopup = true;
    ttt_play_against_human = false;
    ttt_choose_human_or_computer = false;
    ttt_rounds = 0;
    ttt_choose_rounds = false;
    ttt_starting_player_is_x = false;
    ttt_starting_player_chosen = false;
    ttt_timer_set = false;

    // Restart timer - generic function
    timerRunning = false;
    countdownStarted = false;
    countdownSeconds = 30;
}
void ttt_update_is_position_taken(int index)
{
    int gridPosition = index + 1;
    std::string competitor = ttt_opponentsTurn ? "Opponent" : "Player";

    if (ttt_positions[index] == 2)
    {
        int choice = (competitor == "Player") ? ttt_player_choice : ttt_opponent_choice;
        ttt_positions[index] = choice;

        ttt_opponentsTurn = !ttt_opponentsTurn;
    }
    else
    {
        std::cout << competitor << ", Position: " << gridPosition << " is taken. Try again." << std::endl;
    }
}
void ttt_update_ai_logic()
{
    while (ttt_opponentsTurn)
    {
        int aiMoveIndex = ttt_update_find_winning_moves(ttt_positions, ttt_opponent_choice, ttt_player_choice);
        if (aiMoveIndex != -1)
        {
            ttt_update_is_position_taken(aiMoveIndex);
        }
    }
}


// Tic Tac Toe - Handles
void ttt_mouse_handle(int mouseX, int mouseY)
{
    SDL_Point mousePosition = {mouseX, mouseY};

    // ttt - Buttons
    std::vector<SDL_Rect> positionRects = {
        {static_cast<int>(windowWidth * 0.25), static_cast<int>(windowHeight * 0.25), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.45), static_cast<int>(windowHeight * 0.25), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.6), static_cast<int>(windowHeight * 0.25), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.25), static_cast<int>(windowHeight * 0.45), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.45), static_cast<int>(windowHeight * 0.45), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.6), static_cast<int>(windowHeight * 0.45), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.25), static_cast<int>(windowHeight * 0.6), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.45), static_cast<int>(windowHeight * 0.6), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.6), static_cast<int>(windowHeight * 0.6), (windowWidth / 8), (windowHeight / 8)}};

    // Settings - Buttons
    SDL_Rect helpRect = {static_cast<int>(windowWidth * 0.9), static_cast<int>(windowHeight * 0.3), rectWidth, rectHeight};
    SDL_Rect restartRect = {static_cast<int>(windowWidth * 0.9), static_cast<int>(windowHeight * 0.4), rectWidth, rectHeight};
    SDL_Rect settingsRect = {static_cast<int>(windowWidth * 0.9), static_cast<int>(windowHeight * 0.5), rectWidth, rectHeight};
    SDL_Rect worldMapRect = {static_cast<int>(windowWidth * 0.9), static_cast<int>(windowHeight * 0.6), rectWidth, rectHeight};

    // HUD - Buttons
    SDL_Rect timerRect = {static_cast<int>(windowWidth * 0.05), static_cast<int>(windowHeight * 0.05), (windowWidth / 4), (windowHeight / 8)};

    // Popup - Buttons
    SDL_Rect closeButtonRect = {static_cast<int>(windowWidth * 0.68), static_cast<int>(windowHeight * 0.2), rectWidth, rectHeight};
    SDL_Rect ttt_X_rect = {static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.26), rectWidth, rectHeight};
    SDL_Rect ttt_O_rect = {static_cast<int>(windowWidth * 0.36), static_cast<int>(windowHeight * 0.26), rectWidth, rectHeight};
    SDL_Rect ttt_player_start_first_rect = {static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.40), rectWidth, rectHeight};
    SDL_Rect ttt_opponent_start_first_rect = {static_cast<int>(windowWidth * 0.36), static_cast<int>(windowHeight * 0.40), rectWidth, rectHeight};
    SDL_Rect humanRect = {static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.56), rectWidth, rectHeight};
    SDL_Rect computerRect = {static_cast<int>(windowWidth * 0.36), static_cast<int>(windowHeight * 0.56), rectWidth, rectHeight};
    SDL_Rect heartRounds1Rect = {static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.72), rectWidth, rectHeight};
    SDL_Rect heartRounds3Rect = {static_cast<int>(windowWidth * 0.36), static_cast<int>(windowHeight * 0.72), rectWidth, rectHeight};
    SDL_Rect heartRounds5Rect = {static_cast<int>(windowWidth * 0.46), static_cast<int>(windowHeight * 0.72), rectWidth, rectHeight};
    SDL_Rect heartRounds10Rect = {static_cast<int>(windowWidth * 0.56) - buttonXOffset, static_cast<int>(windowHeight * 0.72) - buttonYOffset, buttonWidth, buttonHeight};
    SDL_Rect seconds10Rect = {static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.88), rectWidth, rectHeight};
    SDL_Rect seconds30Rect = {static_cast<int>(windowWidth * 0.31), static_cast<int>(windowHeight * 0.88), rectWidth, rectHeight};
    SDL_Rect seconds60Rect = {static_cast<int>(windowWidth * 0.36), static_cast<int>(windowHeight * 0.88), rectWidth, rectHeight};
    SDL_Rect seconds300Rect = {static_cast<int>(windowWidth * 0.41), static_cast<int>(windowHeight * 0.88), rectWidth, rectHeight};
    SDL_Rect seconds600Rect = {static_cast<int>(windowWidth * 0.46), static_cast<int>(windowHeight * 0.88), rectWidth, rectHeight};
    SDL_Rect seconds1800Rect = {static_cast<int>(windowWidth * 0.51), static_cast<int>(windowHeight * 0.88), rectWidth, rectHeight};
    SDL_Rect seconds3600Rect = {static_cast<int>(windowWidth * 0.56), static_cast<int>(windowHeight * 0.88), rectWidth, rectHeight};

    // Choose X or O to start
    if (!ttt_game_over)
    {
        if (!ttt_showPopup) // main game code
        {
            for (int i = 0; i < positionRects.size(); ++i)
            {
                if (SDL_PointInRect(&mousePosition, &positionRects[i]))
                {
                    int gridPosition = i + 1;
                    std::cout << "Player choose Position: " << gridPosition << std::endl;
                    ttt_update_is_position_taken(i);
                }
            }
        }
        else // Popup code
        {
            if (SDL_PointInRect(&mousePosition, &closeButtonRect))
            {
                if (ttt_player_choose_x_or_o && ttt_starting_player_chosen && ttt_choose_human_or_computer && ttt_choose_rounds && ttt_timer_set)
                {
                    std::cout << "You clicked Close Popup window" << std::endl;
                    ttt_showPopup = false;
                    toggle_countdown();
                }
                else
                {
                    std::cout << "Error: Cannot close popup without choosing new game settings" << std::endl;
                }
            }
            else if (SDL_PointInRect(&mousePosition, &ttt_X_rect))
            {
                std::cout << "You choose: X" << std::endl;
                ttt_player_choice = 1;
                ttt_opponent_choice = 0;
                ttt_player_choose_x_or_o = true;
            }
            else if (SDL_PointInRect(&mousePosition, &ttt_O_rect))
            {
                std::cout << "You choose: O" << std::endl;
                ttt_player_choice = 0;
                ttt_opponent_choice = 1;
                ttt_player_choose_x_or_o = true;
            }
            else if (SDL_PointInRect(&mousePosition, &ttt_player_start_first_rect))
            {
                std::cout << "You choose: Starting player X" << std::endl;
                if (ttt_player_choice == 1)
                {
                    ttt_opponentsTurn = false;
                }
                else if (ttt_player_choice == 0)
                {
                    ttt_opponentsTurn = true;
                }
                ttt_starting_player_is_x = true;
                ttt_starting_player_chosen = true;
            }
            else if (SDL_PointInRect(&mousePosition, &ttt_opponent_start_first_rect))
            {
                std::cout << "You choose: Starting player O" << std::endl;
                if (ttt_player_choice == 0)
                {
                    ttt_opponentsTurn = false;
                }
                else if (ttt_player_choice == 1)
                {
                    ttt_opponentsTurn = true;
                }
                ttt_starting_player_is_x = false;
                ;
                ttt_starting_player_chosen = true;
            }
            else if (SDL_PointInRect(&mousePosition, &humanRect))
            {
                std::cout << "You choose: Play against Human" << std::endl;
                ttt_play_against_human = true;
                ttt_choose_human_or_computer = true;
            }
            else if (SDL_PointInRect(&mousePosition, &computerRect))
            {
                std::cout << "You choose: Play against Computer" << std::endl;
                ttt_play_against_human = false;
                ttt_choose_human_or_computer = true;
            }
            else if (SDL_PointInRect(&mousePosition, &heartRounds1Rect))
            {
                std::cout << "You choose: Play 1 round" << std::endl;
                ttt_choose_rounds = true;
                ttt_rounds = 1;
            }
            else if (SDL_PointInRect(&mousePosition, &heartRounds3Rect))
            {
                std::cout << "You choose: Play 3 rounds" << std::endl;
                ttt_choose_rounds = true;
                ttt_rounds = 3;
            }
            else if (SDL_PointInRect(&mousePosition, &heartRounds5Rect))
            {
                std::cout << "You choose: Play 5 rounds" << std::endl;
                ttt_choose_rounds = true;
                ttt_rounds = 5;
            }
            else if (SDL_PointInRect(&mousePosition, &heartRounds10Rect))
            {
                std::cout << "You choose: Play 10 rounds" << std::endl;
                ttt_choose_rounds = true;
                ttt_rounds = 10;
            }
            else if (SDL_PointInRect(&mousePosition, &seconds10Rect))
            {
                std::cout << "You choose: 10 second timer" << std::endl;
                countdownSeconds = 10;
                ttt_timer_set = true;
            }
            else if (SDL_PointInRect(&mousePosition, &seconds30Rect))
            {
                std::cout << "You choose: 30 second timer" << std::endl;
                countdownSeconds = 30;
                ttt_timer_set = true;
            }
            else if (SDL_PointInRect(&mousePosition, &seconds60Rect))
            {
                std::cout << "You choose: 60 second timer" << std::endl;
                countdownSeconds = 60;
                ttt_timer_set = true;
            }
            else if (SDL_PointInRect(&mousePosition, &seconds300Rect))
            {
                std::cout << "You choose: 5 minutes timer" << std::endl;
                countdownSeconds = 300;
                ttt_timer_set = true;
            }
            else if (SDL_PointInRect(&mousePosition, &seconds600Rect))
            {
                std::cout << "You choose: 10 minutes timer" << std::endl;
                countdownSeconds = 600;
                ttt_timer_set = true;
            }
            else if (SDL_PointInRect(&mousePosition, &seconds1800Rect))
            {
                std::cout << "You choose: 30 minutes timer" << std::endl;
                countdownSeconds = 1800;
                ttt_timer_set = true;
            }
            else if (SDL_PointInRect(&mousePosition, &seconds3600Rect))
            {
                std::cout << "You choose: 1 hour timer" << std::endl;
                countdownSeconds = 3600;
                ttt_timer_set = true;
            }
        }
    }

    // Settings -  Buttons
    if (SDL_PointInRect(&mousePosition, &restartRect))
    {
        std::cout << "You clicked Restart" << std::endl;
        ttt_update_new_game_reset_variables();
    }

    // HUD - Buttons
    if (SDL_PointInRect(&mousePosition, &timerRect))
    {
        std::cout << "You clicked: Restart Timer" << std::endl;
        toggle_countdown();
    }
}
void ttt_keyboard_handle(SDL_Event event)
{
    SDL_Rect ttt_X_rect = {static_cast<int>(windowWidth * 0.4), static_cast<int>(windowHeight * 0.5), (windowWidth / 10), (windowHeight / 10)};
    SDL_Rect ttt_O_rect = {static_cast<int>(windowWidth * 0.5), static_cast<int>(windowHeight * 0.5), (windowWidth / 10), (windowHeight / 10)};

    std::vector<SDL_Rect> positionRects = {
        {static_cast<int>(windowWidth * 0.25), static_cast<int>(windowHeight * 0.25), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.45), static_cast<int>(windowHeight * 0.25), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.6), static_cast<int>(windowHeight * 0.25), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.25), static_cast<int>(windowHeight * 0.45), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.45), static_cast<int>(windowHeight * 0.45), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.6), static_cast<int>(windowHeight * 0.45), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.25), static_cast<int>(windowHeight * 0.6), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.45), static_cast<int>(windowHeight * 0.6), (windowWidth / 8), (windowHeight / 8)},
        {static_cast<int>(windowWidth * 0.6), static_cast<int>(windowHeight * 0.6), (windowWidth / 8), (windowHeight / 8)}};

    SDL_Rect restartRect = {static_cast<int>(windowWidth * 0.9), static_cast<int>(windowHeight * 0.4), (windowWidth / 8), (windowHeight / 8)};

    SDL_Rect closeButtonRect = {static_cast<int>(windowWidth * 0.6), static_cast<int>(windowHeight * 0.35), (windowWidth / 20), (windowHeight / 20)};

    // Highlight the selected option for keyboard arrow key and gamepad d pad key input
    SDL_Rect selectedRect = {
        static_cast<int>(windowWidth * 0.35),
        static_cast<int>(windowHeight * 0.25) + selectedOption * 100, // Spacing (must be bigger then sdl_renders.h same rect)
        static_cast<int>(windowWidth * 0.3),
        static_cast<int>(windowWidth * 0.3)};

    SDL_Point selectedPoint = {selectedRect.x, selectedRect.y}; // This is the selectedRect which overlaps the button rects

    switch (event.key.keysym.sym)
    {
    case SDLK_ESCAPE:
        std::cout << "You clicked button: ESC" << std::endl;
        ttt_SDL_cleanup();
        exit_SDL();
        break;
    case SDLK_UP:
        std::cout << "You clicked button: UP" << std::endl;
        selectedOption = (selectedOption - 1 + menuTotalOptions) % menuTotalOptions;
        break;
    case SDLK_DOWN:
        std::cout << "You clicked button: DOWN" << std::endl;
        selectedOption = (selectedOption + 1) % menuTotalOptions;
        break;
    case SDLK_RETURN:
        std::cout << "You clicked button: RETURN | ENTER" << std::endl;
        if (SDL_PointInRect(&selectedPoint, &restartRect))
        {
            std::cout << "You clicked: Restart game" << std::endl;
            ttt_update_new_game_reset_variables();
        }
        else if (SDL_PointInRect(&selectedPoint, &closeButtonRect))
        {
            std::cout << "You clicked: Close popup" << std::endl;
            ttt_showPopup = false;
        }
        break;
    default:
        std::cout << "You pressed a non-configured keyboard input." << std::endl;
        break;
    }
}
void ttt_gamepad_handle(int button)
{
}

// Tic Tac Toe - SDL Integration
void ttt_SDL_update()
{
    ttt_update_winning_logic();
    if (!ttt_play_against_human && ttt_choose_human_or_computer)
    {
        ttt_update_ai_logic();
    }
    ttt_update_winning_logic();
    songTitle = "assets/sounds/music/Old Rome - PianoAmor.mp3";
    load_music(songTitle);
}
void ttt_SDL_draw()
{
    SDL_RenderCopy(renderer, romeDayBackgroundTexture, NULL, NULL);
    render_text("Ancient Rome - Tic Tac Toe", static_cast<int>(windowWidth * 0.37), static_cast<int>(windowHeight * 0.1));

    // HUD Buttons
    draw_timer(countdownSeconds);
    ttt_draw_settings_buttons();
    draw_win_frequency(ttt_winner_history, ttt_winner_choice_history);
    draw_lives(ttt_rounds);

    ttt_draw_field();

    if (ttt_showPopup)
    {
        ttt_draw_setup_game_popup_window();
    }
    else
    {
        ttt_draw_X_or_O();
    }

    if (ttt_winner == 1)
    {
        render_text("player wins", static_cast<int>(windowWidth * 0.35), static_cast<int>(windowHeight * 0.8));
    }
    else if (ttt_winner == 2)
    {
        render_text("Opponent wins", static_cast<int>(windowWidth * 0.35), static_cast<int>(windowHeight * 0.8));
    }
    else if (ttt_winner == 3)
    {
        render_text("It's a Draw.", static_cast<int>(windowWidth * 0.35), static_cast<int>(windowHeight * 0.8));
    }
}
void ttt_SDL_cleanup()
{
    // ttt_textures
    SDL_DestroyTexture(xTexture);
    SDL_DestroyTexture(oTexture);
    SDL_DestroyTexture(lineTexture);
}

// main.cpp SDL - loop
void handle_events()
{
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            quit = true;
        }
        else if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            ttt_mouse_handle(mouseX, mouseY);
        }
        else if (event.type == SDL_KEYDOWN)
        {
            ttt_keyboard_handle(event);
        }
        else if (event.type == SDL_CONTROLLERBUTTONDOWN)
        {
            SDL_ControllerButtonEvent controllerButtonEvent = event.cbutton;
            int button = controllerButtonEvent.button;
            {
                ttt_gamepad_handle(button);
            }
        }
    }
}
void update()
{
    ttt_SDL_update();
}
void draw()
{
    // Render background colour
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB value for black
    // Clear the renderer
    SDL_RenderClear(renderer);

    // Draw Tic_tac_toe buttons to field
    ttt_SDL_draw();

    // Update the screen
    SDL_RenderPresent(renderer);
}

// main.cpp SDL - main
void run_SDL()
{
    while (!quit)
    {
        // Event handling loop
        handle_events();
        update();
        draw();
    }
}
void exit_SDL()
{
    // Stop the music
    Mix_HaltMusic();
    Mix_FreeMusic(music);

    // Free the sound effect
    Mix_FreeChunk(sfx);
    Mix_FreeChunk(winGameSound);
    Mix_FreeChunk(loseGameSound);

    // Close the audio system
    Mix_CloseAudio();

    // Free fonts
    TTF_CloseFont(font_24);
    TTF_CloseFont(font_36);
    TTF_CloseFont(font_48);

    // Destroy textures
    SDL_DestroyTexture(splashScreenTexture);

    // ttt_textures
    SDL_DestroyTexture(xTexture);
    SDL_DestroyTexture(oTexture);
    SDL_DestroyTexture(lineTexture);

    // Update button
    SDL_DestroyTexture(updateTexture);

    // HUD Buttons
    SDL_DestroyTexture(timerTexture);
    SDL_DestroyTexture(frequencyTexture);
    SDL_DestroyTexture(heartTexture);
    SDL_DestroyTexture(hearts2Texture);
    SDL_DestroyTexture(hearts3Texture);
    SDL_DestroyTexture(hearts4Texture);
    SDL_DestroyTexture(hearts5Texture);
    SDL_DestroyTexture(hearts10Texture);
    SDL_DestroyTexture(helpTexture);
    SDL_DestroyTexture(settingsTexture);
    SDL_DestroyTexture(worldMapTexture);

    // Popup texturea
    SDL_DestroyTexture(humanTexture);
    SDL_DestroyTexture(computerTexture);
    SDL_DestroyTexture(buttonTexture);

    // backgrounds
    SDL_DestroyTexture(romeDayBackgroundTexture); // scene 32

    // Close game controller
    if (controller)
    {
        SDL_GameControllerClose(controller);
    }

    // Destroy renderer and window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Quit SDL subsystems
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char *argv[])
{
    srand(static_cast<unsigned int>(time(0))); // Seed the random number generator using the current time

    find_os();

    start_SDL();
    run_SDL();
    exit_SDL();
    return 0;
}
