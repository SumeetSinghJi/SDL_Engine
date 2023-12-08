
#include <iostream>
#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include "headers/tic_tac_toe.h"
#include <cstdlib> // Include the necessary header for rand() function

/*
    To DO - Tic Tac Toe
    * Add some tutorial messages into a tic_tac_toe.txt
    * on Win draw line
    * Add Timer
    * Add rect player win history
    * Add option to play computer or player
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

// backgrounds
SDL_Texture *romeDayBackgroundTexture = nullptr; // scene 32

// MUSIC SETTINGS VARIABLES
bool isMusicPlaying = NULL;
std::string currentSong = "";                                                 // for music
std::string songTitle = "assets/sounds/music/8 bit dream land - Pixabay.mp3"; // for music

int windowWidth = 1200;
int windowHeight = 800;
bool quit = false;
int fontSize = 24;

SDL_Texture *restartTexture;

// tic_tac_toe_textures
SDL_Texture *tic_tac_toe_position_X_texture;
SDL_Texture *tic_tac_toe_position_O_texture;
SDL_Texture *tic_tac_toe_position_line_texture;

// Game - Tic Tac Toe Variables
int tic_tac_toe_ROWS = 3;
int tic_tac_toe_COLUMNS = 3;
int tic_tac_toe_player_choice = 3;
int tic_tac_toe_opponent_choice = 3;
int tic_tac_toe_position_1 = 2;
int tic_tac_toe_position_2 = 2;
int tic_tac_toe_position_3 = 2;
int tic_tac_toe_position_4 = 2;
int tic_tac_toe_position_5 = 2;
int tic_tac_toe_position_6 = 2;
int tic_tac_toe_position_7 = 2;
int tic_tac_toe_position_8 = 2;
int tic_tac_toe_position_9 = 2;
bool tic_tac_toe_player_choose_x_or_o = false;
bool tic_tac_toe_opponent_won_game = false;
bool tic_tac_toe_player_won_game = false;
bool tic_tac_toe_draw_game = false;
bool tic_tac_toe_game_over = false;
bool tic_tac_toe_player_opponent_game = false;
bool tic_tac_toe_opponentsTurn = false;
// FUNCTION PROTOTYPES
void tic_tac_toe_draw_grid(SDL_Rect &tic_tac_toe_rect);
void tic_tac_tie_load_textures();

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
    restartTexture = load_texture("assets/graphics/buttons/settings/restart-button.png", "Restart Button");
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
    tic_tac_tie_load_textures();
    load_music(songTitle);
    load_sound();
}

void tic_tac_tie_load_textures()
{
    tic_tac_toe_position_X_texture = load_texture("assets/graphics/games/tic_tac_toe/buttons/tic_tac_toe_X_texture.png", "Tic Tac Toe X image");
    tic_tac_toe_position_O_texture = load_texture("assets/graphics/games/tic_tac_toe/buttons/tic_tac_toe_O_texture.png", "Tic Tac Toe O image");
    tic_tac_toe_position_line_texture = load_texture("assets/graphics/games/tic_tac_toe/tic_tac_toe_line_texture.png", "Tic Tac Toe Line image");
    romeDayBackgroundTexture = load_texture("assets/graphics/backgrounds/rome-day.jpg", "Rome Day Background");
}

void tic_tac_toe_draw_field()
{
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // RGB value for play area: light grey
    SDL_Rect tic_tac_toe_rect = {(windowWidth / 4), (windowHeight / 4), (windowWidth / 2), (windowHeight / 2)};
    SDL_RenderFillRect(renderer, &tic_tac_toe_rect);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // RGB value for grid lines: black
    tic_tac_toe_draw_grid(tic_tac_toe_rect);
}
void tic_tac_toe_draw_grid(SDL_Rect &tic_tac_toe_rect)
{
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
void tic_tac_toe_draw_grid_text_positions()
{
    render_text("1", static_cast<int>(windowWidth * 0.3), static_cast<int>(windowHeight * 0.3));
    render_text("2", static_cast<int>(windowWidth * 0.5), static_cast<int>(windowHeight * 0.3));
    render_text("3", static_cast<int>(windowWidth * 0.7), static_cast<int>(windowHeight * 0.3));
    render_text("4", static_cast<int>(windowWidth * 0.3), static_cast<int>(windowHeight * 0.5));
    render_text("5", static_cast<int>(windowWidth * 0.5), static_cast<int>(windowHeight * 0.5));
    render_text("6", static_cast<int>(windowWidth * 0.7), static_cast<int>(windowHeight * 0.5));
    render_text("7", static_cast<int>(windowWidth * 0.3), static_cast<int>(windowHeight * 0.7));
    render_text("8", static_cast<int>(windowWidth * 0.5), static_cast<int>(windowHeight * 0.7));
    render_text("9", static_cast<int>(windowWidth * 0.7), static_cast<int>(windowHeight * 0.7));
}
void tic_tac_toe_draw_choose_x_or_o_first_popup()
{
    if (tic_tac_toe_player_choose_x_or_o == false)
    {
        render_text("Select Crosses (X) or Naughts (0)", static_cast<int>(windowWidth * 0.4), static_cast<int>(windowHeight * 0.75));

        SDL_Rect tic_tac_toe_X_rect = {static_cast<int>(windowWidth * 0.4), static_cast<int>(windowHeight * 0.8), (windowWidth / 10), (windowHeight / 10)};
        SDL_Rect tic_tac_toe_O_rect = {static_cast<int>(windowWidth * 0.5), static_cast<int>(windowHeight * 0.8), (windowWidth / 10), (windowHeight / 10)};

        SDL_RenderCopy(renderer, tic_tac_toe_position_X_texture, nullptr, &tic_tac_toe_X_rect);
        SDL_RenderCopy(renderer, tic_tac_toe_position_O_texture, nullptr, &tic_tac_toe_O_rect);
    }
    else
    {
        if (tic_tac_toe_player_choice == 0)
        {
            render_text("You choose: 0", static_cast<int>(windowWidth * 0.4), static_cast<int>(windowHeight * 0.75));
        }
        else
        {
            render_text("You choose: X", static_cast<int>(windowWidth * 0.4), static_cast<int>(windowHeight * 0.75));
        }
    }
}
void tic_tac_toe_draw_X_or_O()
{

    SDL_Rect restartRect = {static_cast<int>(windowWidth * 0.9), static_cast<int>(windowHeight * 0.4), (windowWidth / 8), (windowHeight / 8)};
    SDL_RenderCopy(renderer, restartTexture, nullptr, &restartRect);

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

    std::vector<int> positions = {
        tic_tac_toe_position_1,
        tic_tac_toe_position_2,
        tic_tac_toe_position_3,
        tic_tac_toe_position_4,
        tic_tac_toe_position_5,
        tic_tac_toe_position_6,
        tic_tac_toe_position_7,
        tic_tac_toe_position_8,
        tic_tac_toe_position_9};

    for (size_t i = 0; i < positions.size(); ++i)
    {
        if (positions[i] == 0)
        {
            SDL_RenderCopy(renderer, tic_tac_toe_position_O_texture, nullptr, &positionRects[i]);
        }
        else if (positions[i] == 1)
        {
            SDL_RenderCopy(renderer, tic_tac_toe_position_X_texture, nullptr, &positionRects[i]);
        }
        else if (positions[i] == 2)
        {
            // Do nothing for position 2
        }
    }
}

void tic_tac_toe_new_game_reset_variables()
{
    tic_tac_toe_player_choice = 3;
    tic_tac_toe_opponent_choice = 3;
    tic_tac_toe_position_1 = 2;
    tic_tac_toe_position_2 = 2;
    tic_tac_toe_position_3 = 2;
    tic_tac_toe_position_4 = 2;
    tic_tac_toe_position_5 = 2;
    tic_tac_toe_position_6 = 2;
    tic_tac_toe_position_7 = 2;
    tic_tac_toe_position_8 = 2;
    tic_tac_toe_position_9 = 2;
    tic_tac_toe_player_choose_x_or_o = false;
    tic_tac_toe_opponent_won_game = false;
    tic_tac_toe_player_won_game = false;
    tic_tac_toe_game_over = false;
    tic_tac_toe_player_opponent_game = false;
    tic_tac_toe_opponentsTurn = false;
}

void tic_tac_toe_mouse_handle(int mouseX, int mouseY)
{
    SDL_Point mousePosition = {mouseX, mouseY};

    SDL_Rect tic_tac_toe_X_rect = {static_cast<int>(windowWidth * 0.4), static_cast<int>(windowHeight * 0.8), (windowWidth / 10), (windowHeight / 10)};
    SDL_Rect tic_tac_toe_O_rect = {static_cast<int>(windowWidth * 0.5), static_cast<int>(windowHeight * 0.8), (windowWidth / 10), (windowHeight / 10)};

    SDL_Rect tic_tac_toe_position_1_rect = {static_cast<int>(windowWidth * 0.25), static_cast<int>(windowHeight * 0.25), (windowWidth / 8), (windowHeight / 8)};
    SDL_Rect tic_tac_toe_position_2_rect = {static_cast<int>(windowWidth * 0.45), static_cast<int>(windowHeight * 0.25), (windowWidth / 8), (windowHeight / 8)};
    SDL_Rect tic_tac_toe_position_3_rect = {static_cast<int>(windowWidth * 0.6), static_cast<int>(windowHeight * 0.25), (windowWidth / 8), (windowHeight / 8)};
    SDL_Rect tic_tac_toe_position_4_rect = {static_cast<int>(windowWidth * 0.25), static_cast<int>(windowHeight * 0.45), (windowWidth / 8), (windowHeight / 8)};
    SDL_Rect tic_tac_toe_position_5_rect = {static_cast<int>(windowWidth * 0.45), static_cast<int>(windowHeight * 0.45), (windowWidth / 8), (windowHeight / 8)};
    SDL_Rect tic_tac_toe_position_6_rect = {static_cast<int>(windowWidth * 0.6), static_cast<int>(windowHeight * 0.45), (windowWidth / 8), (windowHeight / 8)};
    SDL_Rect tic_tac_toe_position_7_rect = {static_cast<int>(windowWidth * 0.25), static_cast<int>(windowHeight * 0.6), (windowWidth / 8), (windowHeight / 8)};
    SDL_Rect tic_tac_toe_position_8_rect = {static_cast<int>(windowWidth * 0.45), static_cast<int>(windowHeight * 0.6), (windowWidth / 8), (windowHeight / 8)};
    SDL_Rect tic_tac_toe_position_9_rect = {static_cast<int>(windowWidth * 0.6), static_cast<int>(windowHeight * 0.6), (windowWidth / 8), (windowHeight / 8)};

    SDL_Rect restartRect = {static_cast<int>(windowWidth * 0.9), static_cast<int>(windowHeight * 0.4), (windowWidth / 8), (windowHeight / 8)};

    // Choose X or O to start
    if (!tic_tac_toe_game_over)
    {
        if (SDL_PointInRect(&mousePosition, &tic_tac_toe_X_rect))
        {
            tic_tac_toe_player_choice = 1;
            tic_tac_toe_opponent_choice = 0;
            tic_tac_toe_player_choose_x_or_o = true;
        }
        else if (SDL_PointInRect(&mousePosition, &tic_tac_toe_O_rect))
        {
            tic_tac_toe_player_choice = 0;
            tic_tac_toe_opponent_choice = 1;
            tic_tac_toe_player_choose_x_or_o = true;
        }

        if (tic_tac_toe_player_choose_x_or_o)
        {
            if (SDL_PointInRect(&mousePosition, &tic_tac_toe_position_1_rect))
            {
                if (tic_tac_toe_player_choice == 0)
                {
                    tic_tac_toe_position_1 = 0;
                }
                else if (tic_tac_toe_player_choice == 1)
                {
                    tic_tac_toe_position_1 = 1;
                }
                std::cout << "Player choose Position 1 value is now: " << tic_tac_toe_position_1 << std::endl;
                if (!tic_tac_toe_game_over)
                {
                    tic_tac_toe_opponentsTurn = true;
                }
                SDL_Delay(1000);
            }
            else if (SDL_PointInRect(&mousePosition, &tic_tac_toe_position_2_rect))
            {
                if (tic_tac_toe_player_choice == 0)
                {
                    tic_tac_toe_position_2 = 0;
                }
                else if (tic_tac_toe_player_choice == 1)
                {
                    tic_tac_toe_position_2 = 1;
                }
                std::cout << "Player choose Position 2 value is now: " << tic_tac_toe_position_2 << std::endl;
                if (!tic_tac_toe_game_over)
                {
                    tic_tac_toe_opponentsTurn = true;
                }
                SDL_Delay(1000);
            }
            else if (SDL_PointInRect(&mousePosition, &tic_tac_toe_position_3_rect))
            {
                if (tic_tac_toe_player_choice == 0)
                {
                    tic_tac_toe_position_3 = 0;
                }
                else if (tic_tac_toe_player_choice == 1)
                {
                    tic_tac_toe_position_3 = 1;
                }
                std::cout << "Player choose Position 3 value is now: " << tic_tac_toe_position_3 << std::endl;
                if (!tic_tac_toe_game_over)
                {
                    tic_tac_toe_opponentsTurn = true;
                }
                SDL_Delay(1000);
            }
            else if (SDL_PointInRect(&mousePosition, &tic_tac_toe_position_4_rect))
            {
                if (tic_tac_toe_player_choice == 0)
                {
                    tic_tac_toe_position_4 = 0;
                }
                else if (tic_tac_toe_player_choice == 1)
                {
                    tic_tac_toe_position_4 = 1;
                }
                std::cout << "Player choose Position 4 value is now: " << tic_tac_toe_position_4 << std::endl;
                if (!tic_tac_toe_game_over)
                {
                    tic_tac_toe_opponentsTurn = true;
                }
                SDL_Delay(1000);
            }
            else if (SDL_PointInRect(&mousePosition, &tic_tac_toe_position_5_rect))
            {
                if (tic_tac_toe_player_choice == 0)
                {
                    tic_tac_toe_position_5 = 0;
                }
                else if (tic_tac_toe_player_choice == 1)
                {
                    tic_tac_toe_position_5 = 1;
                }
                std::cout << "Player choose Position 5 value is now: " << tic_tac_toe_position_5 << std::endl;
                if (!tic_tac_toe_game_over)
                {
                    tic_tac_toe_opponentsTurn = true;
                }
                SDL_Delay(1000);
            }
            else if (SDL_PointInRect(&mousePosition, &tic_tac_toe_position_6_rect))
            {
                if (tic_tac_toe_player_choice == 0)
                {
                    tic_tac_toe_position_6 = 0;
                }
                else if (tic_tac_toe_player_choice == 1)
                {
                    tic_tac_toe_position_6 = 1;
                }
                std::cout << "Player choose Position 6 value is now: " << tic_tac_toe_position_6 << std::endl;
                if (!tic_tac_toe_game_over)
                {
                    tic_tac_toe_opponentsTurn = true;
                }
                SDL_Delay(1000);
            }
            else if (SDL_PointInRect(&mousePosition, &tic_tac_toe_position_7_rect))
            {
                if (tic_tac_toe_player_choice == 0)
                {
                    tic_tac_toe_position_7 = 0;
                }
                else if (tic_tac_toe_player_choice == 1)
                {
                    tic_tac_toe_position_7 = 1;
                }
                std::cout << "Player choose Position 7 value is now: " << tic_tac_toe_position_7 << std::endl;
                if (!tic_tac_toe_game_over)
                {
                    tic_tac_toe_opponentsTurn = true;
                }
                SDL_Delay(1000);
            }
            else if (SDL_PointInRect(&mousePosition, &tic_tac_toe_position_8_rect))
            {
                if (tic_tac_toe_player_choice == 0)
                {
                    tic_tac_toe_position_8 = 0;
                }
                else if (tic_tac_toe_player_choice == 1)
                {
                    tic_tac_toe_position_8 = 1;
                }
                std::cout << "Player choose Position 8 value is now: " << tic_tac_toe_position_8 << std::endl;
                if (!tic_tac_toe_game_over)
                {
                    tic_tac_toe_opponentsTurn = true;
                }
                SDL_Delay(1000);
            }
            else if (SDL_PointInRect(&mousePosition, &tic_tac_toe_position_9_rect))
            {
                if (tic_tac_toe_player_choice == 0)
                {
                    tic_tac_toe_position_9 = 0;
                }
                else if (tic_tac_toe_player_choice == 1)
                {
                    tic_tac_toe_position_9 = 1;
                }
                std::cout << "Player choose Position 9 value is now: " << tic_tac_toe_position_9 << std::endl;
                if (!tic_tac_toe_game_over)
                {
                    tic_tac_toe_opponentsTurn = true;
                }
                SDL_Delay(1000);
            }
        }
    }
    // Settings Buttons
    if (SDL_PointInRect(&mousePosition, &restartRect))
    {
        std::cout << "You clicked Restart" << std::endl;
        tic_tac_toe_new_game_reset_variables();
    }
}

void tic_tac_toe_update_winning_logic()
{
    /* Tic Tac Toe field positioning
    1 | 2 | 3
    _________
    4 | 5 | 6
    _________
    7 | 8 | 9 */

    // Player win condition
    if ((tic_tac_toe_position_1 == tic_tac_toe_player_choice && tic_tac_toe_position_2 == tic_tac_toe_player_choice && tic_tac_toe_position_3 == tic_tac_toe_player_choice) || // Top row
        (tic_tac_toe_position_4 == tic_tac_toe_player_choice && tic_tac_toe_position_5 == tic_tac_toe_player_choice && tic_tac_toe_position_6 == tic_tac_toe_player_choice) || // Middle row
        (tic_tac_toe_position_7 == tic_tac_toe_player_choice && tic_tac_toe_position_8 == tic_tac_toe_player_choice && tic_tac_toe_position_9 == tic_tac_toe_player_choice) || // Bottom row
        (tic_tac_toe_position_1 == tic_tac_toe_player_choice && tic_tac_toe_position_4 == tic_tac_toe_player_choice && tic_tac_toe_position_7 == tic_tac_toe_player_choice) || // Left column
        (tic_tac_toe_position_2 == tic_tac_toe_player_choice && tic_tac_toe_position_5 == tic_tac_toe_player_choice && tic_tac_toe_position_8 == tic_tac_toe_player_choice) || // Middle column
        (tic_tac_toe_position_3 == tic_tac_toe_player_choice && tic_tac_toe_position_6 == tic_tac_toe_player_choice && tic_tac_toe_position_9 == tic_tac_toe_player_choice) || // Right column
        (tic_tac_toe_position_1 == tic_tac_toe_player_choice && tic_tac_toe_position_5 == tic_tac_toe_player_choice && tic_tac_toe_position_9 == tic_tac_toe_player_choice) || // Diagonal 1
        (tic_tac_toe_position_3 == tic_tac_toe_player_choice && tic_tac_toe_position_5 == tic_tac_toe_player_choice && tic_tac_toe_position_7 == tic_tac_toe_player_choice))   // Diagonal 2
    {
        if (!tic_tac_toe_game_over)
        {
            std::cout << "Player wins" << std::endl;
            tic_tac_toe_game_over = true;
            tic_tac_toe_player_won_game = true;
            tic_tac_toe_opponentsTurn = false;
            Mix_PlayChannel(-1, winGameSound, 0);
        }
    }

    // opponent win condition
    if ((tic_tac_toe_position_1 == tic_tac_toe_opponent_choice && tic_tac_toe_position_2 == tic_tac_toe_opponent_choice && tic_tac_toe_position_3 == tic_tac_toe_opponent_choice) || // Top row
        (tic_tac_toe_position_4 == tic_tac_toe_opponent_choice && tic_tac_toe_position_5 == tic_tac_toe_opponent_choice && tic_tac_toe_position_6 == tic_tac_toe_opponent_choice) || // Middle row
        (tic_tac_toe_position_7 == tic_tac_toe_opponent_choice && tic_tac_toe_position_8 == tic_tac_toe_opponent_choice && tic_tac_toe_position_9 == tic_tac_toe_opponent_choice) || // Bottom row
        (tic_tac_toe_position_1 == tic_tac_toe_opponent_choice && tic_tac_toe_position_4 == tic_tac_toe_opponent_choice && tic_tac_toe_position_7 == tic_tac_toe_opponent_choice) || // Left column
        (tic_tac_toe_position_2 == tic_tac_toe_opponent_choice && tic_tac_toe_position_5 == tic_tac_toe_opponent_choice && tic_tac_toe_position_8 == tic_tac_toe_opponent_choice) || // Middle column
        (tic_tac_toe_position_3 == tic_tac_toe_opponent_choice && tic_tac_toe_position_6 == tic_tac_toe_opponent_choice && tic_tac_toe_position_9 == tic_tac_toe_opponent_choice) || // Right column
        (tic_tac_toe_position_1 == tic_tac_toe_opponent_choice && tic_tac_toe_position_5 == tic_tac_toe_opponent_choice && tic_tac_toe_position_9 == tic_tac_toe_opponent_choice) || // Diagonal 1
        (tic_tac_toe_position_3 == tic_tac_toe_opponent_choice && tic_tac_toe_position_5 == tic_tac_toe_opponent_choice && tic_tac_toe_position_7 == tic_tac_toe_opponent_choice))   // Diagonal 2
    {
        if (!tic_tac_toe_game_over)
        {
            std::cout << "Opponent wins" << std::endl;
            tic_tac_toe_game_over = true;
            tic_tac_toe_opponent_won_game = true;
            tic_tac_toe_opponentsTurn = false;
            Mix_PlayChannel(-1, loseGameSound, 0);
        }
    }

    // Draw condition
    if (tic_tac_toe_player_choose_x_or_o) // If game has started
    {
        if (tic_tac_toe_position_1 != 2 && tic_tac_toe_position_2 != 2 && tic_tac_toe_position_3 != 2 &&
            tic_tac_toe_position_4 != 2 && tic_tac_toe_position_5 != 2 && tic_tac_toe_position_6 != 2 &&
            tic_tac_toe_position_7 != 2 && tic_tac_toe_position_8 != 2 && tic_tac_toe_position_9 != 2)
        {
            if (!tic_tac_toe_player_won_game && !tic_tac_toe_opponent_won_game)
            {
                std::cout << "It's a Draw." << std::endl;
                tic_tac_toe_game_over = true;
                tic_tac_toe_draw_game = true;
                tic_tac_toe_opponentsTurn = false;
                Mix_PlayChannel(-1, loseGameSound, 0);
            }
        }
    }
}

