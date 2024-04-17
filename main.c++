#ifndef UNICODE
#define UNICODE
#endif 



#include <string>
#include "main.h"
#include "generate.c++"
#include "graphics.c++"
#include "controls.c++"
#include "textbox.c++"
#include <cstdint>



// Globals //


BOOL gGameIsRunning;
HWND gGameWindow;
GAMESTATE gamestate = GAME_OPENING;

GAMEBITMAP DrawingSurface;
GAMEBITMAP Font;
GAMEBITMAP TextBox,OptionBox;


PREFORMENCE_DATA gPreformance_Data; 
PLAYER Player;
NPC npc;
int32_t gMonitorWidth;
int32_t gMonitorHeight;

GAMEBITMAP Room_Sprite;
GAMEBITMAP Background;

GAMEBITMAP Tile_Sprite_Sheet;

TILE Background_Tiles[NUMB_TILES]; 
TILE Tile_Type_Array[NUMB_TILE_TYPES];

int32_t starting_pos;

Graphics graphics(&DrawingSurface, &Player);

int32_t LoadingFrameCount = 0;
LPSTR LoadingText = "Loading";

Dialogue dialogue(1);
int32_t PageIndex = 0;

int_fast8_t ShowTextBox = 1;
int_fast8_t ShowOptBox = 1;

