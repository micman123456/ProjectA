#ifndef MAIN_H
#define MAIN_H

// Standard Library Includes
#include <algorithm>
#include <chrono>
#include <cstdlib> // For rand() function
#include <cstdint>
#include <fstream>
#include <future>
#include <iostream>
#include <random>
#include <stdexcept>
#include <stdio.h>
#include <string>
#include <stdlib.h> 
#include <vector>
#include <cassert>
#include <stdexcept>

// Windows API Includes
#include <windows.h>
#include <winerror.h>


// Project-Specific Includes
#include "Dialogue.c++"
#include "Dungeon.c++"
#include "menu.c++"

// Constants
#define GAME_WIDTH 400
#define GAME_HEIGHT 240
#define GAME_BPP 32
#define GAME_DRAWING_AREA_MEMORY_SIZE (GAME_WIDTH * GAME_HEIGHT * (GAME_BPP / 8))
#define DESIRED_MIRCOSECONDS 8333 // 16667/2

#define NUM_DIRECTIONS 8
#define NUM_WALK_FRAMES 5
#define NUM_IDLE_FRAMES 2

#define DIR_DOWN 0
#define DIR_LEFT 1
#define DIR_RIGHT 2
#define DIR_UP 3

#define DIR_DOWN_LEFT 4
#define DIR_DOWN_RIGHT 5
#define DIR_UP_LEFT 6
#define DIR_UP_RIGHT 7

#define DIR_DOWN_IDLE 4
#define DIR_LEFT_IDLE 5
#define DIR_RIGHT_IDLE 6
#define DIR_UP_IDLE 7


#define MAP_WIDTH 2400
#define MAP_HEIGHT 2400
#define TILE_SIZE 24

#define MIN_ROOM_SIZE 9
#define MAX_ROOM_SIZE 20

#define NUMB_TILES ((MAP_WIDTH * MAP_HEIGHT) / (TILE_SIZE * TILE_SIZE))
#define CENTER_TILE_INDEX 5000
#define NUMB_TILES_PER_ROW 100 // Sqrt of NUMB_TILES
#define STARTING_TILE 5050

// Tile Types
#define NUMB_TILE_TYPES 29
#define FLOOR1 0
#define FLOOR2 1
#define FLOOR3 2
#define FLOOR4 3
#define FLOOR5 4
#define WALL_FULL 5
#define WALL_DOWN 6
#define WALL_UP 7
#define WALL_LEFT 8
#define WALL_RIGHT 9
#define CORNER_UP_LEFT 10
#define CORNER_UP_RIGHT 11
#define CORNER_DOWN_LEFT 12
#define CORNER_DOWN_RIGHT 13
#define CORNER_UP_LEFT_IN 14
#define CORNER_UP_RIGHT_IN 15
#define CORNER_DOWN_LEFT_IN 16
#define CORNER_DOWN_RIGHT_IN 17
#define ISLAND_SINGLE 18
#define ISLAND_UP 19
#define ISLAND_DOWN 20
#define ISLAND_LEFT 21
#define ISLAND_RIGHT 22
#define ISLAND_CENTER 23
#define V_SINGLE_WALL 24
#define H_SINGLE_WALL 25
#define WALL_FULL_2 26
#define STAIRS 27
#define WALL_FULL_3 28

#define NPC_ON_TILE 29
#define STATIC_OBJECT 30
#define NON_STATIC_OBJECT 31

#define TRANSITION_TILE_N 32
#define TRANSITION_TILE_S 33
#define TRANSITION_TILE_E 34
#define TRANSITION_TILE_W 35


// Border Constants
#define BORDER_LEFT 10
#define BORDER_RIGHT 10
#define BORDER_UP 10
#define BORDER_DOWN 10

// Overworld Identifiers
#define POKEMON_SQUARE 0
#define MAKUHITA_DOJO 1
#define PELIPPER_POST_OFFICE 2

// Player Status

#define PLAYER_MOVING 0
#define PLAYER_IDLE 1
#define PLAYER_ATTACKING 2
#define PLAYER_INTERACTING 3



// Enums
typedef enum GAMESTATE {
    GAME_OPENING,
    GAME_TITLE_SCREEN,
    GAME_OVERWORLD,
    GAME_LOADING_SCREEN,
    GAME_DUNGEON,
    GAME_DUNGEON_LOADING_SCREEN,
    GAME_MAIN_MENU,
    GAME_TEXT_BOX_TESTING
} GAMESTATE;

typedef LONG(NTAPI* _NtQueryTimerResolution) (
  OUT PULONG MinimumResolution,
  OUT PULONG MaximumResolution,
  OUT PULONG CurrentResolution );

_NtQueryTimerResolution NtQueryTimerResolution;

// Typedefs
typedef struct GAMEBITMAP {
    BITMAPINFO bitMapInfo;
    void* memory;
} GAMEBITMAP;

typedef struct PIXEL32 {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint8_t alpha;
} PIXEL;

