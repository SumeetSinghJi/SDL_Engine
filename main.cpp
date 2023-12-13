#include <iostream>
#include <string>
#include <vector>  // for
#include <cstdlib> // Include the necessary header for rand() function
#include <thread>  // for timer
#include <chrono>  // for timer
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

/*
    To DO - Tic Tac Toe
    * Popup expand
    *   Popup only appears when lives are 0; advising who won round
    *   Play against: Computer | Human
    *   Who starts first 1 or 2 - oppponentTurn = xx
    *   Countdown timer, 10, 15, 20, 30, 60, 120, custom
    *   Lives 1, 2, 3, 5, 10, custom
    * ChatGPT - History, Rules, Trivia
    * on Win draw line
    * Draw lives
    * AI logic to do winning moves
    * Parralex image background, settings button on or off
    * voice act rules
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

int windowWidth = 1200;
int windowHeight = 800;
bool quit = false;
int fontSize = 24;

// Timer
bool timerRunning = false;
bool countdownStarted = false;
int countdownSeconds = 20; // Initial countdown time

// Declare variables to track player choices
bool xSelected = false; // Declare variables to track player choices
bool oSelected = false; // Declare variables to track player choices

// Tic Tac Toe - Textures
SDL_Texture *tic_tac_toe_position_X_texture;
SDL_Texture *tic_tac_toe_position_O_texture;
SDL_Texture *tic_tac_toe_position_line_texture;
SDL_Texture *tic_tac_toe_starting_player_texture;

// HUD textures
SDL_Texture *restartTexture = nullptr;   // In game - restart game button
SDL_Texture *timerTexture = nullptr;     // In game - timer box
SDL_Texture *frequencyTexture = nullptr; // In game - frequency box
SDL_Texture *helpTexture = nullptr;      // In game - frequency box
SDL_Texture *settingsTexture = nullptr;  // In game - frequency box
SDL_Texture *worldMapTexture = nullptr;  // In game - frequency box
SDL_Texture *heartTexture = nullptr;     // In game - hearts representing lives if game uses lives

// Tic Tac Toe - Global variables
int tic_tac_toe_ROWS = 3;
int tic_tac_toe_COLUMNS = 3;
int tic_tac_toe_player_choice = 3;
int tic_tac_toe_opponent_choice = 3;
std::vector<int> tic_tac_toe_positions = {2, 2, 2, 2, 2, 2, 2, 2, 2};
bool tic_tac_toe_player_choose_x_or_o = false;
int tic_tac_toe_winner = 0; // 1 = player, 2 = opponent, 3 = Draw
std::vector<int> tic_tac_toe_winner_history;
std::vector<int> tic_tac_toe_winner_choice_history;
bool tic_tac_toe_game_over = false;
bool tic_tac_toe_opponentsTurn = false;
bool tic_tac_toe_showPopup = true;
int tic_tac_toe_choose_lives = 3;
int selectedOption = 0;    // For Keyboard arrow key or Gamepad d-pad selection
int menuTotalOptions = 13; // For Keyboard arrow key or Gamepad d-pad selection

// FUNCTION PROTOTYPES
void tic_tac_toe_load_textures();
void end_SDL();

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
void load_textures()
{
    // Splash screen
    splashScreenTexture = load_texture("assets/graphics/backgrounds/AgniSamooh-HD-logo.png", "Developer Splash screen");
    // HUD textures
    restartTexture = load_texture("assets/graphics/buttons/settings/restart-button.png", "Restart Button");
    timerTexture = load_texture("assets/graphics/HUD/timer.png", "Timer Button");
    heartTexture = load_texture("assets/graphics/HUD/heart.png", "Heart");
    frequencyTexture = load_texture("assets/graphics/HUD/frequency.png", "Frequency");
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
    load_textures();
    tic_tac_toe_load_textures();
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
                if(!tic_tac_toe_showPopup) {
                    tic_tac_toe_game_over = true;
                    tic_tac_toe_winner = 3; // draw
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

// main.cpp HUD Functions
void draw_timer()
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
    int heartWidth = windowWidth / 26; // Width of each heart
    int xHeartSpacing = 5; // Adjust this value for spacing between hearts

    SDL_Rect heartRect = { initialHeartX, static_cast<int>(windowHeight * 0.15), heartWidth, heartWidth };

    for (int i = 0; i < lives; i++) {
        heartRect.x = initialHeartX + (heartWidth + xHeartSpacing) * i;
        SDL_RenderCopy(renderer, heartTexture, nullptr, &heartRect);
    }
}


// Tic Tac Toe - Draws
void tic_tac_toe_load_textures()
{
    tic_tac_toe_position_X_texture = load_texture("assets/graphics/games/tic_tac_toe/buttons/tic_tac_toe_X_texture.png", "Tic Tac Toe X image");
    tic_tac_toe_position_O_texture = load_texture("assets/graphics/games/tic_tac_toe/buttons/tic_tac_toe_O_texture.png", "Tic Tac Toe O image");
    tic_tac_toe_position_line_texture = load_texture("assets/graphics/games/tic_tac_toe/tic_tac_toe_line_texture.png", "Tic Tac Toe Line image");
    tic_tac_toe_starting_player_texture = load_texture("assets/graphics/games/tic_tac_toe/tic_tac_toe_line_texture.png", "Tic Tac Toe Line image");
    romeDayBackgroundTexture = load_texture("assets/graphics/backgrounds/rome-day.jpg", "Rome Day Background");
}
void tic_tac_toe_draw_field()
{
    // Play area
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Set the blend mode to enable transparency
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 64);       // RGB value: light grey, last digit (Alpha value = 256 / 4) (75% transparency)
    SDL_Rect tic_tac_toe_rect = {(windowWidth / 4), (windowHeight / 4), (windowWidth / 2), (windowHeight / 2)};
    SDL_RenderFillRect(renderer, &tic_tac_toe_rect);

    // Grid lines
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB value: Black

    // Calculate the number of squares inside the Tic Tac Toe rectangle
    int innertic_tac_toe_COLUMNS = tic_tac_toe_COLUMNS;
    int innertic_tac_toe_ROWS = tic_tac_toe_ROWS;

    // Calculate the size of each square
    int squareWidth = tic_tac_toe_rect.w / tic_tac_toe_COLUMNS;
    int squareHeight = tic_tac_toe_rect.h / tic_tac_toe_ROWS;

    // Draw vertical lines within the Tic Tac Toe rectangle
    for (int i = 1; i < innertic_tac_toe_COLUMNS; ++i)
    {
        SDL_RenderDrawLine(renderer, tic_tac_toe_rect.x + i * squareWidth, tic_tac_toe_rect.y, tic_tac_toe_rect.x + i * squareWidth, tic_tac_toe_rect.y + tic_tac_toe_rect.h);
    }

    // Draw horizontal lines within the Tic Tac Toe rectangle
    for (int i = 1; i < innertic_tac_toe_ROWS; ++i)
    {
        SDL_RenderDrawLine(renderer, tic_tac_toe_rect.x, tic_tac_toe_rect.y + i * squareHeight, tic_tac_toe_rect.x + tic_tac_toe_rect.w, tic_tac_toe_rect.y + i * squareHeight);
    }
}
void tic_tac_toe_draw_setup_game_popup_window()
{
    // DONT FORGET TO UPDATE mouse handles rects within world-games.cpp

    // Draw popup black border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_Rect borderRect = {(windowWidth / 4) - 2, (windowHeight / 4) - 2, (windowWidth / 2) + 4, (windowHeight / 2) + 4};
    SDL_RenderFillRect(renderer, &borderRect);

    // draw popup
    SDL_SetRenderDrawColor(renderer, 144, 238, 144, 255); // Popup color lime green
    SDL_Rect popupRect = {(windowWidth / 4), (windowHeight / 4), (windowWidth / 2), (windowHeight / 2)};
    SDL_RenderFillRect(renderer, &popupRect);

    // Draw a close button
    SDL_Rect closeButtonRect = {static_cast<int>(windowWidth * 0.69), static_cast<int>(windowHeight * 0.28), (windowWidth / 22), (windowHeight / 22)};
    SDL_RenderCopy(renderer, tic_tac_toe_position_X_texture, nullptr, &closeButtonRect);

    /*

        Choose X or O

    */
    if (!tic_tac_toe_player_choose_x_or_o)
    {
        render_text("Select Crosses (X) or Naughts (0)", static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.26));
    }
    else // after picking choice advise player what they choose
    {
        if (tic_tac_toe_player_choice == 0)
        {
            render_text("You choose: 0", static_cast<int>(windowWidth * 0.69), static_cast<int>(windowHeight * 0.28));
            // highlight selected rect, and in if condition dont hide, so player can repick.
        }
        else
        {
            render_text("You choose: X", static_cast<int>(windowWidth * 0.69), static_cast<int>(windowHeight * 0.28));
            // highlight selected rect, and in if condition dont hide, so player can repick.
        }
    }

    // X button
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect XborderLineRect = {static_cast<int>(windowWidth * 0.32) - 4, static_cast<int>(windowHeight * 0.34) - 4, (windowWidth / 18) + 8, (windowHeight / 18) + 8};
    SDL_RenderFillRect(renderer, &XborderLineRect);

    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect XborderRect = {static_cast<int>(windowWidth * 0.32) - 2, static_cast<int>(windowHeight * 0.34) - 2, (windowWidth / 18) + 4, (windowHeight / 18) + 4};
    SDL_RenderFillRect(renderer, &XborderRect);

    SDL_Rect tic_tac_toe_X_rect = {static_cast<int>(windowWidth * 0.32), static_cast<int>(windowHeight * 0.34), (windowWidth / 18), (windowHeight / 18)};
    SDL_RenderCopy(renderer, tic_tac_toe_position_X_texture, nullptr, &tic_tac_toe_X_rect);

    // O button
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect OborderLineRect = {static_cast<int>(windowWidth * 0.42) - 4, static_cast<int>(windowHeight * 0.34) - 4, (windowWidth / 18) + 8, (windowHeight / 18) + 8};
    SDL_RenderFillRect(renderer, &OborderLineRect);

    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect OborderRect = {static_cast<int>(windowWidth * 0.42) - 2, static_cast<int>(windowHeight * 0.34) - 2, (windowWidth / 18) + 4, (windowHeight / 18) + 4};
    SDL_RenderFillRect(renderer, &OborderRect);

    SDL_Rect tic_tac_toe_O_rect = {static_cast<int>(windowWidth * 0.42), static_cast<int>(windowHeight * 0.34), (windowWidth / 18), (windowHeight / 18)};
    SDL_RenderCopy(renderer, tic_tac_toe_position_O_texture, nullptr, &tic_tac_toe_O_rect);

    /*

        Starting player

    */

    render_text("Starting player", static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.44));

    // player starts first
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect startingPlayerBorderLineRect = {static_cast<int>(windowWidth * 0.32) - 4, static_cast<int>(windowHeight * 0.54) - 4, (windowWidth / 18) + 8, (windowHeight / 18) + 8};
    SDL_RenderFillRect(renderer, &startingPlayerBorderLineRect);

    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect startingPlayerBorderRect = {static_cast<int>(windowWidth * 0.32) - 2, static_cast<int>(windowHeight * 0.54) - 2, (windowWidth / 18) + 4, (windowHeight / 18) + 4};
    SDL_RenderFillRect(renderer, &startingPlayerBorderRect);

    SDL_Rect tic_tac_toe_player_start_first_rect = {static_cast<int>(windowWidth * 0.32), static_cast<int>(windowHeight * 0.54), (windowWidth / 18), (windowHeight / 18)};
    SDL_RenderCopy(renderer, tic_tac_toe_starting_player_texture, nullptr, &tic_tac_toe_player_start_first_rect);

    // Opponent starts first
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect startingOpponentBorderLineRect = {static_cast<int>(windowWidth * 0.42) - 4, static_cast<int>(windowHeight * 0.54) - 4, (windowWidth / 18) + 8, (windowHeight / 18) + 8};
    SDL_RenderFillRect(renderer, &startingOpponentBorderLineRect);

    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect startingOpponentBorderRect = {static_cast<int>(windowWidth * 0.42) - 2, static_cast<int>(windowHeight * 0.54) - 2, (windowWidth / 18) + 4, (windowHeight / 18) + 4};
    SDL_RenderFillRect(renderer, &startingOpponentBorderRect);

    SDL_Rect tic_tac_toe_opponent_start_first_rect = {static_cast<int>(windowWidth * 0.42), static_cast<int>(windowHeight * 0.54), (windowWidth / 18), (windowHeight / 18)};
    SDL_RenderCopy(renderer, tic_tac_toe_starting_player_texture, nullptr, &tic_tac_toe_opponent_start_first_rect);

    /*

        Play against

    

   render_text("Play against", static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.44));

    // player starts first
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect startingPlayerBorderLineRect = {static_cast<int>(windowWidth * 0.32) - 4, static_cast<int>(windowHeight * 0.54) - 4, (windowWidth / 18) + 8, (windowHeight / 18) + 8};
    SDL_RenderFillRect(renderer, &startingPlayerBorderLineRect);

    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect startingPlayerBorderRect = {static_cast<int>(windowWidth * 0.32) - 2, static_cast<int>(windowHeight * 0.54) - 2, (windowWidth / 18) + 4, (windowHeight / 18) + 4};
    SDL_RenderFillRect(renderer, &startingPlayerBorderRect);

    SDL_Rect tic_tac_toe_player_start_first_rect = {static_cast<int>(windowWidth * 0.32), static_cast<int>(windowHeight * 0.54), (windowWidth / 18), (windowHeight / 18)};
    SDL_RenderCopy(renderer, tic_tac_toe_starting_player_texture, nullptr, &tic_tac_toe_player_start_first_rect);

    // Opponent starts first
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect startingOpponentBorderLineRect = {static_cast<int>(windowWidth * 0.42) - 4, static_cast<int>(windowHeight * 0.54) - 4, (windowWidth / 18) + 8, (windowHeight / 18) + 8};
    SDL_RenderFillRect(renderer, &startingOpponentBorderLineRect);

    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect startingOpponentBorderRect = {static_cast<int>(windowWidth * 0.42) - 2, static_cast<int>(windowHeight * 0.54) - 2, (windowWidth / 18) + 4, (windowHeight / 18) + 4};
    SDL_RenderFillRect(renderer, &startingOpponentBorderRect);

    SDL_Rect tic_tac_toe_opponent_start_first_rect = {static_cast<int>(windowWidth * 0.42), static_cast<int>(windowHeight * 0.54), (windowWidth / 18), (windowHeight / 18)};
    SDL_RenderCopy(renderer, tic_tac_toe_starting_player_texture, nullptr, &tic_tac_toe_opponent_start_first_rect);

    */


    /*

        Rounds | Lives

    */

    


    /*

        Timer

    

    render_text("Timer", static_cast<int>(windowWidth * 0.26), static_cast<int>(windowHeight * 0.44));

    // 2 seconds
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect startingPlayerBorderLineRect = {static_cast<int>(windowWidth * 0.32) - 4, static_cast<int>(windowHeight * 0.54) - 4, (windowWidth / 18) + 8, (windowHeight / 18) + 8};
    SDL_RenderFillRect(renderer, &startingPlayerBorderLineRect);

    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect startingPlayerBorderRect = {static_cast<int>(windowWidth * 0.32) - 2, static_cast<int>(windowHeight * 0.54) - 2, (windowWidth / 18) + 4, (windowHeight / 18) + 4};
    SDL_RenderFillRect(renderer, &startingPlayerBorderRect);

    SDL_Rect tic_tac_toe_player_start_first_rect = {static_cast<int>(windowWidth * 0.32), static_cast<int>(windowHeight * 0.54), (windowWidth / 18), (windowHeight / 18)};
    SDL_RenderCopy(renderer, tic_tac_toe_starting_player_texture, nullptr, &tic_tac_toe_player_start_first_rect);

    // 5 seconds
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect startingPlayerBorderLineRect = {static_cast<int>(windowWidth * 0.32) - 4, static_cast<int>(windowHeight * 0.54) - 4, (windowWidth / 18) + 8, (windowHeight / 18) + 8};
    SDL_RenderFillRect(renderer, &startingPlayerBorderLineRect);

    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect startingPlayerBorderRect = {static_cast<int>(windowWidth * 0.32) - 2, static_cast<int>(windowHeight * 0.54) - 2, (windowWidth / 18) + 4, (windowHeight / 18) + 4};
    SDL_RenderFillRect(renderer, &startingPlayerBorderRect);

    SDL_Rect tic_tac_toe_player_start_first_rect = {static_cast<int>(windowWidth * 0.32), static_cast<int>(windowHeight * 0.54), (windowWidth / 18), (windowHeight / 18)};
    SDL_RenderCopy(renderer, tic_tac_toe_starting_player_texture, nullptr, &tic_tac_toe_player_start_first_rect);

    // 10 seconds
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect startingPlayerBorderLineRect = {static_cast<int>(windowWidth * 0.32) - 4, static_cast<int>(windowHeight * 0.54) - 4, (windowWidth / 18) + 8, (windowHeight / 18) + 8};
    SDL_RenderFillRect(renderer, &startingPlayerBorderLineRect);

    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect startingPlayerBorderRect = {static_cast<int>(windowWidth * 0.32) - 2, static_cast<int>(windowHeight * 0.54) - 2, (windowWidth / 18) + 4, (windowHeight / 18) + 4};
    SDL_RenderFillRect(renderer, &startingPlayerBorderRect);

    SDL_Rect tic_tac_toe_player_start_first_rect = {static_cast<int>(windowWidth * 0.32), static_cast<int>(windowHeight * 0.54), (windowWidth / 18), (windowHeight / 18)};
    SDL_RenderCopy(renderer, tic_tac_toe_starting_player_texture, nullptr, &tic_tac_toe_player_start_first_rect);

    // 15 seconds
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect startingOpponentBorderLineRect = {static_cast<int>(windowWidth * 0.42) - 4, static_cast<int>(windowHeight * 0.54) - 4, (windowWidth / 18) + 8, (windowHeight / 18) + 8};
    SDL_RenderFillRect(renderer, &startingOpponentBorderLineRect);

    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect startingOpponentBorderRect = {static_cast<int>(windowWidth * 0.42) - 2, static_cast<int>(windowHeight * 0.54) - 2, (windowWidth / 18) + 4, (windowHeight / 18) + 4};
    SDL_RenderFillRect(renderer, &startingOpponentBorderRect);

    SDL_Rect tic_tac_toe_opponent_start_first_rect = {static_cast<int>(windowWidth * 0.42), static_cast<int>(windowHeight * 0.54), (windowWidth / 18), (windowHeight / 18)};
    SDL_RenderCopy(renderer, tic_tac_toe_starting_player_texture, nullptr, &tic_tac_toe_opponent_start_first_rect);

    // 20 seconds
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect startingOpponentBorderLineRect = {static_cast<int>(windowWidth * 0.42) - 4, static_cast<int>(windowHeight * 0.54) - 4, (windowWidth / 18) + 8, (windowHeight / 18) + 8};
    SDL_RenderFillRect(renderer, &startingOpponentBorderLineRect);

    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect startingOpponentBorderRect = {static_cast<int>(windowWidth * 0.42) - 2, static_cast<int>(windowHeight * 0.54) - 2, (windowWidth / 18) + 4, (windowHeight / 18) + 4};
    SDL_RenderFillRect(renderer, &startingOpponentBorderRect);

    SDL_Rect tic_tac_toe_opponent_start_first_rect = {static_cast<int>(windowWidth * 0.42), static_cast<int>(windowHeight * 0.54), (windowWidth / 18), (windowHeight / 18)};
    SDL_RenderCopy(renderer, tic_tac_toe_starting_player_texture, nullptr, &tic_tac_toe_opponent_start_first_rect);

    // 30 seconds
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect startingOpponentBorderLineRect = {static_cast<int>(windowWidth * 0.42) - 4, static_cast<int>(windowHeight * 0.54) - 4, (windowWidth / 18) + 8, (windowHeight / 18) + 8};
    SDL_RenderFillRect(renderer, &startingOpponentBorderLineRect);

    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect startingOpponentBorderRect = {static_cast<int>(windowWidth * 0.42) - 2, static_cast<int>(windowHeight * 0.54) - 2, (windowWidth / 18) + 4, (windowHeight / 18) + 4};
    SDL_RenderFillRect(renderer, &startingOpponentBorderRect);

    SDL_Rect tic_tac_toe_opponent_start_first_rect = {static_cast<int>(windowWidth * 0.42), static_cast<int>(windowHeight * 0.54), (windowWidth / 18), (windowHeight / 18)};
    SDL_RenderCopy(renderer, tic_tac_toe_starting_player_texture, nullptr, &tic_tac_toe_opponent_start_first_rect);

    // 60 seconds
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect startingOpponentBorderLineRect = {static_cast<int>(windowWidth * 0.42) - 4, static_cast<int>(windowHeight * 0.54) - 4, (windowWidth / 18) + 8, (windowHeight / 18) + 8};
    SDL_RenderFillRect(renderer, &startingOpponentBorderLineRect);

    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect startingOpponentBorderRect = {static_cast<int>(windowWidth * 0.42) - 2, static_cast<int>(windowHeight * 0.54) - 2, (windowWidth / 18) + 4, (windowHeight / 18) + 4};
    SDL_RenderFillRect(renderer, &startingOpponentBorderRect);

    SDL_Rect tic_tac_toe_opponent_start_first_rect = {static_cast<int>(windowWidth * 0.42), static_cast<int>(windowHeight * 0.54), (windowWidth / 18), (windowHeight / 18)};
    SDL_RenderCopy(renderer, tic_tac_toe_starting_player_texture, nullptr, &tic_tac_toe_opponent_start_first_rect);

    // 120 seconds | 2 minutes
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect startingOpponentBorderLineRect = {static_cast<int>(windowWidth * 0.42) - 4, static_cast<int>(windowHeight * 0.54) - 4, (windowWidth / 18) + 8, (windowHeight / 18) + 8};
    SDL_RenderFillRect(renderer, &startingOpponentBorderLineRect);

    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect startingOpponentBorderRect = {static_cast<int>(windowWidth * 0.42) - 2, static_cast<int>(windowHeight * 0.54) - 2, (windowWidth / 18) + 4, (windowHeight / 18) + 4};
    SDL_RenderFillRect(renderer, &startingOpponentBorderRect);

    SDL_Rect tic_tac_toe_opponent_start_first_rect = {static_cast<int>(windowWidth * 0.42), static_cast<int>(windowHeight * 0.54), (windowWidth / 18), (windowHeight / 18)};
    SDL_RenderCopy(renderer, tic_tac_toe_starting_player_texture, nullptr, &tic_tac_toe_opponent_start_first_rect);

    // 300 seconds | 5 minutes
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect startingOpponentBorderLineRect = {static_cast<int>(windowWidth * 0.42) - 4, static_cast<int>(windowHeight * 0.54) - 4, (windowWidth / 18) + 8, (windowHeight / 18) + 8};
    SDL_RenderFillRect(renderer, &startingOpponentBorderLineRect);

    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect startingOpponentBorderRect = {static_cast<int>(windowWidth * 0.42) - 2, static_cast<int>(windowHeight * 0.54) - 2, (windowWidth / 18) + 4, (windowHeight / 18) + 4};
    SDL_RenderFillRect(renderer, &startingOpponentBorderRect);

    SDL_Rect tic_tac_toe_opponent_start_first_rect = {static_cast<int>(windowWidth * 0.42), static_cast<int>(windowHeight * 0.54), (windowWidth / 18), (windowHeight / 18)};
    SDL_RenderCopy(renderer, tic_tac_toe_starting_player_texture, nullptr, &tic_tac_toe_opponent_start_first_rect);

    // 600 seconds | 10 minutes
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB: Black
    SDL_Rect startingOpponentBorderLineRect = {static_cast<int>(windowWidth * 0.42) - 4, static_cast<int>(windowHeight * 0.54) - 4, (windowWidth / 18) + 8, (windowHeight / 18) + 8};
    SDL_RenderFillRect(renderer, &startingOpponentBorderLineRect);

    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // RGB: Light grey
    SDL_Rect startingOpponentBorderRect = {static_cast<int>(windowWidth * 0.42) - 2, static_cast<int>(windowHeight * 0.54) - 2, (windowWidth / 18) + 4, (windowHeight / 18) + 4};
    SDL_RenderFillRect(renderer, &startingOpponentBorderRect);

    SDL_Rect tic_tac_toe_opponent_start_first_rect = {static_cast<int>(windowWidth * 0.42), static_cast<int>(windowHeight * 0.54), (windowWidth / 18), (windowHeight / 18)};
    SDL_RenderCopy(renderer, tic_tac_toe_starting_player_texture, nullptr, &tic_tac_toe_opponent_start_first_rect);

    */
}

