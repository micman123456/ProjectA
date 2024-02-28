#ifndef MAIN_H
#define MAIN_H

#include <string.h>
#include <windows.h>
#include <winerror.h>
#include <stdio.h>
#include <stdint.h>
#include <algorithm>

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
    uint32_t worldPosX;
    uint32_t worldPosY;
    uint32_t movementRemaining;
    uint32_t direction;
    uint32_t animation_step;
    uint32_t idleFrameCount;
    GAMEBITMAP sprite_sheet[5];
   
    
}PLAYER;

typedef struct TILE{
    uint32_t type;
    GAMEBITMAP tile_sprite;
}TILE;

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


//VOID LoadTilesToScreen(TILE*);
VOID BuiltTileMap(TILE*,GAMEBITMAP*);

VOID InitBackgroundFromTileSprite(GAMEBITMAP);
VOID LoadBackgroundToScreen(GAMEBITMAP);
VOID LoadBitFontToScreen(GAMEBITMAP,char*, int16_t, int16_t);
VOID LoadBitMapToScreen(GAMEBITMAP, int16_t, int16_t,int16_t,int16_t);

DWORD LoadSpriteFromSpriteSheet(GAMEBITMAP,GAMEBITMAP*,int16_t, int16_t,int16_t,int16_t);


#endif 
