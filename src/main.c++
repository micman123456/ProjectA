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


// TODO LIST //

// Fix overworld collisions // -- FIXED???
// Fix sprite loading function to be more consistant // -- FIXED
// Fix sprite animations // -- FIXED
// Add a static collision object // -- DONE
// Fix the damn camera aka make it consistant with different game sizes// Camera created WIP 
// Fix rendering based on game size //  NO LOCKED IN GAMESIZE TY
// Add a non static collision objects // DONE
// Add world borders // DONE
// Overall just finish the overworld first //
// Address random crashing // 
// Add assertions to avoid crashing //
// Fix dung generation so that's it's not unwinable // 
// Fix dialogue system // 
// Clean up player controls // 




// -------------------- | GLOBALS | --------------------//



// Game Loop Stuff //
BOOL gGameIsRunning;
HWND gGameWindow;


// GAME STATES //
GAMESTATE gamestate = GAME_OPENING;
GAMESTATE  NextGameState = GAME_OPENING;
GAMESTATE Prvgamestate = GAME_OPENING;


// Graphic stuff //
GAMEBITMAP DrawingSurface;
GAMEBITMAP Font;
GAMEBITMAP TextBox,OptionBox,PortraitBox;
int8_t gridView = 0;


// Player related // 
PLAYER Player;
NPC npc;
int32_t starting_pos;


// Rendering //
PREFORMENCE_DATA gPreformance_Data; 
int32_t gMonitorWidth;
int32_t gMonitorHeight;
GAMEBITMAP Background;
Graphics graphics(&DrawingSurface, &Player);
int32_t LoadingFrameCount = 0;
LPSTR LoadingText = "Loading";


// NPC STUFF //
std::vector<NPC> Loaded_NPCs(2);
int32_t CurrentNPCIndex;

// Tile map stuff //
GAMEBITMAP Room_Sprite;
GAMEBITMAP CollisionsMap;
GAMEBITMAP Tile_Sprite_Sheet;
TILE Background_Tiles[NUMB_TILES]; 
TILE Tile_Type_Array[NUMB_TILE_TYPES];


// Dialogue // 
Dialogue MainMenuDialogue(1);
Dialogue DungeonDialogue(1);
Dialogue TestingDialogue(3);
Dialogue CurrentDialogue;
GAMEBITMAP PortTest; 
int32_t PageIndex = 0;
int_fast8_t ShowTextBox = 1;
int_fast8_t ShowOptBox = 0;
int_fast8_t ShowPortBox = 1;
int_fast8_t YesNoOptions = 0; // Defaut Option selection set to Yes // 
BOOL Interaction = FALSE;
GAMEBITMAP CurrentPortrait;
int32_t cooldown_frames =0;


// Overworld / Dungeons stuff //
Dungeon Dungeons[3];
Overworld Overworlds[4];
int32_t CurrentOverworld = 0;
int32_t OverworldFramePointer =0;
int32_t CurrentDungeon = 0;
int32_t stairs;
int_fast8_t flicker = 0;

// Misc //
Menu MainMenu;
int32_t Mode = 0; // 0 = overworld | 1 = dung //


PIXEL BlackPixel = {0};
PIXEL TestingZoneColor = {0};

CAMERA Camera;



// -------------------- | END OF GLOBALS | --------------------//

















