#include <stdlib.h>
#include <time.h>
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

#define PIECE_X_MAX_SIZE 2
#define PIECE_Y_MAX_SIZE 4
#define NB_POSSIBLE_PIECES 6

#define SHADOW_SIZE 4

enum world_content
{
	background,
	tile,
};

static enum world_content world[SCREEN_X_SIZE][SCREEN_Y_SIZE];

static enum world_content possible_pieces[NB_POSSIBLE_PIECES][PIECE_X_MAX_SIZE][PIECE_Y_MAX_SIZE]=
{
    {  { background, tile      , tile      , tile       },
       { background, tile      , background, background } },

    {  { background, background, tile      , tile       },
       { background, tile      , tile      , background } },

    {  { tile      , tile      , tile      , background },
       { background, background, tile      , background } },

    {  { tile      , tile      , background, background },
       { background, tile      , tile      , background } },

    {  { tile      , tile      , tile      , tile       },
       { background, background, background, background } },

    {  { background, tile      , tile      , background },
       { background, tile      , tile      , background } },
};

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
#ifdef DEBUG_CHECK_LINES
        if(y == SCREEN_Y_SIZE - 1)
            init_line(y, tile);
        else if(y == SCREEN_Y_SIZE - 2)
        {
            for(int x = 0; x < SCREEN_X_SIZE; x++)
            {
                world[x][y] = x % 2;
            }
        }
        else
            init_line(y, background);
#else
        init_line(y, background);
#endif
    }
}

void fix_piece_in_world()
{
    switch(actual_rotation)
    {
        case 0 ://0°
            for(int x = 0; x < PIECE_X_MAX_SIZE; x++)
            {
                for(int y = 0; y < PIECE_Y_MAX_SIZE; y++)
                {
                    int world_x = actual_x + x + 1 - PIECE_X_MAX_SIZE / 2, world_y = actual_y + y + 1 - PIECE_Y_MAX_SIZE / 2;
                    if(world_x >= 0 && world_x < SCREEN_X_SIZE)
                    {
                        if(world_y >= 0 && world_y < SCREEN_Y_SIZE)
                        {
                            enum world_content content = possible_pieces[actual_piece][x][y];
                            if(content != background)
                                world[world_x][world_y] = content;
                        }
                    }
                }
            }
            break;
        case 90 ://90°
            for(int x = 0; x < PIECE_X_MAX_SIZE; x++)
            {
                for(int y = 0; y < PIECE_Y_MAX_SIZE; y++)
                {
                    int world_x = actual_x - y + PIECE_Y_MAX_SIZE / 2, world_y = actual_y + x + 1 - PIECE_X_MAX_SIZE / 2;
                    if(world_x >= 0 && world_x < SCREEN_X_SIZE)
                    {
                        if(world_y >= 0 && world_y < SCREEN_Y_SIZE)
                        {
                            enum world_content content = possible_pieces[actual_piece][x][y];
                            if(content != background)
                                world[world_x][world_y] = content;
                        }
                    }
                }
            }
            break;
        case 180://180°
            for(int x = 0; x < PIECE_X_MAX_SIZE; x++)
            {
                for(int y = 0; y < PIECE_Y_MAX_SIZE; y++)
                {
                    int world_x = actual_x + PIECE_X_MAX_SIZE / 2 - x, world_y = actual_y + PIECE_Y_MAX_SIZE / 2 - y;
                    if(world_x >= 0 && world_x < SCREEN_X_SIZE)
                    {
                        if(world_y >= 0 && world_y < SCREEN_Y_SIZE)
                        {
                            enum world_content content = possible_pieces[actual_piece][x][y];
                            if(content != background)
                                world[world_x][world_y] = content;
                        }
                    }
                }
            }
            break;
        case 270://270°
            for(int x = 0; x < PIECE_X_MAX_SIZE; x++)
            {
                for(int y = 0; y < PIECE_Y_MAX_SIZE; y++)
                {
                    int world_x = actual_x + y + 1 - PIECE_Y_MAX_SIZE / 2, world_y = actual_y + PIECE_X_MAX_SIZE / 2 - x;
                    if(world_x >= 0 && world_x < SCREEN_X_SIZE)
                    {
                        if(world_y >= 0 && world_y < SCREEN_Y_SIZE)
                        {
                            enum world_content content = possible_pieces[actual_piece][x][y];
                            if(content != background)
                                world[world_x][world_y] = content;
                        }
                    }
                }
            }
            break;
    }
}


