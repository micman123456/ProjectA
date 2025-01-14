#include "main.h"

class Overworld{
private:
    int32_t Map;
    
    LPCSTR Path;
    LPCSTR CollisionsMapPath;
    
    GAMEBITMAP Sprites[6];
    GAMEBITMAP CollisionsMap;

    int32_t BorderLeft;
    int32_t BorderRight;
    int32_t BorderUp;
    int32_t BorderDown;

public:
    Overworld() {}   
    Overworld(
    
    int32_t Map, 
    int32_t BorderLeft,
    int32_t BorderRight,
    int32_t BorderUp,
    int32_t BorderDown
    
    ){
        this->Map = Map;
        this->BorderDown = BorderDown;
        this->BorderLeft = BorderLeft;
        this->BorderUp = BorderUp;
        this->BorderRight = BorderRight;
    }

    VOID SetCollisionsMapPath(LPCSTR FloorPlan){
        this->CollisionsMapPath = FloorPlan;
    }
    
    LPCSTR GetCollisionsMapPath(){
        return CollisionsMapPath;
    }
    
    VOID SetCollisionsMap(GAMEBITMAP CollisionsMap){
        this->CollisionsMap = CollisionsMap;
    }

    GAMEBITMAP GetCollisionsMap(){
        return CollisionsMap;
    }
    
    VOID SetSprite(GAMEBITMAP Sprite, int32_t index){
        this->Sprites[index] = Sprite;
    }
    GAMEBITMAP GetSprite(int32_t index){
        return this->Sprites[index];
    }
    LPCSTR GetPath(){
        return Path;
    }

    int32_t GetBorder(int32_t direction)
    {
        switch (direction)
        {
        case DIR_DOWN:
            return BorderDown;
            break;
        
        case DIR_LEFT:
            return BorderLeft;
            break;
        
        case DIR_RIGHT:
            return BorderRight;
            break;
        
        case DIR_UP:
            return BorderUp;
            break;
        
        default:
            //ASSERT FALSE
            break;
        }
    }


};