typedef struct TILE {
    uint32_t type;
    uint8_t visited;
    GAMEBITMAP tile_sprite;
} TILE;

typedef struct PREFORMENCE_DATA {
    uint64_t TotalFramesRendered;
    float RawFPS;
    float CookFPS;
    MONITORINFO MonInfo;
    uint32_t MonWidth;
    uint32_t Monheight;
    LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
    LARGE_INTEGER Frequency;
} PREFORMENCE_DATA;

typedef struct CAMERA {
    int32_t height;
    int32_t width;
    int32_t zoom;
    int32_t worldPosX;
    int32_t worldPosY;
    int32_t CameraLockX;
    int32_t CameraLockY;
    int32_t direction;
    int32_t Mode;
} CAMERA;


typedef struct SPRITESHEET {
    GAMEBITMAP sheet;
    std::string spriteName;
    int32_t width;
    int32_t height;
    int32_t rowCount;
    int32_t columnCount;

    int32_t h_offset;
    int32_t v_offset;

} SPRITESHEET;


typedef struct PLAYER {
    GAMEBITMAP sprite[3][9][15];
    SPRITESHEET spriteSheets[10];
    std::string SpriteFilePath;
    
    uint32_t animationCount;
    uint32_t current_animation;
    uint32_t animation_step;
    uint32_t idleFrameCount;
    int32_t animationTimer;

    int32_t worldPosX;
    int32_t worldPosY;
    int32_t ScreenPosX;
    int32_t ScreenPosY;
    uint32_t movementRemaining;
    uint32_t direction;
    
    
    GAMEBITMAP sprite_sheet[5];
    TILE StandingTile;
    uint32_t StandingTile_Index;
    uint8_t noClip;
    uint8_t InRoom;
    BOOL CameraLock;

    int32_t status;
} PLAYER;

typedef struct NPC {
    GAMEBITMAP sprite[3][9][16];
    SPRITESHEET spriteSheets[10];
    std::string SpriteFilePath;
    std::string Name;
    std::string DialogueFilePath;
    std::string PortraitFilePath;
    
    uint32_t animationCount;
    uint32_t current_animation;
    uint32_t animation_step;
    int32_t animationTimer;

    int32_t worldPosX;
    int32_t worldPosY;
    int32_t ScreenPosX;
    int32_t ScreenPosY;
    int32_t OffsetX;
    int32_t OffsetY;
    TILE StandingTile;
    uint32_t StandingTile_Index;
    uint32_t movementRemaining;
    uint32_t direction;
    uint32_t idleFrameCount;
    GAMEBITMAP sprite_sheet[5];
    uint32_t overworld;
    uint32_t visbility;
    GAMEBITMAP Portrait;
    Dialogue Dialogue;

    
} NPC;

typedef struct ROOM {
    uint32_t Starting_Tile;
    uint32_t Width;
    uint32_t Height;
    uint32_t Exit;
} ROOM;

typedef struct PATH {
    uint32_t Starting_Tile;
    uint32_t Ending_Tile;
    uint32_t Path_Length;
    int32_t Tiles[NUMB_TILES_PER_ROW];
} PATH;

// Function Declarations //

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
DWORD Create_Main_Window(HINSTANCE hInstance);

VOID process_player_input(void);
VOID initBitMap(void);
VOID render_game_frames(void);

BOOL is_already_running(void);

// Bitmap and Sprite Handling //

DWORD Load32BppBitmapFromFile(LPCSTR, GAMEBITMAP*);
DWORD LoadSpriteFromSpriteSheet(GAMEBITMAP, GAMEBITMAP*, int16_t, int16_t, int16_t, int16_t,int16_t,int16_t);
DWORD CreateTextBoxBitMap(GAMEBITMAP*, int32_t, int32_t);
DWORD LoadOverWorldIntoMemory(int32_t, LPCSTR, LPCSTR);
DWORD setDialogFromTextFile(Dialogue*, LPCSTR);
VOID LoadBitMapToScreen(GAMEBITMAP, int16_t, int16_t, int16_t, int16_t);
VOID LoadBackgroundToScreen();
VOID LoadBitFontToScreen(GAMEBITMAP, const std::string&, int16_t, int16_t, int16_t);
VOID InitBackgroundFromTileSprite(GAMEBITMAP);
VOID DisplayTextBox(GAMEBITMAP, Dialogue);
VOID DisplayOptBox(GAMEBITMAP);
VOID DisplayPortrait(GAMEBITMAP);
VOID ToggleTextBox();
VOID ToggleOptBox();

DWORD ImprovedSpriteSheetLoader(GAMEBITMAP, 
    GAMEBITMAP*, 
    int16_t, 
    int16_t, 
    int16_t, 
    int16_t, 
    int32_t, 
    int32_t, 
    int32_t, 
    int32_t);


// Player Initialization and Movement //

