#ifndef UNICODE
#define UNICODE
#endif 



#include <string>
#include "main.h"
#include "generate.c++"
#include "graphics.c++"
#include "controls.c++"
#include "textbox.c++"
#include "Overworld.c++"

#include <cstdint>



// Globals //




BOOL gGameIsRunning;
HWND gGameWindow;
GAMESTATE gamestate = GAME_OPENING;
GAMESTATE  NextGameState = GAME_OPENING;
GAMESTATE Prvgamestate = GAME_OPENING;

GAMEBITMAP DrawingSurface;
GAMEBITMAP Font;
GAMEBITMAP TextBox,OptionBox,PortraitBox;


PREFORMENCE_DATA gPreformance_Data; 
PLAYER Player;
NPC npc;
int32_t gMonitorWidth;
int32_t gMonitorHeight;

GAMEBITMAP Room_Sprite;
GAMEBITMAP Background;
GAMEBITMAP FloorPlan;

GAMEBITMAP PortTest; 

GAMEBITMAP Tile_Sprite_Sheet;

TILE Background_Tiles[NUMB_TILES]; 
TILE Tile_Type_Array[NUMB_TILE_TYPES];

int32_t starting_pos;

Graphics graphics(&DrawingSurface, &Player);

int32_t LoadingFrameCount = 0;
LPSTR LoadingText = "Loading";

Dialogue MainMenuDialogue(1);
Dialogue DungeonDialogue(1);
Dialogue TestingDialogue(3);
Dialogue CurrentDialogue;

int32_t PageIndex = 0;

int_fast8_t ShowTextBox = 1;
int_fast8_t ShowOptBox = 0;
int_fast8_t ShowPortBox = 1;

// Defaut Option selection set to Yes // 
int_fast8_t YesNoOptions = 0;


Dungeon Dungeons[3];
Overworld Overworlds[3];


int32_t CurrentOverworld = 0;
int32_t OverworldFramePointer =0;

int32_t CurrentDungeon = 0;

int32_t stairs;
int_fast8_t flicker = 0;

Menu MainMenu;

BOOL Interaction = FALSE;
GAMEBITMAP CurrentPortrait;
int32_t cooldown_frames =0;


std::vector<NPC> Loaded_NPCs(2);
int32_t CurrentNPCIndex;

// 0 = overworld
// 1 = dung

int32_t Mode = 0;


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)