// ------------------ | MAIN WINDOWS FUNCTION | ------------------ // 

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

    Overworld Pokemon_Square = Overworld(POKEMON_SQUARE,10,8,8,10);
    Overworld Makuhita_Dojo = Overworld(MAKUHITA_DOJO,BORDER_LEFT,BORDER_RIGHT,BORDER_UP,BORDER_DOWN);
    Overworld Pelipper_Post_Office = Overworld(PELIPPER_POST_OFFICE,-5,10,5,5);       
    Overworlds[POKEMON_SQUARE] = Pokemon_Square;
    Overworlds[MAKUHITA_DOJO] = Makuhita_Dojo;
    Overworlds[PELIPPER_POST_OFFICE] = Pelipper_Post_Office;
    LoadOverWorldIntoMemory(POKEMON_SQUARE,"assets\\Overworld\\PokemonSquare\\PS_Col.bmp","assets\\Overworld\\PokemonSquare\\");
    LoadOverWorldIntoMemory(MAKUHITA_DOJO,"assets\\Overworld\\Makuhita_Dojo\\PSFP.bmp","assets\\Overworld\\Makuhita_Dojo\\");
    LoadOverWorldIntoMemory(PELIPPER_POST_OFFICE,"assets\\Overworld\\PostOffice\\PostOfficeCol.bmp","assets\\Overworld\\PostOffice\\");

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
        
    } 
    
    else 
    {
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

     

    while(gGameIsRunning)
    {

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
        
        
        while(gPreformance_Data.ElapsedMicroseconds.QuadPart < (DESIRED_MIRCOSECONDS))
        
        {  
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
// ------------------ | END OF MAIN WINDOWS FUNCTION | ------------------ // 





















// ------------------ | WINDOWS CALLBACK FUNCTION | ------------------ //

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

// ------------------ | END OF WINDOWS CALLBACK FUNCTION | ------------------ //
















// ------------------ | MAIN WINDOW | ------------------ // 

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

// ------------------ | END OF MAIN WINDOW | ------------------ // 

















// --------------------- | PLAYER CONTROLLER | ------------------ //


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

    int16_t G_KeyDown = GetAsyncKeyState('G');

   
    
    static int16_t d_KeyWasDown;
    static int16_t a_KeyWasDown;
    static int16_t s_KeyWasDown;
    static int16_t w_KeyWasDown;
    static int16_t c_KeyWasDown;
    static int16_t t_KeyWasDown;
    static int16_t r_KeyWasDown;
    static int16_t l_KeyWasDown;
    static int16_t enter_KeyWasDown;
    static int16_t g_KeyWasDown;

    switch (gamestate)
    {
    case GAME_LOADING_SCREEN:
        /* code */
        break;
    case GAME_TITLE_SCREEN:

        if(EnterKeyDown && !enter_KeyWasDown )
        {
            HandGamestateChange(gamestate,GAME_MAIN_MENU,Mode);
        }
        if(A_KeyDown && !a_KeyWasDown )
        {
            HandGamestateChange(gamestate,GAME_TEXT_BOX_TESTING,Mode);
        }
        
        break;
     case GAME_MAIN_MENU:
     if(A_KeyDown && !a_KeyWasDown)
            {
                MainMenu.DecrementSelectedItem();
            }
    else if(D_KeyDown && !d_KeyWasDown)
            {
                MainMenu.IncrementSelectedItem();
            }
    else if(EnterKeyDown && !enter_KeyWasDown )
            {
            HandGamestateChange(gamestate,GAME_LOADING_SCREEN,Mode);
            }
     
     break;

    case 10:
    

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
    
    if (Player.movementRemaining == 0)
    {
        if(Player.StandingTile.type == FLOOR2)
        {
            HandleOverworldChange(CurrentOverworld,0);
        }
        

        if (Player.idleFrameCount < 30)
        {
            Player.animation_step = 0;
            Player.idleFrameCount++;
        }
        
        else if (Player.idleFrameCount >= 30 && Player.idleFrameCount < 60)
        {
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

     else if(G_KeyDown && !g_KeyWasDown)
     {
        ToggleGrid();
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

    case GAME_TEXT_BOX_TESTING:
    case GAME_OVERWORLD:

        if (C_KeyDown && !c_KeyWasDown)
        {
            RelocatePlayer(&Player,0,0,0);
            LockCameraToPlayer(&Camera,&Player);
        }

        // Camera Lock //
        if(L_KeyDown && !l_KeyWasDown)
        {
            Loaded_NPCs[0].worldPosX += TILE_SIZE;
            Loaded_NPCs[0].StandingTile_Index -= 1;
            Loaded_NPCs[0].StandingTile = Background_Tiles[Loaded_NPCs[0].StandingTile_Index];
            
        }
        if(G_KeyDown && !g_KeyWasDown)
        {
        ToggleGrid();
        }

         if (Player.movementRemaining == 0)
         {
            UpdateIdleAnimation(&Player, 5, 6, 100);

            if(W_KeyDown && A_KeyDown)
            {
                Player.direction = DIR_UP_LEFT;
                int32_t nextTile = GetNextPlayerTile2D(&Player,DIR_UP_LEFT);
                switch(nextTile)
                {
                    case FLOOR1:
                    if(!Ctr_KeyDown)
                    {
                        Player.animation_step = 1;
                        Player.movementRemaining = 24;
                        Player.idleFrameCount = 0;
                        Player.StandingTile_Index -= (NUMB_TILES_PER_ROW - 1);
                        Player.StandingTile = Background_Tiles[Player.StandingTile_Index];
                    }
                    break;

                    case TRANSITION_TILE_N:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_N);
                    break;
                    case TRANSITION_TILE_S:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_S);
                    break;
                    case TRANSITION_TILE_E:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_E);
                    break;
                    case TRANSITION_TILE_W:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_W);
                    break;

                    default:
                    break;

                }
            }
            
            else if(W_KeyDown && D_KeyDown)
            {
                Player.direction = DIR_UP_RIGHT;
                int32_t nextTile = GetNextPlayerTile2D(&Player,DIR_UP_RIGHT);
                 switch(nextTile)
                {
                    case FLOOR1:
                    if(!Ctr_KeyDown)
                    {
                        Player.animation_step = 1;
                        Player.movementRemaining = 24;
                        Player.idleFrameCount = 0;
                        Player.StandingTile_Index -= (NUMB_TILES_PER_ROW + 1);
                        Player.StandingTile = Background_Tiles[Player.StandingTile_Index];
                    }
                    break;

                    case TRANSITION_TILE_N:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_N);
                    break;
                    case TRANSITION_TILE_S:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_S);
                    break;
                    case TRANSITION_TILE_E:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_E);
                    break;
                    case TRANSITION_TILE_W:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_W);
                    break;

                    default:
                    break;

                }
            }
            else if(S_KeyDown && A_KeyDown)
            {
                Player.direction = DIR_DOWN_LEFT;
                int32_t nextTile = GetNextPlayerTile2D(&Player,DIR_DOWN_LEFT);
                switch(nextTile)
                {
                    case FLOOR1:
                    if(!Ctr_KeyDown)
                    {
                        Player.animation_step = 1;
                        Player.movementRemaining = 24;
                        Player.idleFrameCount = 0;
                        Player.StandingTile_Index += NUMB_TILES_PER_ROW + 1;
                        Player.StandingTile = Background_Tiles[Player.StandingTile_Index];
                    }
                    break;

                    case TRANSITION_TILE_N:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_N);
                    break;
                    case TRANSITION_TILE_S:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_S);
                    break;
                    case TRANSITION_TILE_E:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_E);
                    break;
                    case TRANSITION_TILE_W:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_W);
                    break;

                    default:
                    break;

                }
            }
            else if(S_KeyDown && D_KeyDown)
            {
                Player.direction = DIR_DOWN_RIGHT;
                int32_t nextTile = GetNextPlayerTile2D(&Player,DIR_DOWN_RIGHT);
                switch(nextTile)
                {
                    case FLOOR1:
                    if(!Ctr_KeyDown)
                    {
                        Player.animation_step = 1;
                        Player.movementRemaining = 24;
                        Player.idleFrameCount = 0;
                        Player.StandingTile_Index += NUMB_TILES_PER_ROW - 1;
                        Player.StandingTile = Background_Tiles[Player.StandingTile_Index];
                    }
                    break;

                    case TRANSITION_TILE_N:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_N);
                    break;
                    case TRANSITION_TILE_S:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_S);
                    break;
                    case TRANSITION_TILE_E:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_E);
                    break;
                    case TRANSITION_TILE_W:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_W);
                    break;

                    default:
                    break;

                }
            }


                
            else if (A_KeyDown)
            {
                Player.direction = DIR_LEFT;
                int32_t nextTile = GetNextPlayerTile2D(&Player,1);
                switch(nextTile)
                {
                    case FLOOR1:
                    if(!Ctr_KeyDown)
                    {
                        Player.animation_step = 1;
                        Player.movementRemaining = 24;
                        Player.idleFrameCount = 0;
                        Player.StandingTile_Index += 1;
                        Player.StandingTile = Background_Tiles[Player.StandingTile_Index];
                    }
                    break;

                    case TRANSITION_TILE_N:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_N);
                    break;
                    case TRANSITION_TILE_S:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_S);
                    break;
                    case TRANSITION_TILE_E:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_E);
                    break;
                    case TRANSITION_TILE_W:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_W);
                    break;

                    default:
                    break;

                }
            }
            else if (D_KeyDown)
            {
                Player.direction = DIR_RIGHT;
                int32_t nextTile = GetNextPlayerTile2D(&Player,2);

                switch(nextTile)
                {
                    case FLOOR1:
                    if(!Ctr_KeyDown)
                    {
                        Player.animation_step = 1;
                        Player.movementRemaining = 24;
                        Player.idleFrameCount = 0;
                        Player.StandingTile_Index -= 1;
                        Player.StandingTile = Background_Tiles[Player.StandingTile_Index];
                    }
                    break;

                    case TRANSITION_TILE_N:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_N);
                    break;
                    case TRANSITION_TILE_S:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_S);
                    break;
                    case TRANSITION_TILE_E:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_E);
                    break;
                    case TRANSITION_TILE_W:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_W);
                    break;

                    default:
                    break;

                }
            }
            else if (W_KeyDown)
            {
                
                int32_t nextTile = GetNextPlayerTile2D(&Player,3);
                Player.direction = DIR_UP;

                switch(nextTile)
                {
                    case FLOOR1:
                    if(!Ctr_KeyDown)
                {
                    Player.animation_step = 1;
                    Player.movementRemaining = 24;
                    Player.idleFrameCount = 0;
                    Player.StandingTile_Index -= NUMB_TILES_PER_ROW;
                    Player.StandingTile = Background_Tiles[Player.StandingTile_Index];
                }
                    break;

                    case TRANSITION_TILE_N:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_N);
                    break;
                    case TRANSITION_TILE_S:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_S);
                    break;
                    case TRANSITION_TILE_E:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_E);
                    break;
                    case TRANSITION_TILE_W:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_W);
                    break;

                    default:
                    break;

                }

            }
            else if (S_KeyDown)
            {
                int32_t nextTile = GetNextPlayerTile2D(&Player,0);
                Player.direction = DIR_DOWN;

                switch(nextTile)
                {
                    case FLOOR1:
                    if(!Ctr_KeyDown)
                    {
                        Player.animation_step = 1;
                        Player.movementRemaining = 24;
                        Player.idleFrameCount = 0;
                        Player.StandingTile_Index += NUMB_TILES_PER_ROW;
                        Player.StandingTile = Background_Tiles[Player.StandingTile_Index];
                        
                    }
                    break;

                    case TRANSITION_TILE_N:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_N);
                    break;
                    case TRANSITION_TILE_S:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_S);
                    break;
                    case TRANSITION_TILE_E:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_E);
                    break;
                    case TRANSITION_TILE_W:
                    HandleOverworldChange(CurrentOverworld,TRANSITION_TILE_W);
                    break;

                    default:
                    break;

                }

            }

         }
         
         else
        {
            switch (Player.direction) {
    case DIR_DOWN:
        updatePlayerPosition2D(&Player, DIR_DOWN);
        updateCameraPosition2D(Camera.worldPosX, Camera.worldPosY, DIR_DOWN);
        break;

    case DIR_LEFT:
        updatePlayerPosition2D(&Player, DIR_LEFT);
        updateCameraPosition2D(Camera.worldPosX, Camera.worldPosY, DIR_LEFT);
        break;

    case DIR_RIGHT:
        updatePlayerPosition2D(&Player, DIR_RIGHT);
        updateCameraPosition2D(Camera.worldPosX, Camera.worldPosY, DIR_RIGHT);
        break;

    case DIR_UP:
        updatePlayerPosition2D(&Player, DIR_UP);
        updateCameraPosition2D(Camera.worldPosX, Camera.worldPosY, DIR_UP);
        break;

    case DIR_DOWN_LEFT:
        updatePlayerPosition2D(&Player, DIR_DOWN_LEFT);
        updateCameraPosition2D(Camera.worldPosX, Camera.worldPosY, DIR_DOWN_LEFT);
        break;

    case DIR_DOWN_RIGHT:
        updatePlayerPosition2D(&Player, DIR_DOWN_RIGHT);
        updateCameraPosition2D(Camera.worldPosX, Camera.worldPosY, DIR_DOWN_RIGHT);
        break;

    case DIR_UP_LEFT:
        updatePlayerPosition2D(&Player, DIR_UP_LEFT);
        updateCameraPosition2D(Camera.worldPosX, Camera.worldPosY, DIR_UP_LEFT);
        break;

    case DIR_UP_RIGHT:
        updatePlayerPosition2D(&Player, DIR_UP_RIGHT);
        updateCameraPosition2D(Camera.worldPosX, Camera.worldPosY, DIR_UP_RIGHT);
        break;

    default:
        break;
}

    UpdatePlayerSprite(&Player, 5, 30);
    }




    break;
    
    case GAME_DUNGEON:

     if(G_KeyDown && !g_KeyWasDown)
     {
        ToggleGrid();
     }    

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
    enter_KeyWasDown = EnterKeyDown;
    g_KeyWasDown = G_KeyDown;
    
    if (espKeyDown){
        SendMessageA(gGameWindow,WM_CLOSE,0,0);
    }
    if(cooldown_frames >0){
        cooldown_frames--;
    }



}

