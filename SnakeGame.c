//Include SDL.h
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//Screen dimension constants
const int SCREEN_WIDTH = 300;
const int SCREEN_HEIGHT = 300;
const int GAME_VEL = 240;
const int SNAKE_SIZE = 15;
const int FOOD_SIZE = 10;

//Colors
SDL_Color WHITE = {0xFF, 0xFF, 0xFF, 0xFF};
SDL_Color BLUE = {0x00, 0x00, 0xFF, 0xFF};
SDL_Color GREEN = {0x00, 0xFF, 0x00, 0xFF};

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
    struct node* next;
} typedef Node;

//Game functions
int Random(int min, int max);
void DealWithEvents(enum Direction currDir, enum Direction* nextDir, bool* exit);
SDL_Rect CreateRect(int x, int y, int w, int h);
void UpdateRect(SDL_Rect* rect, SDL_Renderer* renderer, SDL_Color edgeColor, SDL_Color fillColor);
Node* CreateSnake();
void UpdateSnake(Node** headPtr, Node** tailPtr, enum Direction* currDir, enum Direction nextDir, bool* bDead);
void AddNode(Node** head, Node** tail);
void newGame(SDL_Window** windowPtr, bool* exit);

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
        // scanf("%d", &GAME_VEL);
        //Create window
        window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (window == NULL)
        {
            printf("Window could not be created. SDL_Error: %s\n", SDL_GetError());
        }
        else
        {
            bool exit = false;
            while(!exit)
            {
                newGame(&window, &exit);
                if (exit)
                { break; }
                SDL_Event event;
                while (1)
                {
                    if(SDL_PollEvent(&event))
                    {
                        if(event.type == SDL_QUIT)
                        {
                            exit = true;
                            break;
                        }
                        else if(event.type == SDL_KEYDOWN)
                        {
                            printf("Physical %s key acting as %s key\n",
                                SDL_GetScancodeName(event.key.keysym.scancode),
                                SDL_GetKeyName(event.key.keysym.sym));
                            break;
                        }
                    }
                }
            }
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

void DealWithEvents(enum Direction currDir, enum Direction* nextDir, bool* exit)
{
    SDL_Event event;
    if (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                *exit = true;
                break;
            case SDL_KEYDOWN:;
                SDL_Keysym keysym = event.key.keysym;
                switch (keysym.scancode)
                {
                    case SDL_SCANCODE_DOWN:
                        *nextDir = currDir == UP ? UP:DOWN;
                        break;
                    case SDL_SCANCODE_LEFT:
                        *nextDir = currDir == RIGHT ? RIGHT : LEFT;
                        break;
                    case SDL_SCANCODE_RIGHT:
                        *nextDir = currDir == LEFT ? LEFT : RIGHT;
                        break;
                    case SDL_SCANCODE_UP:
                        *nextDir = currDir == DOWN ? DOWN:UP;
                        break;
                }
                break;
        }
    }
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
void UpdateRect(SDL_Rect* rect, SDL_Renderer* renderer, SDL_Color edgeColor, SDL_Color fillColor)
{
    SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    SDL_RenderFillRect(renderer, rect);
    SDL_SetRenderDrawColor(renderer, edgeColor.r, edgeColor.g, edgeColor.b, edgeColor.a);
    SDL_RenderDrawRect(renderer, rect);
}

Node* CreateSnake()
{
    Node* head = (Node*)malloc(sizeof(Node));
    head->next = NULL;
    head->pos = 0;
    head->rect = CreateRect(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, SNAKE_SIZE, SNAKE_SIZE);
    return head;
}

void UpdateSnake(Node** headPtr, Node** tailPtr, enum Direction* currDir, enum Direction nextDir, bool* bDead)
{
    Node* aux = *headPtr;
    int dx = 0, dy = 0;

    switch(*currDir)
    {
        case UP:
            dy = -SNAKE_SIZE;
            break;
        case DOWN:
            dy = +SNAKE_SIZE;
            break;
        case LEFT:
            dx = -SNAKE_SIZE;
            break;
        case RIGHT:
            dx = +SNAKE_SIZE;
            break;
    }
    *currDir = nextDir;
    if ((*headPtr)->rect.y > (SCREEN_HEIGHT - SNAKE_SIZE) || (*headPtr)->rect.y < 0 || (*headPtr)->rect.x > (SCREEN_WIDTH - SNAKE_SIZE) || (*headPtr)->rect.x < 0)
    {
        *bDead = true;
        return;
    }
    if (*headPtr == *tailPtr)
    {
        (*headPtr)->rect.x += dx;
        (*headPtr)->rect.y += dy;
        return;
    }
    while(aux->next != *tailPtr)
    {
        if (SDL_HasIntersection(&((*headPtr)->rect), &(aux->next->rect)))
        {
            *bDead = true;
            return;
        }
        aux = aux->next;
    }
    (*tailPtr)->next = *headPtr;
    aux->next = NULL;
    (*tailPtr)->rect.x = (*headPtr)->rect.x + dx;
    (*tailPtr)->rect.y = (*headPtr)->rect.y + dy;
    *headPtr = *tailPtr;
    *tailPtr = aux;
}

void AddNode(Node** head, Node** tail)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    Node* aux = *head;
    *tail = newNode;
    newNode->next = NULL;
    while(aux->next != NULL)
    {
        aux = aux->next;
    }
    //printf("pos %d\n", aux->pos);
    aux->next = newNode;
    newNode->pos = aux->pos + 1;
    newNode->rect = CreateRect(aux->rect.x, aux->rect.y, SNAKE_SIZE, SNAKE_SIZE);
}

void newGame(SDL_Window** windowPtr, bool* exit)
{
    SDL_Surface* screenSurface = NULL;
    SDL_Renderer* screenRenderer = NULL;
    screenSurface = SDL_GetWindowSurface(*windowPtr);
    screenRenderer = SDL_CreateRenderer(*windowPtr,-1,0);
    //Fill entire surface (NULL) with solid color
    SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0x0,0x0,0x0));
    
    Node* head = CreateSnake();
    Node* tail = head;
    SDL_Rect food = CreateRect(Random(0, SCREEN_WIDTH - FOOD_SIZE), Random(0, SCREEN_HEIGHT - FOOD_SIZE), FOOD_SIZE, FOOD_SIZE);
    enum Direction currDir = RIGHT;
    enum Direction nextDir = currDir;
    int frame = 0;
    bool bDead = false;
    //Game Loop
    while(1)
    {
        //Frame count
        frame++;
        if (frame == GAME_VEL)
        {
            frame = 0;
            UpdateSnake(&head, &tail, &currDir, nextDir, &bDead);
            if(bDead)
            {
                break;
            }
        }
        //Deal with events
        DealWithEvents(currDir, &nextDir, exit);
        if(*exit)
        {
            break;
        }

        //Check collision
        if (SDL_HasIntersection(&(head->rect), &food))
        {
            food.x = Random(0, SCREEN_WIDTH - FOOD_SIZE);
            food.y = Random(0, SCREEN_HEIGHT - FOOD_SIZE);
            AddNode(&head, &tail);
        }
        
        SDL_SetRenderDrawColor(screenRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(screenRenderer);
        Node* aux = head;
        while(aux != NULL)
        {
            UpdateRect(&aux->rect, screenRenderer, BLUE, WHITE);
            aux = aux->next;
        }
        UpdateRect(&food, screenRenderer, GREEN, GREEN);
        SDL_RenderPresent(screenRenderer);
    }
    SDL_DestroyRenderer(screenRenderer);
}

