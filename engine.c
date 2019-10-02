#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

SDL_Texture *images[1000];
char *image_filenames[1000];
int image_count = 0;

TTF_Font *fonts[1000];
char *font_filenames[1000];
int font_sizes[1000];
int font_count = 0;

Mix_Chunk *sounds[1000];
char *sound_filenames[1000];
int sound_count = 0;

Mix_Music *musics[1000];
char *music_filenames[1000];
int music_count = 0;

int width;
int height;
int scale;

SDL_Window *window;
SDL_Renderer *renderer;

int key_count = 84;
char *keys[] = {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r","s","t","u","v","w","x","y","z","1","2","3","4","5","6","7","8","9","0","return","backspace","tab","space","-","=","[","]","\\",";","'","`",",",".","/","capslock","f1","f2","f3","f4","f5","f6","f7","f8","f9","f10","f11","f12","scrolllock","pause","insert","home","pageup","delete","end","pagedown","right","left","down","up","lctrl","lshift","lalt","lgui","rctrl","rshift","ralt","rgui"};
SDL_Scancode scancodes[] = {SDL_SCANCODE_A,SDL_SCANCODE_B,SDL_SCANCODE_C,SDL_SCANCODE_D,SDL_SCANCODE_E,SDL_SCANCODE_F,SDL_SCANCODE_G,SDL_SCANCODE_H,SDL_SCANCODE_I,SDL_SCANCODE_J,SDL_SCANCODE_K,SDL_SCANCODE_L,SDL_SCANCODE_M,SDL_SCANCODE_N,SDL_SCANCODE_O,SDL_SCANCODE_P,SDL_SCANCODE_Q,SDL_SCANCODE_R,SDL_SCANCODE_S,SDL_SCANCODE_T,SDL_SCANCODE_U,SDL_SCANCODE_V,SDL_SCANCODE_W,SDL_SCANCODE_X,SDL_SCANCODE_Y,SDL_SCANCODE_Z,SDL_SCANCODE_1,SDL_SCANCODE_2,SDL_SCANCODE_3,SDL_SCANCODE_4,SDL_SCANCODE_5,SDL_SCANCODE_6,SDL_SCANCODE_7,SDL_SCANCODE_8,SDL_SCANCODE_9,SDL_SCANCODE_0,SDL_SCANCODE_RETURN,SDL_SCANCODE_BACKSPACE,SDL_SCANCODE_TAB,SDL_SCANCODE_SPACE,SDL_SCANCODE_MINUS,SDL_SCANCODE_EQUALS,SDL_SCANCODE_LEFTBRACKET,SDL_SCANCODE_RIGHTBRACKET,SDL_SCANCODE_BACKSLASH,SDL_SCANCODE_SEMICOLON,SDL_SCANCODE_APOSTROPHE,SDL_SCANCODE_GRAVE,SDL_SCANCODE_COMMA,SDL_SCANCODE_PERIOD,SDL_SCANCODE_SLASH,SDL_SCANCODE_CAPSLOCK,SDL_SCANCODE_F1,SDL_SCANCODE_F2,SDL_SCANCODE_F3,SDL_SCANCODE_F4,SDL_SCANCODE_F5,SDL_SCANCODE_F6,SDL_SCANCODE_F7,SDL_SCANCODE_F8,SDL_SCANCODE_F9,SDL_SCANCODE_F10,SDL_SCANCODE_F11,SDL_SCANCODE_F12,SDL_SCANCODE_SCROLLLOCK,SDL_SCANCODE_PAUSE,SDL_SCANCODE_INSERT,SDL_SCANCODE_HOME,SDL_SCANCODE_PAGEUP,SDL_SCANCODE_DELETE,SDL_SCANCODE_END,SDL_SCANCODE_PAGEDOWN,SDL_SCANCODE_RIGHT,SDL_SCANCODE_LEFT,SDL_SCANCODE_DOWN,SDL_SCANCODE_UP,SDL_SCANCODE_LCTRL,SDL_SCANCODE_LSHIFT,SDL_SCANCODE_LALT,SDL_SCANCODE_LGUI,SDL_SCANCODE_RCTRL,SDL_SCANCODE_RSHIFT,SDL_SCANCODE_RALT,SDL_SCANCODE_RGUI};

void error (const char *string)
{
    printf("Error: %s\n", string);
    exit(1);
}

int isDown (lua_State *L)
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);

    if (!lua_isstring(L, 1)) {
        error("isDown: needs string");
    }

    const char *input = lua_tostring(L, 1);
    int boolean = 0;

    if (
        (strcmp(input, "mouseLeft") == 0 && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) ||
        (strcmp(input, "mouseMiddle") == 0 && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_MIDDLE)) ||
        (strcmp(input, "mouseRight") == 0 && SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))
    ) {
        boolean = 1;
    }

    for (int i = 0; i < key_count; i++) {
        if (strcmp(input, keys[i]) == 0) {
            if (state[scancodes[i]]) {
                boolean = 1;
                break;
            }
        }
    }

    lua_pushboolean(L, boolean);
    return 1;
}