// --------------------- | END OF PLAYER CONTROLLER | ------------------ //


















// --------------------- | LOADING FUNCTIONS | --------------------- //

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

// DWORD LoadSpriteFromSpriteSheet(GAMEBITMAP SpriteSheet, GAMEBITMAP *player_spite_box, int16_t SpriteCountRow,int16_t SpriteCountCol, int16_t Row, int16_t Col){
//     DWORD Error = ERROR_SUCCESS;
//     int32_t Starting_Address = 0;
//     int32_t Offset = 0;
    
//     player_spite_box->bitMapInfo.bmiHeader.biSize = sizeof(player_spite_box->bitMapInfo.bmiHeader);
//     player_spite_box->bitMapInfo.bmiHeader.biWidth = 64;
//     player_spite_box->bitMapInfo.bmiHeader.biHeight = 64;
//     player_spite_box->bitMapInfo.bmiHeader.biSizeImage = 4096*4;
    
//     player_spite_box->bitMapInfo.bmiHeader.biBitCount = GAME_BPP;
//     player_spite_box->bitMapInfo.bmiHeader.biCompression = 0;
//     player_spite_box->bitMapInfo.bmiHeader.biPlanes = 1;
    
    
//     player_spite_box->memory = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,player_spite_box->bitMapInfo.bmiHeader.biSizeImage);
//     if (player_spite_box->memory == NULL){
//         Error = ERROR_NOT_ENOUGH_MEMORY;
//         return Error;
//     }


//     Row *= SpriteSheet.bitMapInfo.bmiHeader.biHeight/SpriteCountRow;
    
//     int32_t spriteSheetRowIndex = SpriteSheet.bitMapInfo.bmiHeader.biWidth * 4;  
//     int32_t spriteSheetRowCol = 128; 
//     int32_t spriteSheetPointer = (Row*spriteSheetRowIndex) + (spriteSheetRowCol*Col); // Width * 4 * index of height (jumps by 64)
//     int32_t memoryOffset = 0;
    
    
//     for (int i = 0; i < SpriteSheet.bitMapInfo.bmiHeader.biHeight/SpriteCountRow; i++) {
//     memcpy((char*)player_spite_box->memory + memoryOffset, (char*)SpriteSheet.memory +spriteSheetPointer , 128);
//     spriteSheetPointer += (SpriteSheet.bitMapInfo.bmiHeader.biWidth)*4;
//     memoryOffset += 64*4;
//     }
//     return Error;
// }

DWORD LoadSpriteFromSpriteSheet(
    GAMEBITMAP SpriteSheet, 
    GAMEBITMAP *player_sprite_box, 
    int16_t SpriteCountRow, 
    int16_t SpriteCountCol, 
    int16_t RowIndex, 
    int16_t ColIndex,
    int16_t offsetX, 
    int16_t offsetY

) {
    DWORD Error = ERROR_SUCCESS;
    BOOL addHeight = FALSE;

    // Validate input parameters
    if (SpriteCountRow <= 0 || SpriteCountCol <= 0 || 
        RowIndex < 0 || RowIndex >= SpriteCountRow || 
        ColIndex < 0 || ColIndex >= SpriteCountCol) {
        return ERROR_INVALID_PARAMETER;
    }

    // Calculate sprite dimensions
    int32_t spriteWidth = SpriteSheet.bitMapInfo.bmiHeader.biWidth / SpriteCountCol;
    
    int32_t spriteHeight = SpriteSheet.bitMapInfo.bmiHeader.biHeight / SpriteCountRow;

    if (spriteWidth <= 0 || spriteHeight <= 0) {
        return ERROR_INVALID_DATA;
    }

    // Configure player sprite box properties
    player_sprite_box->bitMapInfo.bmiHeader.biSize = sizeof(player_sprite_box->bitMapInfo.bmiHeader);
    player_sprite_box->bitMapInfo.bmiHeader.biWidth = spriteWidth;
    player_sprite_box->bitMapInfo.bmiHeader.biHeight = spriteHeight;
    player_sprite_box->bitMapInfo.bmiHeader.biSizeImage = spriteWidth * spriteHeight * (GAME_BPP / 8);
    player_sprite_box->bitMapInfo.bmiHeader.biBitCount = GAME_BPP;
    player_sprite_box->bitMapInfo.bmiHeader.biCompression = BI_RGB;
    player_sprite_box->bitMapInfo.bmiHeader.biPlanes = 1;

    // Allocate memory for the player sprite box
    player_sprite_box->memory = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, player_sprite_box->bitMapInfo.bmiHeader.biSizeImage);
    if (player_sprite_box->memory == NULL) {
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    // Calculate starting address in the sprite sheet
    int32_t spriteSheetRowPitch = SpriteSheet.bitMapInfo.bmiHeader.biWidth * (GAME_BPP / 8);
    int32_t spriteStartX = ColIndex * spriteWidth + offsetX;
    int32_t spriteStartY = RowIndex * spriteHeight + offsetY;
    int32_t spriteSheetPointer = spriteStartY * spriteSheetRowPitch + spriteStartX * (GAME_BPP / 8);

    // Validate that the sprite fits within the sprite sheet's memory
    if (spriteSheetPointer + (spriteHeight * spriteSheetRowPitch) > SpriteSheet.bitMapInfo.bmiHeader.biSizeImage) {
        // HeapFree(GetProcessHeap(), 0, player_sprite_box->memory);
        // return ERROR_INVALID_DATA;
        spriteHeight-=1;
        addHeight = TRUE;
    }

    // Copy the sprite row by row
    int32_t memoryOffset = 0;
    for (int32_t i = 0; i < spriteHeight; i++) {
        memcpy(
            (char*)player_sprite_box->memory + memoryOffset,
            (char*)SpriteSheet.memory + spriteSheetPointer,
            spriteWidth * (GAME_BPP / 8)
        );
        spriteSheetPointer += spriteSheetRowPitch;
        memoryOffset += spriteWidth * (GAME_BPP / 8);
    }

    if (addHeight)
    {
        spriteSheetPointer -= spriteSheetRowPitch;
        memcpy(
            (char*)player_sprite_box->memory + memoryOffset,
            (char*)SpriteSheet.memory + spriteSheetPointer,
            spriteWidth * (GAME_BPP / 8)
        );
    }

    return Error;
}