// Defaut Option selection set to Yes // 
int_fast8_t YesNoOptions = 0;

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
    

    if (timeBeginPeriod(1) == TIMERR_NOCANDO){
        MessageBoxA(NULL, "Unable to set time res", "Error", MB_ICONEXCLAMATION | MB_OK);
        result = GetLastError();
        return result;
    }

    dialogue.setDialogue(0, 0, "Would you like to load into Amp Plains?"); 
    dialogue.setDialogue(0, 1, ""); 
    dialogue.setDialogue(0, 2, ""); 
    dialogue.setDialogue(0, 3, "opt"); 
    // dialogue.setDialogue(0, 2, "Page 1 Row 3"); 
    // dialogue.setDialogue(1, 0, "Page 2 Row 1"); 
    // dialogue.setDialogue(1, 1, "Page 2 Row 2"); 
    // dialogue.setDialogue(1, 2, "Page 2 Row 3"); 
    // dialogue.setDialogue(2, 0, "Page 3 Row 1"); 
    // dialogue.setDialogue(2, 1, "Page 3 Row 2"); 
    // dialogue.setDialogue(2, 2, "Page 3 Row 3"); 
    
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
    

    // if (InitNPC() != ERROR_SUCCESS){
    //     MessageBoxA(NULL, "Error ititalizing NPCs sprite", "Error", MB_ICONEXCLAMATION | MB_OK);
    //     result = GetLastError();
    //     return result;
    // }


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
                gamestate = GAME_DUNGEON;
                LoadingFrameCount = 0; 
            }
        case GAME_OPENING:
        LoadingFrameCount++;
            if(LoadingFrameCount>200){
                gamestate = GAME_TITLE_SCREEN;
                LoadingFrameCount = 0; 
            }
            break;
        
        default:
            break;
        }

        process_player_input();
        render_game_frames();

        
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
            ShowCursor(TRUE);
            if (MessageBox(hwnd, L"Regenerate Room?", L"Project", MB_OKCANCEL) == IDOK)
            {
                HandleStairs(Player);
                teleportPlayer(Player);
                LoadingText = "Loading next Room";
                gamestate = GAME_LOADING_SCREEN;
            }
            // Else: User canceled. Do nothing.
            ShowCursor(FALSE);
           
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
    
        
        if(ShowTextBox == 1 && ShowOptBox == 0){
            if(A_KeyDown && !a_KeyWasDown){
                HandleDialog(&dialogue) ? ToggleTextBox() : dialogue.incrementPage();
            }
        }
        else if (ShowOptBox == 1){
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
                    ToggleTextBox();
                    ToggleOptBox();
                    
                    LoadDungeonIntoMemory("assets\\tiles\\Tiles-Amp-Plains.bmp");
                    Sleep(200);
                    gamestate = GAME_LOADING_SCREEN; 
                    break;
                
                default:
                    SendMessageA(gGameWindow,WM_CLOSE,0,0);
                    break;
                }
            }
        }
        

        break;
    
    case GAME_DUNGEON:    

    Player.noClip = (C_KeyDown) ? 1 - Player.noClip : Player.noClip;

    if(T_KeyDown && !t_KeyWasDown){
        teleportPlayer(Player);
    }
    if(R_KeyDown && ! r_KeyWasDown){
        HandleStairs(Player);
    }

   
   if (Player.movementRemaining == 0){
    if(Player.StandingTile.type == STAIRS){
        SendMessageA(gGameWindow,STAIRS,0,0);
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




}
//"assets\\tiles\\test_tiles.bmp"


DWORD LoadDungeonIntoMemory(char* PathToTileBitmapFile){
    DWORD Error = ERROR_SUCCESS;
    if(Load32BppBitmapFromFile(PathToTileBitmapFile,&Tile_Sprite_Sheet) != ERROR_SUCCESS){
        MessageBoxA(NULL, "Unable to load font sheet into memory", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }

    InitTiles(Tile_Sprite_Sheet);
    ProceduralGenerator(1000,10,18,Background_Tiles,Tile_Type_Array,10);
    BuiltTileMap(Background_Tiles,&Background);

    graphics.SetBackgroundBitMap(&Background);



    if (InitPlayer() != ERROR_SUCCESS){
        MessageBoxA(NULL, "Error ititalizing player sprite", "Error", MB_ICONEXCLAMATION | MB_OK);
        Error = GetLastError();
        return Error;
    }



    return Error;
}


VOID HandleStairs(PLAYER p){
        ResetTiles(Background_Tiles,Tile_Type_Array);
        ProceduralGenerator(1000,10,18,Background_Tiles,Tile_Type_Array,10);
        BuiltTileMap(Background_Tiles,&Background);
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

VOID teleportPlayer(PLAYER P){
    InitPlayer();

}

VOID render_game_frames(void){
    
    switch (gamestate)
    {
    case GAME_TITLE_SCREEN:
        RenderTitleScene();
        break;
    case GAME_DUNGEON:
        RenderDungeonScene(Background,&Player);
        break;

    case GAME_LOADING_SCREEN:
        RenderLoadingScene(LoadingText);
        break;
    default:
        
        break;
    }
    
    HDC deviceContext = GetDC(gGameWindow);
    StretchDIBits(deviceContext,0,0,gMonitorWidth,gMonitorHeight,0,0,GAME_WIDTH,GAME_HEIGHT,DrawingSurface.memory,&DrawingSurface.bitMapInfo,DIB_RGB_COLORS,SRCCOPY);
    
    char fpsBuffer[64] = {0};
    //sprintf(fpsBuffer, "Cooked FPS: %.01f Raw FPS: %.01f Screen Position: %d:%d, %d ",gPreformance_Data.CookFPS,gPreformance_Data.RawFPS,Player.worldPosX, Player.worldPosY, Player.InRoom);
    sprintf(fpsBuffer, "Cooked FPS: %.01f Raw FPS: %.01f",gPreformance_Data.CookFPS,gPreformance_Data.RawFPS);
    SetTextColor(deviceContext, RGB(255, 255, 255));  
    SetBkMode(deviceContext, TRANSPARENT );
    TextOutA( deviceContext, 0, 0, fpsBuffer, strlen( fpsBuffer ) );
     
    
    ReleaseDC(gGameWindow,deviceContext);

   

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

DWORD InitNPC(VOID){
    DWORD Error = ERROR_SUCCESS;
    npc.worldPosX=32;
    npc.worldPosY=32;
    npc.movementRemaining = 0;
    npc.animation_step = 1;
    npc.direction = DIR_DOWN;
    npc.idleFrameCount = 0;
    
    /* Load player sprites */
    
    
    /* Load player sprites sheets */
    Error = Load32BppBitmapFromFile("assets\\Player_Sprites\\Walk-Anim.bmp",&npc.sprite_sheet[0]);
    Error = Load32BppBitmapFromFile("assets\\Player_Sprites\\Idle-Anim.bmp",&npc.sprite_sheet[1]);

    /* Walking Animations */

    /* Down */
    Error = LoadSpriteFromSpriteSheet(npc.sprite_sheet[0], &npc.sprite[0][0],8,0,7,0);
    Error = LoadSpriteFromSpriteSheet(npc.sprite_sheet[0], &npc.sprite[0][1],8,0,7,1);
    Error = LoadSpriteFromSpriteSheet(npc.sprite_sheet[0], &npc.sprite[0][2],8,0,7,2);
    Error = LoadSpriteFromSpriteSheet(npc.sprite_sheet[0], &npc.sprite[0][3],8,0,7,4);

    /* Left */
    Error = LoadSpriteFromSpriteSheet(npc.sprite_sheet[0], &npc.sprite[1][0],8,0,1,0);
    Error = LoadSpriteFromSpriteSheet(npc.sprite_sheet[0], &npc.sprite[1][1],8,0,1,1);
    Error = LoadSpriteFromSpriteSheet(npc.sprite_sheet[0], &npc.sprite[1][2],8,0,1,2);
    Error = LoadSpriteFromSpriteSheet(npc.sprite_sheet[0], &npc.sprite[1][3],8,0,1,4);

    /* Right */
    Error = LoadSpriteFromSpriteSheet(npc.sprite_sheet[0], &npc.sprite[2][0],8,0,5,0);
    Error = LoadSpriteFromSpriteSheet(npc.sprite_sheet[0], &npc.sprite[2][1],8,0,5,1);
    Error = LoadSpriteFromSpriteSheet(npc.sprite_sheet[0], &npc.sprite[2][2],8,0,5,2);
    Error = LoadSpriteFromSpriteSheet(npc.sprite_sheet[0], &npc.sprite[2][3],8,0,5,4);

    /* Up */
    Error = LoadSpriteFromSpriteSheet(npc.sprite_sheet[0], &npc.sprite[3][0],8,0,3,0);
    Error = LoadSpriteFromSpriteSheet(npc.sprite_sheet[0], &npc.sprite[3][1],8,0,3,1);
    Error = LoadSpriteFromSpriteSheet(npc.sprite_sheet[0], &npc.sprite[3][2],8,0,3,2);
    Error = LoadSpriteFromSpriteSheet(npc.sprite_sheet[0], &npc.sprite[3][3],8,0,3,4);
    

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


// VOID LoadBackgroundToScreen(GAMEBITMAP BackgroundBitMap){
//     int32_t Starting_Coordinate = ((GAME_HEIGHT*GAME_WIDTH) - GAME_WIDTH);

//     int32_t TilesPerScreenH = GAME_WIDTH/TILE_SIZE; // 16
//     int32_t TilesPerScreenV = GAME_HEIGHT/TILE_SIZE; // 10

//     int32_t StartingCol = STARTING_TILE%NUMB_TILES_PER_ROW;
//     int32_t StartingRow = STARTING_TILE/NUMB_TILES_PER_ROW;

//     int32_t HorizontalMapOffset = (NUMB_TILES_PER_ROW - TilesPerScreenH) / 2;
//     int32_t VerticalMapOffset = (NUMB_TILES_PER_ROW/TilesPerScreenV) /2 ;
//     int32_t Starting_Coordinate_Bitmap = ((BackgroundBitMap.bitMapInfo.bmiHeader.biWidth*BackgroundBitMap.bitMapInfo.bmiHeader.biHeight)/2)  + (BackgroundBitMap.bitMapInfo.bmiHeader.biWidth*TILE_SIZE*VerticalMapOffset +TILE_SIZE*HorizontalMapOffset);


//     int32_t BitMapOffset = 0;
//     int32_t BitMapStart = 0;


//     int32_t PlayerYOffset = Player.worldPosY;
//     int32_t PlayerXOffset = Player.worldPosX;

    
    
//     if (PlayerXOffset >= 41 * TILE_SIZE){
//         PlayerXOffset = 41 * TILE_SIZE;
//     }

//     else if (PlayerXOffset <= (-41 * TILE_SIZE)){
//         PlayerXOffset = (-41 * TILE_SIZE);
//     }

    
    
//      if (PlayerYOffset >= 44 * TILE_SIZE){
//         PlayerYOffset = 44 * TILE_SIZE;
//     }

//     else if (PlayerYOffset <= (-44 * TILE_SIZE)){
//         PlayerYOffset = (-44 * TILE_SIZE);
//     }
    
//     BitMapStart = Starting_Coordinate_Bitmap + (PlayerYOffset*BackgroundBitMap.bitMapInfo.bmiHeader.biWidth) + PlayerXOffset;
  
    
    
//     int32_t MemoryOffset = 0;
//     PIXEL BitmapPixels = {0};
    
    
//     FLOAT darkenFactor = 0.7f;
//     int32_t circleRadius;
//     switch (Player.InRoom)
//     {
//     case 0:
//         circleRadius = 50;
//         break;
    
//     default:
//         circleRadius = 14*TILE_SIZE;
//         break;
//     }
    

//     int32_t circleRadiusSquared = circleRadius * circleRadius;


//     for(int32_t PixelY = 0; PixelY < GAME_HEIGHT; PixelY++){
//         for(int32_t PixelX = 0; PixelX < GAME_WIDTH; PixelX++){
//             int32_t dx = PixelX - Player.ScreenPosX - 4;
//             int32_t dy = PixelY - Player.ScreenPosY - 11;
            
//             // Calculate the squared distance from the current pixel to the player
//             int32_t distanceSquared = dx * dx + dy * dy;

//             MemoryOffset = Starting_Coordinate + PixelX - (GAME_WIDTH*PixelY);
//             BitMapOffset = BitMapStart + PixelX - (BackgroundBitMap.bitMapInfo.bmiHeader.biWidth * PixelY);
            
//             memcpy(&BitmapPixels,(PIXEL*)BackgroundBitMap.memory + BitMapOffset,sizeof(PIXEL));
//             if (distanceSquared > circleRadiusSquared) {
//                 BitmapPixels.red *= darkenFactor;
//                 BitmapPixels.green *= darkenFactor;
//                 BitmapPixels.blue *= darkenFactor;
//             }
            
           
//             memcpy((PIXEL*)DrawingSurface.memory + MemoryOffset,&BitmapPixels,sizeof(PIXEL));
            

//         }
//     }

// }

// VOID LoadBackScreen(){
//     int32_t MemoryOffset = 0;
//     int32_t Starting_Coordinate = GAME_WIDTH*GAME_HEIGHT - GAME_WIDTH;
//     PIXEL p = {0};
    
//     for(int32_t PixelY = 0; PixelY < GAME_HEIGHT; PixelY++){
//         for(int32_t PixelX = 0; PixelX < GAME_WIDTH; PixelX++){

//             MemoryOffset = Starting_Coordinate + PixelX - (GAME_WIDTH*PixelY);
//             memcpy((PIXEL*)DrawingSurface.memory + MemoryOffset,&p,sizeof(PIXEL));
            
    
//         }
//     }
    

// }



// VOID LoadBitMapToScreen(GAMEBITMAP GameBitMap, int16_t x, int16_t y, int16_t VerticalOffset,int16_t HorizontalOffset){
//     x += HorizontalOffset;
//     y += VerticalOffset;
//     int32_t Starting_Coordinate = ((GAME_HEIGHT*GAME_WIDTH) - GAME_WIDTH) - (GAME_WIDTH*y) + x;
//     int32_t Starting_BitMapPixel = (GameBitMap.bitMapInfo.bmiHeader.biHeight * GameBitMap.bitMapInfo.bmiHeader.biWidth) - GameBitMap.bitMapInfo.bmiHeader.biWidth;
//     int32_t MemoryOffset = 0;
//     int32_t BitMapOffset = 0;
//     PIXEL BitmapPixels = {0};
//     PIXEL BackgroundPixels = {0};

    
//     for(int32_t PixelY = 0; PixelY < GameBitMap.bitMapInfo.bmiHeader.biHeight; PixelY++){
//         for(int32_t PixelX = 0; PixelX < GameBitMap.bitMapInfo.bmiHeader.biWidth; PixelX++){
//             //int32_t offset = (Starting_Coordinate + x) - (GAME_WIDTH*y);

//             MemoryOffset = Starting_Coordinate + PixelX - (GAME_WIDTH*PixelY);
//             BitMapOffset = Starting_BitMapPixel + PixelX - (GameBitMap.bitMapInfo.bmiHeader.biWidth * PixelY);

//             memcpy(&BitmapPixels,(PIXEL*)GameBitMap.memory + BitMapOffset,sizeof(PIXEL));
//             if (BitmapPixels.alpha == 255){
//                 memcpy((PIXEL*)DrawingSurface.memory + MemoryOffset,&BitmapPixels,sizeof(PIXEL));
//             }
            

//         }
//     }

// }



// void LoadBitFontToScreen(GAMEBITMAP GameBitMap, const std::string& str, int16_t x, int16_t y, int16_t center) {
    
//     // char* str = string.cstr();
//     int charWidth = GameBitMap.bitMapInfo.bmiHeader.biWidth / 13;
//     int charHeight = GameBitMap.bitMapInfo.bmiHeader.biHeight/ 12;

//     int bytesPerChar = 4 * charWidth * charHeight;
//     int StringLength = str.length();
    
//     GAMEBITMAP stingBitMap = {0};
    
//     stingBitMap.bitMapInfo.bmiHeader.biBitCount = GAME_BPP;
//     stingBitMap.bitMapInfo.bmiHeader.biWidth = charWidth*StringLength;
//     stingBitMap.bitMapInfo.bmiHeader.biHeight = charHeight;
//     stingBitMap.bitMapInfo.bmiHeader.biCompression = 0;
//     stingBitMap.bitMapInfo.bmiHeader.biPlanes = 1;
    
//     stingBitMap.memory = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,bytesPerChar * StringLength);
    

//     for (int i = 0; i < StringLength; i++){
//         int Starting_Address = 0;
//         int FontSheetOffset = 0;
//         int StrBitMapOffset = 0;
//         PIXEL FontSheetPixels = {0};

       

//         /* Starting address = SheetWidth * SheetHeight - (SheetWidth * CharHeight * Row) + (CharWidth * Col) */
        
//         switch(str[i]){
//             case 'a':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 0);
//             break;
//             case 'b':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 1);
//             break;
//             case 'c':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 2);
//             break;
//             case 'd':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 3);
//             break;
//             case 'e':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 4);
//             break;
//             case 'f':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 5);
//             break;
//             case 'g':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 6);
//             break;
//             case 'h':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 7);
//             break;
//             case 'i':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 8);
//             break;
//             case 'j':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 9);
//             break;
//             case 'k':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 10);
//             break;
//             case 'l':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 11);
//             break;
//             case 'm':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 12);
//             break;
//             case 'n':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 0);
//             break;
//             case 'o':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 1);
//             break;
//             case 'p':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 2);
//             break;
//             case 'q':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 3);
//             break;
//             case 'r':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 4);
//             break;
//             case 's':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 5);
//             break;
//             case 't':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 6);
//             break;
//             case 'u':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 7);
//             break;
//             case 'v':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 8);
//             break;
//             case 'w':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 9);
//             break;
//             case 'x':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 10);
//             break;
//             case 'y':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 11);
//             break;
//             case 'z':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 12);
//             break;
//             case 'A':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 0);
//             break;
//             case 'B':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 1);
//             break;
//             case 'C':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 2);
//             break;
//             case 'D':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 3);
//             break;
//             case 'E':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 4);
//             break;
//             case 'F':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 5);
//             break;
//             case 'G':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 6);
//             break;
//             case 'H':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 7);
//             break;
//             case 'I':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 8);
//             break;
//             case 'J':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 9);
//             break;
//             case 'K':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 10);
//             break;
//             case 'L':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 11);
//             break;
//             case 'M':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 12);
//             break;
//             case 'N':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 0);
//             break;
//             case 'O':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 1);
//             break;
//             case 'P':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 2);
//             break;
//             case 'Q':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 3);
//             break;
//             case 'R':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 4);
//             break;
//             case 'S':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 5);
//             break;
//             case 'T':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 6);
//             break;
//             case 'U':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 7);
//             break;
//             case 'V':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 8);
//             break;
//             case 'W':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 9);
//             break;
//             case 'X':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 10);
//             break;
//             case 'Y':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 11);
//             break;
//             case 'Z':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 12);
//             break;
//             case '1':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 0 );
//             break;
//             case '2':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 1);
//             break;
//             case '3':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 2);
//             break;
//             case '4':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 3);
//             break;
//             case '5':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 4);
//             break;
//             case '6':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 5);
//             break;
//             case '7':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 6);
//             break;
//             case '8':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 7);
//             break;
//             case '9':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 8);
//             break;
//             case '0':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 9);
//             break;
//             case ':':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 10);
//             break;
//             case '+':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 11);
//             break;
//             case '-':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 12);
//             break;
//             case ',':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 5) + (charWidth * 0);
//             break;
//             case '.':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 5) + (charWidth * 2);
//             break;
//             case '!':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 5) + (charWidth * 4);
//             break;
//             case '?':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 5) + (charWidth * 5);
//             break;
//             case '\'':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 5) + (charWidth * 6);
//             break;
//             case ' ':
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 12) + (charWidth * 10);
//             break;

//             default:
//             // ? // 
//             Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 5) + (charWidth * 2);

//         }

        
//         for(int32_t PixelY = 0; PixelY < charHeight; PixelY++){
            
//             for(int32_t PixelX =0 ; PixelX < charWidth; PixelX++){
            
//             FontSheetOffset = (Starting_Address + PixelX) - (GameBitMap.bitMapInfo.bmiHeader.biWidth*PixelY);
//             StrBitMapOffset = (i*7) + ((stingBitMap.bitMapInfo.bmiHeader.biHeight*stingBitMap.bitMapInfo.bmiHeader.biWidth) - stingBitMap.bitMapInfo.bmiHeader.biWidth) + PixelX - (stingBitMap.bitMapInfo.bmiHeader.biWidth* PixelY);
            
//             memcpy(&FontSheetPixels,(PIXEL*)GameBitMap.memory + FontSheetOffset,sizeof(PIXEL));
//             if(FontSheetPixels.alpha == 255){
//                 memcpy((PIXEL*)stingBitMap.memory + StrBitMapOffset,&FontSheetPixels,sizeof(PIXEL));
//             }
//         }
        
//     }
    

//     }
     
//     int32_t xOffset = (charWidth*(StringLength)/4);
//     switch (center)
//     {
//     case 1:
//         LoadBitMapToScreen(stingBitMap,x,y,0,-xOffset);
//         break;
    
//     default:
//         LoadBitMapToScreen(stingBitMap,x,y,0,0);
//         break;
//     }
        
//     if(stingBitMap.memory){
//         HeapFree(GetProcessHeap(),0,stingBitMap.memory);
//     }
    

// }

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
        DisplayTextBox(TextBox,dialogue);
        break;
    
    default:
        break;
    }
    
}

VOID RenderTitleScene(){

    graphics.LoadBackScreen();
    graphics.LoadBitFontToScreen(Font,"Project B - Alpha", GAME_WIDTH/2, GAME_HEIGHT/2,1);

    switch (ShowTextBox)
    {
    case 1:
        DisplayTextBox(TextBox,dialogue);
        break;
    
    default:
        break;
    }

    switch (ShowOptBox)
    {
    case 1:
        DisplayOptBox(TextBox);
        break;
    
    default:
        break;
    }
   

}

VOID RenderLoadingScene(char* str){
    graphics.LoadBackScreen();
    graphics.LoadBitFontToScreen(Font,str, GAME_WIDTH/2, GAME_HEIGHT/2,1);
}

VOID DisplayTextBox(GAMEBITMAP TextBox, Dialogue Dialogue) {
     int32_t x = 42;
     int32_t y = 182;
     graphics.LoadBitMapToScreen(TextBox, 40, 180, 0, 0);
     

    for (int i = 0; i < 3; i++){
        
        graphics.LoadBitFontToScreen(Font, Dialogue.getDialogue(Dialogue.getCurrentPage(),i), x, y, 0);
        y += 10;
    }
    
}

VOID DisplayOptBox(GAMEBITMAP TextBox) {
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


BOOL HandleDialog(Dialogue* Dialogue){
    // If on final page return true;
    if(Dialogue->getCurrentPage() >= (Dialogue->getNumberOfPages() - 1)){
        return TRUE;
    }
    else{

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