void tic_tac_toe_draw_X_or_O()
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
    for (size_t i = 0; i < tic_tac_toe_positions.size(); ++i)
    {
        if (tic_tac_toe_positions[i] == 0)
        {
            SDL_RenderCopy(renderer, tic_tac_toe_position_O_texture, nullptr, &positionRects[i]);
        }
        else if (tic_tac_toe_positions[i] == 1)
        {
            SDL_RenderCopy(renderer, tic_tac_toe_position_X_texture, nullptr, &positionRects[i]);
        }
        else if (tic_tac_toe_positions[i] == 2)
        {
            // Do nothing for position 2
        }
    }
}
void tic_tac_toe_draw_settings_buttons()
{
    SDL_Rect helpRect = {static_cast<int>(windowWidth * 0.9), static_cast<int>(windowHeight * 0.3), (windowWidth / 8), (windowHeight / 8)};
    SDL_Rect restartRect = {static_cast<int>(windowWidth * 0.9), static_cast<int>(windowHeight * 0.4), (windowWidth / 8), (windowHeight / 8)};
    SDL_Rect settingsRect = {static_cast<int>(windowWidth * 0.9), static_cast<int>(windowHeight * 0.5), (windowWidth / 8), (windowHeight / 8)};
    SDL_Rect worldMapRect = {static_cast<int>(windowWidth * 0.9), static_cast<int>(windowHeight * 0.6), (windowWidth / 8), (windowHeight / 8)};

    SDL_RenderCopy(renderer, helpTexture, nullptr, &helpRect);
    SDL_RenderCopy(renderer, restartTexture, nullptr, &restartRect);
    SDL_RenderCopy(renderer, settingsTexture, nullptr, &settingsRect);
    SDL_RenderCopy(renderer, worldMapTexture, nullptr, &worldMapRect);
}