DWORD ImprovedSpriteSheetLoader(
    
    GAMEBITMAP SpriteSheet, 
    GAMEBITMAP* player_sprite_box, 
    int16_t NumbRows, 
    int16_t NumbCols, 
    int16_t RowIndex, 
    int16_t ColIndex, 
    int32_t spriteWidth, 
    int32_t spriteHeight, 
    int32_t horizontalOffset, 
    int32_t verticalOffset
) {
    DWORD Error = ERROR_SUCCESS;

    // Validate inputs //
    if (RowIndex < 0 || RowIndex >= NumbRows || ColIndex < 0 || ColIndex >= NumbCols) 
    {
        return ERROR_INVALID_PARAMETER;
    }

    // Set up the player sprite box's bitmap info //
    player_sprite_box->bitMapInfo.bmiHeader.biSize = sizeof(player_sprite_box->bitMapInfo.bmiHeader);
    player_sprite_box->bitMapInfo.bmiHeader.biWidth = spriteWidth;
    player_sprite_box->bitMapInfo.bmiHeader.biHeight = spriteHeight;
    player_sprite_box->bitMapInfo.bmiHeader.biSizeImage = spriteWidth * spriteHeight * (GAME_BPP / 8);
    player_sprite_box->bitMapInfo.bmiHeader.biBitCount = GAME_BPP;
    player_sprite_box->bitMapInfo.bmiHeader.biCompression = BI_RGB;
    player_sprite_box->bitMapInfo.bmiHeader.biPlanes = 1;

    // Allocate memory for the player sprite box //
    player_sprite_box->memory = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, player_sprite_box->bitMapInfo.bmiHeader.biSizeImage);
    if (player_sprite_box->memory == NULL) 
    {
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    // Calculate starting address in the sprite sheet //
    int32_t spriteSheetRowPitch = SpriteSheet.bitMapInfo.bmiHeader.biWidth * (GAME_BPP / 8); 
    int32_t spriteStartX = ColIndex * spriteWidth + horizontalOffset;
    int32_t spriteStartY = RowIndex * spriteHeight + verticalOffset;

    int32_t spriteSheetPointer = (spriteStartY * spriteSheetRowPitch) + (spriteStartX * (GAME_BPP / 8));
    int32_t memoryOffset = 0;

    // Copy the sprite row by row //
    for (int32_t i = 0; i < spriteHeight; i++) {
        memcpy(
            (char*)player_sprite_box->memory + memoryOffset,                  
            (char*)SpriteSheet.memory + spriteSheetPointer,                  
            spriteWidth * (GAME_BPP / 8)                                    
        );
        spriteSheetPointer += spriteSheetRowPitch;                           
        memoryOffset += spriteWidth * (GAME_BPP / 8);                        
    }

    return Error;
}


