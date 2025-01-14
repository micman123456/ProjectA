#include "main.h"


class Graphics {
private:
    GAMEBITMAP* DrawingSurface;
    GAMEBITMAP* BackgroundBitMap;
    PLAYER* Player;
    GAMEBITMAP Font;

public:
    // Constructor
    Graphics(GAMEBITMAP* surface, PLAYER* player) : DrawingSurface(surface), Player(player) {
        // Initialize Font here if necessary
    }

    // Destructor
    ~Graphics() {
        // Cleanup resources if needed
    }

    VOID SetBackgroundBitMap(GAMEBITMAP*BackgroundBitMap){
        this->BackgroundBitMap = BackgroundBitMap;
    }

    // Other member functions as needed

    VOID LoadColorToScreen(PIXEL color) {
    int32_t MemoryOffset = 0;
    int32_t Starting_Coordinate = GAME_WIDTH * GAME_HEIGHT - GAME_WIDTH;

    for (int32_t PixelY = 0; PixelY < GAME_HEIGHT; PixelY++) {
        for (int32_t PixelX = 0; PixelX < GAME_WIDTH; PixelX++) {
            MemoryOffset = Starting_Coordinate + PixelX - (GAME_WIDTH * PixelY);
            memcpy((PIXEL*)DrawingSurface->memory + MemoryOffset, &color, sizeof(PIXEL));
        }
    }
}



    VOID LoadScreenBlackBars(){
    int32_t MemoryOffset = 0;
    int32_t Starting_Coordinate = GAME_WIDTH*GAME_HEIGHT - GAME_WIDTH;
    PIXEL p = {0};
    
    for(int32_t PixelY = 0; PixelY < GAME_HEIGHT; PixelY++){
        for(int32_t PixelX = 0; PixelX < GAME_WIDTH; PixelX++){
            if(PixelX < 40 || PixelX > GAME_WIDTH-40 || PixelY < 24 || PixelY > GAME_HEIGHT - 24){            
            MemoryOffset = Starting_Coordinate + PixelX - (GAME_WIDTH*PixelY);
            memcpy((PIXEL*)DrawingSurface->memory + MemoryOffset,&p,sizeof(PIXEL));
            }

            }
        }
    }

    
    VOID LoadGrid(){
    int32_t MemoryOffset = 0;
    int32_t Starting_Coordinate = GAME_WIDTH*GAME_HEIGHT - GAME_WIDTH;
    PIXEL p = {255};
    
    for(int32_t PixelY = 0; PixelY < GAME_HEIGHT; PixelY++){
        for(int32_t PixelX = 0; PixelX < GAME_WIDTH; PixelX++){
            if(PixelX%TILE_SIZE == 0 || PixelY%TILE_SIZE==0){            
            MemoryOffset = Starting_Coordinate + PixelX - (GAME_WIDTH*PixelY);
            memcpy((PIXEL*)DrawingSurface->memory + MemoryOffset,&p,sizeof(PIXEL));
            }

            }
        }
    }
    