{
    
    LRESULT result = 0;
    
    if (is_already_running()){
        MessageBoxA(NULL, "Another instance is already running", "Error", MB_ICONEXCLAMATION | MB_OK);
        result = GetLastError();
        return result;
    }


    if (Create_Main_Window(hInstance) != ERROR_SUCCESS){
        return 0;
    }

    initBitMap();
    
    if (DrawingSurface.memory == NULL){
        MessageBoxA(NULL, "Failed to allocate memory for bitmap", "Error", MB_ICONEXCLAMATION | MB_OK);
    }

    if(Load32BppBitmapFromFile("assets\\fonts\\pmd-fontsheet.bmp",&Font) != ERROR_SUCCESS){
        MessageBoxA(NULL, "Unable to load font sheet into memory", "Error", MB_ICONEXCLAMATION | MB_OK);
        result = GetLastError();
        return result;
    }



    
    if (CreateTextBoxBitMap(&TextBox,320,48) != ERROR_SUCCESS){
        MessageBoxA(NULL, "Unable to find Create Textbox sprite", "Error", MB_ICONEXCLAMATION | MB_OK);
        result = GetLastError();
        return result;
    }

        if (CreateTextBoxBitMap(&OptionBox,48,32) != ERROR_SUCCESS){
        MessageBoxA(NULL, "Unable to find Create Textbox sprite", "Error", MB_ICONEXCLAMATION | MB_OK);
        result = GetLastError();
        return result;
    }

     if (CreateTextBoxBitMap(&PortraitBox,44,46) != ERROR_SUCCESS){
        MessageBoxA(NULL, "Unable to find Create Textbox sprite", "Error", MB_ICONEXCLAMATION | MB_OK);
        result = GetLastError();
        return result;
    }
    

    if (timeBeginPeriod(1) == TIMERR_NOCANDO){
        MessageBoxA(NULL, "Unable to set time res", "Error", MB_ICONEXCLAMATION | MB_OK);
        result = GetLastError();
        return result;
    }


    // Sets Overworlds // 

    Overworld Pokemon_Square = Overworld(POKEMON_SQUARE);
    Overworld Makuhita_Dojo = Overworld(MAKUHITA_DOJO);    
    Overworlds[POKEMON_SQUARE] = Pokemon_Square;
    Overworlds[MAKUHITA_DOJO] = Makuhita_Dojo;
    LoadOverWorldIntoMemory(POKEMON_SQUARE,"assets\\Overworld\\PokemonSquare\\PSFP.bmp","assets\\Overworld\\PokemonSquare\\");
    LoadOverWorldIntoMemory(MAKUHITA_DOJO,"assets\\Overworld\\Makuhita_Dojo\\PSFP.bmp","assets\\Overworld\\Makuhita_Dojo\\");

    // Sets Main menu Dialogue // 
    MainMenuDialogue.setDialogue(0, 0, "Would you like to load into Amp Plains?"); 
    MainMenuDialogue.setDialogue(0, 1, ""); 
    MainMenuDialogue.setDialogue(0, 2, ""); 
    MainMenuDialogue.setDialogue(0, 3, "opt"); 

    DungeonDialogue.setDialogue(0, 0, "Proceed to the next floor?"); 
    DungeonDialogue.setDialogue(0, 1, ""); 
    DungeonDialogue.setDialogue(0, 2, ""); 
    DungeonDialogue.setDialogue(0, 3, "opt"); 


    MainMenu.addItem("Amp Plains");
    MainMenu.addItem("Apple Woods");
    MainMenu.addItem("Desert Cave");

    Load32BppBitmapFromFile("assets\\Player_Sprites\\MakPort.bmp",&PortTest);
 
    

    // Setting Dungeons // 
    Dungeon AmpPlains = Dungeon("Amp Plains","assets\\tiles\\Tiles-Amp-Plains.bmp",3);
    Dungeon AppleWoods= Dungeon("Apple Woods","assets\\tiles\\test_tiles.bmp",10);
    Dungeon DesertCave= Dungeon("Desert Cave","assets\\tiles\\Desert.bmp",10);

    Dungeons[0] = AmpPlains;
    Dungeons[1] = AppleWoods;
    Dungeons[2] = DesertCave;
    
    HMODULE NtDllModuleHandle;
    if ((NtDllModuleHandle = GetModuleHandleA("ntdll.dll")) == NULL){
        MessageBoxA(NULL, "Unable to load ntdll.dll", "Error", MB_ICONEXCLAMATION | MB_OK);
        result = GetLastError();
        return result;
    }

    NtQueryTimerResolution = (_NtQueryTimerResolution)(GetProcAddress(NtDllModuleHandle,"NtQueryTimerResolution"));
    if (NtQueryTimerResolution == NULL){
        MessageBoxA(NULL, "Unable to find NtQueryTimerResolution", "Error", MB_ICONEXCLAMATION | MB_OK);
        result = GetLastError();
        return result;
    }

     ULONG minResolution, maxResolution, currentResolution;
     ULONG currentResolutionInMircoSeconds = 0;
     ULONG minResolutionInMircoSeconds = 0;
    if (NtQueryTimerResolution(&minResolution, &maxResolution, &currentResolution) == 0) {
        
          currentResolutionInMircoSeconds = ((100*currentResolution)/1000);
          minResolutionInMircoSeconds = ((100*minResolution)/1000);
         
         printf("Timer res: %lu\n", currentResolution);
         printf("Max Timer res: %lu\n", maxResolution);
         printf("Min Timer res: %lu\n", minResolution);
         printf("Current timer res (mirco): %lu\n", currentResolutionInMircoSeconds);
        
    } else {
        MessageBoxA(NULL, "Unable to find NtQueryTimerResolution", "Error", MB_ICONEXCLAMATION | MB_OK);
        result = GetLastError();
        return result;
    }


    

    MSG msg = { 0 };
    gGameIsRunning = TRUE;
    


    QueryPerformanceFrequency(&gPreformance_Data.Frequency);
    int64_t elapsedMsPerFrameAccumRaw = 0;
    int64_t elapsedMsPerFrameAccumCooked = 0;
    ShowCursor(FALSE);

     

    while(gGameIsRunning){

        QueryPerformanceCounter(&gPreformance_Data.StartingTime);
         
        
        while (PeekMessageA(&msg,gGameWindow, 0,0,PM_REMOVE))
        {    
            DispatchMessageA(&msg);
            
        }

        switch (gamestate)
        {
        case GAME_LOADING_SCREEN:
            LoadingFrameCount++;
            if(LoadingFrameCount>200){
                HandGamestateChange(gamestate,Prvgamestate,Mode);
                LoadingFrameCount = 0; 
            }

        case GAME_OPENING:
        
        LoadingFrameCount++;
            if(LoadingFrameCount>200){
                HandGamestateChange(gamestate,GAME_TITLE_SCREEN,Mode);
                LoadingFrameCount = 0; 
            }
            break;
        
        case GAME_OVERWORLD:
        
        LoadingFrameCount++;
            if(LoadingFrameCount>20){
                Background = Overworlds[CurrentOverworld].GetSprite(OverworldFramePointer);
                OverworldFramePointer++;
                graphics.SetBackgroundBitMap(&Background);
                LoadingFrameCount = 0;
                if(OverworldFramePointer>5){
                    OverworldFramePointer=0;
                } 
            }
            break;
        
        default:
            break;
        }

        process_player_input();
        render_game_frames();
        NPC_AMV_Handler();

        
        QueryPerformanceCounter(&gPreformance_Data.EndingTime);
        gPreformance_Data.ElapsedMicroseconds.QuadPart = gPreformance_Data.EndingTime.QuadPart - gPreformance_Data.StartingTime.QuadPart;
        gPreformance_Data.ElapsedMicroseconds.QuadPart *= 1000000;
        gPreformance_Data.ElapsedMicroseconds.QuadPart /= gPreformance_Data.Frequency.QuadPart;
        elapsedMsPerFrameAccumRaw += gPreformance_Data.ElapsedMicroseconds.QuadPart;
        
        
        while(gPreformance_Data.ElapsedMicroseconds.QuadPart < (DESIRED_MIRCOSECONDS)){

            
            gPreformance_Data.ElapsedMicroseconds.QuadPart = gPreformance_Data.EndingTime.QuadPart - gPreformance_Data.StartingTime.QuadPart;
            gPreformance_Data.ElapsedMicroseconds.QuadPart *= 1000000;
            gPreformance_Data.ElapsedMicroseconds.QuadPart /= gPreformance_Data.Frequency.QuadPart;
            
            /*

            Perfect 60 FPS Hack 

            if (gPreformance_Data.ElapsedMicroseconds.QuadPart < (DESIRED_MIRCOSECONDS - minResolutionInMircoSeconds)){
                Sleep(1);
            }
            
            
            if (gPreformance_Data.ElapsedMicroseconds.QuadPart < (DESIRED_MIRCOSECONDS - currentResolutionInMircoSeconds)){
                Sleep(0);
            }
            */

            if (gPreformance_Data.ElapsedMicroseconds.QuadPart < (LONGLONG)(DESIRED_MIRCOSECONDS - (currentResolutionInMircoSeconds*2))){
                Sleep(1);
            }

            QueryPerformanceCounter(&gPreformance_Data.EndingTime);
              
        }
        
        
        gPreformance_Data.TotalFramesRendered++;
        elapsedMsPerFrameAccumCooked += gPreformance_Data.ElapsedMicroseconds.QuadPart;

          
        if (gPreformance_Data.TotalFramesRendered % 100 == 0){

            float AvgMicroSecPerFrameCooked = (elapsedMsPerFrameAccumCooked / 100);
            float AvgMsPerFrameCooked = AvgMicroSecPerFrameCooked / 1000; 
            gPreformance_Data.CookFPS = (1.0f / AvgMsPerFrameCooked) * 1000;

            float AvgMicroSecPerFrameRaw = (elapsedMsPerFrameAccumRaw / 100);
            float AvgMsPerFrameRaw = AvgMicroSecPerFrameRaw / 1000; 
            gPreformance_Data.RawFPS = (1.0f / AvgMsPerFrameRaw) * 1000;
            
            elapsedMsPerFrameAccumCooked = 0;
            elapsedMsPerFrameAccumRaw = 0;

            flicker = flicker^1;

        }
        
       
    }

    
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    
    switch (uMsg)
    {
    case WM_DESTROY:
        gGameIsRunning = FALSE;
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        {
           
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // All painting occurs here, between BeginPaint and EndPaint.

            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

            EndPaint(hwnd, &ps);
        }
        return 0;
    
    case STAIRS:
        {
            HandleStairs();           
        }
        return 0;
    
    case WM_CLOSE:
    ShowCursor(TRUE);
    if (MessageBox(hwnd, L"Really quit?", L"Project", MB_OKCANCEL) == IDOK)
    {
        DestroyWindow(hwnd);
    }
    // Else: User canceled. Do nothing.
    ShowCursor(FALSE);
    return 0;
    
    
    }
    
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

DWORD Create_Main_Window(HINSTANCE hInstance){
    DWORD result = ERROR_SUCCESS;
    const wchar_t CLASS_NAME[]  = L"Sample Window Class";
    WNDCLASS windowClass = { 0 };

    windowClass.lpfnWndProc   = WindowProc;
    windowClass.hInstance     = hInstance;
    windowClass.lpszClassName = CLASS_NAME;
    
    //windowClass.hIcon = LoadIconA(NULL,(LPCSTR)IDI_APPLICATION);
       
    
    if (RegisterClassW(&windowClass) == 0) {
        result = GetLastError();
        MessageBox(NULL,L"Window Reg Failed",L"ERROR",MB_ICONEXCLAMATION | MB_OK);
        return result;
    }

    
    

    gGameWindow = CreateWindowExW(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class 
        L"Project",    // Window text
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

    if (gGameWindow == NULL)
    {
        result = GetLastError();
        return result;
    }

    gPreformance_Data.MonInfo = {sizeof(MONITORINFO)};
    if (GetMonitorInfoA(MonitorFromWindow(gGameWindow,MONITOR_DEFAULTTOPRIMARY),&gPreformance_Data.MonInfo) == 0){
        result = 0x80261001; // ERROR_MONITOR_NO_DESCRIPTOR 
        return result;
    }

    gMonitorWidth = gPreformance_Data.MonInfo.rcMonitor.right - gPreformance_Data.MonInfo.rcMonitor.left;
    gMonitorHeight = gPreformance_Data.MonInfo.rcMonitor.bottom - gPreformance_Data.MonInfo.rcMonitor.top;
    
    if (SetWindowPos(gGameWindow,HWND_TOP,gPreformance_Data.MonInfo.rcMonitor.left,gPreformance_Data.MonInfo.rcMonitor.top,gMonitorWidth,gMonitorHeight,SWP_SHOWWINDOW) == 0){
        result = GetLastError();
        return result;
    }

    if (SetWindowLongPtrA(gGameWindow,GWL_STYLE, (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~WS_OVERLAPPEDWINDOW) == 0){
        result = GetLastError();
        return result;
    } 

   return result;
        
    
}



BOOL is_already_running(void){
    
    HANDLE MUTEX_LOCK = NULL; 
    MUTEX_LOCK = CreateMutexA(NULL, FALSE, "PROC_MUTEX");

    if (GetLastError() == ERROR_ALREADY_EXISTS){
        return TRUE;
    }
    else{
    return FALSE;    
    }
    
}

VOID process_player_input(void){
    short espKeyDown = GetAsyncKeyState(VK_ESCAPE);
    short EnterKeyDown = GetAsyncKeyState(VK_RETURN);
    int16_t D_KeyDown = GetAsyncKeyState('D');
    int16_t A_KeyDown = GetAsyncKeyState('A');
    int16_t W_KeyDown = GetAsyncKeyState('W');
    int16_t S_KeyDown = GetAsyncKeyState('S');
    int16_t Ctr_KeyDown = GetAsyncKeyState(VK_CONTROL);
    int16_t C_KeyDown = GetAsyncKeyState('C');
    int16_t T_KeyDown = GetAsyncKeyState('T');
    int16_t R_KeyDown = GetAsyncKeyState('R');
    int16_t L_KeyDown = GetAsyncKeyState('L');

   
    
    static int16_t d_KeyWasDown;
    static int16_t a_KeyWasDown;
    static int16_t s_KeyWasDown;
    static int16_t w_KeyWasDown;
    static int16_t c_KeyWasDown;
    static int16_t t_KeyWasDown;
    static int16_t r_KeyWasDown;
    static int16_t l_KeyWasDown;
    static int16_t enter_KeyWasDown;

    switch (gamestate)
    {
    case GAME_LOADING_SCREEN:
        /* code */
        break;
    case GAME_TITLE_SCREEN:

        if(EnterKeyDown && !enter_KeyWasDown ){
            HandGamestateChange(gamestate,GAME_MAIN_MENU,Mode);
        }
        if(A_KeyDown && !a_KeyWasDown ){
            HandGamestateChange(gamestate,GAME_TEXT_BOX_TESTING,Mode);
        }
        
        break;
     case GAME_MAIN_MENU:
     if(A_KeyDown && !a_KeyWasDown){
                MainMenu.DecrementSelectedItem();
            }
    else if(D_KeyDown && !d_KeyWasDown){
                MainMenu.IncrementSelectedItem();
            }
    else if(EnterKeyDown && !enter_KeyWasDown ){
            HandGamestateChange(gamestate,GAME_LOADING_SCREEN,Mode);
        }
     
     break;

    case GAME_OVERWORLD:

    if(Interaction){
        if(ShowOptBox){
            if(S_KeyDown && !s_KeyWasDown){
                HandleOptionSelection(0);
            }
            else if(W_KeyDown && !w_KeyWasDown){
                HandleOptionSelection(1);
            }
            else if(EnterKeyDown && !enter_KeyWasDown ){
                switch (YesNoOptions)
                {
                case 0:
                    ToggleOptBox();
                    InteractionEnd();
                    cooldown_frames = 15;
                    CurrentDialogue.ResetDialogue();
                    Mode = 1;
                    HandGamestateChange(gamestate,GAME_DUNGEON,Mode);
                    
                    break;
                
                default:
                    ToggleOptBox();
                    if(HandleDialog(&CurrentDialogue)){
                    InteractionEnd();
                    CurrentDialogue.ResetDialogue();
                    cooldown_frames = 15;
                    
                    }
                    break;
                }
            }
        }
        else{
        if(EnterKeyDown && !enter_KeyWasDown){
        if(HandleDialog(&CurrentDialogue)){
            InteractionEnd();
            CurrentDialogue.ResetDialogue();
            cooldown_frames = 15;
            
        }
       
        }
        }
    }    
    
    if (Player.movementRemaining == 0){
        if(Player.StandingTile.type == FLOOR2){
            HandleOverworldChange(CurrentOverworld,0);
        }
        

        if (Player.idleFrameCount < 30){
            Player.animation_step = 0;
            Player.idleFrameCount++;
            }
        
        else if (Player.idleFrameCount >= 30 && Player.idleFrameCount < 60){
            Player.animation_step = 3;
            Player.idleFrameCount++;
            }
        
        else {
        Player.idleFrameCount = 0;
        }
    
        if (A_KeyDown && !Interaction){
            Player.direction = DIR_LEFT;
            if(GetNextPlayerTile(&Player,1) < 5 || Player.noClip == 1){
            if(!Ctr_KeyDown){
                Player.animation_step = 1;
                Player.movementRemaining = 24;
                Player.idleFrameCount = 0;
                Player.StandingTile_Index += 1;
                Player.StandingTile = Background_Tiles[Player.StandingTile_Index];
            }
            }
         
        }
        else if (D_KeyDown && !Interaction){
            Player.direction = DIR_RIGHT;
            if(GetNextPlayerTile(&Player,2) < 5 || Player.noClip == 1){
            if(!Ctr_KeyDown){
                Player.animation_step = 1;
                Player.movementRemaining = 24;
                Player.idleFrameCount = 0;
                Player.StandingTile_Index -= 1;
                Player.StandingTile = Background_Tiles[Player.StandingTile_Index];

            }}
        
        }
        
        else if (W_KeyDown && !Interaction){
            Player.direction = DIR_UP;
         if(GetNextPlayerTile(&Player,3) < 5 || Player.noClip == 1){
        if(!Ctr_KeyDown){
            Player.animation_step = 1;
            Player.movementRemaining = 24;
            Player.idleFrameCount = 0;
            Player.StandingTile_Index -= NUMB_TILES_PER_ROW;
            Player.StandingTile = Background_Tiles[Player.StandingTile_Index];
            }
         }
        }
        
        else if (S_KeyDown && !Interaction){
        Player.direction = DIR_DOWN;
        if(GetNextPlayerTile(&Player,0) < 5 || Player.noClip == 1){
            if(!Ctr_KeyDown){
            Player.animation_step = 1;
            Player.movementRemaining = 24;
            Player.idleFrameCount = 0;
            Player.StandingTile_Index += NUMB_TILES_PER_ROW;
            Player.StandingTile = Background_Tiles[Player.StandingTile_Index];
        }
        }

    }   else if(EnterKeyDown && !enter_KeyWasDown){
        if(GetNextPlayerTile(&Player,Player.direction) == 10){
            
            switch (Player.direction)
            {
            case DIR_DOWN:
                Loaded_NPCs[CurrentNPCIndex].direction = DIR_UP;
                break;
            case DIR_LEFT:
                Loaded_NPCs[CurrentNPCIndex].direction = DIR_RIGHT;
                break;
            case DIR_RIGHT:
                Loaded_NPCs[CurrentNPCIndex].direction = DIR_LEFT;
                break;
            case DIR_UP:
                Loaded_NPCs[CurrentNPCIndex].direction = DIR_DOWN;
                break;
            default:
                break;
            }
            
            
            InteractionStart(Player,Loaded_NPCs[CurrentNPCIndex]);            
            }
    }

    }
    
    else {


     switch (Player.direction) {
        case DIR_DOWN:
            updatePlayerPosition(Player.worldPosY, TILE_SIZE*-8, TILE_SIZE*8, 120,DIR_DOWN);
            //UpdateWorldPos(&npc,DIR_DOWN);
            break;
        case DIR_LEFT:
            updatePlayerPosition(Player.worldPosX, TILE_SIZE*-9, TILE_SIZE*9, 200, DIR_LEFT);
            //UpdateWorldPos(&npc,DIR_LEFT);
            break;
        case DIR_RIGHT:
            updatePlayerPosition(Player.worldPosX, TILE_SIZE*-9, TILE_SIZE*9, 200,DIR_RIGHT);
            //UpdateWorldPos(&npc,DIR_RIGHT);
            break;
        case DIR_UP:
            updatePlayerPosition(Player.worldPosY, TILE_SIZE*-8, TILE_SIZE*8, 120, DIR_UP);
            //UpdateWorldPos(&npc,DIR_UP);
            break;
        default:
            break;
    }

    Player.movementRemaining--;

    if (Player.movementRemaining < 20) {
        Player.animation_step = 2;
    }

    if (Player.movementRemaining < 12) {
        Player.animation_step = 3;
    }
    
    }

    break;
    
    case GAME_DUNGEON:    

    Player.noClip = (C_KeyDown) ? 1 - Player.noClip : Player.noClip;

    if(T_KeyDown && !t_KeyWasDown){
        teleportPlayer();
    }
    if(R_KeyDown && ! r_KeyWasDown){
        HandleStairs();
    }

   
   if (Player.movementRemaining == 0){
    if(Player.StandingTile.type == STAIRS){
        
         ShowTextBox = 1;
         ShowOptBox = 1;
        
        if(S_KeyDown && !s_KeyWasDown){
                HandleOptionSelection(0);
            }
            else if(W_KeyDown && !w_KeyWasDown){
                HandleOptionSelection(1);
            }
            else if(EnterKeyDown && !enter_KeyWasDown ){
                switch (YesNoOptions)
                {
                case 0:
                    SendMessageA(gGameWindow,STAIRS,0,0);
                    break;
                
                default:
                    teleportPlayer();
                    break;
                }
            }
        
    }
    else{

    if (Player.idleFrameCount < 30){
        Player.animation_step = 0;
        Player.idleFrameCount++;
        
    }
    else if (Player.idleFrameCount >= 30 && Player.idleFrameCount < 60){
        Player.animation_step = 3;
        Player.idleFrameCount++;
    }
    else {
        Player.idleFrameCount = 0;
    }
    
    
    if (A_KeyDown){
        Player.direction = DIR_LEFT;
         if(GetNextPlayerTile(&Player,1) < 5 || GetNextPlayerTile(&Player,1) == STAIRS|| Player.noClip == 1){
            if(!Ctr_KeyDown){
                Player.animation_step = 1;
                Player.movementRemaining = 24;
                Player.idleFrameCount = 0;
                Player.StandingTile_Index += 1;
                Player.StandingTile = Background_Tiles[Player.StandingTile_Index];
            }
         }
    }
    else if (D_KeyDown){
         Player.direction = DIR_RIGHT;
         if(GetNextPlayerTile(&Player,2) < 5 || GetNextPlayerTile(&Player,2) == STAIRS|| Player.noClip == 1){
            if(!Ctr_KeyDown){
                Player.animation_step = 1;
                Player.movementRemaining = 24;
                Player.idleFrameCount = 0;
                Player.StandingTile_Index -= 1;
                Player.StandingTile = Background_Tiles[Player.StandingTile_Index];
            }
        }
    }
    else if (W_KeyDown){
         Player.direction = DIR_UP;
         if(GetNextPlayerTile(&Player,3) < 5 || GetNextPlayerTile(&Player,3) == STAIRS|| Player.noClip == 1){
        if(!Ctr_KeyDown){
        
            Player.animation_step = 1;
            Player.movementRemaining = 24;
            Player.idleFrameCount = 0;
            Player.StandingTile_Index -= NUMB_TILES_PER_ROW;
            Player.StandingTile = Background_Tiles[Player.StandingTile_Index];
        }
         }

    }
    else if (S_KeyDown){
        Player.direction = DIR_DOWN;
        if(GetNextPlayerTile(&Player,0) < 5 || GetNextPlayerTile(&Player,0) == STAIRS || Player.noClip == 1){
            if(!Ctr_KeyDown){
            Player.animation_step = 1;
            Player.movementRemaining = 24;
            Player.idleFrameCount = 0;
            Player.StandingTile_Index += NUMB_TILES_PER_ROW;
            Player.StandingTile = Background_Tiles[Player.StandingTile_Index];
        }

        }
        

    }
   }

   }
    
   else {


     switch (Player.direction) {
        case DIR_DOWN:
            updatePlayerPosition(Player.worldPosY, TILE_SIZE*-44, TILE_SIZE*44, 120,DIR_DOWN);
            break;
        case DIR_LEFT:
            updatePlayerPosition(Player.worldPosX, TILE_SIZE*-41, TILE_SIZE*41, 200, DIR_LEFT);
            break;
        case DIR_RIGHT:
            updatePlayerPosition(Player.worldPosX, TILE_SIZE*-41, TILE_SIZE*41, 200,DIR_RIGHT);
            break;
        case DIR_UP:
            updatePlayerPosition(Player.worldPosY, TILE_SIZE*-44, TILE_SIZE*44, 120, DIR_UP);
            break;
        default:
            break;
    }

    Player.movementRemaining--;

    if (Player.movementRemaining < 20) {
        Player.animation_step = 2;
    }

    if (Player.movementRemaining < 12) {
        Player.animation_step = 3;
    }
    if(Player.movementRemaining == 0){
        if(isPlayerInRoom(Player)){
            Player.InRoom = 1;
        }
        else{
            Player.InRoom = 0;
        }
    }
    
}


        break;
    default:
        break;
    }
    d_KeyWasDown = D_KeyDown;
    w_KeyWasDown = W_KeyDown;
    a_KeyWasDown = A_KeyDown;
    s_KeyWasDown = S_KeyDown;
    c_KeyWasDown = C_KeyDown;
    t_KeyWasDown = T_KeyDown;
    r_KeyWasDown = R_KeyDown;
    l_KeyWasDown = L_KeyDown;
    enter_KeyWasDown =EnterKeyDown;
    
    if (espKeyDown){
        SendMessageA(gGameWindow,WM_CLOSE,0,0);
    }
    if(cooldown_frames >0){
        cooldown_frames--;
    }



}


DWORD LoadDungeonIntoMemory(Dungeon Dungeon){
    DWORD Error = ERROR_SUCCESS;
    try{

    if(Load32BppBitmapFromFile(Dungeon.getPath(),&Tile_Sprite_Sheet) != ERROR_SUCCESS){
        MessageBoxA(NULL, "Unable to load font sheet into memory", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }

    InitTiles(Tile_Sprite_Sheet);
    stairs = ProceduralGenerator(1000,10,18,Background_Tiles,Tile_Type_Array,10);
    BuiltTileMap(Background_Tiles,&Background);

    graphics.SetBackgroundBitMap(&Background);



    if (InitPlayer() != ERROR_SUCCESS){
        MessageBoxA(NULL, "Error ititalizing player sprite", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }

    }
    catch(const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        
    }



    return Error;
}


DWORD LoadOverWorldIntoMemory(int32_t index, LPCSTR FloorPlanPath, LPCSTR DirPath){
    DWORD Error = ERROR_SUCCESS;
    
    GAMEBITMAP Temp;
    for(int i = 1; i < 7; i++){
        std::string indexStr = std::to_string(i);
        std::string fullPath = std::string(DirPath) + indexStr + ".bmp";
        LPCSTR Path = fullPath.c_str();
      

        if(Load32BppBitmapFromFile(Path,&Temp) != ERROR_SUCCESS){
            MessageBoxA(NULL, "Unable to load overworld", "Error", MB_ICONEXCLAMATION | MB_OK);
            Error = GetLastError();
        return Error;
        }
        Overworlds[index].SetSprite(Temp,(i-1));   

    }

    if(Load32BppBitmapFromFile(FloorPlanPath,&Temp) != ERROR_SUCCESS){
        MessageBoxA(NULL, "Unable to load overworld floorplan", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }
    Overworlds[index].SetFloorPlan(Temp);

    return Error;
}

DWORD SetCurrentOverWorld(Overworld Overworld){
    DWORD Error = ERROR_SUCCESS;
    FloorPlan = Overworld.GetFloorPlan();
    SetTilesOverworld(Background_Tiles,&FloorPlan);
    Background = Overworld.GetSprite(OverworldFramePointer);
    graphics.SetBackgroundBitMap(&Background);
    
    if (InitPlayerOverWorld() != ERROR_SUCCESS){
        MessageBoxA(NULL, "Error ititalizing player sprite to Overworld", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }

    

    if (InitNPC(&Loaded_NPCs[0],"assets\\Player_Sprites\\Walk-Anim-m.bmp","assets\\Player_Sprites\\MakPort.bmp","assets\\dialogue\\makuhita-dialog.txt",MAKUHITA_DOJO,-125,125,-8,-35) != ERROR_SUCCESS){
        MessageBoxA(NULL, "Error ititalizing player sprite to Overworld", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }
    if (InitNPC(&Loaded_NPCs[1],"assets\\Player_Sprites\\Walk-Anim-ch.bmp","assets\\Player_Sprites\\port-ch.bmp","assets\\dialogue\\chatot-dialog.txt",POKEMON_SQUARE,50,0,-14,-23) != ERROR_SUCCESS){
        MessageBoxA(NULL, "Error ititalizing player sprite to Overworld", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }
    //memcpy(&npc,&Loaded_NPCs[0],sizeof(NPC));
    
    return Error;

}

VOID HandleStairs(){
        if(Dungeons[0].getCurrentFloor() >= Dungeons[0].getNumberOfFloors()){
            LoadingText = "Completed";
            Mode = 0;
             HandGamestateChange(gamestate,GAME_OVERWORLD,Mode);
             
        }
        else{
        // Set gamestate and Loading text to whatever floor of the dungeon // 
        Dungeons[0].NextFloor();
        LoadingText = Dungeons[0].getNameWithCurrentFloor();
        
        HandGamestateChange(gamestate,GAME_LOADING_SCREEN,Mode);

        // Generate Next floor // 
        ResetTiles(Background_Tiles,Tile_Type_Array);
        stairs = ProceduralGenerator(1000,10,18,Background_Tiles,Tile_Type_Array,10);
        BuiltTileMap(Background_Tiles,&Background);

        // Init player
        teleportPlayer();
        ShowTextBox = 0;
        ShowOptBox = 0;
        }
}

void updatePlayerPosition(int32_t& playerPos, int lowerBound, int upperBound, int screenLimit, int direction) {
    switch (direction) {
        case DIR_DOWN:

        if (playerPos <= lowerBound){Player.ScreenPosY++;}

        else if (playerPos >= upperBound){
            
            if (Player.ScreenPosY < screenLimit){Player.ScreenPosY++;}
            else{playerPos--;}
            
            }

            
        else{playerPos--;}
        break;
        
        case DIR_LEFT:

        if (playerPos <= lowerBound){Player.ScreenPosX--;}
            else if (playerPos >= upperBound){

                if (Player.ScreenPosX > screenLimit){Player.ScreenPosX--;}
                else{playerPos--;}
            
            }
            else{playerPos--;}
            break;

        case DIR_RIGHT:

        if (playerPos >= upperBound){Player.ScreenPosX++;}
            else if (playerPos <= lowerBound){

                if (Player.ScreenPosX < screenLimit){Player.ScreenPosX++;}
                else{playerPos++;}

            }
            else{playerPos++;}
            break;

        case DIR_UP:
        
        if (playerPos >= upperBound){Player.ScreenPosY--;}
        else if (Player.worldPosY <= lowerBound){
            
            if (Player.ScreenPosY > screenLimit){Player.ScreenPosY--;}
            else{playerPos++;}

            }
            else{playerPos++;}
            
            break;
        default:
            break;
    }


}

VOID teleportPlayer(){
    InitPlayer();

}

VOID TeleportToStairs(PLAYER Player){
    
}

VOID SetWorldPosition(int32_t TileIndex){
    int32_t x,xDifference,y,yDifference;
    x = (TileIndex%NUMB_TILES_PER_ROW) - (NUMB_TILES_PER_ROW/2);
    y = (TileIndex/NUMB_TILES_PER_ROW) - (NUMB_TILES_PER_ROW/2);


    Player.worldPosX = x;
    Player.worldPosY = y;
    //Player.ScreenPosX += xDifference;
    //Player.ScreenPosY += yDifference;
    Player.StandingTile = Background_Tiles[TileIndex];
    Player.StandingTile_Index = TileIndex;
    

}

VOID render_game_frames(void){
    
    switch (gamestate)
    {
    case GAME_TITLE_SCREEN:
        RenderTitleScene();
        break;
    case GAME_MAIN_MENU:
        RenderMainMenuScene();
        break;
    case GAME_OVERWORLD:
        RenderOverWorld(&Player);
        break;
    case GAME_DUNGEON:
        RenderDungeonScene(Background,&Player);
        break;

    case GAME_LOADING_SCREEN:
        RenderLoadingScene(LoadingText);
        break;
    case GAME_TEXT_BOX_TESTING:
        RenderTestZone();
    
    default:
        
        break;
    }
    
    HDC deviceContext = GetDC(gGameWindow);
    StretchDIBits(deviceContext,0,0,gMonitorWidth,gMonitorHeight,0,0,GAME_WIDTH,GAME_HEIGHT,DrawingSurface.memory,&DrawingSurface.bitMapInfo,DIB_RGB_COLORS,SRCCOPY);
    
    char fpsBuffer[64] = {0};
    sprintf(fpsBuffer, "Cooked FPS: %.01f Raw FPS: %.01f Screen Position: %d:%d, %d %d",gPreformance_Data.CookFPS,gPreformance_Data.RawFPS,Player.worldPosX, Player.worldPosY, Player.StandingTile_Index,IsTileOnScreen(STARTING_TILE));
    //sprintf(fpsBuffer, "Cooked FPS: %.01f Raw FPS: %.01f",gPreformance_Data.CookFPS,gPreformance_Data.RawFPS);
    SetTextColor(deviceContext, RGB(255, 255, 255));  
    SetBkMode(deviceContext, TRANSPARENT );
    TextOutA( deviceContext, 0, 0, fpsBuffer, strlen( fpsBuffer ) );
     
    
    ReleaseDC(gGameWindow,deviceContext);

   

}
// Animation Movement and Vis // 
VOID NPC_AMV_Handler(){
        for (int i = 0; i < Loaded_NPCs.size();i++){

        switch (Loaded_NPCs[i].idleFrameCount)
        {
        case 40:
            Loaded_NPCs[i].animation_step = 0;
            Loaded_NPCs[i].idleFrameCount++;
            break;
        case 80:
            Loaded_NPCs[i].animation_step = 1;
            Loaded_NPCs[i].idleFrameCount = 0;
            break;
        // case 120:
        //     Loaded_NPCs[i].animation_step = ;
        //     Loaded_NPCs[i].idleFrameCount = 0;
        //     break;
        // case 175:
        //     Loaded_NPCs[i].animation_step = 3;
        //     Loaded_NPCs[i].idleFrameCount = 0;
        //     break;
        
        default:
            Loaded_NPCs[i].idleFrameCount++;
            break;
        }
        }
    
}



VOID initBitMap(void){
     
     
     DrawingSurface.bitMapInfo.bmiHeader.biSize = sizeof(DrawingSurface.bitMapInfo.bmiHeader);
     DrawingSurface.bitMapInfo.bmiHeader.biWidth = GAME_WIDTH;
     DrawingSurface.bitMapInfo.bmiHeader.biHeight = GAME_HEIGHT;
     DrawingSurface.bitMapInfo.bmiHeader.biBitCount = GAME_BPP;
     DrawingSurface.bitMapInfo.bmiHeader.biCompression = 0;
     DrawingSurface.bitMapInfo.bmiHeader.biPlanes = 1;
     DrawingSurface.memory = VirtualAlloc(NULL,GAME_DRAWING_AREA_MEMORY_SIZE,MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

}

DWORD Load32BppBitmapFromFile(LPCSTR fileName,GAMEBITMAP* GameBitMap){
    DWORD Error = ERROR_SUCCESS;
    HANDLE FileHandle = INVALID_HANDLE_VALUE;
    WORD BitMapHeader = 0;
    DWORD PixelOffset = 0;
    DWORD BytesToRead = 2;

    FileHandle = CreateFileA(fileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
    if (FileHandle == INVALID_HANDLE_VALUE){
        Error = GetLastError();
        CloseHandle(FileHandle);
        return Error;
    }

    
    if(ReadFile(FileHandle,&BitMapHeader,2,&BytesToRead,NULL) == 0){
        Error = GetLastError();
        CloseHandle(FileHandle);
        return Error;
    }
    

    if (BitMapHeader != 0x4d42){
        Error = ERROR_FILE_INVALID;
        CloseHandle(FileHandle);
        return Error;
    }

    if(SetFilePointer(FileHandle,0XA,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER){
        Error = GetLastError();
        CloseHandle(FileHandle);
        return Error;
    }
    

    if(ReadFile(FileHandle,&PixelOffset,sizeof(DWORD),&BytesToRead,NULL) == 0){
        Error = GetLastError();
        CloseHandle(FileHandle);
        return Error;
    }

    if(SetFilePointer(FileHandle,0XE,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER){
        Error = GetLastError();
        CloseHandle(FileHandle);
        return Error;
    }

    if(ReadFile(FileHandle,&GameBitMap->bitMapInfo.bmiHeader,sizeof(BITMAPINFOHEADER),&BytesToRead,NULL) == 0){
        Error = GetLastError();
        CloseHandle(FileHandle);
        return Error;
    }

    GameBitMap->memory = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,GameBitMap->bitMapInfo.bmiHeader.biSizeImage);
    if (GameBitMap->memory == NULL){
        Error = ERROR_NOT_ENOUGH_MEMORY;
        CloseHandle(FileHandle);
        return Error;
    }

    if(SetFilePointer(FileHandle,PixelOffset,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER){
        Error = GetLastError();
        CloseHandle(FileHandle);
        return Error;
    }
    
    
    if(ReadFile(FileHandle,GameBitMap->memory,GameBitMap->bitMapInfo.bmiHeader.biSizeImage,&BytesToRead,NULL) == 0){
        Error = GetLastError();

        CloseHandle(FileHandle);
        return Error;
    }
    

    CloseHandle(FileHandle);
    return Error;

}


DWORD InitPlayer(VOID){
    
    DWORD Error = ERROR_SUCCESS;

    int16_t RandomSpawnX = std::rand() % 41 - 20;
    int16_t RandomSpawnY = std::rand() % 41 - 20;
    int32_t index = STARTING_TILE-(RandomSpawnX)- NUMB_TILES_PER_ROW*RandomSpawnY;
    
    while (Background_Tiles[index].type > FLOOR5)
    {
        RandomSpawnX = std::rand() % 41 - 20;
        RandomSpawnY = std::rand() % 41 - 20;
        index = STARTING_TILE-(RandomSpawnX)- NUMB_TILES_PER_ROW*RandomSpawnY;
    }

    Player.worldPosX=TILE_SIZE*RandomSpawnX;
    Player.worldPosY=TILE_SIZE*RandomSpawnY;

    Player.ScreenPosX = GAME_WIDTH/2;
    Player.ScreenPosY = GAME_HEIGHT/2;
    Player.movementRemaining = 0;
    Player.animation_step = 1;
    Player.direction = DIR_DOWN;
    Player.idleFrameCount = 0;
    Player.StandingTile = Background_Tiles[index];
    Player.StandingTile_Index = index;
    Player.noClip = 0;
    Player.InRoom = 1;
    

    
    /* Load player sprites */
    
    
    /* Load player sprites sheets */
    Error = Load32BppBitmapFromFile("assets\\Player_Sprites\\Walk-Anim.bmp",&Player.sprite_sheet[0]);
    Error = Load32BppBitmapFromFile("assets\\Player_Sprites\\Idle-Anim.bmp",&Player.sprite_sheet[1]);

    /* Walking Animations */

    /* Down */
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[0][0],8,5,7,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[0][1],8,5,7,1);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[0][2],8,5,7,2);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[0][3],8,5,7,4);

    /* Left */
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[1][0],8,5,1,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[1][1],8,5,1,1);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[1][2],8,5,1,2);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[1][3],8,5,1,4);

    /* Right */
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[2][0],8,5,5,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[2][1],8,5,5,1);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[2][2],8,5,5,2);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[2][3],8,5,5,4);

    /* Up */
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[3][0],8,5,3,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[3][1],8,5,3,1);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[3][2],8,5,3,2);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[3][3],8,5,3,4);
    
    

    if (Error != ERROR_SUCCESS){
        Error = GetLastError();
    }
    return Error;
}

DWORD InitPlayerOverWorld(VOID){
    
    DWORD Error = ERROR_SUCCESS;


    Player.worldPosX=0;
    Player.worldPosY=0;
    // Player.worldPosY=0-TILE_SIZE*3;

    Player.ScreenPosX = GAME_WIDTH/2;
    Player.ScreenPosY = GAME_HEIGHT/2;
    Player.movementRemaining = 0;
    Player.animation_step = 1;
    Player.direction = DIR_DOWN;
    Player.idleFrameCount = 0;
    // Player.StandingTile = Background_Tiles[STARTING_TILE + (NUMB_TILES_PER_ROW*3)];
    // Player.StandingTile_Index = STARTING_TILE + (NUMB_TILES_PER_ROW*3);
    Player.StandingTile = Background_Tiles[STARTING_TILE];
    Player.StandingTile_Index = STARTING_TILE; 
    Player.noClip = 0;
    // Player.InRoom = 1;
    

    
    /* Load player sprites */
    
    
    /* Load player sprites sheets */
    Error = Load32BppBitmapFromFile("assets\\Player_Sprites\\Walk-Anim.bmp",&Player.sprite_sheet[0]);
    Error = Load32BppBitmapFromFile("assets\\Player_Sprites\\Idle-Anim.bmp",&Player.sprite_sheet[1]);

    /* Walking Animations */

    /* Down */
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[0][0],8,5,7,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[0][1],8,5,7,1);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[0][2],8,5,7,2);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[0][3],8,5,7,4);

    /* Left */
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[1][0],8,5,1,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[1][1],8,5,1,1);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[1][2],8,5,1,2);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[1][3],8,5,1,4);

    /* Right */
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[2][0],8,5,5,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[2][1],8,5,5,1);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[2][2],8,5,5,2);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[2][3],8,5,5,4);

    /* Up */
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[3][0],8,5,3,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[3][1],8,5,3,1);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[3][2],8,5,3,2);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[3][3],8,5,3,4);
    
    

    if (Error != ERROR_SUCCESS){
        Error = GetLastError();
    }
    return Error;
}