DWORD LoadDungeonIntoMemory(Dungeon Dungeon)
{
    DWORD Error = ERROR_SUCCESS;
    try
    {

    
    if(Load32BppBitmapFromFile(Dungeon.getPath(),&Tile_Sprite_Sheet) != ERROR_SUCCESS){
        MessageBoxA(NULL, "Unable to load font sheet into memory", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }
    printf("Tiles loaded from D path\n");

    InitTiles(Tile_Sprite_Sheet);
    printf("Init tiles done\n");
    ResetTiles(Background_Tiles,Tile_Type_Array);
    stairs = ProceduralGenerator(1000,10,18,Background_Tiles,Tile_Type_Array,10);
    printf("Stairs done e\n");
    BuiltTileMap(Background_Tiles,&Background);
    printf("Tilemap built\n");

    
    graphics.SetBackgroundBitMap(&Background);
    printf("graphics set\n");


    // if (ResetPlayer() != ERROR_SUCCESS){
    //     MessageBoxA(NULL, "Error ititalizing player sprite", "Error", MB_ICONEXCLAMATION | MB_OK);
    //     Error = GetLastError();
    //     return Error;
    // }
    ResetPlayer();
    printf("Player initialized\n");

    }
    catch(const std::exception& e) 
    {
        std::cerr << "Exception caught: " << e.what() << std::endl;   
    }



    return Error;
}



DWORD LoadOverWorldIntoMemory(int32_t index, LPCSTR CollisionsMapPath, LPCSTR DirPath)
{
    DWORD Error = ERROR_SUCCESS;
    
    GAMEBITMAP Temp;
    for(int i = 1; i < 7; i++)
    {
        std::string indexStr = std::to_string(i);
        std::string fullPath = std::string(DirPath) + indexStr + ".bmp";
        LPCSTR Path = fullPath.c_str();
      

        if(Load32BppBitmapFromFile(Path,&Temp) != ERROR_SUCCESS)
        {
            MessageBoxA(NULL, "Unable to load overworld", "Error", MB_ICONEXCLAMATION | MB_OK);
            Error = GetLastError();
        return Error;
        }

        Overworlds[index].SetSprite(Temp,(i-1));   

    }

    if(Load32BppBitmapFromFile(CollisionsMapPath,&Temp) != ERROR_SUCCESS)
    {
        MessageBoxA(NULL, "Unable to load overworld CollisionsMap", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }
    Overworlds[index].SetCollisionsMap(Temp);

    return Error;
}

DWORD SetCurrentOverWorld(Overworld Overworld)
{
    DWORD Error = ERROR_SUCCESS;
    
    CollisionsMap = Overworld.GetCollisionsMap();
    SetTilesOverworld(Background_Tiles,&CollisionsMap);
    Background = Overworld.GetSprite(OverworldFramePointer);
    graphics.SetBackgroundBitMap(&Background);
    
    if (InitPlayerOverWorld() != ERROR_SUCCESS){
        MessageBoxA(NULL, "Error ititalizing player sprite to Overworld", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }

    

    if (InitCamera() != ERROR_SUCCESS){
        MessageBoxA(NULL, "Error ititalizing CAMERA", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }
    
    if (InitPlayerOverWorld() != ERROR_SUCCESS){
        MessageBoxA(NULL, "Error ititalizing player sprite to Overworld", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }

    
    if (ImprovedInitNPC(&Loaded_NPCs[0],"assets\\Player_Sprites\\Walk-Anim-ch.bmp","assets\\Player_Sprites\\port-ch.bmp","assets\\dialogue\\chatot-dialog.txt",TILE_SIZE*2,0,-12,-8) != ERROR_SUCCESS){
        MessageBoxA(NULL, "Error ititalizing player sprite to Overworld", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }
    
    return Error;

}

DWORD SetTestingZone()
{
    DWORD Error = ERROR_SUCCESS;
    TestingZoneColor.red = 50;
    TestingZoneColor.green = 50;


    if(Load32BppBitmapFromFile("assets\\tiles\\Tiles-Amp-Plains.bmp",&Tile_Sprite_Sheet) != ERROR_SUCCESS)
    {
        MessageBoxA(NULL, "Unable to load font sheet into memory", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }

    InitTiles(Tile_Sprite_Sheet);
    
    BuiltTileMap(Background_Tiles,&Background);
    
    SetTilesTestingZone(Background_Tiles);

    // if(Load32BppBitmapFromFile("assets\\TestingZone\\tz.bmp",&Background) != ERROR_SUCCESS){
    //     MessageBoxA(NULL, "Unable to load tz background", "Error", MB_ICONEXCLAMATION | MB_OK);
    //     Error = GetLastError();
    //     return Error;
    // }
    graphics.SetBackgroundBitMap(&Background);
    
    if (InitCamera() != ERROR_SUCCESS)
    {
        MessageBoxA(NULL, "Error ititalizing CAMERA", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }
    
    if (InitPlayerOverWorld() != ERROR_SUCCESS)
    {
        MessageBoxA(NULL, "Error ititalizing player sprite to Overworld", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }

    
    if (ImprovedInitNPC(&Loaded_NPCs[0],"assets\\Player_Sprites\\Walk-Anim-ch.bmp","assets\\Player_Sprites\\port-ch.bmp","assets\\dialogue\\chatot-dialog.txt",48,0,-12,-8) != ERROR_SUCCESS)
    {
        MessageBoxA(NULL, "Error ititalizing player sprite to Overworld", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }

    
    return Error;

}

// --------------------- | END OF LOADING FUNCTIONS | --------------------- //



















// --------------------- | DUNG FUNCTIONS | --------------------- //

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



VOID teleportPlayer(){
    InitPlayer();

}

VOID TeleportToStairs(PLAYER Player){
    
}

// --------------------- | END OF DUNG FUNCTIONS | --------------------- //



















// ---------------- | MAIN RENDERING FUNCTION | ---------------- //

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
        if(gridView == 1)
        {
            graphics.LoadGrid();
        }
        break;
    case GAME_DUNGEON:
        RenderDungeonScene(Background,&Player);
        if(gridView == 1)
        {
            graphics.LoadGrid();
        }
        
        break;

    case GAME_LOADING_SCREEN:
        RenderLoadingScene(LoadingText);
        break;
    case GAME_TEXT_BOX_TESTING:
        RenderTestZone(&Player);
        if(gridView == 1)
        {
            graphics.LoadGrid();
        }
    
    default:
        
        break;
    }
    
    HDC deviceContext = GetDC(gGameWindow);
    StretchDIBits(deviceContext,0,0,gMonitorWidth,gMonitorHeight,0,0,GAME_WIDTH,GAME_HEIGHT,DrawingSurface.memory,&DrawingSurface.bitMapInfo,DIB_RGB_COLORS,SRCCOPY);
    
    char fpsBuffer[64] = {0};
    sprintf(fpsBuffer, "Cooked FPS: %.01f Raw FPS: %.01f Screen Position: %d:%d, %d %d",gPreformance_Data.CookFPS,gPreformance_Data.RawFPS,Player.worldPosX, Player.worldPosY, Player.StandingTile_Index,GetNextPlayerTile(&Player,Player.direction));
    //sprintf(fpsBuffer, "Cooked FPS: %.01f Raw FPS: %.01f",gPreformance_Data.CookFPS,gPreformance_Data.RawFPS);
    SetTextColor(deviceContext, RGB(255, 255, 255));  
    SetBkMode(deviceContext, TRANSPARENT );
    TextOutA( deviceContext, 0, 0, fpsBuffer, strlen( fpsBuffer ) );
     
    
    ReleaseDC(gGameWindow,deviceContext);

}

// ---------------- | END OF MAIN RENDERING FUNCTION | ---------------- //














// ----------------------- | Initalizing Functions | ----------------------- //

VOID initBitMap(void)

{
          
     DrawingSurface.bitMapInfo.bmiHeader.biSize = sizeof(DrawingSurface.bitMapInfo.bmiHeader);
     DrawingSurface.bitMapInfo.bmiHeader.biWidth = GAME_WIDTH;
     DrawingSurface.bitMapInfo.bmiHeader.biHeight = GAME_HEIGHT;
     DrawingSurface.bitMapInfo.bmiHeader.biBitCount = GAME_BPP;
     DrawingSurface.bitMapInfo.bmiHeader.biCompression = 0;
     DrawingSurface.bitMapInfo.bmiHeader.biPlanes = 1;
     DrawingSurface.memory = VirtualAlloc(NULL,GAME_DRAWING_AREA_MEMORY_SIZE,MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

}

DWORD ResetPlayer(VOID){
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

    return Error;

}

DWORD InitCamera(VOID)
{
    DWORD Error = ERROR_SUCCESS;
    Camera.height = GAME_HEIGHT;
    Camera.width = GAME_WIDTH;
    Camera.zoom = 1;
    Camera.worldPosX = 0;
    Camera.worldPosY = 0;
    Camera.Mode = 0;

    if (Error != ERROR_SUCCESS){
        Error = GetLastError();
    }
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
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[0][0],8,5,7,0,0,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[0][1],8,5,7,1,0,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[0][2],8,5,7,2,0,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[0][3],8,5,7,4,0,0);

    /* Left */
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[1][0],8,5,1,0,0,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[1][1],8,5,1,1,0,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[1][2],8,5,1,2,0,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[1][3],8,5,1,4,0,0);

    /* Right */
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[2][0],8,5,5,0,0,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[2][1],8,5,5,1,0,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[2][2],8,5,5,2,0,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[2][3],8,5,5,4,0,0);

    /* Up */
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[3][0],8,5,3,0,0,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[3][1],8,5,3,1,0,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[3][2],8,5,3,2,0,0);
    Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[3][3],8,5,3,4,0,0);
    
    

    if (Error != ERROR_SUCCESS){
        Error = GetLastError();
    }
    return Error;
}


VOID RelocatePlayer(PLAYER* Player, int32_t worldPosX, int32_t worldPosY, int32_t direction)
{
 
 
 int32_t tileIndex = STARTING_TILE-(worldPosX/TILE_SIZE)- NUMB_TILES_PER_ROW*(worldPosY/TILE_SIZE);
 Player->worldPosX = worldPosX;
 Player->worldPosY = worldPosY;
 Player->direction = direction;
 Player->StandingTile = Background_Tiles[tileIndex];
 Player->StandingTile_Index = tileIndex;

}



DWORD InitPlayerOverWorld() {
    DWORD Error = ERROR_SUCCESS;

    // Initialize player properties
    Player.worldPosX = 0;
    Player.worldPosY = 0;
    Player.ScreenPosX = GAME_WIDTH / 2;
    Player.ScreenPosY = GAME_HEIGHT / 2;
    Player.movementRemaining = 0;
    Player.animation_step = 1;
    Player.direction = DIR_DOWN_LEFT;
    Player.idleFrameCount = 0;
    Player.StandingTile = Background_Tiles[STARTING_TILE];
    Player.StandingTile_Index = STARTING_TILE;
    Player.noClip = 0;

    // Load player sprite sheets
    Error = Load32BppBitmapFromFile("assets\\Player_Sprites\\Walk-Anim.bmp", &Player.sprite_sheet[0]);
    if (Error != ERROR_SUCCESS) return Error;

    Error = Load32BppBitmapFromFile("assets\\Player_Sprites\\Idle-Anim.bmp", &Player.sprite_sheet[1]);
    if (Error != ERROR_SUCCESS) return Error;

    // Define sprite sheet properties
    const int16_t numRows = 8; 
    const int16_t numCols = 5; 

    // Map each direction to its corresponding row in the sprite sheet
    const int directionRows[NUM_DIRECTIONS] = {
        7, // DIR_DOWN
        1, // DIR_LEFT
        5, // DIR_RIGHT
        3, // DIR_UP
        0, // DIR_DOWN_LEFT
        6, // DIR_DOWN_RIGHT
        2, // DIR_UP_LEFT
        4  // DIR_UP_RIGHT
    };

    // Load walking and idle animations for each direction
    for (int direction = 0; direction < NUM_DIRECTIONS; ++direction) {
        int row = directionRows[direction];

        // Load walking frames
        for (int frame = 0; frame < NUM_WALK_FRAMES; ++frame) {
            Error = LoadSpriteFromSpriteSheet(
                Player.sprite_sheet[0],
                &Player.sprite[direction][frame],
                numRows, numCols, row, frame, 0, 0);
            if (Error != ERROR_SUCCESS) return Error;
        }

        // Load idle frames
        for (int frame = 0; frame < NUM_IDLE_FRAMES; ++frame) {
            Error = LoadSpriteFromSpriteSheet(
                Player.sprite_sheet[1],
                &Player.sprite[direction][NUM_WALK_FRAMES + frame],
                numRows, 2, row, frame, 0, 8); // Idle frames have an offset
            if (Error != ERROR_SUCCESS) return Error;
        }
    }

    return Error;
}

// DWORD InitPlayerOverWorld(VOID) {
//     DWORD Error = ERROR_SUCCESS;

//     // Initialize player properties
//     Player.worldPosX = 0;
//     Player.worldPosY = 0;
//     Player.ScreenPosX = GAME_WIDTH / 2;
//     Player.ScreenPosY = GAME_HEIGHT / 2;
//     Player.movementRemaining = 0;
//     Player.animation_step = 1;
//     Player.direction = DIR_DOWN_LEFT;
//     Player.idleFrameCount = 0;
//     Player.StandingTile = Background_Tiles[STARTING_TILE];
//     Player.StandingTile_Index = STARTING_TILE;
//     Player.noClip = 0;

//     // Load player sprite sheets
//     Error = Load32BppBitmapFromFile("assets\\Player_Sprites\\Walk-Anim.bmp", &Player.sprite_sheet[0]);
//     if (Error != ERROR_SUCCESS) return Error;

//     Error = Load32BppBitmapFromFile("assets\\Player_Sprites\\Idle-Anim.bmp", &Player.sprite_sheet[1]);
//     if (Error != ERROR_SUCCESS) return Error;

//     // Define sprite dimensions and offsets
//     const int32_t spriteWidth = 32;  // Example width of each sprite
//     const int32_t spriteHeight = 32; // Example height of each sprite
//     const int32_t horizontalOffset = 0; // Offset within each sprite
//     const int32_t verticalOffset = 0;

//     // Load walking animations
//     const int16_t numRows = 8; // Total rows in the sprite sheet
//     const int16_t numCols = 5; // Total columns in the sprite sheet

//   /* Walking Animations */

//     /* Down */
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_DOWN][0],8,5,7,0,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_DOWN][1],8,5,7,1,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_DOWN][2],8,5,7,2,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_DOWN][3],8,5,7,3,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_DOWN][4],8,5,7,4,0,0);