void tic_tac_toe_update_ai_logic()
{
    if (tic_tac_toe_player_choice == 1)
    {
        tic_tac_toe_opponent_choice = 0;
    }
    else if (tic_tac_toe_player_choice == 0)
    {
        tic_tac_toe_opponent_choice = 1;
    }

    std::vector<int> positionsTaken = {0, 9};

    while (tic_tac_toe_opponentsTurn)
    {
        int tic_tac_toe_ai_position_choice = rand() % 9 + 1;

        switch (tic_tac_toe_ai_position_choice)
        {
        case 1:
            if (tic_tac_toe_position_1 != tic_tac_toe_player_choice && tic_tac_toe_position_1 != tic_tac_toe_opponent_choice)
            {
                tic_tac_toe_position_1 = tic_tac_toe_opponent_choice;
                std::cout << "Opponent chooses Position 1. value is now: " << tic_tac_toe_opponent_choice << std::endl;
                tic_tac_toe_opponentsTurn = false;
                positionsTaken.push_back(0);
            }
            else
            {
                std::cout << "Opponent chooses Position 1, but it's taken." << std::endl;
            }
            break;
        case 2:
            if (tic_tac_toe_position_2 != tic_tac_toe_player_choice && tic_tac_toe_position_2 != tic_tac_toe_opponent_choice)
            {
                tic_tac_toe_position_2 = tic_tac_toe_opponent_choice;
                std::cout << "Opponent chooses Position 2. value is now: " << tic_tac_toe_opponent_choice << std::endl;
                tic_tac_toe_opponentsTurn = false;
                positionsTaken.push_back(1);
            }
            else
            {
                std::cout << "Opponent chooses Position 2, but it's taken." << std::endl;
            }
            break;
        case 3:
            if (tic_tac_toe_position_3 != tic_tac_toe_player_choice && tic_tac_toe_position_3 != tic_tac_toe_opponent_choice)
            {
                tic_tac_toe_position_3 = tic_tac_toe_opponent_choice;
                std::cout << "Opponent chooses Position 3. value is now: " << tic_tac_toe_opponent_choice << std::endl;
                tic_tac_toe_opponentsTurn = false;
                positionsTaken.push_back(2);
            }
            else
            {
                std::cout << "Opponent chooses Position 3, but it's taken." << std::endl;
            }
            break;
        case 4:
            if (tic_tac_toe_position_4 != tic_tac_toe_player_choice && tic_tac_toe_position_4 != tic_tac_toe_opponent_choice)
            {
                tic_tac_toe_position_4 = tic_tac_toe_opponent_choice;
                std::cout << "Opponent chooses Position 4. value is now: " << tic_tac_toe_opponent_choice << std::endl;
                tic_tac_toe_opponentsTurn = false;
                positionsTaken.push_back(3);
            }
            else
            {
                std::cout << "Opponent chooses Position 4, but it's taken." << std::endl;
            }
            break;
        case 5:
            if (tic_tac_toe_position_5 != tic_tac_toe_player_choice && tic_tac_toe_position_5 != tic_tac_toe_opponent_choice)
            {
                tic_tac_toe_position_5 = tic_tac_toe_opponent_choice;
                std::cout << "Opponent chooses Position 5. value is now: " << tic_tac_toe_opponent_choice << std::endl;
                tic_tac_toe_opponentsTurn = false;
                positionsTaken.push_back(4);
            }
            else
            {
                std::cout << "Opponent chooses Position 5, but it's taken." << std::endl;
            }
            break;
        case 6:
            if (tic_tac_toe_position_6 != tic_tac_toe_player_choice && tic_tac_toe_position_6 != tic_tac_toe_opponent_choice)
            {
                tic_tac_toe_position_6 = tic_tac_toe_opponent_choice;
                std::cout << "Opponent chooses Position 6. value is now: " << tic_tac_toe_opponent_choice << std::endl;
                tic_tac_toe_opponentsTurn = false;
                positionsTaken.push_back(5);
            }
            else
            {
                std::cout << "Opponent chooses Position 6, but it's taken." << std::endl;
            }
            break;
        case 7:
            if (tic_tac_toe_position_7 != tic_tac_toe_player_choice && tic_tac_toe_position_7 != tic_tac_toe_opponent_choice)
            {
                tic_tac_toe_position_7 = tic_tac_toe_opponent_choice;
                std::cout << "Opponent chooses Position 7. value is now: " << tic_tac_toe_opponent_choice << std::endl;
                tic_tac_toe_opponentsTurn = false;
                positionsTaken.push_back(6);
            }
            else
            {
                std::cout << "Opponent chooses Position 7, but it's taken." << std::endl;
            }
            break;
        case 8:
            if (tic_tac_toe_position_8 != tic_tac_toe_player_choice && tic_tac_toe_position_8 != tic_tac_toe_opponent_choice)
            {
                tic_tac_toe_position_8 = tic_tac_toe_opponent_choice;
                std::cout << "Opponent chooses Position 8. value is now: " << tic_tac_toe_opponent_choice << std::endl;
                tic_tac_toe_opponentsTurn = false;
                positionsTaken.push_back(7);
            }
            else
            {
                std::cout << "Opponent chooses Position 8, but it's taken." << std::endl;
            }
            break;
        case 9:
            if (tic_tac_toe_position_9 != tic_tac_toe_player_choice && tic_tac_toe_position_9 != tic_tac_toe_opponent_choice)
            {
                tic_tac_toe_position_9 = tic_tac_toe_opponent_choice;
                std::cout << "Opponent chooses Position 9. value is now: " << tic_tac_toe_opponent_choice << std::endl;
                tic_tac_toe_opponentsTurn = false;
                positionsTaken.push_back(8);
            }
            else
            {
                std::cout << "Opponent chooses Position 9, but it's taken." << std::endl;
            }
            break;
        }
        if (positionsTaken.size() == 10)
        {
            tic_tac_toe_opponentsTurn = false;
        }
    }
}