int mouseX (lua_State *L)
{
    int x;
    SDL_GetMouseState(&x, NULL);
    lua_pushnumber(L, x/scale);
    return 1;
}

int mouseY (lua_State *L)
{
    int y;
    SDL_GetMouseState(NULL, &y);
    lua_pushnumber(L, y/scale);
    return 1;
}

int get_number(lua_State *L, int x, int d)
{
    if (lua_isnoneornil(L, x)) {
        return d;
    } else {
        return lua_tonumber(L, x);
    }
}

int image (lua_State *L)
{
    if (!lua_isstring(L, 1)) error("image: filename is not a string");
    if (!lua_isnumber(L, 2) && !lua_isnoneornil(L, 2)) error("image: x is not a number");
    if (!lua_isnumber(L, 3) && !lua_isnoneornil(L, 3)) error("image: y is not a number");
    if (!lua_isnumber(L, 4) && !lua_isnoneornil(L, 4)) error("image: angle is not a number");
    if (!lua_isnumber(L, 5) && !lua_isnoneornil(L, 5)) error("image: sx is not a number");
    if (!lua_isnumber(L, 6) && !lua_isnoneornil(L, 6)) error("image: sy is not a number");
    if (!lua_isnumber(L, 7) && !lua_isnoneornil(L, 7)) error("image: ox is not a number");
    if (!lua_isnumber(L, 8) && !lua_isnoneornil(L, 8)) error("image: oy is not a number");
    if (!lua_isnumber(L, 9) && !lua_isnoneornil(L, 9)) error("image: r is not a number");
    if (!lua_isnumber(L, 10) && !lua_isnoneornil(L, 10)) error("image: g is not a number");
    if (!lua_isnumber(L, 11) && !lua_isnoneornil(L, 11)) error("image: b is not a number");
    if (!lua_isnumber(L, 12) && !lua_isnoneornil(L, 12)) error("image: a is not a number");

    const char *filename = lua_tostring(L, 1);
    int x = get_number(L, 2, 0);
    int y = get_number(L, 3, 0);
    double angle = get_number(L, 4, 0);
    double sx = get_number(L, 5, 1);
    double sy = get_number(L, 6, 1);
    int ox = get_number(L, 7, 0);
    int oy = get_number(L, 8, 0);
    int r = get_number(L, 9, 255);
    int g = get_number(L, 10, 255);
    int b = get_number(L, 11, 255);
    int a = get_number(L, 12, 255);

    SDL_Texture *texture = NULL;

    for (int i = 0; i < image_count; i++) {
        if (strcmp(filename, image_filenames[i]) == 0) {
            texture = images[i];
            break;
        }
    }

    if (!texture) {
        texture = IMG_LoadTexture(renderer, filename);
        if (!texture) {
            error(SDL_GetError());
        }
        images[image_count] = texture;
        image_filenames[image_count] = malloc(strlen(filename) + 1);
        strcpy(image_filenames[image_count], filename);
        image_count++;
    }

    SDL_Point center;
    center.x = ox;
    center.y = oy;

    SDL_Rect dst;
    dst.x = x;
    dst.y = y;

    SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);
    dst.w *= sx;
    dst.h *= sy;

    SDL_SetTextureColorMod(texture, r, g, b);
    SDL_SetTextureAlphaMod(texture, a);

    SDL_RenderCopyEx(renderer, texture, NULL, &dst, angle, &center, SDL_FLIP_NONE);
}