//     /* Idle Down */
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[1], &Player.sprite[DIR_DOWN][5],8,2,7,0,0,8);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[1], &Player.sprite[DIR_DOWN][6],8,2,7,1,0,8);

//     /* Left */
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_LEFT][0],8,5,1,0,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_LEFT][1],8,5,1,1,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_LEFT][2],8,5,1,2,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_LEFT][3],8,5,1,3,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_LEFT][4],8,5,1,4,0,0);

//     /* Idle Left */
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[1], &Player.sprite[DIR_LEFT][5],8,2,1,0,0,8);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[1], &Player.sprite[DIR_LEFT][6],8,2,1,1,0,8);

//     /* Right */
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_RIGHT][0],8,5,5,0,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_RIGHT][1],8,5,5,1,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_RIGHT][2],8,5,5,2,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_RIGHT][3],8,5,5,3,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_RIGHT][4],8,5,5,4,0,0);

//     /* Idle right */
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[1], &Player.sprite[DIR_RIGHT][5],8,2,5,0,0,8);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[1], &Player.sprite[DIR_RIGHT][6],8,2,5,1,0,8);

//     /* Up */
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_UP][0],8,5,3,0,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_UP][1],8,5,3,1,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_UP][2],8,5,3,2,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_UP][3],8,5,3,3,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_UP][4],8,5,3,4,0,0);

//     /* Idle up */
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[1], &Player.sprite[DIR_UP][5],8,2,3,0,0,8);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[1], &Player.sprite[DIR_UP][6],8,2,3,1,0,8);

    
//     /* Down Left */
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_DOWN_LEFT][0],8,5,0,0,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_DOWN_LEFT][1],8,5,0,1,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_DOWN_LEFT][2],8,5,0,2,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_DOWN_LEFT][3],8,5,0,3,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_DOWN_LEFT][4],8,5,0,4,0,0);

    
//     /* Idle Down Left*/
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[1], &Player.sprite[DIR_DOWN_LEFT][5],8,2,0,0,0,8);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[1], &Player.sprite[DIR_DOWN_LEFT][6],8,2,0,1,0,8);

//     /* Down right */
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_DOWN_LEFT][0],8,5,0,0,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_DOWN_LEFT][1],8,5,0,1,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_DOWN_LEFT][2],8,5,0,2,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_DOWN_LEFT][3],8,5,0,3,0,0);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[0], &Player.sprite[DIR_DOWN_LEFT][4],8,5,0,4,0,0);

    
//     /* Idle Down right*/
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[1], &Player.sprite[DIR_DOWN_LEFT][5],8,2,0,0,0,8);
//     Error = LoadSpriteFromSpriteSheet(Player.sprite_sheet[1], &Player.sprite[DIR_DOWN_LEFT][6],8,2,0,1,0,8);

    
    

//     if (Error != ERROR_SUCCESS){
//         Error = GetLastError();
//     }
//     return Error;
// }


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
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[0][0],8,4,7,0,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[0][1],8,4,7,1,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[0][2],8,4,7,2,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[0][3],8,4,7,3,0,0);

    /* Left */
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[1][0],8,0,1,0,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[1][1],8,0,1,1,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[1][2],8,0,1,2,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[1][3],8,0,1,3,0,0);

    /* Right */
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[2][0],8,0,5,0,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[2][1],8,0,5,1,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[2][2],8,0,5,2,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[2][3],8,0,5,3,0,0);

    /* Up */
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[3][0],8,0,3,0,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[3][1],8,0,3,1,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[3][2],8,0,3,2,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[3][3],8,0,3,3,0,0);


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




DWORD ImprovedInitNPC(NPC* npc, LPCSTR SpriteFilePath,LPCSTR PortraitFilePath, LPCSTR DialogueFilePath, int32_t x, int32_t y, int32_t offsetX,int32_t offsetY)
{
    DWORD Error = ERROR_SUCCESS;
    
    npc->worldPosX=x;
    npc->worldPosY=y;
    

    
    npc->OffsetX = offsetX;
    npc->OffsetY = offsetY;

    npc->StandingTile_Index = STARTING_TILE-(npc->worldPosX/TILE_SIZE)- NUMB_TILES_PER_ROW*(npc->worldPosY/TILE_SIZE);
    
    npc->StandingTile = Background_Tiles[npc->StandingTile_Index];

    npc->movementRemaining = 0;
    npc->animation_step = 1;
    npc->direction = DIR_DOWN;
    npc->idleFrameCount = 0;
    npc->visbility = 1;
    
    /* Load sprites */
    
    
    /* Load sprites sheets */
    Error = Load32BppBitmapFromFile(SpriteFilePath,&npc->sprite_sheet[0]);
    Error = Load32BppBitmapFromFile(PortraitFilePath,&npc->Portrait);
    

    /* Walking Animations */

    /* Down */
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[0][0],8,6,7,0,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[0][1],8,6,7,1,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[0][2],8,6,7,2,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[0][3],8,6,7,3,0,0);

    /* Left */
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[1][0],8,6,1,0,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[1][1],8,6,1,1,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[1][2],8,6,1,2,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[1][3],8,6,1,3,0,0);

    /* Right */
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[2][0],8,6,5,0,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[2][1],8,6,5,1,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[2][2],8,6,5,2,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[2][3],8,6,5,3,0,0);

    /* Up */
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[3][0],8,6,3,0,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[3][1],8,6,3,1,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[3][2],8,6,3,2,0,0);
    Error = LoadSpriteFromSpriteSheet(npc->sprite_sheet[0], &npc->sprite[3][3],8,6,3,3,0,0);


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
    //Background_Tiles[i] = Tile_Type_Array[WALL_FULL];
    Background_Tiles[i] = Tile_Type_Array[FLOOR1];
   }
   
   Background_Tiles[STARTING_TILE+5] = Tile_Type_Array[WALL_FULL];

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


// ----------------------- | END OF Initalizing Functions |----------------------- //













// ----------------------- | Rendering Functions | ----------------------- // 


VOID RenderDungeonScene(GAMEBITMAP BackgroundBitMap, PLAYER* Player){
    
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

    graphics.LoadColorToScreen(BlackPixel);
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
    
    graphics.LoadBackgroundFromCamera(Camera);
    CameraWorldPosBasedRender(&Loaded_NPCs[0],&Camera);  
    RenderPlayerCharacter(&Camera,Player);
}

VOID RenderMainMenuScene(){

    graphics.LoadColorToScreen(BlackPixel);
    graphics.LoadBitFontToScreen(Font,"Select a Dungeon", GAME_WIDTH/2, GAME_HEIGHT/2,1);
    DisplayMainMenuToScreen(MainMenu,45,180);
    // graphics.LoadBackScreen();
    // graphics.LoadOverWorldToDrawingSurface();

}

