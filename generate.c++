#include "main.h"

DWORD ProceduralGenerator(int32_t NumberOfRooms,int32_t MinRoomSize, int32_t MaxRoomSize, TILE *Background_Tiles, TILE* Tile_Type_Array){
    return 0;
}


/*
TODO:
- Create a method to track which tiles belong to a given room
- Create a function to check if 2 rooms are touching
- Modify GenerateRoom to check if 2 rooms are touching or within 3 tiles of each other and regenerate.

*/

VOID GenerateRoom(int32_t NumberOfRooms, int32_t MinRoomSize, int32_t MaxRoomSize, TILE *Background_Tiles, TILE* Tile_Type_Array){
    
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<int> distribution(MinRoomSize, MaxRoomSize);

    std::uniform_int_distribution<int> distribution_Tile(NUMB_TILES_PER_ROW*3, NUMB_TILES - (NUMB_TILES_PER_ROW*3));
    
    ROOM Room_Array[100];
    int32_t Room_Size,Room_Location,Row_Index, Room_Size_Edge;
    
    // Track what tiles belong to Rooms
    int32_t Room_Tiles_Indexes[NUMB_TILES];
    int32_t Room_Tiles_Index_Pointer = 0;

    for (int i = 0; i < NumberOfRooms; i++){
        ROOM Room = {0};
        
        
        Room.Width = distribution(gen);
        Room.Height = distribution(gen);
        Room.Starting_Tile = distribution_Tile(gen);

         while (!IsRoomValid(Room,Room_Tiles_Indexes,Room_Tiles_Index_Pointer)){
            Room.Width = distribution(gen);
            Room.Height = distribution(gen);
            Room.Starting_Tile = distribution_Tile(gen);
        }

        Room_Array[i] = Room;
        Room_Tiles_Index_Pointer = DrawRoomToMap(Room,Background_Tiles,Tile_Type_Array,Room_Tiles_Indexes,Room_Tiles_Index_Pointer);
    
    }
    printf("number of tiles: %d\n",Room_Tiles_Index_Pointer);
    
}

int32_t DrawRoomToMap(ROOM Room,TILE *Background_Tiles, TILE* Tile_Type_Array, int32_t* Room_Tiles_Indexes, int32_t Room_Tiles_Index_Pointer){
        
        int32_t tile_index;
        
        for(int y = 0; y < Room.Height; y++){
            for (int x = 0; x < Room.Width; x++){
                
                /* Add each tile index to the list of all tile indexs contained in rooms */
                tile_index = Room.Starting_Tile + y*NUMB_TILES_PER_ROW + x;
                Room_Tiles_Indexes[Room_Tiles_Index_Pointer] = tile_index;
                Room_Tiles_Index_Pointer++;
                
                if (y == 0){
                    if (x == 0){
                        Background_Tiles[tile_index] = Tile_Type_Array[WALL5];
                    }
                    else if (x == Room.Width-1){
                        Background_Tiles[tile_index] = Tile_Type_Array[WALL4];
                    }
                    else{
                        Background_Tiles[tile_index] = Tile_Type_Array[WALL3];
                    }
                    
                }
                else if (y == Room.Height-1){
                    if (x == 0){
                        Background_Tiles[tile_index] = Tile_Type_Array[WALL8];
                    }
                    else if (x == Room.Width-1){
                        Background_Tiles[tile_index] = Tile_Type_Array[WALL7];
                    }
                    else{
                        Background_Tiles[tile_index] = Tile_Type_Array[WALL6];
                    }

                }

                else{
                    if (x == 0){
                        Background_Tiles[tile_index] = Tile_Type_Array[WALL9];
                    }
                    else if (x == Room.Width-1){
                        Background_Tiles[tile_index] = Tile_Type_Array[WALL10];
                    }
                    else{
                        Background_Tiles[tile_index] = Tile_Type_Array[FLOOR1];
                    }
                    
                }
                       
           
        }
        
    }
    return Room_Tiles_Index_Pointer;
    
}




BOOL IsRoomValid(ROOM Room, int32_t* Room_Tiles_Indexes,int32_t Room_Tiles_Index_Pointer){
    int32_t Room_Row_Index = Room.Starting_Tile / NUMB_TILES_PER_ROW;
    int32_t Room_Edge_Row_Index = (Room.Starting_Tile +Room.Width+3) / NUMB_TILES_PER_ROW;
    int32_t Tile_Index, Room_Tile_Index;
    int32_t Top_Left, Top_Right, Bottom_Left,Bottom_Right;
    Top_Left = Room.Starting_Tile;
    Top_Right = Room.Starting_Tile + Room.Width + 1;
    Bottom_Left = Room.Starting_Tile + NUMB_TILES_PER_ROW*Room.Height + 1;
    Bottom_Right = NUMB_TILES_PER_ROW*Room.Height + Room.Width + 1;
    
    // Check Location + Size fits within the Tile area
    if ( (Room.Starting_Tile + (NUMB_TILES_PER_ROW*Room.Height + Room.Width)) >= (NUMB_TILES - (NUMB_TILES_PER_ROW*2))){
        return FALSE;
    }
    
    // Check if the Room doesn't overflow onto other side of X axis
    if (Room_Edge_Row_Index != Room_Row_Index){
        return FALSE;
    }
    // Check if the room isn't touching another room 
    for (int i = 0; i < Room_Tiles_Index_Pointer; i++){
        Tile_Index = Room_Tiles_Indexes[i];

        for(int y = 0; y < Room.Height; y++){
            for (int x = 0; x < Room.Width; x++){
                Room_Tile_Index = Room.Starting_Tile + y*NUMB_TILES_PER_ROW + x;
                if (Tile_Index == Room_Tile_Index){
                    return false;
                }
            }
        }

      
    } 

    return true;
    // Check if room isn't with 3 tiles of another room
    // Returns True is all above conditions are met 
}