// Tic Tac Toe - Updates
void tic_tac_toe_update_winning_logic()
{
    /* Tic Tac Toe field positioning
    1 | 2 | 3
    _________
    4 | 5 | 6
    _________
    7 | 8 | 9 */

    // Player win condition
    if ((tic_tac_toe_positions[0] == tic_tac_toe_player_choice && tic_tac_toe_positions[1] == tic_tac_toe_player_choice && tic_tac_toe_positions[2] == tic_tac_toe_player_choice) || // Top row
        (tic_tac_toe_positions[3] == tic_tac_toe_player_choice && tic_tac_toe_positions[4] == tic_tac_toe_player_choice && tic_tac_toe_positions[5] == tic_tac_toe_player_choice) || // Middle row
        (tic_tac_toe_positions[6] == tic_tac_toe_player_choice && tic_tac_toe_positions[7] == tic_tac_toe_player_choice && tic_tac_toe_positions[8] == tic_tac_toe_player_choice) || // Bottom row
        (tic_tac_toe_positions[0] == tic_tac_toe_player_choice && tic_tac_toe_positions[3] == tic_tac_toe_player_choice && tic_tac_toe_positions[6] == tic_tac_toe_player_choice) || // Left column
        (tic_tac_toe_positions[1] == tic_tac_toe_player_choice && tic_tac_toe_positions[4] == tic_tac_toe_player_choice && tic_tac_toe_positions[7] == tic_tac_toe_player_choice) || // Middle column
        (tic_tac_toe_positions[2] == tic_tac_toe_player_choice && tic_tac_toe_positions[5] == tic_tac_toe_player_choice && tic_tac_toe_positions[8] == tic_tac_toe_player_choice) || // Right column
        (tic_tac_toe_positions[0] == tic_tac_toe_player_choice && tic_tac_toe_positions[4] == tic_tac_toe_player_choice && tic_tac_toe_positions[8] == tic_tac_toe_player_choice) || // Diagonal 1
        (tic_tac_toe_positions[2] == tic_tac_toe_player_choice && tic_tac_toe_positions[4] == tic_tac_toe_player_choice && tic_tac_toe_positions[6] == tic_tac_toe_player_choice))   // Diagonal 2
    {
        if (!tic_tac_toe_game_over)
        {
            std::cout << "Player wins" << std::endl;
            toggle_countdown();
            tic_tac_toe_game_over = true;
            tic_tac_toe_winner = 1;
            tic_tac_toe_opponentsTurn = false;
            Mix_PlayChannel(-1, winGameSound, 0);
        }
    }

    // Opponent win condition
    if ((tic_tac_toe_positions[0] == tic_tac_toe_opponent_choice && tic_tac_toe_positions[1] == tic_tac_toe_opponent_choice && tic_tac_toe_positions[2] == tic_tac_toe_opponent_choice) || // Top row
        (tic_tac_toe_positions[3] == tic_tac_toe_opponent_choice && tic_tac_toe_positions[4] == tic_tac_toe_opponent_choice && tic_tac_toe_positions[5] == tic_tac_toe_opponent_choice) || // Middle row
        (tic_tac_toe_positions[6] == tic_tac_toe_opponent_choice && tic_tac_toe_positions[7] == tic_tac_toe_opponent_choice && tic_tac_toe_positions[8] == tic_tac_toe_opponent_choice) || // Bottom row
        (tic_tac_toe_positions[0] == tic_tac_toe_opponent_choice && tic_tac_toe_positions[3] == tic_tac_toe_opponent_choice && tic_tac_toe_positions[6] == tic_tac_toe_opponent_choice) || // Left column
        (tic_tac_toe_positions[1] == tic_tac_toe_opponent_choice && tic_tac_toe_positions[4] == tic_tac_toe_opponent_choice && tic_tac_toe_positions[7] == tic_tac_toe_opponent_choice) || // Middle column
        (tic_tac_toe_positions[2] == tic_tac_toe_opponent_choice && tic_tac_toe_positions[5] == tic_tac_toe_opponent_choice && tic_tac_toe_positions[8] == tic_tac_toe_opponent_choice) || // Right column
        (tic_tac_toe_positions[0] == tic_tac_toe_opponent_choice && tic_tac_toe_positions[4] == tic_tac_toe_opponent_choice && tic_tac_toe_positions[8] == tic_tac_toe_opponent_choice) || // Diagonal 1
        (tic_tac_toe_positions[2] == tic_tac_toe_opponent_choice && tic_tac_toe_positions[4] == tic_tac_toe_opponent_choice && tic_tac_toe_positions[6] == tic_tac_toe_opponent_choice))   // Diagonal 2
    {
        if (!tic_tac_toe_game_over)
        {
            std::cout << "Opponent wins" << std::endl;
            toggle_countdown();
            tic_tac_toe_game_over = true;
            tic_tac_toe_winner = 2;
            tic_tac_toe_opponentsTurn = false;
            Mix_PlayChannel(-1, loseGameSound, 0);
        }
    }

    // Draw condition
    if (!tic_tac_toe_showPopup && !tic_tac_toe_game_over) // If game has started and not over (to not keep cout "it's a draw")
    {
        bool tic_tac_toe_all_positions_used = true;
        for (int i = 0; i < tic_tac_toe_positions.size(); ++i)
        {
            
            if (tic_tac_toe_positions[i] == 2) // Check for any empty position
            {
                tic_tac_toe_all_positions_used = false; // no empty positions
                break;
            }
        }

        if (tic_tac_toe_all_positions_used && tic_tac_toe_winner != 1 && tic_tac_toe_winner != 2)
        {
            toggle_countdown();
            std::cout << "It's a Draw." << std::endl;
            tic_tac_toe_opponentsTurn = false;
            tic_tac_toe_game_over = true;
            tic_tac_toe_winner = 3;
            Mix_PlayChannel(-1, loseGameSound, 0);
        }
    }

    if (tic_tac_toe_game_over) // Add winner and combination to Frequency rect
    {
        tic_tac_toe_winner_history.push_back(tic_tac_toe_winner);

        if (tic_tac_toe_winner == 1)
        {
            tic_tac_toe_winner_choice_history.push_back(tic_tac_toe_player_choice);
        }
        else if (tic_tac_toe_winner == 2)
        {
            tic_tac_toe_winner_choice_history.push_back(tic_tac_toe_opponent_choice);
        }
        else if (tic_tac_toe_winner == 3)
        {
            tic_tac_toe_winner_choice_history.push_back(2); // Draw
        }
    }
}
void tic_tac_toe_update_new_game_reset_variables()
{
    tic_tac_toe_player_choice = 3;
    tic_tac_toe_opponent_choice = 3;
    tic_tac_toe_positions = {2, 2, 2, 2, 2, 2, 2, 2, 2};
    tic_tac_toe_player_choose_x_or_o = false;
    tic_tac_toe_winner = 0;
    tic_tac_toe_game_over = false;
    tic_tac_toe_opponentsTurn = false;
    tic_tac_toe_showPopup = true;

    // Restart timer - generic function
    timerRunning = false;
    countdownStarted = false;
    countdownSeconds = 20;
}
void tic_tac_toe_update_is_position_taken(int index)
{
    int gridPosition = index + 1;
    std::string competitor = tic_tac_toe_opponentsTurn ? "Opponent" : "Player";

    if (tic_tac_toe_positions[index] == 2)
    {
        int choice = (competitor == "Player") ? tic_tac_toe_player_choice : tic_tac_toe_opponent_choice;
        tic_tac_toe_positions[index] = choice;

        tic_tac_toe_opponentsTurn = !tic_tac_toe_opponentsTurn;
    }
    else
    {
        std::cout << competitor << ", Position: " << gridPosition << " is taken. Try again." << std::endl;
    }
}
void tic_tac_toe_update_ai_logic()
{
    while (tic_tac_toe_opponentsTurn)
    {
        int i = rand() % 9; // look for 0 - 8 only as tic_tac_toe_positions vector is from 0 - 8 (9 values)
        int gridPosition = i + 1;
        std::cout << "AI choose Position: " << gridPosition << std::endl;
        tic_tac_toe_update_is_position_taken(i);
    }
}