VOID RenderLoadingScene(char* str){
    graphics.LoadColorToScreen(BlackPixel);
    graphics.LoadBitFontToScreen(Font,str, GAME_WIDTH/2, GAME_HEIGHT/2,1);
}


VOID RenderTestZone(PLAYER* Player){
    graphics.LoadBackgroundFromCamera(Camera);
    CameraWorldPosBasedRender(&Loaded_NPCs[0],&Camera);  
    RenderPlayerCharacter(&Camera,Player);
     
}

VOID WorldPosBasedRender(NPC* npc, PLAYER* p){
    // calulate npc screen pos based on relative player tile // 
    npc->ScreenPosX = npc->worldPosX - p->worldPosX + (GAME_WIDTH/2);
    npc->ScreenPosY = npc->worldPosY + p->worldPosY + (GAME_HEIGHT/2);
    graphics.LoadBitMapToScreen(npc->sprite[npc->direction][npc->animation_step],npc->ScreenPosX,npc->ScreenPosY,npc->OffsetY,npc->OffsetX);
}

VOID CameraWorldPosBasedRender(NPC* npc, CAMERA* Camera){
    // calulate npc screen pos based on relative camera tile // 
    npc->ScreenPosX = npc->worldPosX - Camera->worldPosX + (GAME_WIDTH / 2);
    npc->ScreenPosY = -(npc->worldPosY - Camera->worldPosY) + (GAME_HEIGHT / 2); 

    graphics.LoadBitMapToScreen(npc->sprite[npc->direction][npc->animation_step],npc->ScreenPosX,npc->ScreenPosY,npc->OffsetY,npc->OffsetX);
}




VOID RenderPlayerCharacter(CAMERA* Cam, PLAYER* p) {
    p->ScreenPosX = p->worldPosX - Cam->worldPosX + (GAME_WIDTH / 2);
    p->ScreenPosY = -(p->worldPosY - Cam->worldPosY) + (GAME_HEIGHT / 2); 
    
    graphics.LoadBitMapToScreen(p->sprite[p->direction][p->animation_step], p->ScreenPosX, p->ScreenPosY, -12, -12);
}


//  ----------------------- | END OF RENDERING |----------------------- // 













//  ----------------------- | GAME STATE  | ----------------------- //



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
            SetTestingZone();
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
        printf("Load text done\n");

        LoadDungeonIntoMemory(Dungeons[0]); 

        printf("Dung loaded\n");

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

VOID HandleOverworldChange(int32_t CurrentOW, int32_t tile){
    
    switch (CurrentOW)
    {
    case POKEMON_SQUARE:

        if (tile == TRANSITION_TILE_S)
        {
        CurrentOverworld = MAKUHITA_DOJO;
        HandGamestateChange(GAME_OVERWORLD,GAME_LOADING_SCREEN,Mode);
        SetCurrentOverWorld(Overworlds[CurrentOverworld]);
        return;
        }
        if (tile == TRANSITION_TILE_N)
        {
        CurrentOverworld = MAKUHITA_DOJO;
        HandGamestateChange(GAME_OVERWORLD,GAME_LOADING_SCREEN,Mode);
        SetCurrentOverWorld(Overworlds[CurrentOverworld]);
        return;
        }
        if (tile == TRANSITION_TILE_E)
        {
        CurrentOverworld = PELIPPER_POST_OFFICE;
        HandGamestateChange(GAME_OVERWORLD,GAME_LOADING_SCREEN,Mode);
        SetCurrentOverWorld(Overworlds[CurrentOverworld]);
        RelocatePlayer(&Player,0,0,DIR_RIGHT);
        LockCameraToPlayer(&Camera,&Player);
        
        return;
        }
        if (tile == TRANSITION_TILE_W)
        {
        CurrentOverworld = MAKUHITA_DOJO;
        HandGamestateChange(GAME_OVERWORLD,GAME_LOADING_SCREEN,Mode);
        SetCurrentOverWorld(Overworlds[CurrentOverworld]);
        return;
        }
       
        

        break;
    case MAKUHITA_DOJO:
        CurrentOverworld = POKEMON_SQUARE;
        HandGamestateChange(GAME_OVERWORLD,GAME_LOADING_SCREEN,Mode);
        SetCurrentOverWorld(Overworlds[CurrentOverworld]);
        RelocatePlayer(&Player,0,-9*TILE_SIZE,DIR_UP);
        LockCameraToPlayer(&Camera,&Player);
        return;
        break;
    
    case PELIPPER_POST_OFFICE:
        CurrentOverworld = POKEMON_SQUARE;
        HandGamestateChange(GAME_OVERWORLD,GAME_LOADING_SCREEN,Mode);
        SetCurrentOverWorld(Overworlds[CurrentOverworld]);
        RelocatePlayer(&Player,16*TILE_SIZE,0,DIR_LEFT);
        LockCameraToPlayer(&Camera,&Player);
        return;
        break;
    
    default:
        break;
    }        
}

//  ----------------------- | END OF GAME STATE  | ----------------------- //


















// ------------------------ | POSITION FUNCTIONS | ----------------------- //

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


void updatePlayerPosition2D(PLAYER* Player, int32_t Direction) {
    switch (Direction) {
        case DIR_DOWN:
            Player->worldPosY--;
            break;

        case DIR_LEFT:
            Player->worldPosX--;
            break;

        case DIR_RIGHT:
            Player->worldPosX++;
            break;

        case DIR_UP:
            Player->worldPosY++;
            break;

        case DIR_DOWN_LEFT:
            Player->worldPosY--;
            Player->worldPosX--;
            break;

        case DIR_DOWN_RIGHT:
            Player->worldPosY--;
            Player->worldPosX++;
            break;

        case DIR_UP_LEFT:
            Player->worldPosY++;
            Player->worldPosX--;
            break;

        case DIR_UP_RIGHT:
            Player->worldPosY++;
            Player->worldPosX++;
            break;

        default:
            // Invalid direction; no action
            break;
    }
}



VOID LockCameraToPlayer(CAMERA* Camera, PLAYER* Player)
{
    Camera->worldPosX = Player->worldPosX; 
    Camera->worldPosY = Player->worldPosY; 
    updateCameraPosition2D(Camera->worldPosX,Camera->worldPosY,Player->direction);
}




void updateCameraPosition(int32_t& CameraPos, int direction) {
    
    // else perfrom normal movement   
        switch (direction) {
            case DIR_DOWN:

            if(Camera.CameraLockY == 1)
            {
                if (Player.ScreenPosY > GAME_HEIGHT/2)
                {Camera.CameraLockY = 0;}
                else
                {return;}
                
            }
            if (CameraPos < -BORDER_DOWN*TILE_SIZE)
            {Camera.CameraLockY=1;}
            else
            {CameraPos--;}
            break;
            
            case DIR_LEFT:
            if(Camera.CameraLockX == 1)
            {
                if (Player.ScreenPosX < GAME_WIDTH/2)
                {Camera.CameraLockX = 0;}
                else
                {return;}
                
            }
            if (CameraPos < -BORDER_LEFT*TILE_SIZE)
            {Camera.CameraLockX=1;}
            else
            {CameraPos--;}    
            break;
            
            case DIR_RIGHT:
            
            if(Camera.CameraLockX == 1)
            {
                if (Player.ScreenPosX > GAME_WIDTH/2)
                {Camera.CameraLockX = 0;}
                else
                {return;}
                
            }
            if (CameraPos > BORDER_RIGHT*TILE_SIZE)
            {Camera.CameraLockX=1;}
            else
            {CameraPos++;}
            break;
            
            case DIR_UP:

            if(Camera.CameraLockY == 1)
            {
                if (Player.ScreenPosY < GAME_HEIGHT/2)
                {Camera.CameraLockY = 0;}
                else
                {return;}
                
            }
            
            if (CameraPos > BORDER_UP*TILE_SIZE)
            {Camera.CameraLockY=1;}
            else
            {CameraPos++;}
            
            break;
            default:
            //assert false?//
                break;
        }
    

}