DWORD InitNPC(NPC* npc, LPCSTR SpriteFilePath,LPCSTR PortraitFilePath, LPCSTR DialogueFilePath, int32_t overworld, int32_t x, int32_t y, int32_t offsetX,int32_t offsetY){
    DWORD Error = ERROR_SUCCESS;
    npc->worldPosX=x;
    npc->worldPosY=y;
    npc->ScreenPosX=0;
    npc->ScreenPosY=0;
    npc->OffsetX = offsetX;
    npc->OffsetY = offsetY;
    npc->StandingTile_Index = STARTING_TILE - npc->worldPosX%TILE_SIZE + (npc->worldPosY%TILE_SIZE)*NUMB_TILES_PER_ROW;

    npc->movementRemaining = 0;
    npc->animation_step = 1;
    npc->direction = DIR_DOWN;
    npc->idleFrameCount = 0;
    npc->overworld = overworld;
    npc->visbility = 0;

    if(CurrentOverworld == npc->overworld){
        npc->visbility = 1;
    }
    
    /* Load player sprites */
    
    
    /* Load player sprites sheets */
    Error = Load32BppBitmapFromFile(SpriteFilePath,&npc->sprite_sheet[0]);
    Error = Load32BppBitmapFromFile(PortraitFilePath,&npc->Portrait);
    

    /* Walking Animations */

    /* Down */
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[0][0],8,4,7,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[0][1],8,4,7,1);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[0][2],8,4,7,2);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[0][3],8,4,7,3);

    /* Left */
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[1][0],8,0,1,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[1][1],8,0,1,1);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[1][2],8,0,1,2);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[1][3],8,0,1,3);

    /* Right */
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[2][0],8,0,5,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[2][1],8,0,5,1);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[2][2],8,0,5,2);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[2][3],8,0,5,3);

    /* Up */
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[3][0],8,0,3,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[3][1],8,0,3,1);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[3][2],8,0,3,2);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[3][3],8,0,3,3);


    if (!npc->Dialogue.IsSet()){
    npc->Dialogue = Dialogue();
    setDialogFromTextFile(&npc->Dialogue,DialogueFilePath);
    npc->Dialogue.Set();
    }


    


    if (Error != ERROR_SUCCESS){
        Error = GetLastError();
    }
    return Error;
}