DWORD InitPlayer(VOID);
DWORD ResetPlayer(VOID);
DWORD InitPlayerOverWorld(VOID);
VOID updatePlayerPosition(int32_t&, int, int, int, int);
BOOL isPlayerInRoom(PLAYER);
int32_t GetPlayerTile(PLAYER*);
int32_t GetNextPlayerTile(PLAYER*, int32_t Direction);
VOID teleportPlayer();
VOID TeleportToStairs(PLAYER);
VOID UpdatePlayerSprite(PLAYER*,int32_t,int32_t);
VOID UpdateIdleAnimation(PLAYER*, int32_t, int32_t, int32_t);
void updatePlayerPosition2D(PLAYER*, int32_t);
int32_t GetNextPlayerTile2D(PLAYER*, int32_t);
VOID RelocatePlayer(PLAYER*, int32_t, int32_t, int32_t);
DWORD InitPlayerDung();




// NPC Initialization and Interaction //

DWORD InitNPC(NPC*, LPCSTR SpriteFilePath, LPCSTR PortraitFilePath, LPCSTR, int32_t overworld, int32_t x, int32_t y, int32_t, int32_t);
DWORD ImprovedInitNPC(NPC* npc,
    LPCSTR SpriteFilePath,
    LPCSTR DialogueFilePath,
    int32_t x,
    int32_t y,
    int32_t offsetX,
    int32_t offsetY);
VOID NPC_AMV_Handler();
VOID InteractionStart(PLAYER Player, NPC NPC);
VOID InteractionEnd();
VOID WorldPosBasedRender(NPC*, PLAYER*);
VOID CameraWorldPosBasedRender(NPC*, CAMERA*);
VOID UpdateIdleAnimationNPC(NPC*, int32_t);
VOID UpdateSprite(PLAYER*,int32_t);

// Tile and Map Generation // 

VOID InitTiles(GAMEBITMAP);
VOID ResetTiles(TILE*, TILE*);
VOID SetTilesOverworld(TILE*, int32_t*);
VOID GenerateRoomsSetNumber(int32_t, int32_t, int32_t, TILE*, TILE*);
VOID GenerateRoomsAttempts(int32_t, int32_t, int32_t, TILE*, TILE*);
int32_t DrawRoomToMap(ROOM, TILE*, TILE*, int32_t*, int32_t);
VOID GenerateCorridors(int32_t, TILE*, TILE*);
VOID GenerateConnectingPaths(int32_t, TILE*, TILE*);
BOOL GenerateConnectingPathsImproved(ROOM, ROOM, TILE*, TILE*);
VOID RemoveRoom(std::vector<ROOM>&, int);
VOID GenerateCorridorsNoWalls(int32_t, TILE*, TILE*);
int32_t GenerateStairTile(TILE*);
DWORD InitDungeon(Dungeon Dungeon);
VOID HandleMenuSelection(int32_t);

// Tile Drawing and Rendering //

VOID DrawTile(int32_t, int32_t, TILE*, TILE*);
VOID DrawTileOverride(int32_t, int32_t, TILE*, TILE*);
VOID DrawTileMap(TILE*, TILE*);
VOID DrawTileDetails(TILE*, TILE*);
VOID DrawCorners(TILE*, TILE*, int8_t, int8_t, int32_t);
VOID BuiltTileMap(TILE*, GAMEBITMAP*);

// World Rendering and Interaction //

VOID RenderDungeonScene(PLAYER*);
VOID RenderTitleScene();
VOID RenderLoadingScene(char*);
VOID RenderMainMenuScene();
VOID RenderOverWorld(PLAYER*);
VOID RenderTestZone(PLAYER*);
VOID DisplayMainMenuToScreen(Menu, int32_t, int32_t);
VOID HandleOverworldChange(int32_t, int32_t);
VOID SetWorldPosition(int32_t);
VOID HandleStairs();
BOOLEAN IsTileOnScreen(int32_t);
VOID ChangeOverworld(int32_t);
VOID HandleOverworldChange(int32_t, int32_t);


// Camera Handling // 

DWORD InitCamera(VOID);
VOID updateCameraPosition(int32_t&, int);
VOID RenderPlayerCharacter(CAMERA*, PLAYER*);
VOID updateCameraPosition2D(int32_t&, int32_t&, int);
VOID LockCameraToPlayer(CAMERA*, PLAYER*);


// Game Logic and State Handling //


BOOL HandleDialog(Dialogue*);
VOID HandleOptionSelection(int_fast8_t);
VOID HandleGamestateChange(GAMESTATE);

// Utility Functions //

BOOL IsRoomValid(ROOM, int32_t*, int32_t);
BOOL IsTileValid(int32_t, int8_t, TILE*, int8_t, int8_t);
BOOL IsTileValidNoWall(int32_t, int8_t, TILE*, int8_t, int8_t);
int32_t GetNextDirection(int32_t, TILE*, int8_t, int8_t);
int32_t GetFloorType(TILE*, int32_t);
int32_t GetFloorDetail(TILE*, int32_t);
VOID ToggleGrid();
VOID LoadGrid();





#endif 