void updateCameraPosition2D(int32_t& CameraPosX, int32_t& CameraPosY, int direction) {
    
    int32_t Border = Overworlds[CurrentOverworld].GetBorder(direction);

    switch (direction) {
        case DIR_DOWN:
            if (Camera.CameraLockY == 1) {
                if (Player.ScreenPosY > GAME_HEIGHT / 2) {
                    Camera.CameraLockY = 0;
                } else {
                    return;
                }
            }
            if (CameraPosY < -Border * TILE_SIZE) {
                Camera.CameraLockY = 1;
                
            } else {
                CameraPosY--;
            }
            break;

        case DIR_LEFT:
            if (Camera.CameraLockX == 1) {
                if (Player.ScreenPosX < GAME_WIDTH / 2) {
                    Camera.CameraLockX = 0;
                } else {
                    return;
                }
            }
            if (CameraPosX < -Border * TILE_SIZE) {
                Camera.CameraLockX = 1;
                // CameraPosX = -Border * TILE_SIZE;
            } else {
                CameraPosX--;
            }
            break;

        case DIR_RIGHT:
            if (Camera.CameraLockX == 1) {
                if (Player.ScreenPosX > GAME_WIDTH / 2) {
                    Camera.CameraLockX = 0;
                } else {
                    return;
                }
            }
            if (CameraPosX > Border * TILE_SIZE) {
                Camera.CameraLockX = 1;
            } else {
                CameraPosX++;
            }
            break;

        case DIR_UP:
            if (Camera.CameraLockY == 1) {
                if (Player.ScreenPosY < GAME_HEIGHT / 2) {
                    Camera.CameraLockY = 0;
                } else {
                    return;
                }
            }
            if (CameraPosY > Border * TILE_SIZE) {
                Camera.CameraLockY = 1;
            } else {
                CameraPosY++;
            }
            break;

        case DIR_DOWN_LEFT:
            updateCameraPosition2D(CameraPosX, CameraPosY, DIR_DOWN);
            updateCameraPosition2D(CameraPosX, CameraPosY, DIR_LEFT);
            break;

        case DIR_DOWN_RIGHT:
            updateCameraPosition2D(CameraPosX, CameraPosY, DIR_DOWN);
            updateCameraPosition2D(CameraPosX, CameraPosY, DIR_RIGHT);
            break;

        case DIR_UP_LEFT:
            updateCameraPosition2D(CameraPosX, CameraPosY, DIR_UP);
            updateCameraPosition2D(CameraPosX, CameraPosY, DIR_LEFT);
            break;

        case DIR_UP_RIGHT:
            updateCameraPosition2D(CameraPosX, CameraPosY, DIR_UP);
            updateCameraPosition2D(CameraPosX, CameraPosY, DIR_RIGHT);
            break;

        default:
            // Invalid direction; no action
            break;
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
    
        
        for (int i = 0; i < Loaded_NPCs.size();i++){
            if(tileIndex == Loaded_NPCs[i].StandingTile_Index && Loaded_NPCs[i].visbility ==1){
                CurrentNPCIndex = i;
            return NPC_ON_TILE;
        }
        }
        
        return Background_Tiles[tileIndex].type;
        
        
}

int32_t GetNextPlayerTile2D(PLAYER *player, int32_t Direction) {
    int32_t tileIndex = player->StandingTile_Index;

    switch (Direction) {
        case DIR_DOWN:
            tileIndex += NUMB_TILES_PER_ROW;
            break;

        case DIR_LEFT:
            tileIndex += 1;
            break;

        case DIR_RIGHT:
            tileIndex -= 1;
            break;

        case DIR_UP:
            tileIndex -= NUMB_TILES_PER_ROW;
            break;

        case DIR_DOWN_LEFT:
            tileIndex += (NUMB_TILES_PER_ROW + 1);
            break;

        case DIR_DOWN_RIGHT:
            tileIndex += (NUMB_TILES_PER_ROW - 1);
            break;

        case DIR_UP_LEFT:
            tileIndex -= (NUMB_TILES_PER_ROW - 1);
            break;

        case DIR_UP_RIGHT:
            tileIndex -= (NUMB_TILES_PER_ROW + 1);
            break;

        default:
            // No direction
            break;
    }

    // Check if the tile has an NPC
    for (int i = 0; i < Loaded_NPCs.size(); i++) {
        if (tileIndex == Loaded_NPCs[i].StandingTile_Index && Loaded_NPCs[i].visbility == 1) {
            CurrentNPCIndex = i;
            return NPC_ON_TILE;
        }
    }

    // Return the type of the background tile
    return Background_Tiles[tileIndex].type;
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


VOID UpdatePlayerSprite(PLAYER* player, int32_t totalSprites, int32_t fps) 
{
        if (player->movementRemaining > 0) 
        {
            player->movementRemaining--;

            // Calculate the current animation step based on remaining movement
            int32_t stepInterval = fps / totalSprites; // Frames per sprite
            int32_t currentStep = (fps - player->movementRemaining) / stepInterval;

            // Clamp to the total number of sprites
            if (currentStep >= totalSprites) 
            {
                currentStep = totalSprites - 1;
            }

            player->animation_step = currentStep; // Animation step starts from 1
    }
}

VOID UpdateIdleAnimation(PLAYER* player, int32_t animationStartStep, int32_t animationEndStep, int32_t idleCycleFrames) 
{
    if (player->idleFrameCount < idleCycleFrames) 
    {
        player->animation_step = animationStartStep;
    } 
    else if (player->idleFrameCount >= idleCycleFrames && player->idleFrameCount < idleCycleFrames + 20) 
    {
        player->animation_step = animationEndStep;
    } else 
    {
        player->idleFrameCount = 0;
        return;
    }

    player->idleFrameCount++;
}


VOID UpdateIdleAnimationNPC(NPC* npc, int32_t animationStartStep, int32_t animationEndStep, int32_t idleCycleFrames) 
{
    if (npc->idleFrameCount < idleCycleFrames) 
    {
        npc->animation_step = animationStartStep;
    } 
    else if (npc->idleFrameCount >= idleCycleFrames && npc->idleFrameCount < idleCycleFrames + 20) 
    {
        npc->animation_step = animationEndStep;
    } else 
    {
        npc->idleFrameCount = 0;
        return;
    }

    npc->idleFrameCount++;
}





// ------------------------ | END OF POSITION FUNCTIONS | ----------------------- //













//  ----------------------- | TEXT/OPT/MENU/DIAL | ----------------------- // 

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

VOID ToggleTextBox(){
    ShowTextBox = ShowTextBox^1;
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

VOID ToggleOptBox(){
    ShowOptBox = ShowOptBox^1;
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

// ----------------------- | END OF TEXT/OPT/MENU/DIAL | ----------------------- // 
















//  ----------------------- | MISC FUNCTIONS  | ----------------------- //  




BOOLEAN IsTileOnScreen(int32_t TileIndex) {
    int32_t left_limit = Player.StandingTile_Index % 100 + GAME_WIDTH / (2 * TILE_SIZE);
    int32_t right_limit = Player.StandingTile_Index % 100 - GAME_WIDTH / (2 * TILE_SIZE);
    int32_t top_limit = Player.StandingTile_Index / 100 + GAME_HEIGHT / (2 * TILE_SIZE);
    int32_t bot_limit = Player.StandingTile_Index / 100 - GAME_HEIGHT / (2 * TILE_SIZE);

    return (TileIndex%100 <= left_limit && TileIndex%100 >= right_limit && TileIndex/100 <= top_limit+1 && TileIndex/100 >= bot_limit-1);
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

// Animation Movement and Vis // 
VOID NPC_AMV_Handler()
{
        for (int i = 0; i < Loaded_NPCs.size();i++)
        {
        UpdateIdleAnimationNPC(&Loaded_NPCs[i],0,1,100);
        }   
}



VOID ToggleGrid()
{
    gridView = gridView^1;
}




//  ----------------------- | END OF MISC FUNCTIONS |  ----------------------- // 