void tic_tac_toe_SDL_draw()
{
    SDL_RenderCopy(renderer, romeDayBackgroundTexture, NULL, NULL);
    render_text("Ancient Rome - Tic Tac Toe", static_cast<int>(windowWidth * 0.4), static_cast<int>(windowHeight * 0.1));
    tic_tac_toe_draw_field();
    // tic_tac_toe_draw_grid_text_positions(); - for debugging only
    tic_tac_toe_draw_X_or_O();
    tic_tac_toe_draw_choose_x_or_o_first_popup();

    if (tic_tac_toe_player_won_game)
    {
        render_text("player wins", static_cast<int>(windowWidth * 0.35), static_cast<int>(windowHeight * 0.8));
    }
    else if (tic_tac_toe_opponent_won_game)
    {
        render_text("Opponent wins", static_cast<int>(windowWidth * 0.35), static_cast<int>(windowHeight * 0.8));
    }
    else if (tic_tac_toe_draw_game)
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

void tic_tac_toe_cleanup()
{
    // tic_tac_toe_textures
    SDL_DestroyTexture(tic_tac_toe_position_X_texture);
    SDL_DestroyTexture(tic_tac_toe_position_O_texture);
    SDL_DestroyTexture(tic_tac_toe_position_line_texture);
}

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
