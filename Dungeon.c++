#include "main.h"

class Dungeon {
private:
    LPSTR Name;
    LPCSTR Path;
    int32_t NumberOfFloors;
    int32_t CurrentFloor;

public:

    Dungeon() {}
    Dungeon(LPSTR Name, LPCSTR Path, int32_t NumberOfFloors) {
        this->Path = Path;
        this->NumberOfFloors = NumberOfFloors;
        this->Name = Name;
        CurrentFloor = 1;
    }

   
    LPSTR getName() const {
        return Name;
    }

    
    LPCSTR getPath() const {
        return Path;
    }

    
    int32_t getNumberOfFloors() const {
        return NumberOfFloors;
    }

    
    int32_t getCurrentFloor() const {
        return CurrentFloor;
    }

    VOID NextFloor(){
        CurrentFloor++;
    }

    VOID Reset(){
        CurrentFloor = 1;
    }

    LPSTR getNameWithCurrentFloor() const {
        
        char buffer[100]; 
        if (CurrentFloor > NumberOfFloors){
            sprintf(buffer, "%s COMPLETED", Name);
        }
        else{
        sprintf(buffer, "%s BF%d", Name, CurrentFloor);
        }
        LPSTR result = new char[strlen(buffer) + 1];
        strcpy(result, buffer);
        return result;
    }

    
};