int check_if_piece_has_collision(void)
{
    switch(actual_rotation)
    {
        case 0 ://0°
            for(int x = 0; x < PIECE_X_MAX_SIZE; x++)
            {
                for(int y = 0; y < PIECE_Y_MAX_SIZE; y++)
                {
                    int world_x = actual_x + x + 1 - PIECE_X_MAX_SIZE / 2, world_y = actual_y + y + 1 - PIECE_Y_MAX_SIZE / 2;
                    if(possible_pieces[actual_piece][x][y] != background)
                    {
                        if(world_x >= 0 && world_x < SCREEN_X_SIZE)
                        {
                            if(world_y >= 0)
                            {
                                if(world_y >= SCREEN_Y_SIZE)//Reach floor of level
                                    return 1;
                                if(world[world_x][world_y] != background)//Reach other already put pieces
                                    return 1;
                            }
                        }
                        else
                            return 1;
                    }
                }
            }
            break;
        case 90 ://90°
            for(int x = 0; x < PIECE_X_MAX_SIZE; x++)
            {
                for(int y = 0; y < PIECE_Y_MAX_SIZE; y++)
                {
                    int world_x = actual_x - y + PIECE_Y_MAX_SIZE / 2, world_y = actual_y + x + 1 - PIECE_X_MAX_SIZE / 2;
                    if(possible_pieces[actual_piece][x][y] != background)
                    {
                        if(world_x >= 0 && world_x < SCREEN_X_SIZE)
                        {
                            if(world_y >= 0)
                            {
                                if(world_y >= SCREEN_Y_SIZE)//Reach floor of level
                                    return 1;
                                if(world[world_x][world_y] != background)//Reach other already put pieces
                                    return 1;
                            }
                        }
                        else
                            return 1;
                    }
                }
            }
            break;
        case 180://180°
            for(int x = 0; x < PIECE_X_MAX_SIZE; x++)
            {
                for(int y = 0; y < PIECE_Y_MAX_SIZE; y++)
                {
                    int world_x = actual_x + PIECE_X_MAX_SIZE / 2 - x, world_y = actual_y + PIECE_Y_MAX_SIZE / 2 - y;
                    if(possible_pieces[actual_piece][x][y] != background)
                    {
                        if(world_x >= 0 && world_x < SCREEN_X_SIZE)
                        {
                            if(world_y >= 0)
                            {
                                if(world_y >= SCREEN_Y_SIZE)//Reach floor of level
                                    return 1;
                                if(world[world_x][world_y] != background)//Reach other already put pieces
                                    return 1;
                            }
                        }
                        else
                            return 1;
                    }
                }
            }
            break;
        case 270://270°
            for(int x = 0; x < PIECE_X_MAX_SIZE; x++)
            {
                for(int y = 0; y < PIECE_Y_MAX_SIZE; y++)
                {
                    int world_x = actual_x + y + 1 - PIECE_Y_MAX_SIZE / 2, world_y = actual_y + PIECE_X_MAX_SIZE / 2 - x;
                    if(possible_pieces[actual_piece][x][y] != background)
                    {
                        if(world_x >= 0 && world_x < SCREEN_X_SIZE)
                        {
                            if(world_y >= 0)
                            {
                                if(world_y >= SCREEN_Y_SIZE)//Reach floor of level
                                    return 1;
                                if(world[world_x][world_y] != background)//Reach other already put pieces
                                    return 1;
                            }
                        }
                        else
                            return 1;
                    }
                }
            }
            break;
    }
    return 0;
}


void draw_tile(SDL_Renderer *renderer, int x, int y, enum world_content content);

void _draw_piece(SDL_Renderer *renderer, enum world_content shape[PIECE_X_MAX_SIZE][PIECE_Y_MAX_SIZE])
{
    switch(actual_rotation)
    {
        case 0 ://0°
            for(int x = 0; x < PIECE_X_MAX_SIZE; x++)
            {
                for(int y = 0; y < PIECE_Y_MAX_SIZE; y++)
                {
                    int world_x = actual_x + x + 1 - PIECE_X_MAX_SIZE / 2, world_y = actual_y + y + 1 - PIECE_Y_MAX_SIZE / 2;
                    if(world_x >= 0 && world_x < SCREEN_X_SIZE)
                    {
                        if(world_y >= 0 && world_y < SCREEN_Y_SIZE)
                        {
                            enum world_content content = world[world_x][world_y];
                            if(shape != NULL && shape[x][y] != background)
                                content = shape[x][y];
                            draw_tile(renderer, world_x, world_y, content);
                        }
                    }
                }
            }
            break;
        case 90 ://90°
            for(int x = 0; x < PIECE_X_MAX_SIZE; x++)
            {
                for(int y = 0; y < PIECE_Y_MAX_SIZE; y++)
                {
                    int world_x = actual_x - y + PIECE_Y_MAX_SIZE / 2, world_y = actual_y + x + 1 - PIECE_X_MAX_SIZE / 2;
                    if(world_x >= 0 && world_x < SCREEN_X_SIZE)
                    {
                        if(world_y >= 0 && world_y < SCREEN_Y_SIZE)
                        {
                            enum world_content content = world[world_x][world_y];
                            if(shape != NULL && shape[x][y] != background)
                                content = shape[x][y];
                            draw_tile(renderer, world_x, world_y, content);
                        }
                    }
                }
            }
            break;
        case 180://180°
            for(int x = 0; x < PIECE_X_MAX_SIZE; x++)
            {
                for(int y = 0; y < PIECE_Y_MAX_SIZE; y++)
                {
                    int world_x = actual_x + PIECE_X_MAX_SIZE / 2 - x, world_y = actual_y + PIECE_Y_MAX_SIZE / 2 - y;
                    if(world_x >= 0 && world_x < SCREEN_X_SIZE)
                    {
                        if(world_y >= 0 && world_y < SCREEN_Y_SIZE)
                        {
                            enum world_content content = world[world_x][world_y];
                            if(shape != NULL && shape[x][y] != background)
                                content = shape[x][y];
                            draw_tile(renderer, world_x, world_y, content);
                        }
                    }
                }
            }
            break;
        case 270://270°
            for(int x = 0; x < PIECE_X_MAX_SIZE; x++)
            {
                for(int y = 0; y < PIECE_Y_MAX_SIZE; y++)
                {
                    int world_x = actual_x + y + 1 - PIECE_Y_MAX_SIZE / 2, world_y = actual_y + PIECE_X_MAX_SIZE / 2 - x;
                    if(world_x >= 0 && world_x < SCREEN_X_SIZE)
                    {
                        if(world_y >= 0 && world_y < SCREEN_Y_SIZE)
                        {
                            enum world_content content = world[world_x][world_y];
                            if(shape != NULL && shape[x][y] != background)
                                content = shape[x][y];
                            draw_tile(renderer, world_x, world_y, content);
                        }
                    }
                }
            }
            break;
    }
}