    VOID LoadBackgroundToScreen(){
    int32_t Starting_Coordinate = ((GAME_HEIGHT*GAME_WIDTH) - GAME_WIDTH);

    int32_t TilesPerScreenH = GAME_WIDTH/TILE_SIZE; // 16
    int32_t TilesPerScreenV = GAME_HEIGHT/TILE_SIZE; // 10

    int32_t StartingCol = STARTING_TILE%NUMB_TILES_PER_ROW;
    int32_t StartingRow = STARTING_TILE/NUMB_TILES_PER_ROW;

    int32_t HorizontalMapOffset = (NUMB_TILES_PER_ROW - TilesPerScreenH) / 2;
    int32_t VerticalMapOffset = (NUMB_TILES_PER_ROW/TilesPerScreenV) /2 ;
    int32_t Starting_Coordinate_Bitmap = ((BackgroundBitMap->bitMapInfo.bmiHeader.biWidth*BackgroundBitMap->bitMapInfo.bmiHeader.biHeight)/2)  + (BackgroundBitMap->bitMapInfo.bmiHeader.biWidth*TILE_SIZE*VerticalMapOffset +TILE_SIZE*HorizontalMapOffset);


    int32_t BitMapOffset = 0;
    int32_t BitMapStart = 0;


    int32_t PlayerYOffset = Player->worldPosY;
    int32_t PlayerXOffset = Player->worldPosX;

    
    
    if (PlayerXOffset >= 41 * TILE_SIZE){
        PlayerXOffset = 41 * TILE_SIZE;
    }

    else if (PlayerXOffset <= (-41 * TILE_SIZE)){
        PlayerXOffset = (-41 * TILE_SIZE);
    }

    
    
     if (PlayerYOffset >= 44 * TILE_SIZE){
        PlayerYOffset = 44 * TILE_SIZE;
    }

    else if (PlayerYOffset <= (-44 * TILE_SIZE)){
        PlayerYOffset = (-44 * TILE_SIZE);
    }
    
    BitMapStart = Starting_Coordinate_Bitmap + (PlayerYOffset*BackgroundBitMap->bitMapInfo.bmiHeader.biWidth) + PlayerXOffset;
  
    
    
    int32_t MemoryOffset = 0;
    PIXEL BitmapPixels = {0};
    
    
    FLOAT darkenFactor = 0.7f;
    int32_t circleRadius;
    switch (Player->InRoom)
    {
    case 0:
        circleRadius = 50;
        break;
    
    default:
        circleRadius = 14*TILE_SIZE;
        break;
    }
    

    int32_t circleRadiusSquared = circleRadius * circleRadius;
    


    for(int32_t PixelY = 0; PixelY < GAME_HEIGHT; PixelY++){
        for(int32_t PixelX = 0; PixelX < GAME_WIDTH; PixelX++){
            int32_t dx = PixelX - Player->ScreenPosX - 4;
            int32_t dy = PixelY - Player->ScreenPosY - 11;
            
            // Calculate the squared distance from the current pixel to the player
            int32_t distanceSquared = dx * dx + dy * dy;

            MemoryOffset = Starting_Coordinate + PixelX - (GAME_WIDTH*PixelY);
            BitMapOffset = BitMapStart + PixelX - (BackgroundBitMap->bitMapInfo.bmiHeader.biWidth * PixelY);
            
            memcpy(&BitmapPixels,(PIXEL*)BackgroundBitMap->memory + BitMapOffset,sizeof(PIXEL));
            if (distanceSquared > circleRadiusSquared) {
                BitmapPixels.red *= darkenFactor;
                BitmapPixels.green *= darkenFactor;
                BitmapPixels.blue *= darkenFactor;
            }
            
           
            memcpy((PIXEL*)DrawingSurface->memory + MemoryOffset,&BitmapPixels,sizeof(PIXEL));
            

        }
    }

}


VOID LoadOverWorldToDrawingSurface(){
    

    int32_t MemoryOffset = 0;
    int32_t BitMapOffset = 0;
    PIXEL BitmapPixels = {0};

    

    int32_t Starting_Coordinates = ((GAME_HEIGHT*GAME_WIDTH) - GAME_WIDTH);

    int32_t TilesPerScreenH = GAME_WIDTH/TILE_SIZE; // 16
    int32_t TilesPerScreenV = GAME_HEIGHT/TILE_SIZE; // 10

    int32_t StartingCol = STARTING_TILE%NUMB_TILES_PER_ROW;
    int32_t StartingRow = STARTING_TILE/NUMB_TILES_PER_ROW;

    int32_t HorizontalMapOffset = (NUMB_TILES_PER_ROW - TilesPerScreenH) / 2;
    int32_t VerticalMapOffset = (NUMB_TILES_PER_ROW/TilesPerScreenV) /2 ;
    int32_t Starting_Coordinate_Bitmap = ((BackgroundBitMap->bitMapInfo.bmiHeader.biWidth*BackgroundBitMap->bitMapInfo.bmiHeader.biHeight)/2)  + (BackgroundBitMap->bitMapInfo.bmiHeader.biWidth*TILE_SIZE*VerticalMapOffset +TILE_SIZE*HorizontalMapOffset);

    int32_t PlayerYOffset = Player->worldPosY;
    int32_t PlayerXOffset = Player->worldPosX;
    
    if (PlayerXOffset >= 41 * TILE_SIZE){
        PlayerXOffset = 41 * TILE_SIZE;
    }

    else if (PlayerXOffset <= (-41 * TILE_SIZE)){
        PlayerXOffset = (-41 * TILE_SIZE);
    }

    
    
     if (PlayerYOffset >= 44 * TILE_SIZE){
        PlayerYOffset = 44 * TILE_SIZE;
    }

    else if (PlayerYOffset <= (-44 * TILE_SIZE)){
        PlayerYOffset = (-44 * TILE_SIZE);
    }

    int32_t BitMapStart = Starting_Coordinate_Bitmap + (PlayerYOffset*BackgroundBitMap->bitMapInfo.bmiHeader.biWidth) + PlayerXOffset;


    for(int32_t PixelY = 0; PixelY < GAME_HEIGHT; PixelY++){
        for(int32_t PixelX = 0; PixelX < GAME_WIDTH; PixelX++){

            MemoryOffset = Starting_Coordinates + PixelX - (GAME_WIDTH*PixelY);
            BitMapOffset = BitMapStart + PixelX - (BackgroundBitMap->bitMapInfo.bmiHeader.biWidth * PixelY);

            memcpy(&BitmapPixels,(PIXEL*)BackgroundBitMap->memory + BitMapOffset,sizeof(PIXEL));
            memcpy((PIXEL*)DrawingSurface->memory + MemoryOffset,&BitmapPixels,sizeof(PIXEL));
            
        }
    }
    
    // memset((PIXEL*)DrawingSurface->memory,155,sizeof(PIXEL)*GAME_WIDTH*GAME_HEIGHT);


}

VOID LoadBackgroundFromCamera(CAMERA Camera) {
    int32_t MemoryOffset = 0;
    int32_t BitMapOffset = 0;
    PIXEL BitmapPixels = {0};

    int32_t Starting_Coordinates = (GAME_HEIGHT * GAME_WIDTH) - GAME_WIDTH;

    int32_t TilesPerScreenH = GAME_WIDTH / TILE_SIZE; // 16
    int32_t TilesPerScreenV = GAME_HEIGHT / TILE_SIZE; // 10

    int32_t HorizontalMapOffset = (NUMB_TILES_PER_ROW - TilesPerScreenH) / 2;
    int32_t VerticalMapOffset = (NUMB_TILES_PER_ROW / TilesPerScreenV) / 2;

    int32_t Starting_Coordinate_Bitmap =
        ((BackgroundBitMap->bitMapInfo.bmiHeader.biWidth * BackgroundBitMap->bitMapInfo.bmiHeader.biHeight) / 2) +
        (BackgroundBitMap->bitMapInfo.bmiHeader.biWidth * TILE_SIZE * VerticalMapOffset +
         TILE_SIZE * HorizontalMapOffset);

    int32_t CameraXOffset = Camera.worldPosX;
    int32_t CameraYOffset = Camera.worldPosY;

    // Clamp camera offsets to stay within valid bounds
    if (CameraXOffset >= 41 * TILE_SIZE) {
        CameraXOffset = 41 * TILE_SIZE;
    } else if (CameraXOffset <= -41 * TILE_SIZE) {
        CameraXOffset = -41 * TILE_SIZE;
    }

    if (CameraYOffset >= 44 * TILE_SIZE) {
        CameraYOffset = 44 * TILE_SIZE;
    } else if (CameraYOffset <= -44 * TILE_SIZE) {
        CameraYOffset = -44 * TILE_SIZE;
    }

    int32_t BitMapStart = Starting_Coordinate_Bitmap +
                          (CameraYOffset * BackgroundBitMap->bitMapInfo.bmiHeader.biWidth) + CameraXOffset;

    // Draw the background based on the camera position
    for (int32_t PixelY = 0; PixelY < GAME_HEIGHT; PixelY++) {
        for (int32_t PixelX = 0; PixelX < GAME_WIDTH; PixelX++) {
            MemoryOffset = Starting_Coordinates + PixelX - (GAME_WIDTH * PixelY);
            BitMapOffset = BitMapStart + PixelX - (BackgroundBitMap->bitMapInfo.bmiHeader.biWidth * PixelY);

            memcpy(&BitmapPixels, (PIXEL*)BackgroundBitMap->memory + BitMapOffset, sizeof(PIXEL));
            memcpy((PIXEL*)DrawingSurface->memory + MemoryOffset, &BitmapPixels, sizeof(PIXEL));
        }
    }
}




VOID LoadBitMapToScreen(GAMEBITMAP GameBitMap, int16_t x, int16_t y, int16_t VerticalOffset,int16_t HorizontalOffset){
    x += HorizontalOffset;
    y += VerticalOffset;
    int32_t Starting_Coordinate = ((GAME_HEIGHT*GAME_WIDTH) - GAME_WIDTH) - (GAME_WIDTH*y) + x;
    int32_t Starting_BitMapPixel = (GameBitMap.bitMapInfo.bmiHeader.biHeight * GameBitMap.bitMapInfo.bmiHeader.biWidth) - GameBitMap.bitMapInfo.bmiHeader.biWidth;
    int32_t MemoryOffset = 0;
    int32_t BitMapOffset = 0;
    PIXEL BitmapPixels = {0};
    PIXEL BackgroundPixels = {0};

    
    int32_t startingX = x;
    
    for(int32_t PixelY = 0; PixelY < GameBitMap.bitMapInfo.bmiHeader.biHeight; PixelY++){
        
        for(int32_t PixelX = 0; PixelX < GameBitMap.bitMapInfo.bmiHeader.biWidth; PixelX++){

            MemoryOffset = Starting_Coordinate + PixelX - (GAME_WIDTH*PixelY);
            BitMapOffset = Starting_BitMapPixel + PixelX - (GameBitMap.bitMapInfo.bmiHeader.biWidth * PixelY);
            if (MemoryOffset < 0 || MemoryOffset >= GAME_HEIGHT * GAME_WIDTH) {
                continue; 
            }

            if (BitMapOffset < 0 || BitMapOffset >= GameBitMap.bitMapInfo.bmiHeader.biHeight * GameBitMap.bitMapInfo.bmiHeader.biWidth) {
                continue; 
            }

            

            if(startingX + PixelX < GAME_WIDTH && startingX + PixelX > 0){
            memcpy(&BitmapPixels,(PIXEL*)GameBitMap.memory + BitMapOffset,sizeof(PIXEL));
             
            if (BitmapPixels.alpha == 255){
               memcpy((PIXEL*)DrawingSurface->memory + MemoryOffset,&BitmapPixels,sizeof(PIXEL));
            }
            }
            
            

        }
    }

}
void LoadBitFontToScreen(GAMEBITMAP GameBitMap, const std::string& str, int16_t x, int16_t y, int16_t center) {
    
    // char* str = string.cstr();
    int charWidth = GameBitMap.bitMapInfo.bmiHeader.biWidth / 13;
    int charHeight = GameBitMap.bitMapInfo.bmiHeader.biHeight/ 12;

    int bytesPerChar = 4 * charWidth * charHeight;
    int StringLength = str.length();
    
    GAMEBITMAP stingBitMap = {0};
    
    stingBitMap.bitMapInfo.bmiHeader.biBitCount = GAME_BPP;
    stingBitMap.bitMapInfo.bmiHeader.biWidth = charWidth*StringLength;
    stingBitMap.bitMapInfo.bmiHeader.biHeight = charHeight;
    stingBitMap.bitMapInfo.bmiHeader.biCompression = 0;
    stingBitMap.bitMapInfo.bmiHeader.biPlanes = 1;
    
    stingBitMap.memory = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,bytesPerChar * StringLength);
    

