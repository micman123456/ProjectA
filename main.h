#ifndef MAIN_H
#define MAIN_H

#include <string.h>
#include <windows.h>
#include <winerror.h>
#include <stdio.h>
#include <stdint.h>
#include <algorithm>
#include <random>

//256 x 192

#define GAME_WIDTH 400*2
#define GAME_HEIGHT 240*2
#define GAME_BPP 32
#define GAME_DRAWING_AREA_MEMORY_SIZE (GAME_WIDTH * GAME_HEIGHT * (GAME_BPP / 8))
#define DESIRED_MIRCOSECONDS 16667
#define DIR_DOWN 0
#define DIR_LEFT 1
#define DIR_RIGHT 2
#define DIR_UP 3

#define MAP_WIDTH 2000
#define MAP_HEIGHT 2000
#define TILE_SIZE 25

#define NUMB_TILES (MAP_WIDTH * MAP_HEIGHT)/(TILE_SIZE * TILE_SIZE)
#define CENTER_TILE_INDEX 3200
// Sqrt of NUMB_TILES
#define NUMB_TILES_PER_ROW 80
#define STARTING_TILE 3240

/* Tile Types */
#define NUMB_TILE_TYPES 6
#define FLOOR1 0
#define FLOOR2 1
#define FLOOR3 2
#define WALL1 3
#define WALL2 4
#define WALL3 5



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD Create_Main_Window(HINSTANCE hInstance);

VOID process_player_input(void);
VOID initBitMap(void);
VOID render_game_frames(void);

BOOL is_already_running(void);


typedef LONG(NTAPI* _NtQueryTimerResolution) (
  OUT PULONG MinimumResolution,
  OUT PULONG MaximumResolution,
  OUT PULONG CurrentResolution );

_NtQueryTimerResolution NtQueryTimerResolution;

typedef struct GAMEBITMAP{
    BITMAPINFO bitMapInfo;
    void* memory;
}GAMEBITMAP;


typedef struct PIXEL32 {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} PIXEL;

typedef struct TILE{
    uint32_t type;
    GAMEBITMAP tile_sprite;
}TILE;


typedef struct PREFORMENCE_DATA{
    uint64_t TotalFramesRendered;
    float RawFPS;
    float CookFPS;
    MONITORINFO MonInfo;
    uint32_t MonWidth;
    uint32_t Monheight;
    LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
    LARGE_INTEGER Frequency;



}PREFORMENCE_DATA;

typedef struct PLAYER{
    GAMEBITMAP sprite[5][6];
    int32_t worldPosX;
    int32_t worldPosY;
    int32_t ScreenPosX;
    int32_t ScreenPosY;
    uint32_t movementRemaining;
    uint32_t direction;
    uint32_t animation_step;
    uint32_t idleFrameCount;
    GAMEBITMAP sprite_sheet[5];
    TILE StandingTile;
    uint32_t StandingTile_Index;
   
    
}PLAYER;



typedef struct NPC{
    GAMEBITMAP sprite[5][6];
    uint32_t worldPosX;
    uint32_t worldPosY;
    uint32_t movementRemaining;
    uint32_t direction;
    uint32_t animation_step;
    uint32_t idleFrameCount;
    GAMEBITMAP sprite_sheet[5];
    
    
}NPC;


DWORD Load32BppBitmapFromFile(LPCSTR,GAMEBITMAP*);
DWORD InitPlayer(VOID);
DWORD InitNPC(VOID);
int32_t GetPlayerTile(PLAYER*); 
int32_t GetNextPlayerTile(PLAYER*,int32_t Direction); 
VOID InitTiles(GAMEBITMAP);  
VOID GenerateRoom(int32_t);

//VOID LoadTilesToScreen(TILE*);
VOID BuiltTileMap(TILE*,GAMEBITMAP*);

VOID InitBackgroundFromTileSprite(GAMEBITMAP);
VOID LoadBackgroundToScreen(GAMEBITMAP);
VOID LoadBitFontToScreen(GAMEBITMAP,char*, int16_t, int16_t);
VOID LoadBitMapToScreen(GAMEBITMAP, int16_t, int16_t,int16_t,int16_t);

void updatePlayerPosition(int32_t&,int,int,int,int);
void updateScreenPosition(int screenLimit, bool increment, bool horizontal);

DWORD LoadSpriteFromSpriteSheet(GAMEBITMAP,GAMEBITMAP*,int16_t, int16_t,int16_t,int16_t);


#endif 