VOID InitTiles(GAMEBITMAP tile_spritesheet){

   

   int32_t Tile_Starting_Points[NUMB_TILE_TYPES];
   int32_t Starting_Address;
   int32_t bytesPerTile = 4 * TILE_SIZE * TILE_SIZE;
   

   
    Tile_Starting_Points[FLOOR1] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*12 - (TILE_SIZE+1)*(tile_spritesheet.bitMapInfo.bmiHeader.biWidth*16)) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[FLOOR2] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*1 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*13) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[FLOOR3] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*19 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*1) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[FLOOR4] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*12 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*4)- tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[FLOOR5] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*20 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*15)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;

    Tile_Starting_Points[WALL_FULL] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*4 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*1)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[WALL_DOWN] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*4 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*2)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[WALL_UP] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*4 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*0)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[WALL_LEFT] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*3 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*1)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[WALL_RIGHT] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*5 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*1)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;

    Tile_Starting_Points[CORNER_UP_LEFT] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*3 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*0)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[CORNER_UP_RIGHT] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*5 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*0)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[CORNER_DOWN_LEFT] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*3 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*2)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[CORNER_DOWN_RIGHT] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*5 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*2)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;

    Tile_Starting_Points[CORNER_UP_LEFT_IN] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*3 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*16)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[CORNER_UP_RIGHT_IN] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*4 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*16)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[CORNER_DOWN_LEFT_IN] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*3 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*15)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[CORNER_DOWN_RIGHT_IN] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*4 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*15)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;

    Tile_Starting_Points[ISLAND_SINGLE] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*4 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*4)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[ISLAND_DOWN] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*4 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*6)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[ISLAND_UP] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*4 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*8)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[ISLAND_LEFT] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*3 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*7)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[ISLAND_RIGHT] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*5 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*7)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[ISLAND_CENTER] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*4 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*7)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;

    Tile_Starting_Points[V_SINGLE_WALL] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*3 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*4)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[H_SINGLE_WALL] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*4 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*3)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[WALL_FULL_2] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*7 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*1)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[WALL_FULL_3] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*10 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*1)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;
    Tile_Starting_Points[STAIRS] = ((tile_spritesheet.bitMapInfo.bmiHeader.biHeight * tile_spritesheet.bitMapInfo.bmiHeader.biWidth) - tile_spritesheet.bitMapInfo.bmiHeader.biWidth) + ((TILE_SIZE+1)*7 - (TILE_SIZE+1)*tile_spritesheet.bitMapInfo.bmiHeader.biWidth*21)-tile_spritesheet.bitMapInfo.bmiHeader.biWidth;


   for (int i = 0; i < NUMB_TILE_TYPES; i++){
    Starting_Address = Tile_Starting_Points[i];
    GAMEBITMAP tileBitMap = {0};
    tileBitMap.bitMapInfo.bmiHeader.biBitCount = GAME_BPP;
    tileBitMap.bitMapInfo.bmiHeader.biWidth = TILE_SIZE;
    tileBitMap.bitMapInfo.bmiHeader.biHeight = TILE_SIZE;
    tileBitMap.bitMapInfo.bmiHeader.biCompression = 0;
    tileBitMap.bitMapInfo.bmiHeader.biPlanes = 1;
    tileBitMap.memory = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,bytesPerTile);

    int32_t Tile_Sprite_Offset = 0;
    int32_t Tile_Memory_Offset = 0;
    PIXEL TilePixels = {0};
    TILE Tile;
    
    for(int32_t PixelY = 0; PixelY < TILE_SIZE; PixelY++){
            
            for(int32_t PixelX = 0; PixelX < TILE_SIZE; PixelX++){
            
            Tile_Sprite_Offset = (Starting_Address + PixelX) - (tile_spritesheet.bitMapInfo.bmiHeader.biWidth*PixelY);
            Tile_Memory_Offset = ((tileBitMap.bitMapInfo.bmiHeader.biHeight*tileBitMap.bitMapInfo.bmiHeader.biWidth) - tileBitMap.bitMapInfo.bmiHeader.biWidth) + PixelX - (tileBitMap.bitMapInfo.bmiHeader.biWidth* PixelY);
            
            memcpy(&TilePixels,(PIXEL*)tile_spritesheet.memory + Tile_Sprite_Offset,sizeof(PIXEL));
            memcpy((PIXEL*)tileBitMap.memory + Tile_Memory_Offset,&TilePixels,sizeof(PIXEL));
        }
        
    }
    
    Tile.tile_sprite = tileBitMap;
    Tile.type = i;
    Tile.visited = 0;
    Tile_Type_Array[i] = Tile;

   }

   for (int i = 0; i<NUMB_TILES;i++){
    Background_Tiles[i] = Tile_Type_Array[WALL_FULL];
   }

}