int text (lua_State *L)
{
    const char *string = lua_tostring(L, 1);
    const char *filename = lua_tostring(L, 2);
    int size = lua_tonumber(L, 3);
    int x = lua_tonumber(L, 4);
    int y = lua_tonumber(L, 5);

    SDL_Color color;
    int a;
    if (!lua_isnumber(L, 6) || !lua_isnumber(L, 7) || !lua_isnumber(L, 8)) {
        color.r = 0;
        color.g = 0;
        color.b = 0;
        a = 255;
    } else {
        color.r = lua_tonumber(L, 6);
        color.g = lua_tonumber(L, 7);
        color.b = lua_tonumber(L, 8);
        if (!lua_isnumber(L, 9)) {
            a = 255;
        } else {
            a = lua_tonumber(L, 9);
        }
    }

    TTF_Font *font = NULL;
    for (int i = 0; i < font_count; i++) {
        if (strcmp(filename, font_filenames[i]) == 0 && font_sizes[i] == size) {
            font = fonts[i];
            break;
        }
    }

    if (!font) {
        font = TTF_OpenFont(filename, size);
        if (!font) {
            error("Couldn't open font");
        }
        fonts[font_count] = font;
        font_filenames[font_count] = malloc(strlen(filename) + 1);
        strcpy(font_filenames[font_count], filename);
        font_sizes[font_count] = size;
        font_count++;
    }

    SDL_Surface *surface = TTF_RenderText_Blended(font, string, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dst;
    dst.x = x;
    dst.y = y;

    SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h);
    SDL_SetTextureAlphaMod(texture, a);
    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int line (lua_State *L)
{
    if (!lua_isnumber(L, 1)) error("line: x1 is not a number");
    if (!lua_isnumber(L, 2)) error("line: y1 is not a number");
    if (!lua_isnumber(L, 3)) error("line: x2 is not a number");
    if (!lua_isnumber(L, 4)) error("line: y2 is not a number");

    int x1 = lua_tonumber(L, 1);
    int y1 = lua_tonumber(L, 2);
    int x2 = lua_tonumber(L, 3);
    int y2 = lua_tonumber(L, 4);

    int r;
    int g;
    int b;
    int a;

    if (!lua_isnumber(L, 5) || !lua_isnumber(L, 6) || !lua_isnumber(L, 7)) {
        r = 0;
        g = 0;
        b = 0;
        a = 255;
    } else {
        r = lua_tonumber(L, 5);
        g = lua_tonumber(L, 6);
        b = lua_tonumber(L, 7);
        if (!lua_isnumber(L, 8)) {
            a = 255;
        } else {
            a = lua_tonumber(L, 8);
        }
    }

    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

int rectangle (lua_State *L)
{
    if (!lua_isnumber(L, 1)) error("rectangle: x is not a number");
    if (!lua_isnumber(L, 2)) error("rectangle: y is not a number");
    if (!lua_isnumber(L, 3)) error("rectangle: width is not a number");
    if (!lua_isnumber(L, 4)) error("rectangle: height is not a number");

    int x = lua_tonumber(L, 1);
    int y = lua_tonumber(L, 2);
    int w = lua_tonumber(L, 3);
    int h = lua_tonumber(L, 4);

    int r;
    int g;
    int b;
    int a;

    if (!lua_isnumber(L, 5) || !lua_isnumber(L, 6) || !lua_isnumber(L, 7)) {
        r = 0;
        g = 0;
        b = 0;
        a = 255;
    } else {
        r = lua_tonumber(L, 5);
        g = lua_tonumber(L, 6);
        b = lua_tonumber(L, 7);
        if (lua_isnoneornil(L, 8)) {
            a = 255;
        } else {
            a = lua_tonumber(L, 8);
        }
    }

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;

    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderFillRect(renderer, &rect);
}

int sound (lua_State *L)
{
    const char *filename = lua_tostring(L, 1);
    Mix_Chunk *chunk = NULL;

    for (int i = 0; i < sound_count; i++) {
        if (strcmp(filename, sound_filenames[i]) == 0) {
            chunk = sounds[i];
            break;
        }
    }

    if (!chunk) {
        chunk = Mix_LoadWAV(filename);
        if (!chunk) {
            error("sound: Couldn't open sound file");
        }
        sounds[sound_count] = chunk;
        sound_filenames[sound_count] = malloc(strlen(filename) + 1);
        strcpy(sound_filenames[sound_count], filename);
        sound_count++;
    }

    Mix_PlayChannel(-1, chunk, 0);
}

int loop (lua_State *L)
{
    Mix_HaltMusic();

    const char *filename = lua_tostring(L, 1);
    Mix_Music *music = NULL;

    for (int i = 0; i < music_count; i++) {
        if (strcmp(filename, music_filenames[i]) == 0) {
            music = musics[i];
            break;
        }
    }

    if (!music) {
        music = Mix_LoadMUS(filename);
        if (!music) {
            error("loop: Couldn't open file");
        }
        musics[music_count] = music;
        music_filenames[music_count] = malloc(strlen(filename) + 1);
        strcpy(music_filenames[music_count], filename);
        music_count++;
    }

    Mix_PlayMusic(music, -1);
}

int stopLoop (lua_State *L)
{
    Mix_HaltMusic();
}

int traceback (lua_State *L) {
    const char *message = lua_tostring(L, 1);

    if (message) {
        luaL_traceback(L, L, message, 1);
    } else if (!lua_isnoneornil(L, 1)) {
        if (!luaL_callmeta(L, 1, "__tostring")) {
            lua_pushliteral(L, "(no error message)");
        }
    }

    return 1;
}

int call_mouse_function (lua_State *L, char *g, char *l) {
    lua_pushcfunction(L, traceback);

    lua_getglobal(L, "engine");
    lua_getfield(L, -1, g);

    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 3);
        return 0;
    }

    lua_pushstring(L, l);

    if (lua_pcall(L, 1, 0, -4)) {
        printf("%s", lua_tostring(L, -1));
        lua_pop(L, 3);
        return 1;
    }

    lua_pop(L, 2);

    return 0;
}

