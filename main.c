#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#define SPEED_RATE 1

#define TILE_SIZE 30
#define TILE_X_SIZE TILE_SIZE
#define TILE_Y_SIZE TILE_SIZE
#define SCREEN_X_SIZE (SCREEN_WIDTH/TILE_X_SIZE)
#define SCREEN_Y_SIZE (SCREEN_HEIGHT/TILE_Y_SIZE)
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define PIECE_X_MAX_SIZE 4
#define PIECE_Y_MAX_SIZE 6

#define SHADOW_SIZE 4

enum world_content
{
	background,
	tile,
};

static enum world_content world[SCREEN_X_SIZE][SCREEN_Y_SIZE];

static enum world_content possibles_pieces[4][PIECE_X_MAX_SIZE][PIECE_Y_MAX_SIZE];

static int actual_piece, actual_rotation, actual_x, actual_y;

void init_line(int y, enum world_content content)
{
    for(int x = 0; x < SCREEN_X_SIZE; x++)
    {
        world[x][y] = content;
    }
}

void init_world()
{
    for(int y = 0; y < SCREEN_Y_SIZE; y++)
    {
        if(y == SCREEN_Y_SIZE - 1)
            init_line(y, tile);
        else
            init_line(y, background);
    }
}

void draw_tile(SDL_Renderer *renderer, int x, int y, enum world_content content)
{
    uint8_t r, g, b;
    int shadow_size = SHADOW_SIZE;
    SDL_Rect pos = {x * TILE_X_SIZE,  y * TILE_Y_SIZE, TILE_X_SIZE, TILE_Y_SIZE};
    uint8_t shadow_color = 0;
    do
    {
        switch(content)
        {
            case background :
                r = 235 - shadow_color;
                g = 235 - shadow_color;
                b = 235 - shadow_color;
                break;
            case tile :
                r = 20 + shadow_color;
                g = 20 + shadow_color;
                b = 20 + shadow_color;
                break;
        }
        SDL_SetRenderDrawColor(renderer, r, g, b, 1);
        if(SDL_RenderDrawRect(renderer, &pos) == -1)
        {
            printf("Can't fill rect : %s\n", SDL_GetError());
        }
        pos.x++;
        pos.y++;
        pos.w -= 2;
        pos.h -= 2;
        shadow_color += 50;
    }
    while(shadow_size--);
    switch(content)
    {
        case background :
            r = 20;
            g = 20;
            b = 20;
            break;
        case tile :
            r = 235;
            g = 235;
            b = 235;
            break;
    }
    SDL_SetRenderDrawColor(renderer, r, g, b, 1);
    if(SDL_RenderFillRect(renderer, &pos) == -1)
    {
        printf("Can't fill rect : %s\n", SDL_GetError());
    }
}

void draw_line(SDL_Renderer *r, int y)
{
    for(int x = 0; x < SCREEN_X_SIZE; x++)
    {
        draw_tile(r, x, y, world[x][y]);
    }
}

void move_line_and_redraw(SDL_Renderer *r, int old_y, int new_y)
{
    for(int x = 0; x < SCREEN_X_SIZE; x++)
    {
        world[x][new_y] = world[x][old_y];
        draw_tile(r, x, new_y, world[x][new_y]);
        world[x][old_y] = background;
        draw_tile(r, x, new_y, background);
    }
}

int check_line_and_redraw(SDL_Renderer *r, int y)
{
    printf("check line %d\n", y);
    for(int x = 0; x < SCREEN_X_SIZE; x++)
    {
        if(world[x][y] == background)
        {
            printf("tile %d/%d is background\n", x, y);
            return 0;
        }
    }
    do
    {
        printf("move line %d to %d\n", y, y - 1);
        move_line_and_redraw(r, y - 1, y);
        y--;
    }
    while(y > 0);
    init_line(y, background);
    draw_line(r, y);
    return 1;
}

void check_world_and_redraw(SDL_Renderer *r)
{
    for(int y = SCREEN_Y_SIZE - 1; y >= 0; y--)
    {
        if(check_line_and_redraw(r, y) == 1)
            break;
    }
}

void draw_world(SDL_Renderer *r)
{
    for(int y = 0; y < SCREEN_Y_SIZE; y++)
    {
    	draw_line(r, y);
    }
}

int main( int argc, char *argv[ ] )
{
    SDL_Window *window;
    if( SDL_Init( SDL_INIT_VIDEO ) == -1 )
    {
        printf( "Can't init SDL:  %s\n", SDL_GetError( ) );
        return EXIT_FAILURE;
    }

    atexit( SDL_Quit );
    SDL_Surface *surface;
    window = SDL_CreateWindow("Ma fenêtre de jeu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0 );
    surface = SDL_GetWindowSurface(window);
    SDL_Renderer *renderer = SDL_GetRenderer(window);

    if( surface == NULL )
    {
        printf( "Can't set video mode: %s\n", SDL_GetError( ) );
        return EXIT_FAILURE;
    }   

    init_world();
    draw_world(renderer);
    // Main loop
    SDL_Event event;
    while(1)
    {
       // Check for messages
        if (SDL_PollEvent(&event))
        {
            // Check for the quit message
            switch (event.type)
            {
                case SDL_QUIT:
                    SDL_Quit();
                    return EXIT_SUCCESS;// Quit the program
                    break;
                case SDL_KEYUP:
                case SDL_KEYDOWN:
                        printf("keydown %d\n", event.key.keysym.sym);
                        switch(event.key.keysym.sym)
                        {
                            case SDLK_DOWN:
//                                yspeed = (event.type == SDL_KEYDOWN ? SPEED_RATE : 0);
                                break;
                            case SDLK_UP:
//                                yspeed = (event.type == SDL_KEYDOWN ? -SPEED_RATE : 0);
                                break;
                            case SDLK_RIGHT:
//                                xspeed = (event.type == SDL_KEYDOWN ? SPEED_RATE : 0);
                                break;
                            case SDLK_LEFT:
//                                xspeed = (event.type == SDL_KEYDOWN ? -SPEED_RATE : 0);
                                break;
                            case SDLK_ESCAPE:
                                SDL_Quit();
                                return EXIT_SUCCESS;// Quit the program
                                break;
                        }
                        break;
            }
        }
       
        check_world_and_redraw(renderer);
        
        //Update the display
	SDL_RenderPresent(renderer);
        SDL_Delay(200);
        
    }

    // Tell the SDL to clean up and shut down
    SDL_Quit();

    return EXIT_SUCCESS;
}