int32_t GetPlayerTile(PLAYER *player){
    
    return player->StandingTile_Index;

}

int32_t GetNextPlayerTile(PLAYER *player,int32_t Direction){
        int32_t tileIndex = player->StandingTile_Index;

        

        switch (Direction)
        {
        case 0:
            tileIndex += NUMB_TILES_PER_ROW;

            break;
        case 1:
            tileIndex += 1;
            break;

        case 2:
            tileIndex -= 1;
            break;
        case 3:
            tileIndex -= NUMB_TILES_PER_ROW;
            break;
        
        default:
            /* No dir */
            break;
        }
    
        //printf("next tile: %d\n",tileIndex);
        for (int i = 0; i < Loaded_NPCs.size();i++){
            if(tileIndex == Loaded_NPCs[i].StandingTile_Index && Loaded_NPCs[i].visbility ==1){
                CurrentNPCIndex = i;
            return 10;
        }
        }
        
        return Background_Tiles[tileIndex].type;
        
        
}

VOID BuiltTileMap(TILE* Tile_Array,GAMEBITMAP* backgroundBitMap){

    
    backgroundBitMap->bitMapInfo.bmiHeader.biBitCount = GAME_BPP;
    backgroundBitMap->bitMapInfo.bmiHeader.biWidth = MAP_WIDTH;
    backgroundBitMap->bitMapInfo.bmiHeader.biHeight = MAP_HEIGHT;
    backgroundBitMap->bitMapInfo.bmiHeader.biCompression = 0;
    backgroundBitMap->bitMapInfo.bmiHeader.biPlanes = 1;
    int32_t bytesNeeded = 4 * backgroundBitMap->bitMapInfo.bmiHeader.biWidth * backgroundBitMap->bitMapInfo.bmiHeader.biHeight;
    backgroundBitMap->memory = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,bytesNeeded);

    int32_t Starting_Address = Tile_Array[0].tile_sprite.bitMapInfo.bmiHeader.biWidth * Tile_Array[0].tile_sprite.bitMapInfo.bmiHeader.biHeight - Tile_Array[0].tile_sprite.bitMapInfo.bmiHeader.biWidth;
    int32_t Tile_Sprite_Offset = 0;
    int32_t BackgroundBitMapOffset = 0;
    PIXEL BackgroundPixels = {0};

    int32_t tileIndex = NUMB_TILES;
    for (int y = NUMB_TILES_PER_ROW - 1; y >= 0; y--){
        
        for (int x = 0; x < NUMB_TILES_PER_ROW; x++){

        GAMEBITMAP TileBitMap = Tile_Array[tileIndex].tile_sprite;
        tileIndex -=1;
        
        for(int32_t PixelY = 0; PixelY < TILE_SIZE; PixelY++){

            for(int32_t PixelX = 0; PixelX < TILE_SIZE; PixelX++){

                Tile_Sprite_Offset = Starting_Address + PixelX - (TileBitMap.bitMapInfo.bmiHeader.biWidth*PixelY);
                
                BackgroundBitMapOffset = (x*TILE_SIZE) + ((backgroundBitMap->bitMapInfo.bmiHeader.biHeight*backgroundBitMap->bitMapInfo.bmiHeader.biWidth) - backgroundBitMap->bitMapInfo.bmiHeader.biWidth) + PixelX - (backgroundBitMap->bitMapInfo.bmiHeader.biWidth* PixelY) - (y*backgroundBitMap->bitMapInfo.bmiHeader.biWidth*TILE_SIZE);
                
                memcpy(&BackgroundPixels,(PIXEL*)TileBitMap.memory + Tile_Sprite_Offset,sizeof(PIXEL));
                
                memcpy((PIXEL*)backgroundBitMap->memory + BackgroundBitMapOffset,&BackgroundPixels,sizeof(PIXEL));
                }
            }
        }
    }

}


