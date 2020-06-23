//Include SDL.h
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Game functions
void UpdateRect(SDL_Rect* rect, SDL_Renderer* renderer);
void CreateRect(SDL_Rect* rect, int x, int y, int w, int h);
int Random(int min, int max);

//Required main to SDL
int main(int argc, char* args[])
{
    //Ptr to the window that we'll instance
    SDL_Window* window = NULL;
    //Ptr to the surface in the window
    SDL_Surface* screenSurface = NULL;
    //Ptr to the renderer in the window
    SDL_Renderer* screenRenderer = NULL;
    bool exit = false;
    srand(time(NULL));
    //First call to rand() doesn't seem to be random at all, 
    //so we're calling it here in an attempt to make next calls less deterministic
    rand();

    //Init SDL just with SDL's video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize. SDL_Error: %s\n", SDL_GetError());
    }
    else
    {
        //Create window
        window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            printf("Window could not be created. SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
            //Get window surface and renderer
            screenSurface = SDL_GetWindowSurface(window);
            screenRenderer = SDL_CreateRenderer(window,-1,0);
            SDL_Rect snake;
            SDL_Rect food;
            CreateRect(&snake, SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 15, 15);
            CreateRect(&food, Random(0, SCREEN_WIDTH - 5), Random(0, SCREEN_HEIGHT - 5), 5, 5);
            //Fill enitre surface (NULL) with solid color
            SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x0,0x0,0x0));
            //Update the surface
            while(!exit)
            {
                SDL_Event event;
                while (SDL_PollEvent(&event))
                {
                    switch (event.type)
                    {
                        case SDL_QUIT:
                            exit = true;
                            break;
                        case SDL_KEYDOWN:;
                            SDL_Keysym keysym = event.key.keysym;
                            switch (keysym.scancode)
                            {
                                case SDL_SCANCODE_DOWN:
                                    snake.y += 10;
                                    if (snake.y > (SCREEN_HEIGHT - snake.h))
                                    {
                                        snake.y = SCREEN_HEIGHT - snake.h;
                                    }
                                    break;
                                case SDL_SCANCODE_LEFT:
                                    snake.x -= 10;
                                    if (snake.x < 0)
                                    {
                                        snake.x = 0;
                                    }
                                    break;
                                case SDL_SCANCODE_RIGHT:
                                    snake.x += 10;
                                    if (snake.x > (SCREEN_WIDTH - snake.w))
                                    {
                                        snake.x = SCREEN_WIDTH - snake.w;
                                    }
                                    break;
                                case SDL_SCANCODE_UP:
                                    snake.y -= 10;
                                    if (snake.y < 0)
                                    {
                                        snake.y = 0;
                                    }
                                    break;
                            }
                            break;
                    }
                    if (SDL_HasIntersection(&snake, &food))
                    {
                        food.x = Random(0, SCREEN_WIDTH - 5);
                        food.y = Random(0, SCREEN_HEIGHT - 5);
                    }
                }
                SDL_SetRenderDrawColor(screenRenderer, 0x00, 0x00, 0x00, 0xFF);
                SDL_RenderClear(screenRenderer);
                UpdateRect(&snake, screenRenderer);
                UpdateRect(&food, screenRenderer);
                SDL_RenderPresent(screenRenderer);
            }
        }
    }

    //Destroy window
    SDL_DestroyRenderer(screenRenderer);
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}
//TODO: Use SDL_Color structure to receive background color and rect color
void UpdateRect(SDL_Rect* rect, SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer, rect);
    SDL_RenderDrawRect(renderer, rect);
}

void CreateRect(SDL_Rect* rect, int x, int y, int w, int h)
{
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
}

int Random(int min, int max)
{
    return (int)((max + 1 - min)*((float)rand()/RAND_MAX) + min );
}