// Tic Tac Toe - Handles
void tic_tac_toe_mouse_handle(int mouseX, int mouseY)
{
    SDL_Point mousePosition = {mouseX, mouseY};

    SDL_Rect tic_tac_toe_X_rect = {static_cast<int>(windowWidth * 0.32), static_cast<int>(windowHeight * 0.34), (windowWidth / 18), (windowHeight / 18)};
    SDL_Rect tic_tac_toe_O_rect = {static_cast<int>(windowWidth * 0.42), static_cast<int>(windowHeight * 0.34), (windowWidth / 18), (windowHeight / 18)};

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
    SDL_Rect timerRect = {static_cast<int>(windowWidth * 0.05), static_cast<int>(windowHeight * 0.05), (windowWidth / 4), (windowHeight / 8)};

    SDL_Rect closeButtonRect = {static_cast<int>(windowWidth * 0.69), static_cast<int>(windowHeight * 0.28), (windowWidth / 22), (windowHeight / 22)};

    // Choose X or O to start
    if (!tic_tac_toe_game_over)
    {
        if (!tic_tac_toe_showPopup) // main game code
        {
            for (int i = 0; i < positionRects.size(); ++i)
            {
                if (SDL_PointInRect(&mousePosition, &positionRects[i]))
                {
                    int gridPosition = i + 1;
                    std::cout << "Player choose Position: " << gridPosition << std::endl;
                    tic_tac_toe_update_is_position_taken(i);
                }
            }
        }
        else // Popup code
        {
            if (SDL_PointInRect(&mousePosition, &closeButtonRect))
            {
                if (tic_tac_toe_player_choose_x_or_o)
                {
                    std::cout << "You clicked Close Popup window" << std::endl;
                    tic_tac_toe_showPopup = false;
                    toggle_countdown();
                }
                else
                {
                    std::cout << "Error: Cannot close popup without choosing X or O." << std::endl;
                }
            }
            else if (SDL_PointInRect(&mousePosition, &tic_tac_toe_X_rect))
            {
                std::cout << "You choose X" << std::endl;
                tic_tac_toe_player_choice = 1;
                tic_tac_toe_opponent_choice = 0;
                tic_tac_toe_player_choose_x_or_o = true;
            }
            else if (SDL_PointInRect(&mousePosition, &tic_tac_toe_O_rect))
            {
                std::cout << "You choose O" << std::endl;
                tic_tac_toe_player_choice = 0;
                tic_tac_toe_opponent_choice = 1;
                tic_tac_toe_player_choose_x_or_o = true;
            }
        }
    }
    // Settings Buttons
    if (SDL_PointInRect(&mousePosition, &restartRect))
    {
        std::cout << "You clicked Restart" << std::endl;
        tic_tac_toe_update_new_game_reset_variables();
    }
    else if (SDL_PointInRect(&mousePosition, &timerRect))
    {
        std::cout << "You clicked: Restart Timer" << std::endl;
        toggle_countdown();
    }
    // Timer buttons
}
void tic_tac_toe_keyboard_handle(SDL_Event event)
{
    SDL_Rect tic_tac_toe_X_rect = {static_cast<int>(windowWidth * 0.4), static_cast<int>(windowHeight * 0.5), (windowWidth / 10), (windowHeight / 10)};
    SDL_Rect tic_tac_toe_O_rect = {static_cast<int>(windowWidth * 0.5), static_cast<int>(windowHeight * 0.5), (windowWidth / 10), (windowHeight / 10)};

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
        end_SDL();
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
            tic_tac_toe_update_new_game_reset_variables();
        }
        else if (SDL_PointInRect(&selectedPoint, &closeButtonRect))
        {
            std::cout << "You clicked: Close popup" << std::endl;
            tic_tac_toe_showPopup = false;
        }
        break;
    default:
        std::cout << "You pressed a non-configured keyboard input." << std::endl;
        break;
    }
}
void tic_tac_toe_gamepad_handle(int button)
{
}