DWORD LoadSpriteFromSpriteSheet(GAMEBITMAP SpriteSheet, GAMEBITMAP *player_spite_box, int16_t SpriteCountRow,int16_t SpriteCountCol, int16_t Row, int16_t Col){
    DWORD Error = ERROR_SUCCESS;
    int32_t Starting_Address = 0;
    int32_t Offset = 0;
    
    player_spite_box->bitMapInfo.bmiHeader.biSize = sizeof(player_spite_box->bitMapInfo.bmiHeader);
    player_spite_box->bitMapInfo.bmiHeader.biWidth = 64;
    player_spite_box->bitMapInfo.bmiHeader.biHeight = 64;
    player_spite_box->bitMapInfo.bmiHeader.biSizeImage = 4096*4;
    
    player_spite_box->bitMapInfo.bmiHeader.biBitCount = GAME_BPP;
    player_spite_box->bitMapInfo.bmiHeader.biCompression = 0;
    player_spite_box->bitMapInfo.bmiHeader.biPlanes = 1;
    
    
    player_spite_box->memory = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,player_spite_box->bitMapInfo.bmiHeader.biSizeImage);
    if (player_spite_box->memory == NULL){
        Error = ERROR_NOT_ENOUGH_MEMORY;
        return Error;
    }


    Row *= SpriteSheet.bitMapInfo.bmiHeader.biHeight/SpriteCountRow;
    
    int32_t spriteSheetRowIndex = SpriteSheet.bitMapInfo.bmiHeader.biWidth * 4;  
    int32_t spriteSheetRowCol = 128; 
    int32_t spriteSheetPointer = (Row*spriteSheetRowIndex) + (spriteSheetRowCol*Col); // Width * 4 * index of height (jumps by 64)
    int32_t memoryOffset = 0;
    
    
    for (int i = 0; i < SpriteSheet.bitMapInfo.bmiHeader.biHeight/SpriteCountRow; i++) {
    memcpy((char*)player_spite_box->memory + memoryOffset, (char*)SpriteSheet.memory +spriteSheetPointer , 128);
    spriteSheetPointer += (SpriteSheet.bitMapInfo.bmiHeader.biWidth)*4;
    memoryOffset += 64*4;
    }
    return Error;
}