int call_key_function (lua_State *L, SDL_Event event, char *g) {
    lua_pushcfunction(L, traceback);

    lua_getglobal(L, "engine");
    lua_getfield(L, -1, g);

    if (!lua_isfunction(L, -1)) {
        lua_pop(L, 3);
        return 0;
    } else {
        for (int i = 0; i < key_count; i++) {
            if (event.key.keysym.scancode == scancodes[i]) {
                lua_pushstring(L, keys[i]);
                if (lua_pcall(L, 1, 0, -4)) {
                    printf("%s", lua_tostring(L, -1));
                    lua_pop(L, 3);
                    return 1;
                } else {
                    lua_pop(L, 2);
                    return 0;
                }
            }
        }
    }

    lua_pop(L, 3);

    return 0;
}

int get_setting (lua_State *L, char *g, int default_) {
    lua_getglobal(L, "engine");
    lua_getfield(L, -1, g);
    int number = lua_tonumber(L, -1);
    lua_pop(L, 2);
    if (number == 0) {
        return default_;
    } else {
        return number;
    }
}

int main (int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: engine file.lua\n");
        return 1;
    }

    SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    Mix_Init(MIX_INIT_MP3 | MIX_INIT_OGG);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    TTF_Init();

    lua_State *L = luaL_newstate();

    luaL_openlibs(L);
    
    lua_getglobal(L, "math");
    lua_getfield(L, -1, "randomseed");
    lua_pushnumber(L, time(NULL));
    lua_call(L, 1, 0);
    lua_getfield(L, -1, "random");
    lua_call(L, 0, 0);
    lua_getfield(L, -1, "random");
    lua_call(L, 0, 0);
    lua_pop(L, 1);

    lua_newtable(L);

    lua_pushcfunction(L, isDown);
    lua_setfield(L, -2, "isDown");
    lua_pushcfunction(L, text);
    lua_setfield(L, -2, "text");
    lua_pushcfunction(L, isDown);
    lua_setfield(L, -2, "isDown");
    lua_pushcfunction(L, mouseX);
    lua_setfield(L, -2, "mouseX");
    lua_pushcfunction(L, mouseY);
    lua_setfield(L, -2, "mouseY");
    lua_pushcfunction(L, image);
    lua_setfield(L, -2, "image");
    lua_pushcfunction(L, text);
    lua_setfield(L, -2, "text");
    lua_pushcfunction(L, line);
    lua_setfield(L, -2, "line");
    lua_pushcfunction(L, rectangle);
    lua_setfield(L, -2, "rectangle");
    lua_pushcfunction(L, sound);
    lua_setfield(L, -2, "sound");
    lua_pushcfunction(L, loop);
    lua_setfield(L, -2, "loop");
    lua_pushcfunction(L, stopLoop);
    lua_setfield(L, -2, "stopLoop");

    lua_setglobal(L, "engine");

    SDL_assert(lua_gettop(L) == 0);

    lua_pushcfunction(L, traceback);

    if (luaL_loadfile(L, argv[1]) || lua_pcall(L, 0, 0, -2)) {
        printf("Cannot run script: %s", lua_tostring(L, -1));
        lua_pop(L, 2);
        return 1;
    }

    lua_pop(L, 1);
    SDL_assert(lua_gettop(L) == 0);

    lua_getglobal(L, "engine");
    lua_getfield(L, -1, "frame");
    if (!lua_isfunction(L, -1)) {
        printf("No 'frame' function");
        return 1;
    }
    lua_pop(L, 2);

    width = get_setting(L, "width", 500);
    height = get_setting(L, "height", 500);
    scale = get_setting(L, "scale", 1);
    
    window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width * scale, height * scale, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_RenderSetScale(renderer, scale, scale);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_Event event;

    int start_time = SDL_GetTicks();
    int end_time;
    int delay;
    int call_result;

    while (1) {
        call_result = 0;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                return 0;
            } else if (event.type == SDL_KEYDOWN && !event.key.repeat) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    return 0;
                }
                call_result = call_key_function(L, event, "down");
            } else if (event.type == SDL_KEYUP) {
                call_result = call_key_function(L, event, "up");
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    call_result = call_mouse_function(L, "down", "mouseLeft");
                } else if (event.button.button == SDL_BUTTON_MIDDLE) {
                    call_result = call_mouse_function(L, "down", "mouseMiddle");
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    call_result = call_mouse_function(L, "down", "mouseRight");
                }
            } else if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    call_result = call_mouse_function(L, "up", "mouseLeft");
                } else if (event.button.button == SDL_BUTTON_MIDDLE) {
                    call_result = call_mouse_function(L, "up", "mouseMiddle");
                } else if (event.button.button == SDL_BUTTON_RIGHT) {
                    call_result = call_mouse_function(L, "up", "mouseRight");
                }
            } else if (event.type == SDL_MOUSEWHEEL) {
                if (event.wheel.y < 0) {
                    call_result = call_mouse_function(L, "down", "mouseWheelDown");
                } else if (event.wheel.y > 0) {
                    call_result = call_mouse_function(L, "down", "mouseWheelUp");
                }
            }
        }

        if (call_result == 1) {
            return 1;
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        SDL_assert(lua_gettop(L) == 0);
        lua_pushcfunction(L, traceback);

        lua_getglobal(L, "engine");
        lua_getfield(L, -1, "frame");
        if (lua_pcall(L, 0, 0, -2)) {
            printf("Cannot run script: %s", lua_tostring(L, -1));
            return 1;
        } else {
            lua_pop(L, 2);
            SDL_assert(lua_gettop(L) == 0);
            SDL_RenderPresent(renderer);
            end_time = SDL_GetTicks();
            delay = 33 - end_time + start_time;
            start_time = end_time;
            if (delay > 0) {
                SDL_Delay(delay);
            }
            SDL_assert(lua_gettop(L) == 0);
        }
    }

    SDL_assert(lua_gettop(L) == 0);
    return 0;
}