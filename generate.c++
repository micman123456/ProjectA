
#include "main.h"


std::random_device rd;
std::mt19937 gen(rd());

std::vector<ROOM> Room_Vector(0);
std::vector<ROOM> Room_Vector_Copy(0);
int32_t Room_Count = 0;



PATH Path_Array[100];
int32_t Path_Count;

int32_t Room_Tiles_Indexes[NUMB_TILES];
int32_t Room_Tiles_Index_Pointer = 0;


VOID DrawTile(int32_t Background_Index, int32_t Tile_Type, TILE *Background_Tiles, TILE* Tile_Type_Array){
    if (Background_Tiles[Background_Index].visited == 0){
        Background_Tiles[Background_Index] = Tile_Type_Array[Tile_Type];
        Background_Tiles[Background_Index].visited = 1;
    }
    
}

VOID DrawTileOverride(int32_t Background_Index, int32_t Tile_Type, TILE *Background_Tiles, TILE* Tile_Type_Array){
    Background_Tiles[Background_Index] = Tile_Type_Array[Tile_Type];
    Background_Tiles[Background_Index].visited = 1;
}

int32_t ProceduralGenerator(int32_t RoomAttempts, int32_t MinRoomSize, int32_t MaxRoomSize, TILE *Background_Tiles, TILE* Tile_Type_Array, int32_t Corridors){
    try {
        std::ofstream logFile("Procedural_generation_log.txt");
        if (!logFile.is_open()) {
            throw std::runtime_error("Error opening log file.");
        }

    GenerateRoomsAttempts(RoomAttempts,MinRoomSize,MaxRoomSize,Background_Tiles,Tile_Type_Array);



    if(Room_Vector.size() % 2 != 0){
        Room_Vector.pop_back();
    }
    Room_Vector_Copy = Room_Vector;
    int32_t stairTile = GenerateStairTile(Background_Tiles);
    logFile << "Tile Count" << NUMB_TILES << std::endl;
    logFile << "Rooms Generated Successfully: " << Room_Vector.size() << std::endl;
    logFile << "Stairs Generated Successfully: " << stairTile << std::endl;
    
    

    ROOM Room1 = Room_Vector[rand() % Room_Vector.size()];
    ROOM Room2 = Room_Vector[rand() % Room_Vector.size()];



    int attempts = 0;
    while(true){

        logFile << "Rooms Selected " << " Room1 Start: " << Room1.Starting_Tile << " Room2 Start "  << Room2.Starting_Tile << std::endl;

        if(GenerateConnectingPathsImproved(Room1,Room2,Background_Tiles,Tile_Type_Array)){
            logFile << "Path created from " << Room1.Starting_Tile << " to " << Room2.Starting_Tile << std::endl;
            logFile << "Removing Rooms from vector" << std::endl;
            RemoveRoom(Room_Vector,Room1.Starting_Tile);
            RemoveRoom(Room_Vector,Room2.Starting_Tile);
            logFile << "Rooms remaining: " << Room_Vector.size() << std::endl;
        }
        if (Room_Vector.size() == 0){
            break;
        }
        Room1 = Room_Vector[rand() % Room_Vector.size()];
        Room2 = Room_Vector[rand() % Room_Vector.size()];
       attempts++;
    }
    logFile << "All Paths generated " << std::endl;
    
    DrawTileMap(Background_Tiles,Tile_Type_Array);
    DrawTileMap(Background_Tiles,Tile_Type_Array);
    DrawTileMap(Background_Tiles,Tile_Type_Array);



    DrawTileDetails(Background_Tiles,Tile_Type_Array);
    DrawTileOverride(stairTile,STAIRS,Background_Tiles,Tile_Type_Array);
    return stairTile;

    }catch(const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

    
    
    
}

VOID DrawTileMap(TILE *Background_Tiles,TILE* Tile_Type_Array){
    // First we draw the external wall tiles to be WALL_FULL.

    for (int i = 0; i<NUMB_TILES;i++){

    if (i%NUMB_TILES_PER_ROW==(NUMB_TILES_PER_ROW-1) || i%NUMB_TILES_PER_ROW==1 || i%NUMB_TILES_PER_ROW==0 || i%NUMB_TILES_PER_ROW==2){  
        DrawTileOverride(i,WALL_FULL,Background_Tiles,Tile_Type_Array);
    }
    else if (i < NUMB_TILES_PER_ROW*2 || i >NUMB_TILES -(NUMB_TILES_PER_ROW*2)){
        DrawTileOverride(i,WALL_FULL,Background_Tiles,Tile_Type_Array);    
    }

   }

   for (int i = NUMB_TILES_PER_ROW*2; i < (NUMB_TILES-NUMB_TILES_PER_ROW*2); i++){


        // Ignores Tiles on the edges // 

        if (i%NUMB_TILES_PER_ROW==(NUMB_TILES_PER_ROW-1) || i%NUMB_TILES_PER_ROW==1 || i%NUMB_TILES_PER_ROW==0 || i%NUMB_TILES_PER_ROW==2){  
            continue;
        }
        
        int32_t Tile_Type = GetFloorType(Background_Tiles,i);
        DrawTileOverride(i,Tile_Type,Background_Tiles,Tile_Type_Array);

   }
    

}

VOID DrawTileDetails(TILE *Background_Tiles,TILE* Tile_Type_Array){

   for (int i = NUMB_TILES_PER_ROW*2; i < (NUMB_TILES-NUMB_TILES_PER_ROW*2); i++){


        // Ignores Tiles on the edges // 

        if (i%NUMB_TILES_PER_ROW==(NUMB_TILES_PER_ROW-1) || i%NUMB_TILES_PER_ROW==1 || i%NUMB_TILES_PER_ROW==0 || i%NUMB_TILES_PER_ROW==2){  
            continue;
        }
        
        
        int32_t Tile_Type = GetFloorDetail(Background_Tiles,i);
        DrawTileOverride(i,Tile_Type,Background_Tiles,Tile_Type_Array);

   }
}

int32_t GenerateStairTile(TILE* Background_Tiles){
    ROOM Room1 = Room_Vector[rand() % Room_Vector.size()];
    int16_t randomX = (rand() % Room1.Width-1) + 1;
    int16_t randomY = (rand() % Room1.Height-1) + 1;
    int32_t stairTile = Room1.Starting_Tile + randomX + randomY*NUMB_TILES_PER_ROW;

    while (TRUE)
    {
        if(Background_Tiles[stairTile].type == FLOOR1){
            break;
        }
        randomX = (rand() % Room1.Width-1) + 1;
        randomY = (rand() % Room1.Height-1) + 1;
        stairTile = Room1.Starting_Tile + randomX + randomY*NUMB_TILES_PER_ROW;
    }
    


    return stairTile;

    
}


VOID GenerateRoomsAttempts(int32_t Attempts, int32_t MinRoomSize, int32_t MaxRoomSize, TILE *Background_Tiles, TILE* Tile_Type_Array) {
    try {
        std::ofstream logFile("room_generation_log.txt");
        if (!logFile.is_open()) {
            throw std::runtime_error("Error opening log file.");
        }

        for (int i = 0; i < Attempts; i++) {
            ROOM Room = {0};
            Room.Width = rand() % (MaxRoomSize - MinRoomSize + 1) + MinRoomSize;
            Room.Height = rand() % (MaxRoomSize - MinRoomSize + 1) + MinRoomSize;
            Room.Starting_Tile = rand() % (NUMB_TILES - (NUMB_TILES_PER_ROW * 3)) + (NUMB_TILES_PER_ROW * 3);

            logFile << "Attempted Room " << i+1 << " - Width: " << Room.Width << ", Height: " << Room.Height << ", Starting Tile: " << Room.Starting_Tile << std::endl;

            if (IsRoomValid(Room, Room_Tiles_Indexes, Room_Tiles_Index_Pointer)) {
                
                Room_Vector.push_back(Room);
                Room_Tiles_Index_Pointer = DrawRoomToMap(Room, Background_Tiles, Tile_Type_Array, Room_Tiles_Indexes, Room_Tiles_Index_Pointer);
                Room_Count++;
                logFile << "Room " << Room_Count << " successfully generated." << std::endl;
            } else {
                logFile << "Attempted Room " << i+1 << " failed." << std::endl;
            }
        }

        logFile << "Number of tiles: " << Room_Tiles_Index_Pointer << "\nNumber of rooms: " << Room_Count << std::endl;
    } catch(const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
}

BOOL isPlayerInRoom(PLAYER Player) {
    // Calculate player's (x, y) position
    int32_t PlayerX = Player.StandingTile_Index % NUMB_TILES_PER_ROW;
    int32_t PlayerY =  Player.StandingTile_Index / NUMB_TILES_PER_ROW;

    // Iterate through each room
    for (const auto& currentRoom : Room_Vector_Copy) {
        // Check if player is within the bounds of the room
        if (PlayerX >= (currentRoom.Starting_Tile%NUMB_TILES_PER_ROW + 1) && PlayerX < ((currentRoom.Starting_Tile%NUMB_TILES_PER_ROW) + currentRoom.Width - 1) &&
            PlayerY >= (currentRoom.Starting_Tile/NUMB_TILES_PER_ROW + 1) && PlayerY < ((currentRoom.Starting_Tile/NUMB_TILES_PER_ROW) + currentRoom.Height - 1) ) {
            return TRUE; 
        }
    }

    return FALSE; 
}



int32_t DrawRoomToMap(ROOM Room,TILE *Background_Tiles, TILE* Tile_Type_Array, int32_t* Room_Tiles_Indexes, int32_t Room_Tiles_Index_Pointer){
        
        int32_t tile_index, Path_Start;
        int32_t Index = 0;
    
        
        for(int y = 0; y < Room.Height; y++){
            for (int x = 0; x < Room.Width; x++){
                
                /* Add each tile index to the list of all tile indexs contained in rooms */
                tile_index = Room.Starting_Tile + y*NUMB_TILES_PER_ROW + x;
                Room_Tiles_Indexes[Room_Tiles_Index_Pointer] = tile_index;
                Room_Tiles_Index_Pointer++;
                
                if (y == 0){
                    if (x == 0){
                        DrawTile(tile_index,WALL_FULL,Background_Tiles,Tile_Type_Array);
                        //Background_Tiles[tile_index] = Tile_Type_Array[WALL5];
                    }
                    else if (x == Room.Width-1){
                        DrawTile(tile_index,WALL_FULL,Background_Tiles,Tile_Type_Array);
                        //Background_Tiles[tile_index] = Tile_Type_Array[WALL4];
                    }
                    else{
                        DrawTile(tile_index,WALL_FULL,Background_Tiles,Tile_Type_Array);
                        //Background_Tiles[tile_index] = Tile_Type_Array[WALL3];
                    }
                    
                }
                else if (y == Room.Height-1){
                    if (x == 0){
                        DrawTile(tile_index,WALL_FULL,Background_Tiles,Tile_Type_Array);
                        //Background_Tiles[tile_index] = Tile_Type_Array[WALL8];
                    }
                    else if (x == Room.Width-1){
                        DrawTile(tile_index,WALL_FULL,Background_Tiles,Tile_Type_Array);
                        //Background_Tiles[tile_index] = Tile_Type_Array[WALL7];
                    }
                    else{
                        DrawTile(tile_index,WALL_FULL,Background_Tiles,Tile_Type_Array);
                        //Background_Tiles[tile_index] = Tile_Type_Array[WALL6];
                    }

                }

                else{
                    if (x == 0){
                        DrawTile(tile_index,WALL_FULL,Background_Tiles,Tile_Type_Array);
                        //Background_Tiles[tile_index] = Tile_Type_Array[WALL9];
                    }
                    else if (x == Room.Width-1){
                        DrawTile(tile_index,WALL_FULL,Background_Tiles,Tile_Type_Array);
                        //Background_Tiles[tile_index] = Tile_Type_Array[WALL10];
                    }
                    else{
                        DrawTile(tile_index,FLOOR1,Background_Tiles,Tile_Type_Array);
                        //Room.Tiles[Index] = tile_index;
                        //Background_Tiles[tile_index] = Tile_Type_Array[FLOOR1];
                    }
                    
                }
                       
           
        }
        
    }
    return Room_Tiles_Index_Pointer;
    
}




BOOL IsRoomValid(ROOM Room, int32_t* Room_Tiles_Indexes,int32_t Room_Tiles_Index_Pointer){
    int32_t Room_Row_Index = Room.Starting_Tile / NUMB_TILES_PER_ROW;
    int32_t Room_Edge_Row_Index = (Room.Starting_Tile + (Room.Width + 2)) / NUMB_TILES_PER_ROW;

    /*
        int32_t Room_Row_Index = (int32_t)(floor((double)(Room.Starting_Tile / (double)NUMB_TILES_PER_ROW)));
        int32_t Room_Edge_Row_Index = (int32_t)(floor((double)(Room.Starting_Tile + Room.Width+3) / (double)NUMB_TILES_PER_ROW));
    */

    int32_t Tile_Index, Room_Tile_Index;
    int32_t Top_Left, Top_Right, Bottom_Left,Bottom_Right;

    if (Room.Starting_Tile%NUMB_TILES_PER_ROW==(NUMB_TILES_PER_ROW-1) || Room.Starting_Tile%NUMB_TILES_PER_ROW==1 || Room.Starting_Tile%NUMB_TILES_PER_ROW==0 || Room.Starting_Tile%NUMB_TILES_PER_ROW==2){
         return FALSE;
    }

    
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
                    return FALSE;
                }
            }
        }

    }

    // Check if room isn't with 3 tiles of another room 

    int32_t TopLeft = Room.Starting_Tile - 3;
    int32_t TopRight = Room.Starting_Tile+Room.Width + 3;

    int32_t TopLeftUp = Room.Starting_Tile - 3*NUMB_TILES_PER_ROW;
    int32_t TopRightUp = Room.Starting_Tile+Room.Width - 3*NUMB_TILES_PER_ROW;

    int32_t BotLeft = Room.Starting_Tile*Room.Height*NUMB_TILES_PER_ROW - 3;
    int32_t BotRight = Room.Starting_Tile*Room.Height*NUMB_TILES_PER_ROW + Room.Width + 3;

    int32_t BotLeftDown = Room.Starting_Tile*Room.Height*NUMB_TILES_PER_ROW + 3*NUMB_TILES_PER_ROW;
    int32_t BotRightDown = Room.Starting_Tile*Room.Height*NUMB_TILES_PER_ROW + Room.Width + 3*NUMB_TILES_PER_ROW;




     // Check if the room isn't within 3 tiles of another room
    for (int i = 0; i < Room_Tiles_Index_Pointer; i++) {
        Tile_Index = Room_Tiles_Indexes[i];
        if(Tile_Index == TopLeft || Tile_Index == TopRight || Tile_Index == TopLeftUp || Tile_Index == TopRightUp){
            return false;
        }
        if(Tile_Index == BotLeft || Tile_Index == BotRight || Tile_Index == BotLeftDown || Tile_Index == BotRightDown){
            return false;
        }
    }



    // Returns True is all above conditions are met
    return true;
    
    
 
}