// Tic Tac Toe - SDL Integration
void tic_tac_toe_SDL_draw()
{
    SDL_RenderCopy(renderer, romeDayBackgroundTexture, NULL, NULL);
    render_text("Ancient Rome - Tic Tac Toe", static_cast<int>(windowWidth * 0.37), static_cast<int>(windowHeight * 0.1));

    // HUD Buttons
    draw_timer();
    draw_lives(tic_tac_toe_choose_lives);
    tic_tac_toe_draw_settings_buttons();
    draw_win_frequency(tic_tac_toe_winner_history, tic_tac_toe_winner_choice_history);
    // draw_lives(game_uses_lives);

    tic_tac_toe_draw_field();

    if (tic_tac_toe_showPopup)
    {
        tic_tac_toe_draw_setup_game_popup_window();
    }
    else
    {
        tic_tac_toe_draw_X_or_O();
    }

    if (tic_tac_toe_winner == 1)
    {
        render_text("player wins", static_cast<int>(windowWidth * 0.35), static_cast<int>(windowHeight * 0.8));
    }
    else if (tic_tac_toe_winner == 2)
    {
        render_text("Opponent wins", static_cast<int>(windowWidth * 0.35), static_cast<int>(windowHeight * 0.8));
    }
    else if (tic_tac_toe_winner == 3)
    {
        render_text("It's a Draw.", static_cast<int>(windowWidth * 0.35), static_cast<int>(windowHeight * 0.8));
    }
}
void tic_tac_toe_SDL_update()
{
    tic_tac_toe_update_winning_logic();
    tic_tac_toe_update_ai_logic();
    tic_tac_toe_update_winning_logic();
    songTitle = "assets/sounds/music/Old Rome - PianoAmor.mp3";
    load_music(songTitle);
}
void tic_tac_toe_SDL_cleanup()
{
    // tic_tac_toe_textures
    SDL_DestroyTexture(tic_tac_toe_position_X_texture);
    SDL_DestroyTexture(tic_tac_toe_position_O_texture);
    SDL_DestroyTexture(tic_tac_toe_position_line_texture);
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
            tic_tac_toe_mouse_handle(mouseX, mouseY);
        }
        else if (event.type == SDL_KEYDOWN)
        {
            tic_tac_toe_keyboard_handle(event);
        }
        else if (event.type == SDL_CONTROLLERBUTTONDOWN)
        {
            SDL_ControllerButtonEvent controllerButtonEvent = event.cbutton;
            int button = controllerButtonEvent.button;
            {
                tic_tac_toe_gamepad_handle(button);
            }
        }
    }
}
void update()
{
    tic_tac_toe_SDL_update();
}
void draw()
{
    // Render background colour
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // RGB value for Grey
    // Clear the renderer
    SDL_RenderClear(renderer);

    // Draw Tic_tac_toe buttons to field
    tic_tac_toe_SDL_draw();

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
void end_SDL()
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

    // tic_tac_toe_textures
    SDL_DestroyTexture(tic_tac_toe_position_X_texture);
    SDL_DestroyTexture(tic_tac_toe_position_O_texture);
    SDL_DestroyTexture(tic_tac_toe_position_line_texture);
    SDL_DestroyTexture(tic_tac_toe_starting_player_texture);

    // HUD Buttons
    SDL_DestroyTexture(timerTexture);
    SDL_DestroyTexture(frequencyTexture);
    SDL_DestroyTexture(heartTexture);
    SDL_DestroyTexture(helpTexture);
    SDL_DestroyTexture(settingsTexture);
    SDL_DestroyTexture(worldMapTexture);

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

    start_SDL();
    run_SDL();
    end_SDL();
    return 0;
}
