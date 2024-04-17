#include "main.h"


DWORD CreateTextBoxBitMap(GAMEBITMAP* TextBox,int32_t width, int32_t height){
    DWORD Error = ERROR_SUCCESS;
    
    TextBox->bitMapInfo.bmiHeader.biSize = sizeof(TextBox->bitMapInfo.bmiHeader);
    TextBox->bitMapInfo.bmiHeader.biBitCount = GAME_BPP;
    TextBox->bitMapInfo.bmiHeader.biWidth = width;
    TextBox->bitMapInfo.bmiHeader.biHeight = height;
    TextBox->bitMapInfo.bmiHeader.biCompression = 0;
    TextBox->bitMapInfo.bmiHeader.biPlanes = 1;
    TextBox->bitMapInfo.bmiHeader.biSizeImage = TextBox->bitMapInfo.bmiHeader.biWidth*TextBox->bitMapInfo.bmiHeader.biHeight*4;

    try {

    TextBox->memory = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,TextBox->bitMapInfo.bmiHeader.biSizeImage);
    
    if (TextBox->memory == NULL){
        Error = ERROR_NOT_ENOUGH_MEMORY;
        return Error;
    }

    int32_t Memory_Offset = 0;
    
    PIXEL Background;
    Background.alpha = 255;
    Background.red = 32;
    Background.green = 32;
    Background.blue = 32;

    PIXEL Border;
    Border.alpha = 255;
    Border.red = 251;
    Border.green = 150;
    Border.blue = 106;

    PIXEL BorderInnerUpRight;
    BorderInnerUpRight.alpha = 255;
    BorderInnerUpRight.red = 255;
    BorderInnerUpRight.green = 166;
    BorderInnerUpRight.blue = 131;


    PIXEL BorderInnerDownLeft;
    BorderInnerDownLeft.alpha = 255;
    BorderInnerDownLeft.red = 231;
    BorderInnerDownLeft.green = 130;
    BorderInnerDownLeft.blue = 86;

    PIXEL DeadPixel = {0};


    

    memset(TextBox->memory,255,TextBox->bitMapInfo.bmiHeader.biSizeImage);
    

    for(int32_t PixelY = 0; PixelY < TextBox->bitMapInfo.bmiHeader.biHeight; PixelY++){
            
            for(int32_t PixelX =0 ; PixelX < TextBox->bitMapInfo.bmiHeader.biWidth; PixelX++){

                if(PixelX == 0 || PixelX == (width-1)){
                    if(PixelY == 0|| PixelY == (height-1) || PixelY == 1|| PixelY == (height-2)){
                    memcpy((PIXEL*)((BYTE*)TextBox->memory + Memory_Offset), &DeadPixel, sizeof(PIXEL));
                    }
                    else{
                    memcpy((PIXEL*)((BYTE*)TextBox->memory + Memory_Offset), &Border, sizeof(PIXEL));
                    }
                }
                else if (PixelX == 1|| PixelX == (width-2)){
                     if (PixelY == 0 || PixelY == (height-1)){
                         memcpy((PIXEL*)((BYTE*)TextBox->memory + Memory_Offset), &DeadPixel, sizeof(PIXEL));      
                     }
                     else{
                        memcpy((PIXEL*)((BYTE*)TextBox->memory + Memory_Offset), &Border, sizeof(PIXEL));
                     }
                     
                }
                else if (PixelX == 2 || PixelX == (width-3)){
                    memcpy((PIXEL*)((BYTE*)TextBox->memory + Memory_Offset), &Border, sizeof(PIXEL));
                }
                
                else if(PixelY == 0|| PixelY == (height-1)){
                    memcpy((PIXEL*)((BYTE*)TextBox->memory + Memory_Offset), &Border, sizeof(PIXEL));

                }
                
                else if(PixelY == 1 || PixelY == (height-2)){
                    memcpy((PIXEL*)((BYTE*)TextBox->memory + Memory_Offset), &Border, sizeof(PIXEL));
                }
                
                else{

                    if(PixelY == 2 || PixelX == (width-4)){
                        memcpy((PIXEL*)((BYTE*)TextBox->memory + Memory_Offset), &BorderInnerUpRight, sizeof(PIXEL));
                    }
                    else if(PixelY == (height-3) || PixelX == 3){
                        memcpy((PIXEL*)((BYTE*)TextBox->memory + Memory_Offset), &BorderInnerDownLeft, sizeof(PIXEL));
                    }
                    else{
                        memcpy((PIXEL*)((BYTE*)TextBox->memory + Memory_Offset), &Background, sizeof(PIXEL));
                    }
                    
                }


                Memory_Offset += 4;
               

            
        }
        
    }

    }
    catch(const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        Error = GetLastError();
    }

    return Error;
    
    
    
}

