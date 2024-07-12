#include "main.h"

class Overworld{
private:
    int32_t Map;
    
    LPCSTR Path;
    LPCSTR FloorPlanPath;
    
    GAMEBITMAP Sprites[6];
    GAMEBITMAP FloorPlan;

public:
    Overworld() {}   
    Overworld(int32_t Map){
        this->Map = Map;
        this->Path = Path;
    }

    VOID SetFloorPlanPath(LPCSTR FloorPlan){
        this->FloorPlanPath = FloorPlan;
    }
    
    LPCSTR GetFloorPlanPath(){
        return FloorPlanPath;
    }
    
    VOID SetFloorPlan(GAMEBITMAP FloorPlan){
        this->FloorPlan = FloorPlan;
    }

    GAMEBITMAP GetFloorPlan(){
        return FloorPlan;
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


};