void draw_piece(SDL_Renderer *renderer)
{
    _draw_piece(renderer, possible_pieces[actual_piece]);
}

void erase_piece(SDL_Renderer *renderer)
{
    _draw_piece(renderer, NULL);
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
        enum world_content content = background;
        if(old_y >= 0)
            content = world[x][old_y];
        world[x][new_y] = content;
        draw_tile(r, x, new_y, content);
    }
}

int check_line_and_redraw(SDL_Renderer *r, int y)
{
//    printf("check line %d\n", y);
    for(int x = 0; x < SCREEN_X_SIZE; x++)
    {
        if(world[x][y] == background)
        {
//            printf("tile %d/%d is background\n", x, y);
            return 0;
        }
    }
    do
    {
//        printf("move line %d to %d\n", y, y - 1);
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
    int actual_rotation_speed, delay;
    srand(time(NULL));//Init randome seed.
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
    actual_piece = 5;//rand() % 6;
    actual_rotation = 0;
    actual_rotation_speed = 90;
    actual_x = SCREEN_X_SIZE / 2;
    actual_y = 0;
    delay = 2000;
    uint32_t old_ticks = SDL_GetTicks();
    while(1)
    {
        erase_piece(renderer);
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
                                if(event.type == SDL_KEYDOWN)
                                    delay = 20;
                                else
                                    delay = 2000;
                                break;
                            case SDLK_UP:
                                if(event.type == SDL_KEYDOWN)
                                {
                                    actual_rotation += (event.type == SDL_KEYDOWN ? actual_rotation_speed : 0);
                                    if(actual_rotation == 360)
                                        actual_rotation = 0;
                                    actual_rotation_speed = 0;
                                }
                                else
                                {
                                    actual_rotation_speed = 90;
                                }
                                printf("actual_rotation = %d\n", actual_rotation);
                                break;
                            case SDLK_RIGHT:
                                actual_x += (event.type == SDL_KEYDOWN ? 1 : 0);
                                if(check_if_piece_has_collision() == 1)
                                    actual_x -= (event.type == SDL_KEYDOWN ? 1 : 0);
                                break;
                            case SDLK_LEFT:
                                actual_x -= (event.type == SDL_KEYDOWN ? 1 : 0);
                                if(check_if_piece_has_collision() == 1)
                                    actual_x += (event.type == SDL_KEYDOWN ? 1 : 0);
                                break;
                            case SDLK_ESCAPE:
                                SDL_Quit();
                                return EXIT_SUCCESS;// Quit the program
                                break;
                        }
                        break;
            }
        }
        
        if(SDL_GetTicks() - old_ticks > delay)
        {
            actual_y++;
            if(check_if_piece_has_collision() == 1)
            {
                actual_y--;
                fix_piece_in_world();
                draw_piece(renderer);
                
                actual_piece = rand() % 6;
                actual_rotation = 0;
                actual_x = SCREEN_X_SIZE / 2;
                actual_y = 0;
                if(check_if_piece_has_collision() == 1)
                {
                    printf("!!!!!!!!  GAME OVER  !!!!!!!!!!!!!\n");
                    SDL_Quit();
                    return EXIT_SUCCESS;// Quit the program
                }
            }
            old_ticks = SDL_GetTicks();
        }
        draw_piece(renderer);
        check_world_and_redraw(renderer);
        
        //Update the display
	SDL_RenderPresent(renderer);
        
    }

    // Tell the SDL to clean up and shut down
    SDL_Quit();

    return EXIT_SUCCESS;
}