// RENDERING VARIOUS SCRENES // 


VOID RenderDungeonScene(GAMEBITMAP BackgroundBitMap, PLAYER* Player){
    // LoadBackgroundToScreen(BackgroundBitMap,Player,&DrawingSurface);
    
    graphics.LoadBackgroundToScreen();
    graphics.LoadBitMapToScreen(Player->sprite[Player->direction][Player->animation_step],Player->ScreenPosX,Player->ScreenPosY,-20,-12);
    
     
     switch (ShowTextBox)
    {
    case 1:
        DisplayTextBox(TextBox,DungeonDialogue);
        break;
    
    default:
        break;
    }
    switch (ShowOptBox)
    {
    case 1:
        DisplayOptBox(OptionBox);
        break;
    
    default:
        break;
    }
    
}

VOID RenderTitleScene(){

    graphics.LoadBackScreen();
    graphics.LoadBitFontToScreen(Font,"Project B - Alpha", GAME_WIDTH/2, GAME_HEIGHT/2,1);
    switch (flicker)
    {
    case 1:
        graphics.LoadBitFontToScreen(Font,"Press Enter to continue", GAME_WIDTH/2, GAME_HEIGHT/2 + 15,1);
        break;
    
    default:
        break;
    }
    
       

}



VOID RenderOverWorld(PLAYER* Player){
    
    graphics.LoadOverWorldToDrawingSurface();
    // graphics.LoadBitMapToScreen(Player->sprite[Player->direction][Player->animation_step],Player->ScreenPosX,Player->ScreenPosY,-20,-12);
    for(int i =0; i < 2; i++){
        if(Loaded_NPCs[i].visbility == 1){
        WorldPosBasedRender(&Loaded_NPCs[i],Player);   
    }

    }

     graphics.LoadBitMapToScreen(Player->sprite[Player->direction][Player->animation_step],Player->ScreenPosX,Player->ScreenPosY,-20,-12);

    switch (Interaction)
    {
    case TRUE:
            DisplayTextBox(TextBox,CurrentDialogue);
            DisplayPortrait(CurrentPortrait);
            DisplayOptBox(OptionBox);
        break;
    
    default:
        break;
    }

   
    
    
    
    //graphics.LoadScreenBlackBars();
}

VOID RenderMainMenuScene(){

    graphics.LoadBackScreen();
    graphics.LoadBitFontToScreen(Font,"Select a Dungeon", GAME_WIDTH/2, GAME_HEIGHT/2,1);
    DisplayMainMenuToScreen(MainMenu,45,180);
    // graphics.LoadBackScreen();
    // graphics.LoadOverWorldToDrawingSurface();

}

VOID RenderLoadingScene(char* str){
    graphics.LoadBackScreen();
    graphics.LoadBitFontToScreen(Font,str, GAME_WIDTH/2, GAME_HEIGHT/2,1);
}


VOID RenderTestZone(){
    graphics.LoadBackScreen();
    graphics.LoadBitFontToScreen(Font,"Testing Zone", GAME_WIDTH/2, 24,1);
    DisplayTextBox(TextBox,TestingDialogue);
    DisplayOptBox(OptionBox);
    DisplayPortrait(PortTest);
}