VOID GenerateConnectingPaths(int32_t Corridors, TILE *Background_Tiles, TILE* Tile_Type_Array){
    try {
        std::ofstream logFile("path_generation_log.txt");
        if (!logFile.is_open()) {
            throw std::runtime_error("Error opening log file.");
        }
    
    
    for (int k = 0; k < Corridors; k++){

    // Generates a random room Tile //

    
    int32_t Left_Tile = Room_Tiles_Indexes[rand() % (Room_Tiles_Index_Pointer + 1)];
    int32_t Right_Tile = Room_Tiles_Indexes[rand() % (Room_Tiles_Index_Pointer + 1)];

    // Loop until leftmost tile is smaller then rightmost tile //

    while(TRUE){
           if((Left_Tile % NUMB_TILES_PER_ROW) < (Right_Tile % NUMB_TILES_PER_ROW)){
            if( abs(Left_Tile/NUMB_TILES_PER_ROW - Right_Tile/NUMB_TILES_PER_ROW) < 20){
                if((Right_Tile % NUMB_TILES_PER_ROW) - (Left_Tile % NUMB_TILES_PER_ROW) < 40){
                    break;
                }
            }
           }
           Left_Tile = Room_Tiles_Indexes[rand() % (Room_Tiles_Index_Pointer + 1)];
           Right_Tile = Room_Tiles_Indexes[rand() % (Room_Tiles_Index_Pointer + 1)];
      }



    // Calculate the Vertical Distance and Direction Down - 0 Up - 1

    int32_t Left_Tile_Row_Index = Left_Tile / NUMB_TILES_PER_ROW;
    int32_t Right_Tile_Row_Index = Right_Tile / NUMB_TILES_PER_ROW;
    int32_t Vertical_Dir = (Left_Tile_Row_Index > Right_Tile_Row_Index) ? 0 : 1;
    int32_t VDistance = abs(Right_Tile_Row_Index - Left_Tile_Row_Index);

    // Set our starting tile to be the leftmost tile
    int32_t Tile = Left_Tile;

    // Calculate Horizontal Distance
    int32_t HDistance = (Right_Tile % NUMB_TILES_PER_ROW) - (Left_Tile% NUMB_TILES_PER_ROW);
    

    logFile << "Path Started " << k+1 << " Start: " << Left_Tile << ", End: " << Right_Tile  << " Direction: " << ((Vertical_Dir==0) ? "Down" : "Up") << std::endl;
    logFile << "Path " << k+1 << " Starting HDistance " << HDistance << ", Starting VDistance " << VDistance << " Starting Row Index " << Left_Tile_Row_Index << " Ending Row index " << Right_Tile_Row_Index  << std::endl;
    
    BOOL Continue = TRUE;
    int32_t Attempts = 0;

    while (Continue)
    {

        // Generate a random distance to move between 0 and the remaining distance (inclusive)
        int32_t Move_Horizontal = rand() % (HDistance+1) % 11;
        int32_t Move_Vertical = rand() % (VDistance+1) % 11;

        // Spin until (Left tile MOD NUMB_TILES_PER_ROW) + Move_Horizontal doesn't exceed the (right tile MOD NUMB_TILES_PER_ROW)
        while((Tile%NUMB_TILES_PER_ROW) + Move_Horizontal > (Right_Tile%NUMB_TILES_PER_ROW)){

            Move_Horizontal = rand() % (HDistance+1) % 11;
            Attempts++;
            if (Attempts > 1000){
                printf("First While Loop");
                Attempts = 0;
                break;
            }

        }
        
        // Spin until (Left tile / NUMB_TILES_PER_ROW) +- Move_Vertical doesn't exceed the (right tile / NUMB_TILES_PER_ROW)
        
        
        switch (Vertical_Dir)
        {
        case 0:
            while(TRUE){
            if((Tile/NUMB_TILES_PER_ROW) - Move_Vertical >= (Right_Tile/NUMB_TILES_PER_ROW)){
                break;
            }
            else if (VDistance<3){
                    Move_Vertical = VDistance;
                    break;
                }
            Move_Vertical = rand() % (VDistance+1) % 11;
             Attempts++;
            if (Attempts > 1000){
                //printf("2nd While Loop\n");
                //printf("%d - %d < %d\n",Tile/NUMB_TILES_PER_ROW,Move_Vertical,(Right_Tile/NUMB_TILES_PER_ROW));
                Attempts = 0;
                break;
            }
            }
            break;
        
        default:
            while(TRUE){
                
                if( (Tile%NUMB_TILES_PER_ROW) + Move_Vertical <= (Right_Tile/NUMB_TILES_PER_ROW)){
                    break;
                }
                else if (VDistance<3){
                    Move_Vertical = VDistance;
                    break;
                }
                Move_Vertical = rand() % (VDistance+1) % 11;
                 Attempts++;
            if (Attempts > 1000){
                //printf("3rd While Loop\n");
                //printf("%d + %d > %d\n",Tile/NUMB_TILES_PER_ROW,Move_Vertical,(Right_Tile/NUMB_TILES_PER_ROW));
                Attempts = 0;
                break;
            }
            }
            break;
        }

        // Use a for loop to draw the tiles in the H direction
        for (int i = 0; i < Move_Horizontal; i++) {
            DrawTileOverride(Tile,FLOOR1,Background_Tiles,Tile_Type_Array);
            Tile++;
            }
        
        // Use a for loop to draw the tiles in the V direction
        for (int i = 0; i < Move_Vertical; i++) {
                DrawTileOverride(Tile,FLOOR1,Background_Tiles,Tile_Type_Array);
                if (Vertical_Dir == 1){
                    Tile += NUMB_TILES_PER_ROW;
                }
                else{
                    Tile -= NUMB_TILES_PER_ROW;
                }
    }
    // Update the HDistance and VDistance
    HDistance -= Move_Horizontal;
    VDistance -= Move_Vertical;

    if (HDistance == 0 && VDistance == 0){
        Continue = FALSE;
    }
    
    }
    

     logFile << "Path Finished " << k+1 << " HDIS: " << HDistance << ", VDIS: " << VDistance << std::endl;


    }

    }catch(const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
    
    

}

BOOL GenerateConnectingPathsImproved(ROOM Room1, ROOM Room2, TILE *Background_Tiles, TILE* Tile_Type_Array){
    try {
        std::ofstream logFile("path_generation_log.txt");
        if (!logFile.is_open()) {
            throw std::runtime_error("Error opening log file.");
        }

    if(Room1.Starting_Tile == Room2.Starting_Tile){
        return FALSE;
    }

    if(Room1.Starting_Tile%NUMB_TILES_PER_ROW > Room2.Starting_Tile%NUMB_TILES_PER_ROW){
        return FALSE;
    }

    logFile << "Rooms Selected " << " Room1 Start: " << Room1.Starting_Tile << " Room2 Start "  << Room2.Starting_Tile << std::endl;
    
    int32_t Room1Tiles[MAX_ROOM_SIZE*MAX_ROOM_SIZE];
    int32_t Room2Tiles[MAX_ROOM_SIZE*MAX_ROOM_SIZE];
    int32_t RoomTilePointer = 0;


    // Fetches all the tiles in Room1 and stores into array //
    for(int y = 1; y < Room1.Height-1; y++){
        for(int x = 1; x < Room1.Width-1; x++){
            Room1Tiles[RoomTilePointer] = Room1.Starting_Tile + x + y*NUMB_TILES_PER_ROW;
            RoomTilePointer++;
        }
    }

    RoomTilePointer = 0;

    // Fetches all the tiles in Room2 and stores into array //
     for(int y = 1; y < Room2.Height-1; y++){
        for(int x = 1; x < Room2.Width-1; x++){
            Room2Tiles[RoomTilePointer] = Room2.Starting_Tile + x + y*NUMB_TILES_PER_ROW;
            RoomTilePointer++;
        }
    }
    
    
    // int32_t Left_Tile = Room1Tiles[rand() % (Room1.Width*Room1.Height)];
    // int32_t Right_Tile = Room2Tiles[rand() % (Room2.Width*Room2.Height)];

    int32_t Left_Tile = Room1.Starting_Tile + (Room1.Width/2) + (Room1.Height/2)*NUMB_TILES_PER_ROW;
    int32_t Right_Tile = Room2.Starting_Tile + (Room2.Width/2) + (Room2.Height/2)*NUMB_TILES_PER_ROW;

    
    while(TRUE){
        if((Left_Tile % NUMB_TILES_PER_ROW) < (Right_Tile % NUMB_TILES_PER_ROW)){
            break;
    }
    
    Left_Tile = Room1Tiles[rand() % ((Room1.Width-1)*(Room1.Height-1)-1)];
    Right_Tile = Room2Tiles[rand() % ((Room2.Width-1)*(Room2.Height-1)-1)];
    
    }


    // Calculate the Vertical Distance and Direction Down - 0 Up - 1

    int32_t Left_Tile_Row_Index = Left_Tile / NUMB_TILES_PER_ROW;
    int32_t Right_Tile_Row_Index = Right_Tile / NUMB_TILES_PER_ROW;
    int32_t Vertical_Dir = (Left_Tile_Row_Index > Right_Tile_Row_Index) ? 0 : 1;
    int32_t VDistance = abs(Right_Tile_Row_Index - Left_Tile_Row_Index);

    // Set our starting tile to be the leftmost tile
    int32_t Tile = Left_Tile;

    // Calculate Horizontal Distance
    int32_t HDistance = (Right_Tile % NUMB_TILES_PER_ROW) - (Left_Tile% NUMB_TILES_PER_ROW);
    
    
    BOOL Continue = TRUE;
    int32_t Attempts = 0;

        while (Continue)
    {

        // Generate a random distance to move between 0 and the remaining distance (inclusive)
        int32_t Move_Horizontal = rand() % (HDistance+1) % 11;
        int32_t Move_Vertical = rand() % (VDistance+1) % 11;

        // Spin until (Left tile MOD NUMB_TILES_PER_ROW) + Move_Horizontal doesn't exceed the (right tile MOD NUMB_TILES_PER_ROW)
        while((Tile%NUMB_TILES_PER_ROW) + Move_Horizontal > (Right_Tile%NUMB_TILES_PER_ROW)){

            Move_Horizontal = rand() % (HDistance+1) % 11;
            Attempts++;
            if (Attempts > 1000){
                printf("First While Loop");
                Attempts = 0;
                break;
            }

        }
        
        // Spin until (Left tile / NUMB_TILES_PER_ROW) +- Move_Vertical doesn't exceed the (right tile / NUMB_TILES_PER_ROW)
        
        
        switch (Vertical_Dir)
        {
        case 0:
            while(TRUE){
            if((Tile/NUMB_TILES_PER_ROW) - Move_Vertical >= (Right_Tile/NUMB_TILES_PER_ROW)){
                break;
            }
            else if (VDistance<3){
                    Move_Vertical = VDistance;
                    break;
                }
            Move_Vertical = rand() % (VDistance+1) % 11;
             Attempts++;
            if (Attempts > 1000){
                //printf("2nd While Loop\n");
                //printf("%d - %d < %d\n",Tile/NUMB_TILES_PER_ROW,Move_Vertical,(Right_Tile/NUMB_TILES_PER_ROW));
                Attempts = 0;
                break;
            }
            }
            break;
        
        default:
            while(TRUE){
                
                if( (Tile%NUMB_TILES_PER_ROW) + Move_Vertical <= (Right_Tile/NUMB_TILES_PER_ROW)){
                    break;
                }
                else if (VDistance<3){
                    Move_Vertical = VDistance;
                    break;
                }
                Move_Vertical = rand() % (VDistance+1) % 11;
                 Attempts++;
            if (Attempts > 1000){
                //printf("3rd While Loop\n");
                //printf("%d + %d > %d\n",Tile/NUMB_TILES_PER_ROW,Move_Vertical,(Right_Tile/NUMB_TILES_PER_ROW));
                Attempts = 0;
                break;
            }
            }
            break;
        }

        // Use a for loop to draw the tiles in the H direction
        for (int i = 0; i < Move_Horizontal; i++) {
            DrawTileOverride(Tile,FLOOR1,Background_Tiles,Tile_Type_Array);
            Tile++;
            }
        
        // Use a for loop to draw the tiles in the V direction
        for (int i = 0; i < Move_Vertical; i++) {
                DrawTileOverride(Tile,FLOOR1,Background_Tiles,Tile_Type_Array);
                if (Vertical_Dir == 1){
                    Tile += NUMB_TILES_PER_ROW;
                }
                else{
                    Tile -= NUMB_TILES_PER_ROW;
                }
    }
    // Update the HDistance and VDistance
    HDistance -= Move_Horizontal;
    VDistance -= Move_Vertical;

    if (HDistance == 0 && VDistance == 0){
        Continue = FALSE;
    }
    
    }
    

    logFile << "Path Finished " << " HDIS: " << HDistance << ", VDIS: " << VDistance << std::endl;
    return TRUE;
    

    }catch(const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
        DWORD error = GetLastError();
        printf("%s",error);
    }
    
    

}


int32_t GetFloorType(TILE* Background_Tiles,int32_t Tile){

    if(Background_Tiles[Tile].type == FLOOR1){
        return FLOOR1;
    }
    int32_t TileArray[8];
     
    int32_t Tile_Type = WALL_FULL;
    int32_t Top_Left,Top_Center,Top_Right,Left,Right,Bot_Left,Bot_Center,Bot_Right;
    
    Top_Left = (Tile - NUMB_TILES_PER_ROW - 1);
    Top_Center = (Tile- NUMB_TILES_PER_ROW);
    Top_Right = (Tile - NUMB_TILES_PER_ROW + 1);
    Left = Tile-1;
    Right = Tile+1;
    Bot_Left = (Tile + NUMB_TILES_PER_ROW - 1);
    Bot_Center = (Tile+ NUMB_TILES_PER_ROW);
    Bot_Right = (Tile + NUMB_TILES_PER_ROW + 1);

    int8_t index = 4;
    TileArray[0] = Top_Center;
    TileArray[1] = Left;
    TileArray[2] = Right;
    TileArray[3] = Bot_Center;

   

    // Check the top most part of the tile first 

    if(Background_Tiles[Top_Center].type == FLOOR1){
        if (Background_Tiles[Top_Left].type == FLOOR1 && Background_Tiles[Top_Right].type == FLOOR1){
            Tile_Type = WALL_UP;
        }
        if (Background_Tiles[Top_Left].type == FLOOR1 && Background_Tiles[Top_Right].type != FLOOR1){
            Tile_Type = WALL_UP;
        }
        if (Background_Tiles[Top_Left].type != FLOOR1 && Background_Tiles[Top_Right].type == FLOOR1){
            Tile_Type = WALL_UP;
        }

    }

    if(Tile_Type == WALL_UP || Tile_Type == CORNER_UP_LEFT){

    
    if(Background_Tiles[Bot_Center].type == FLOOR1){
        if (Background_Tiles[Bot_Left].type == FLOOR1 && Background_Tiles[Bot_Right].type == FLOOR1){
            Tile_Type = FLOOR2;
        }
        if (Background_Tiles[Bot_Left].type == FLOOR1 && Background_Tiles[Bot_Right].type != FLOOR1){
            Tile_Type = FLOOR2;
        }
        if (Background_Tiles[Bot_Left].type != FLOOR1 && Background_Tiles[Bot_Right].type == FLOOR1){
            Tile_Type = FLOOR2;
        }

    }
    }
    else{
        if(Background_Tiles[Bot_Center].type == FLOOR1){
        if (Background_Tiles[Bot_Left].type == FLOOR1 && Background_Tiles[Bot_Right].type == FLOOR1){
            Tile_Type = WALL_DOWN;
        }
        if (Background_Tiles[Bot_Left].type == FLOOR1 && Background_Tiles[Bot_Right].type != FLOOR1){
            Tile_Type = WALL_DOWN;
        }
        if (Background_Tiles[Bot_Left].type != FLOOR1 && Background_Tiles[Bot_Right].type == FLOOR1){
            Tile_Type = WALL_DOWN;
        }

    }
    }

    if(Background_Tiles[Left].type == FLOOR1){
        if (Background_Tiles[Bot_Left].type == FLOOR1 && Background_Tiles[Bot_Center].type == FLOOR1){
            Tile_Type = CORNER_DOWN_RIGHT;
        }
        if (Background_Tiles[Bot_Right].type == FLOOR1 && Background_Tiles[Bot_Center].type == FLOOR1){
            Tile_Type = CORNER_DOWN_RIGHT;
        }
        else{
            Tile_Type = WALL_RIGHT;
        }
    }

    if(Background_Tiles[Right].type == FLOOR1){
        if (Background_Tiles[Bot_Left].type == FLOOR1 && Background_Tiles[Bot_Center].type == FLOOR1){
            Tile_Type = CORNER_DOWN_LEFT;
        }
        if (Background_Tiles[Bot_Right].type == FLOOR1 && Background_Tiles[Bot_Center].type == FLOOR1){
            Tile_Type = CORNER_DOWN_LEFT;
        }
        else{
            Tile_Type = WALL_LEFT;
        }
    }
    
    if((Background_Tiles[Top_Center].type == FLOOR1) && (Background_Tiles[Bot_Center].type == FLOOR1)){
             Tile_Type = H_SINGLE_WALL;
    }
    if((Background_Tiles[Right].type == FLOOR1) && (Background_Tiles[Left].type == FLOOR1)){
             Tile_Type = V_SINGLE_WALL;
    }

    if(Background_Tiles[Top_Center].type == FLOOR1){
    
    if(Background_Tiles[Right].type == FLOOR1 && Background_Tiles[Left].type != FLOOR1){
            Tile_Type = CORNER_UP_LEFT;
        }
        if(Background_Tiles[Left].type == FLOOR1 && Background_Tiles[Right].type != FLOOR1){
            Tile_Type = CORNER_UP_RIGHT;
        }


     }

    if(Background_Tiles[Bot_Center].type == FLOOR1){
    
    if(Background_Tiles[Right].type == FLOOR1 && Background_Tiles[Left].type != FLOOR1){
            Tile_Type = CORNER_DOWN_LEFT;
        }
        if(Background_Tiles[Left].type == FLOOR1 && Background_Tiles[Right].type != FLOOR1){
            Tile_Type = CORNER_DOWN_RIGHT;
        }


     }

     int count = 0;   
     for(int i = 0; i<index; i++){
        if (Background_Tiles[TileArray[i]].type != FLOOR1){
            count++;
        }
     }
     if (count<2){
        
        if(Background_Tiles[Right].type != FLOOR1){
            Tile_Type = ISLAND_RIGHT;
        }
        if(Background_Tiles[Left].type != FLOOR1){
            Tile_Type = ISLAND_LEFT;
        }
        if(Background_Tiles[Top_Center].type != FLOOR1){
            Tile_Type = ISLAND_UP;
        }
        if(Background_Tiles[Bot_Center].type != FLOOR1){
            Tile_Type = ISLAND_DOWN;
        }
        
     }
     if (count>3){
        if (Background_Tiles[Bot_Left].type == FLOOR1){
        Tile_Type = CORNER_DOWN_LEFT_IN;
        }
        if (Background_Tiles[Bot_Right].type == FLOOR1){
        Tile_Type = CORNER_DOWN_RIGHT_IN;
        }
        if (Background_Tiles[Top_Left].type == FLOOR1){
        Tile_Type = CORNER_UP_LEFT_IN;
        }
        if (Background_Tiles[Top_Right].type == FLOOR1){
        Tile_Type = CORNER_UP_RIGHT_IN;
        }
        
    }

    
    return Tile_Type;


}


int32_t GetFloorDetail(TILE* Background_Tiles,int32_t Tile){

   
    int32_t TileArray[8];
    
     
    int32_t Tile_Type = Background_Tiles[Tile].type;
    //return Tile_Type;
    int32_t Top_Left,Top_Center,Top_Right,Left,Right,Bot_Left,Bot_Center,Bot_Right;
    
    Top_Left = (Tile - NUMB_TILES_PER_ROW - 1);
    Top_Center = (Tile- NUMB_TILES_PER_ROW);
    Top_Right = (Tile - NUMB_TILES_PER_ROW + 1);
    Left = Tile-1;
    Right = Tile+1;
    Bot_Left = (Tile + NUMB_TILES_PER_ROW - 1);
    Bot_Center = (Tile+ NUMB_TILES_PER_ROW);
    Bot_Right = (Tile + NUMB_TILES_PER_ROW + 1);

    int8_t index = 4;
    TileArray[0] = Top_Center;
    TileArray[1] = Left;
    TileArray[2] = Right;
    TileArray[3] = Bot_Center;

   

    // Check the top most part of the tile first 

    if(Tile_Type == FLOOR1){

    if(Background_Tiles[Top_Center].type > FLOOR5 || Background_Tiles[Left].type > FLOOR5 || Background_Tiles[Right].type > FLOOR5 || Background_Tiles[Bot_Center].type > FLOOR5){
        int32_t random_number = rand() % 9 + 2;
        switch (random_number)
        {
        case 2:
            Tile_Type = FLOOR4;
            break;
        
        case 3:
            Tile_Type = FLOOR3;
            break;
        
        case 4:
            Tile_Type = FLOOR4;
            break;

        case 5:
            Tile_Type = FLOOR4;
            break;
        default:
            Tile_Type = FLOOR1;
            break;
        }

    }
    
    }
    else if (Tile_Type == WALL_FULL){
        int32_t random_number = rand() % 9 + 2;
        switch (random_number)
        {
        case 2:
            Tile_Type = WALL_FULL_2;
            break;
        
        case 3:
            Tile_Type = WALL_FULL_2;
            break;
        
        case 4:
            Tile_Type = WALL_FULL_3;
            break;
        case 5:
            Tile_Type = WALL_FULL_3;
            break;
        default:
            Tile_Type = WALL_FULL;
            break;
        }

    }
    

    
    return Tile_Type;


}

VOID RemoveRoom(std::vector<ROOM>& rooms, int starting_tile) {
    for (auto it = rooms.begin(); it != rooms.end(); ++it) {
        if (it->Starting_Tile == starting_tile) {
            rooms.erase(it);
            return;
        }
    }

    std::cout << "Error no rooms with that starting tile. Something is fucked up... " << starting_tile << std::endl;
}


VOID ResetTiles(TILE *Background_Tiles, TILE* Tile_Type_Array){
    Room_Vector.clear();
    Room_Vector_Copy.clear();
    Room_Count = 0;
    Room_Tiles_Index_Pointer = 0;
    for (int i = 0; i < 100; ++i) {
        Path_Array[i] = PATH();
    }
    for (int i = 0; i < NUMB_TILES; ++i) {
        Room_Tiles_Indexes[i] = 0; // Assuming resetting to 0 is appropriate
    }
    
    for (int i = 0; i<NUMB_TILES;i++){
    Background_Tiles[i] = Tile_Type_Array[WALL_FULL];
   }

}

VOID SetTilesOverworld(TILE *Background_Tiles,GAMEBITMAP* FloorPlan){

    int32_t Starting_Coordinate = FloorPlan->bitMapInfo.bmiHeader.biWidth*FloorPlan->bitMapInfo.bmiHeader.biHeight - FloorPlan->bitMapInfo.bmiHeader.biWidth;
    int32_t GBM_Height = FloorPlan->bitMapInfo.bmiHeader.biHeight;
    int32_t GBM_Width = FloorPlan->bitMapInfo.bmiHeader.biWidth;
    int32_t Memory_Offset = 0;
    int32_t tile_index = NUMB_TILES-1;
    PIXEL Pixel;
    
    for(int PixelY = 0; PixelY<GBM_Height; PixelY+=TILE_SIZE){
        for(int PixelX = 0; PixelX<GBM_Width; PixelX+=TILE_SIZE){
            
            memcpy(&Pixel,(PIXEL*)FloorPlan->memory + Memory_Offset,sizeof(PIXEL));
            
            if(Pixel.red == 255 && Pixel.blue == 255 && Pixel.green == 255){
                Background_Tiles[tile_index+2].type=FLOOR1;
            }
            else if(Pixel.red == 255 && Pixel.blue == 255 && Pixel.green == 0){
                Background_Tiles[tile_index+2].type=FLOOR2;
            }
            else if(Pixel.red == 0 && Pixel.blue == 0 && Pixel.green == 255){
                Background_Tiles[tile_index+2].type=FLOOR3;
            }
            
            else{
                Background_Tiles[tile_index+2].type=WALL_FULL;
            }
            
            tile_index--;
            Memory_Offset = PixelX + FloorPlan->bitMapInfo.bmiHeader.biWidth*PixelY;
            
        }
    }
      

}