//Include SDL.h
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
int gameVel = 240;

//Enums
enum Direction
{
    UP,
    DOWN,
    LEFT,
    RIGHT
};

//Node
struct node {
    SDL_Rect rect;
    int pos;
    int lastX;
    int lastY;
    struct node* next;
} typedef Node;

//Game functions
int Random(int min, int max);
SDL_Rect CreateRect(int x, int y, int w, int h);
void UpdateRect(SDL_Rect* rect, SDL_Renderer* renderer);
Node* CreateSnake();
void UpdateSnake(Node** headPtr, enum Direction currDir, bool* exitPtr);
void AddNode(Node** head);
void newGame(SDL_Window** windowPtr);

//Required main to SDL
int main(int argc, char* args[])
{
    //Ptr to the window that we'll instance
    SDL_Window* window = NULL;
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
        // printf("Game velocity (suggested 240, > -> slow): ");
        // scanf("%d", &gameVel);
        //Create window
        window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            printf("Window could not be created. SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
            newGame(&window);
        }
    }

    //Destroy window
    SDL_DestroyWindow(window);

    //Quit SDL subsystems
    SDL_Quit();

    return 0;
}

int Random(int min, int max)
{
    return (int)((max + 1 - min)*((float)rand()/RAND_MAX) + min );
}

SDL_Rect CreateRect(int x, int y, int w, int h)
{
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    
    return rect;
}

//TODO: Use SDL_Color structure to receive background color and rect color
void UpdateRect(SDL_Rect* rect, SDL_Renderer* renderer)
{
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(renderer, rect);
    SDL_SetRenderDrawColor(renderer, 0x0, 0x0, 0xFF, 0xFF);
    SDL_RenderDrawRect(renderer, rect);
}

Node* CreateSnake()
{
    Node* head = (Node*)malloc(sizeof(Node));
    head->next = NULL;
    head->pos = 0;
    head->rect = CreateRect(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 15, 15);
    head->lastX = head->rect.x;
    head->lastY = head->rect.y;
    return head;
}

void UpdateSnake(Node** headPtr, enum Direction currDir, bool* exitPtr)
{
    int dx = 0, dy = 0;
    switch(currDir)
    {
        case UP:
            dy = -15;
            break;
        case DOWN:
            dy = +15;
            break;
        case LEFT:
            dx = -15;
            break;
        case RIGHT:
            dx = +15;
            break;
    }

    Node* aux = (*headPtr)->next;
    (*headPtr)->lastX = (*headPtr)->rect.x;
    (*headPtr)->lastY = (*headPtr)->rect.y;
    (*headPtr)->rect.x += dx;
    (*headPtr)->rect.y += dy;
    int lastX = (*headPtr)->lastX;
    int lastY = (*headPtr)->lastY;

    while(aux != NULL)
    {
        if (SDL_HasIntersection(&((*headPtr)->rect), &(aux->rect)))
        {
            *exitPtr = true;
        }
        aux->lastX = aux->rect.x;
        aux->lastY = aux->rect.y;
        aux->rect.x = lastX;
        aux->rect.y = lastY;
        lastX = aux->lastX;
        lastY = aux->lastY;
        aux = aux->next;
    }
}

void AddNode(Node** head)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    Node* aux = *head;
    newNode->next = NULL;
    while(aux->next != NULL)
    {
        aux = aux->next;
    }
    //printf("pos %d\n", aux->pos);
    aux->next = newNode;
    newNode->lastX = 0;
    newNode->lastY = 0;
    newNode->pos = aux->pos + 1;
    newNode->rect = CreateRect(aux->lastX, aux->lastY, 15, 15);
}

void newGame(SDL_Window** windowPtr)
{
    SDL_Surface* screenSurface = NULL;
    SDL_Renderer* screenRenderer = NULL;
    screenSurface = SDL_GetWindowSurface(*windowPtr);
    screenRenderer = SDL_CreateRenderer(*windowPtr,-1,0);
    //Fill enitre surface (NULL) with solid color
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x0,0x0,0x0));
    
    Node* head = CreateSnake();
    SDL_Rect food = CreateRect(Random(0, SCREEN_WIDTH - 5), Random(0, SCREEN_HEIGHT - 5), 5, 5);
    enum Direction currDir = RIGHT;
    int frame = 0;
    bool exit = false;

    //Game Loop
    while(!exit)
    {
        //Check collision
        if (SDL_HasIntersection(&(head->rect), &food))
        {
            food.x = Random(0, SCREEN_WIDTH - 5);
            food.y = Random(0, SCREEN_HEIGHT - 5);
            AddNode(&head);
        }
        //Frame count
        frame++;
        if (frame == gameVel)
        {
            frame = 0;
        }
        if (frame == 1)
        {
            UpdateSnake(&head, currDir, &exit);
        }
        if (head->rect.y > (SCREEN_HEIGHT - head->rect.h) || head->rect.y < 0 || head->rect.x > (SCREEN_WIDTH - head->rect.w) || head->rect.x < 0)
        {
            exit = true;
        }
        SDL_Event event;
        if (SDL_PollEvent(&event))
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
                            currDir = currDir == UP ? UP:DOWN;
                            break;
                        case SDL_SCANCODE_LEFT:
                            currDir = currDir == RIGHT ? RIGHT : LEFT;
                            break;
                        case SDL_SCANCODE_RIGHT:
                            currDir = currDir == LEFT ? LEFT : RIGHT;
                            break;
                        case SDL_SCANCODE_UP:
                            currDir = currDir == DOWN ? DOWN:UP;
                            break;
                    }
                    break;
            }
        }
        SDL_SetRenderDrawColor(screenRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(screenRenderer);
        Node* aux = head;
        while(aux != NULL)
        {
            UpdateRect(&aux->rect, screenRenderer);
            aux = aux->next;
        }
        UpdateRect(&food, screenRenderer);
        SDL_RenderPresent(screenRenderer);
    }
    SDL_DestroyRenderer(screenRenderer);
}