    for (int i = 0; i < StringLength; i++){
        int Starting_Address = 0;
        int FontSheetOffset = 0;
        int StrBitMapOffset = 0;
        PIXEL FontSheetPixels = {0};

       

        /* Starting address = SheetWidth * SheetHeight - (SheetWidth * CharHeight * Row) + (CharWidth * Col) */
        
        switch(str[i]){
            case 'a':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 0);
            break;
            case 'b':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 1);
            break;
            case 'c':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 2);
            break;
            case 'd':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 3);
            break;
            case 'e':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 4);
            break;
            case 'f':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 5);
            break;
            case 'g':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 6);
            break;
            case 'h':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 7);
            break;
            case 'i':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 8);
            break;
            case 'j':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 9);
            break;
            case 'k':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 10);
            break;
            case 'l':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 11);
            break;
            case 'm':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - GameBitMap.bitMapInfo.bmiHeader.biWidth + (charWidth * 12);
            break;
            case 'n':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 0);
            break;
            case 'o':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 1);
            break;
            case 'p':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 2);
            break;
            case 'q':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 3);
            break;
            case 'r':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 4);
            break;
            case 's':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 5);
            break;
            case 't':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 6);
            break;
            case 'u':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 7);
            break;
            case 'v':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 8);
            break;
            case 'w':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 9);
            break;
            case 'x':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 10);
            break;
            case 'y':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 11);
            break;
            case 'z':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 1) + (charWidth * 12);
            break;
            case 'A':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 0);
            break;
            case 'B':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 1);
            break;
            case 'C':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 2);
            break;
            case 'D':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 3);
            break;
            case 'E':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 4);
            break;
            case 'F':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 5);
            break;
            case 'G':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 6);
            break;
            case 'H':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 7);
            break;
            case 'I':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 8);
            break;
            case 'J':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 9);
            break;
            case 'K':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 10);
            break;
            case 'L':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 11);
            break;
            case 'M':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 2) + (charWidth * 12);
            break;
            case 'N':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 0);
            break;
            case 'O':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 1);
            break;
            case 'P':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 2);
            break;
            case 'Q':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 3);
            break;
            case 'R':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 4);
            break;
            case 'S':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 5);
            break;
            case 'T':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 6);
            break;
            case 'U':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 7);
            break;
            case 'V':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 8);
            break;
            case 'W':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 9);
            break;
            case 'X':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 10);
            break;
            case 'Y':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 11);
            break;
            case 'Z':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 3) + (charWidth * 12);
            break;
            case '1':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 0 );
            break;
            case '2':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 1);
            break;
            case '3':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 2);
            break;
            case '4':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 3);
            break;
            case '5':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 4);
            break;
            case '6':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 5);
            break;
            case '7':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 6);
            break;
            case '8':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 7);
            break;
            case '9':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 8);
            break;
            case '0':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 9);
            break;
            case ':':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 10);
            break;
            case '+':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 11);
            break;
            case '-':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 4) + (charWidth * 12);
            break;
            case ',':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 5) + (charWidth * 0);
            break;
            case '.':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 5) + (charWidth * 1);
            break;
            case '!':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 5) + (charWidth * 4);
            break;
            case '?':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 5) + (charWidth * 5);
            break;
            case '\'':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 5) + (charWidth * 6);
            break;
            case ' ':
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 12) + (charWidth * 10);
            break;

            default:
            // ? // 
            Starting_Address = (GameBitMap.bitMapInfo.bmiHeader.biWidth * GameBitMap.bitMapInfo.bmiHeader.biHeight) - (GameBitMap.bitMapInfo.bmiHeader.biWidth + GameBitMap.bitMapInfo.bmiHeader.biWidth * charHeight * 5) + (charWidth * 2);

        }

        
        for(int32_t PixelY = 0; PixelY < charHeight; PixelY++){
            
            for(int32_t PixelX =0 ; PixelX < charWidth; PixelX++){
            
            FontSheetOffset = (Starting_Address + PixelX) - (GameBitMap.bitMapInfo.bmiHeader.biWidth*PixelY);
            StrBitMapOffset = (i*7) + ((stingBitMap.bitMapInfo.bmiHeader.biHeight*stingBitMap.bitMapInfo.bmiHeader.biWidth) - stingBitMap.bitMapInfo.bmiHeader.biWidth) + PixelX - (stingBitMap.bitMapInfo.bmiHeader.biWidth* PixelY);
            
            memcpy(&FontSheetPixels,(PIXEL*)GameBitMap.memory + FontSheetOffset,sizeof(PIXEL));
            if(FontSheetPixels.alpha == 255){
                memcpy((PIXEL*)stingBitMap.memory + StrBitMapOffset,&FontSheetPixels,sizeof(PIXEL));
            }
        }
        
    }
    

    }
     
    int32_t xOffset = (charWidth*(StringLength)/4);
    switch (center)
    {
    case 1:
        LoadBitMapToScreen(stingBitMap,x,y,0,-xOffset);
        break;
    
    default:
        LoadBitMapToScreen(stingBitMap,x,y,0,0);
        break;
    }
        
    if(stingBitMap.memory){
        HeapFree(GetProcessHeap(),0,stingBitMap.memory);
    }
    }

};