VOID DisplayTextBox(GAMEBITMAP TextBox, Dialogue Dialogue) {
    if(ShowTextBox){
    int32_t x = 40;
    int32_t y = 180;
    graphics.LoadBitMapToScreen(TextBox, x, y, 0, 0);
     if(Dialogue.getDialogue(Dialogue.getCurrentPage(),3) == "opt"){
            ShowOptBox = 1;
    }
    else if (Dialogue.getDialogue(Dialogue.getCurrentPage(),3) == "menu"){
        // Toggle current menu //
    }



    for (int i = 0; i < 3; i++){
        graphics.LoadBitFontToScreen(Font, Dialogue.getDialogue(Dialogue.getCurrentPage(),i), x+1, y+1, 0);
        y += 10;
    }
    }
    
}

VOID DisplayOptBox(GAMEBITMAP OptionBox) {

    if(ShowOptBox){
        
    

     int32_t x = 312;
     int32_t y = 140;

     
     graphics.LoadBitMapToScreen(OptionBox, x, y, 0, 0);
     
     graphics.LoadBitFontToScreen(Font,"Yes", x+10, y+2, 0);
     graphics.LoadBitFontToScreen(Font,"No", x+12, y+14, 0);
    
    switch (YesNoOptions)
    {
    case 0:
        graphics.LoadBitFontToScreen(Font,"-", x+2, y+2, 0);
        break;
    
    default:
        graphics.LoadBitFontToScreen(Font,"-", x+2, y+14, 0);
        break;
    }
    }
    
}

VOID DisplayPortrait(GAMEBITMAP Portrait){
    graphics.LoadBitMapToScreen(PortraitBox, 40, 130, 0, 0);
    graphics.LoadBitMapToScreen(Portrait, 44, 133, 0, 0);
}


BOOL HandleDialog(Dialogue* Dialogue){
    // If on final page return true;
    if(Dialogue->getCurrentPage() >= (Dialogue->getNumberOfPages() - 1)){
        return TRUE;
    }
    else{
        Dialogue->incrementPage();
        return FALSE;
    }
}

VOID HandleOptionSelection(int_fast8_t input){
    switch (input)
    {
    case 0:
        YesNoOptions = 1;
        break;
    case 1:
        YesNoOptions = 0;
        break;
    default:
        break;
    }
}

VOID ToggleTextBox(){
    ShowTextBox = ShowTextBox^1;
}

VOID ToggleOptBox(){
    ShowOptBox = ShowOptBox^1;
}

VOID HandGamestateChange(GAMESTATE Current, GAMESTATE Next, int32_t Mode){
    

    switch (Current)
    {

    case GAME_OPENING:

        gamestate = GAME_TITLE_SCREEN;
        break;
    
    case GAME_MAIN_MENU:
    
        ToggleTextBox();
        ToggleOptBox();
        LoadDungeonIntoMemory(Dungeons[MainMenu.getSelectedItem()]);      
        gamestate = GAME_LOADING_SCREEN;
        break;
    
    case GAME_TITLE_SCREEN:

        switch (Next)
        {
        case GAME_TEXT_BOX_TESTING:
            gamestate = GAME_TEXT_BOX_TESTING;
            break;
        case GAME_MAIN_MENU:
            gamestate = GAME_MAIN_MENU;
        
        default:
        SetCurrentOverWorld(Overworlds[CurrentOverworld]);
        gamestate = GAME_OVERWORLD;
        break;
        
        }
        
        break;
    
     case GAME_OVERWORLD:
        

        switch (Mode)
        {

        case 0:
            // Stay in Overworld //
            LoadingText = "";
            Prvgamestate = GAME_OVERWORLD;
            gamestate = GAME_LOADING_SCREEN;
            break;
        
        case 1:
        // Switching to Dung // 
        ToggleTextBox();
        LoadingText = Dungeons[0].getNameWithCurrentFloor();
        LoadDungeonIntoMemory(Dungeons[0]);      
        gamestate = GAME_LOADING_SCREEN;
        break;
        
        default:
            break;
        }
        
        break;
        
    case GAME_DUNGEON:
        
        switch (Mode)
        {
        case 1:
            // Stay in Dung //
            gamestate = GAME_LOADING_SCREEN;            
            break;
        case 0:
            // Switching to Overworld // 
            SetCurrentOverWorld(Overworlds[CurrentOverworld]);
            Dungeons[0].Reset();
            ShowTextBox = 1;
            ShowOptBox = 0;
            gamestate = GAME_LOADING_SCREEN;
            break;
        
        default:
            
            break;
        }


        break;

    case GAME_LOADING_SCREEN:

        switch (Mode)
        {
        case 1:
            
            gamestate = GAME_DUNGEON;
            break;
        case 0:
            gamestate = GAME_OVERWORLD;
            break;
        
        default:
            break;
        }


        break;
    
    default:
        break;
    }
}

VOID DisplayMainMenuToScreen(Menu menu, int32_t x, int32_t y){
    int32_t distance = GAME_WIDTH/(menu.GetCount()+1);
    int32_t start = GAME_WIDTH*GAME_HEIGHT - GAME_WIDTH;
    int32_t offset = 0;

    for(int32_t i = 0; i < menu.GetCount();i++){
        graphics.LoadBitFontToScreen(Font,menu.GetItemContent(i), x, y, 0);
        if(menu.getSelectedItem() == i){
        offset = start + (x+4) - ((y+12)*GAME_WIDTH);
        memset((PIXEL*)DrawingSurface.memory + offset,255,sizeof(PIXEL)*(distance-25));
        }
        x+=distance;
   
    }
}

VOID HandleOverworldChange(int32_t Current, int32_t Direction){
    
    switch (Current)
    {
    case POKEMON_SQUARE:
        CurrentOverworld++;
        HandGamestateChange(GAME_OVERWORLD,GAME_LOADING_SCREEN,Mode);
        SetCurrentOverWorld(Overworlds[CurrentOverworld]);
        

        break;
    case MAKUHITA_DOJO:
        CurrentOverworld--;
        HandGamestateChange(GAME_OVERWORLD,GAME_LOADING_SCREEN,Mode);
        SetCurrentOverWorld(Overworlds[CurrentOverworld]);
        Player.worldPosY -= 9*TILE_SIZE;
        Player.direction = 3;
        Player.StandingTile_Index = 5950;
        Player.StandingTile = Background_Tiles[5950];
        break;
    
    default:
        break;
    }        
}


BOOLEAN IsTileOnScreen(int32_t TileIndex) {
    int32_t left_limit = Player.StandingTile_Index % 100 + GAME_WIDTH / (2 * TILE_SIZE);
    int32_t right_limit = Player.StandingTile_Index % 100 - GAME_WIDTH / (2 * TILE_SIZE);
    int32_t top_limit = Player.StandingTile_Index / 100 + GAME_HEIGHT / (2 * TILE_SIZE);
    int32_t bot_limit = Player.StandingTile_Index / 100 - GAME_HEIGHT / (2 * TILE_SIZE);

    return (TileIndex%100 <= left_limit && TileIndex%100 >= right_limit && TileIndex/100 <= top_limit+1 && TileIndex/100 >= bot_limit-1);
}


VOID WorldPosBasedRender(NPC* npc, PLAYER* p){
    // calulate npc screen pos based on relative player tile // 


    
    npc->ScreenPosX = npc->worldPosX - p->worldPosX + (GAME_WIDTH/2);
    npc->ScreenPosY = npc->worldPosY + p->worldPosY + (GAME_HEIGHT/2);
    graphics.LoadBitMapToScreen(npc->sprite[npc->direction][npc->animation_step],npc->ScreenPosX,npc->ScreenPosY,npc->OffsetY,npc->OffsetX);


}


VOID InteractionStart(PLAYER Player, NPC NPC){
    
    if(cooldown_frames != 0){
        return;
    }

    CurrentDialogue.setNumberOfPages(NPC.Dialogue.getNumberOfPages());
    for (int32_t i = 0; i < NPC.Dialogue.getNumberOfPages();i++){
        for (int32_t j = 0; j < 4; j++){
            CurrentDialogue.setDialogue(i,j,NPC.Dialogue.getDialogue(i,j));
        }
        
    }
    
    CurrentPortrait = NPC.Portrait;
    Interaction = TRUE;    
}


VOID InteractionEnd(){
    //CurrentDialogue = {0};
    CurrentPortrait = {0};
    Interaction = FALSE;  
    ShowOptBox = 0;  
}


DWORD setDialogFromTextFile(Dialogue* D, LPCSTR FileName){
    std::ifstream file(FileName);
    std::string str;

    std::getline(file, str);
    D->setNumberOfPages(stoi(str));
    for (int i = 0; i < D->getNumberOfPages(); i++){
        for (int j = 0; j < 4; j++){
            if(std::getline(file, str)){
                if(str != "//"){
                D->setDialogue(i,j,str);
                }
                else{
                    D->setDialogue(i,j,"");
                }
            }
        }
    }

}

