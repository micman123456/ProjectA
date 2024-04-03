
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

VOID ProceduralGenerator(int32_t RoomAttempts, int32_t MinRoomSize, int32_t MaxRoomSize, TILE *Background_Tiles, TILE* Tile_Type_Array, int32_t Corridors){
    try {
        std::ofstream logFile("Procedural_generation_log.txt");
        if (!logFile.is_open()) {
            throw std::runtime_error("Error opening log file.");
        }

    GenerateRoomsAttempts(RoomAttempts,MinRoomSize,MaxRoomSize,Background_Tiles,Tile_Type_Array);

    

    
    //GenerateConnectingPaths(Corridors,Background_Tiles,Tile_Type_Array);

    if(Room_Vector.size() % 2 != 0){
        Room_Vector.pop_back();
    }
    Room_Vector_Copy = Room_Vector;
    int32_t stairTile = GenerateStairTile();

    logFile << "Rooms Generated Successfully: " << Room_Vector.size() << std::endl;
    logFile << "Stairs Generated Successfully: " << stairTile << std::endl;
    
    

    ROOM Room1 = Room_Vector[rand() % Room_Vector.size()];
    ROOM Room2 = Room_Vector[rand() % Room_Vector.size()];

    // ROOM Room1 = Room_Vector[0];
    // ROOM Room2 = Room_Vector[1];

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
    // if(GenerateConnectingPathsImproved(Room1,Room2,Background_Tiles,Tile_Type_Array)){
    //         RemoveRoom(Room_Vector,Room1.Starting_Tile);
    //         RemoveRoom(Room_Vector,Room2.Starting_Tile);
    // }
    
    DrawTileDetails(Background_Tiles,Tile_Type_Array);
    DrawTileDetails(Background_Tiles,Tile_Type_Array);
    DrawTileDetails(Background_Tiles,Tile_Type_Array);

    DrawTileOverride(stairTile,STAIRS,Background_Tiles,Tile_Type_Array);

    }catch(const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }
    
    
}

VOID DrawTileDetails(TILE *Background_Tiles,TILE* Tile_Type_Array){
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

int32_t GenerateStairTile(){
    ROOM Room1 = Room_Vector[rand() % Room_Vector.size()];
    int16_t randomX = (rand() % Room1.Width-1) + 1;
    int16_t randomY = (rand() % Room1.Height-1) + 1;
    
    return Room1.Starting_Tile + randomX + randomY*NUMB_TILES_PER_ROW;

    
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


/*

VOID GenerateRoomsSetNumber(int32_t NumberOfRooms, int32_t MinRoomSize, int32_t MaxRoomSize, TILE *Background_Tiles, TILE* Tile_Type_Array){
    
  

    std::uniform_int_distribution<int> distribution(MinRoomSize, MaxRoomSize);
    std::uniform_int_distribution<int> distribution_Tile(NUMB_TILES_PER_ROW*3, NUMB_TILES - (NUMB_TILES_PER_ROW*3));
    
    ROOM Room_Array[100];
    int32_t Room_Count = 0;
    int32_t Room_Size,Room_Location,Row_Index, Room_Size_Edge;
    
    // Track what tiles belong to Rooms

    int16_t Attempts;

    for (int i = 0; i < NumberOfRooms; i++){
        Attempts = 0;
        ROOM Room = {0};
        
        
        Room.Width = distribution(gen);
        Room.Height = distribution(gen);
        Room.Starting_Tile = distribution_Tile(gen);

    

         while (!IsRoomValid(Room,Room_Tiles_Indexes,Room_Tiles_Index_Pointer)){
            if (Attempts > 10){
                NumberOfRooms--;
                break;
            }
            Room.Width = distribution(gen);
            Room.Height = distribution(gen);
            Room.Starting_Tile = distribution_Tile(gen);
            Attempts++;
            
        }
        if (Attempts < 10){
            Room_Array[i] = Room;
            Room_Tiles_Index_Pointer = DrawRoomToMap(Room,Background_Tiles,Tile_Type_Array,Room_Tiles_Indexes,Room_Tiles_Index_Pointer);
            Room_Count++;
        }
        

    
    }
    printf("number of tiles: %d\nnumber of Rooms: %d\n",Room_Tiles_Index_Pointer,Room_Count);
    
    
}
*/


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



/*

VOID GenerateCorridors(int32_t Corridors, TILE *Background_Tiles, TILE* Tile_Type_Array){
    //int32_t Tile = rand() % (NUMB_TILES - (NUMB_TILES_PER_ROW * 3)) + (NUMB_TILES_PER_ROW * 3);
    try{


    int32_t Tile = STARTING_TILE;
    int8_t Length = rand() % 5 + 5;
    int16_t maxLenth = 0;
    
    int8_t Direction = GetNextDirection(Tile,Background_Tiles,Length,0);
    int8_t Previous_Direction = Direction;
    
    

    

    
    for (int i = 0; i < Corridors; i++){
        
        while(Direction < 5){
            if (maxLenth>35){
                switch (Previous_Direction)
                {
                case 0:
                     DrawTileOverride(Tile,WALL_UP,Background_Tiles,Tile_Type_Array);
                    break;
                
                case 1:
                    DrawTileOverride(Tile,WALL_RIGHT,Background_Tiles,Tile_Type_Array);
                    break;
                
                case 2:
                    DrawTileOverride(Tile,WALL_LEFT,Background_Tiles,Tile_Type_Array);
                    break;
                
                case 3:
                    DrawTileOverride(Tile,WALL_DOWN,Background_Tiles,Tile_Type_Array);
                    break;
                
                default:
                
                    break;
                }

                break;
            }
    

            switch (Direction)
            {
            case 0:

                if(Direction!=Previous_Direction){
                DrawCorners(Background_Tiles,Tile_Type_Array,Direction,Previous_Direction,Tile);
                }


                for (int l = 0; l < Length; l++){
                    DrawTile(Tile,FLOOR1,Background_Tiles,Tile_Type_Array);
                    DrawTile(Tile-NUMB_TILES_PER_ROW,WALL_DOWN,Background_Tiles,Tile_Type_Array);
                    DrawTile(Tile+1,WALL_RIGHT,Background_Tiles,Tile_Type_Array);
                    DrawTile(Tile-1,WALL_LEFT,Background_Tiles,Tile_Type_Array);

                Tile += NUMB_TILES_PER_ROW; 
                }
                break;
            case 1:
                if(Direction!=Previous_Direction){
                DrawCorners(Background_Tiles,Tile_Type_Array,Direction,Previous_Direction,Tile);
                }
                
                for (int l = 0; l < Length; l++){
                    DrawTile(Tile,FLOOR1,Background_Tiles,Tile_Type_Array);
                    DrawTile(Tile-NUMB_TILES_PER_ROW,WALL_DOWN,Background_Tiles,Tile_Type_Array);
                    DrawTile(Tile+NUMB_TILES_PER_ROW,WALL_UP,Background_Tiles,Tile_Type_Array);
                    DrawTile(Tile-1,WALL_LEFT,Background_Tiles,Tile_Type_Array);

                Tile += 1; 
                }
                break;
            case 2:
                if(Direction!=Previous_Direction){
                DrawCorners(Background_Tiles,Tile_Type_Array,Direction,Previous_Direction,Tile);
                }
                for (int l = 0; l < Length; l++){
                    DrawTile(Tile,FLOOR1,Background_Tiles,Tile_Type_Array);
                    DrawTile(Tile-NUMB_TILES_PER_ROW,WALL_DOWN,Background_Tiles,Tile_Type_Array);
                    DrawTile(Tile+NUMB_TILES_PER_ROW,WALL_UP,Background_Tiles,Tile_Type_Array);
                    DrawTile(Tile+1,WALL_RIGHT,Background_Tiles,Tile_Type_Array);

                Tile -= 1; 
                }
                break;
            case 3:
                if(Direction!=Previous_Direction){
                DrawCorners(Background_Tiles,Tile_Type_Array,Direction,Previous_Direction,Tile);
                }
                for (int l = 0; l < Length; l++){
                    DrawTile(Tile,FLOOR1,Background_Tiles,Tile_Type_Array);
                    DrawTile(Tile+NUMB_TILES_PER_ROW,WALL_UP,Background_Tiles,Tile_Type_Array);
                    DrawTile(Tile+1,WALL_RIGHT,Background_Tiles,Tile_Type_Array);
                    DrawTile(Tile-1,WALL_LEFT,Background_Tiles,Tile_Type_Array);

                Tile -= NUMB_TILES_PER_ROW; 
                }
                break;
            
            default:
                switch (Previous_Direction)
                {
                case 0:
                     DrawTileOverride(Tile,WALL_UP,Background_Tiles,Tile_Type_Array);
                    break;
                
                case 1:
                    DrawTileOverride(Tile,WALL_RIGHT,Background_Tiles,Tile_Type_Array);
                    break;
                
                case 2:
                    DrawTileOverride(Tile,WALL_LEFT,Background_Tiles,Tile_Type_Array);
                    break;
                
                case 3:
                    DrawTileOverride(Tile,WALL_DOWN,Background_Tiles,Tile_Type_Array);
                    break;
                
                default:
                
                    break;
                }
                break;
            }
            maxLenth+= Length;
            Length = rand() % 5 + 5;
            Previous_Direction = Direction;
            Direction = GetNextDirection(Tile,Background_Tiles,Length,Previous_Direction);
            
            
        }
        Tile = rand() % (NUMB_TILES - (NUMB_TILES_PER_ROW * 3)) + (NUMB_TILES_PER_ROW * 3);
        Direction = GetNextDirection(Tile,Background_Tiles,Length,Previous_Direction);
        maxLenth = 0;

    }
        } catch(const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

}





VOID GenerateCorridorsNoWalls(int32_t Corridors, TILE *Background_Tiles, TILE* Tile_Type_Array){
    //int32_t Tile = rand() % (NUMB_TILES - (NUMB_TILES_PER_ROW * 3)) + (NUMB_TILES_PER_ROW * 3);
     try {
        std::ofstream logFile("path_generation_log.txt");
        if (!logFile.is_open()) {
            throw std::runtime_error("Error opening log file.");
        }

    
    
    int32_t Path_Array_Index = 0;
    
    
    int32_t p_index = 0;
    


    int32_t Tile = STARTING_TILE;
    int8_t Length = rand() % 5 + 8;
    int16_t maxLenth = 0;
    
    int8_t Direction = GetNextDirection(Tile,Background_Tiles,Length,5);
    int8_t Previous_Direction = Direction;
    
    
    
    for (int i = 0; i < Corridors; i++){
        
        PATH p = {0};
        
        p.Starting_Tile = Tile;

        
        while(Direction < 5){
            p.Starting_Tile = Tile;


            if (maxLenth>55){
                break;
            }
    

            switch (Direction)
            {
            case 0:

                for (int l = 0; l < Length; l++){
                    DrawTileOverride(Tile,FLOOR1,Background_Tiles,Tile_Type_Array);
                    p.Tiles[p_index] = Tile;
                    p.Path_Length++;
                    p_index++;

                    Tile += NUMB_TILES_PER_ROW; 
                }

                break;
            case 1:

                
                for (int l = 0; l < Length; l++){
                    DrawTileOverride(Tile,FLOOR1,Background_Tiles,Tile_Type_Array);
                    p.Tiles[p_index] = Tile;
                    p.Path_Length++;
                    p_index++;

                Tile += 1; 
                }
                break;
            case 2:

                for (int l = 0; l < Length; l++){
                    DrawTileOverride(Tile,FLOOR1,Background_Tiles,Tile_Type_Array);
                    p.Tiles[p_index] = Tile;
                    p.Path_Length++;
                    p_index++;

                Tile -= 1; 
                }
                break;
            case 3:

                for (int l = 0; l < Length; l++){
                    DrawTileOverride(Tile,FLOOR1,Background_Tiles,Tile_Type_Array);
                    p.Tiles[p_index] = Tile;
                    p.Path_Length++;
                    p_index++;

                Tile -= NUMB_TILES_PER_ROW; 
                }
                break;
            
            default:
               
                break;
            }
            maxLenth+= Length;
            Length = rand() % 5 + 5;
            Previous_Direction = Direction;
            Direction = GetNextDirection(Tile,Background_Tiles,Length,Previous_Direction);
            
            
        }
        p.Ending_Tile = Tile;
       
        Path_Array[Path_Array_Index] = p;
        Path_Array_Index++;

        logFile << "Path Created " << i+1 << " Start: " << p.Starting_Tile << ", End: " << p.Ending_Tile << std::endl;
        for (int j = 0; j < p_index;j++){
            logFile << "Tile: " << p.Tiles[j] << std::endl;
        }
         p_index = 0;

        Tile = rand() % (NUMB_TILES - (NUMB_TILES_PER_ROW * 3));
        Tile + (NUMB_TILES_PER_ROW * 3);
        Direction = GetNextDirection(Tile,Background_Tiles,Length,Previous_Direction);
        maxLenth = 0;

    }
        } catch(const std::exception& e) {
        std::cerr << "Exception caught: " << e.what() << std::endl;
    }

}


int32_t GetNextDirection(int32_t Tile_Index,TILE *Background_Tiles,int8_t Length, int8_t Previous_Direction){
    int8_t Direction = rand() % 4;
    int8_t Attempts[4] = {0};

    while(!IsTileValidNoWall(Tile_Index,Direction,Background_Tiles,Length,Previous_Direction)){
        // Direction has been tried and failed
        Attempts[Direction] = 1;
        Direction = rand() % 4;
        
        // All Directions has been tried and failed
        if (Attempts[0] == 1 && Attempts[1] == 1 && Attempts[2] == 1 && Attempts[3] == 1){
            return 5;
        }
    }
    return Direction;

}




BOOL IsTileValid(int32_t Tile_Index, int8_t Direction, TILE *Background_Tiles,int8_t Length,int8_t Previous_Direction){
    
    // Check if Tiles on the the corner have been visited on the original tile 

    int32_t Tile_Row_Index = Tile_Index / NUMB_TILES_PER_ROW;
    int32_t Tile_Edge_Row_Index;

    if (Tile_Index%NUMB_TILES_PER_ROW==(NUMB_TILES_PER_ROW-1) || Tile_Index%NUMB_TILES_PER_ROW==1 || Tile_Index%NUMB_TILES_PER_ROW==0 || Tile_Index%NUMB_TILES_PER_ROW==2){
         return FALSE;
    }



    if (Tile_Index > (NUMB_TILES - (NUMB_TILES_PER_ROW*2) ) || Tile_Index < NUMB_TILES_PER_ROW*2 ) {
        return FALSE;
    }



    // Checks if path goes off the map on the X axis or if it overflows/underflows the tilemap
    switch (Direction)
    {
    case 0:
    if (Tile_Index + (Length * NUMB_TILES_PER_ROW) >= (NUMB_TILES - (NUMB_TILES_PER_ROW*2))){
        return FALSE;
    }
        break;
    case 1:
    Tile_Edge_Row_Index = (Tile_Index - (Length+1)) / NUMB_TILES_PER_ROW;
    if (Tile_Row_Index!=Tile_Edge_Row_Index){
        return FALSE;
    }
        break;
    case 2:
    Tile_Edge_Row_Index = (Tile_Index + (Length+1)) / NUMB_TILES_PER_ROW;
    if (Tile_Row_Index!=Tile_Edge_Row_Index){
        return FALSE;
    }
        break;
    case 3:
    if (Tile_Index - (Length * NUMB_TILES_PER_ROW) <= (NUMB_TILES_PER_ROW*2)){
        return FALSE;
    }
        break;
    
    default:
        break;
    }


    // Checks if the OG tile is already visited
    if (Background_Tiles[Tile_Index].visited == 1){
        return FALSE;
    }  
    switch (Previous_Direction)
    {
        // Down //
    case 0:
    // Check next tile Down
        switch (Direction)
        {
        case 1:
            // Check next next tile Right
            break;
        case 2:
            // Check next next tile Left
            break;
        
        default:
            break;
        }
        break;
    
    // Left //
    case 1:
    // Check next tile Left
        switch (Direction)
        {
        case 0:
            // Check Next next tile up
            break;
        case 3:
            // Check Next next tile down
            break;
        
        default:
            break;
        }
        break;

        // Right //
    case 2:
    // Check next tile Right
        switch (Direction)
        {
        case 0:
           // Check Next next tile up
            break;
        case 3:
            // Check Next next tile down
            break;
        
        default:
            break;
        }
        break;

        // Up //
    case 3:
        // Check next tile Up

        switch (Direction)
        {
        case 1:
            // Check next next right tile 
            break;
        case 2:
            // Check next next left tile 
            break;
        
        default:
            if(Background_Tiles[Tile_Index].visited == 1 || Background_Tiles[Tile_Index + NUMB_TILES_PER_ROW].visited == 1 ||  Background_Tiles[Tile_Index - NUMB_TILES_PER_ROW].visited == 1 || Background_Tiles[Tile_Index + 1].visited == 1 || Background_Tiles[Tile_Index - 1].visited == 1){
                        return FALSE;
                    }
            break;
        }
        break;
    
    default:

        
        break;
    }
    
    
    int32_t NextTile = Tile_Index;

    for (int i = 0; i < Length+1; i++){
    
    switch (Direction)
            {
            case 0:
                NextTile += NUMB_TILES_PER_ROW; 
                break;
            case 1:
                NextTile += 1; 
                break;
            case 2:
                NextTile -= 1; 
                break;
            case 3:
                NextTile -= NUMB_TILES_PER_ROW; 
                break;
            
            default:
                break;
            }
            


    if (NextTile%NUMB_TILES_PER_ROW==(NUMB_TILES_PER_ROW-1) || NextTile%NUMB_TILES_PER_ROW==1 || NextTile%NUMB_TILES_PER_ROW==0 || NextTile%NUMB_TILES_PER_ROW==2){
         return FALSE;
    }

    if (Tile_Index)
    
    // Check Location + Size fits within the Tile area
    if ( NextTile >= (NUMB_TILES - (NUMB_TILES_PER_ROW*2))){
        return FALSE;
    }

    if (i > 0){

    switch (Direction)
            {
            case 0:
                // Check if next tile, or next next tiles right and left have been visited
                if(Background_Tiles[NextTile + 2].visited == 1 || Background_Tiles[NextTile-2].visited == 1 || Background_Tiles[NextTile - 1].visited == 1 || Background_Tiles[NextTile + 1].visited == 1){
                    return FALSE;
                    }

                if(Background_Tiles[NextTile].visited == 1 || Background_Tiles[NextTile + NUMB_TILES_PER_ROW].visited == 1){
                        return FALSE;
                    }
                break;
            case 1:
                // Check if next tile, or next next tiles up and down have been visited
                if(Background_Tiles[NextTile + 2*(NUMB_TILES_PER_ROW)].visited == 1 || Background_Tiles[NextTile - 2*(NUMB_TILES_PER_ROW)].visited == 1 || Background_Tiles[NextTile + 1*(NUMB_TILES_PER_ROW)].visited == 1 || Background_Tiles[NextTile - 1*(NUMB_TILES_PER_ROW)].visited == 1){
                    return FALSE;
                    }
                if(Background_Tiles[NextTile].visited == 1 || Background_Tiles[NextTile + 1].visited == 1){
                        return FALSE;
                    }
                break;
            case 2:
                // Check if next tile, or next next tiles up and down have been visited
                if(Background_Tiles[NextTile + 2*(NUMB_TILES_PER_ROW)].visited == 1 || Background_Tiles[NextTile - 2*(NUMB_TILES_PER_ROW)].visited == 1 || Background_Tiles[NextTile + 1*(NUMB_TILES_PER_ROW)].visited == 1 || Background_Tiles[NextTile - 1*(NUMB_TILES_PER_ROW)].visited == 1){
                    return FALSE;
                    }
                if(Background_Tiles[NextTile].visited == 1 || Background_Tiles[NextTile - 1].visited == 1){
                        return FALSE;
                    }
                break;
            case 3:
                // Check if next tile, or next next tiles right and left have been visited
                if(Background_Tiles[NextTile + 2].visited == 1 || Background_Tiles[NextTile-2].visited == 1 || Background_Tiles[NextTile - 1].visited == 1 || Background_Tiles[NextTile + 1].visited == 1){
                    return FALSE;
                    }
                if(Background_Tiles[NextTile].visited == 1 || Background_Tiles[NextTile - NUMB_TILES_PER_ROW].visited == 1){
                        return FALSE;
                    }
                break;
            
            default:
                break;
            }
    }


    if(Background_Tiles[NextTile].visited == 1){
        return FALSE;
    }

    }
    

    return TRUE;
    // Check if room isn't with 3 tiles of another room
    // Returns True is all above conditions are met 
}

BOOL IsTileValidNoWall(int32_t Tile_Index, int8_t Direction, TILE *Background_Tiles,int8_t Length,int8_t Previous_Direction){
    int32_t Tile_Row_Index = Tile_Index / NUMB_TILES_PER_ROW;
    int32_t Tile_Edge_Row_Index;

    
    
    if (Tile_Index%NUMB_TILES_PER_ROW==(NUMB_TILES_PER_ROW-1) || Tile_Index%NUMB_TILES_PER_ROW==1 || Tile_Index%NUMB_TILES_PER_ROW==0 || Tile_Index%NUMB_TILES_PER_ROW==2){
         return FALSE;
    }



    if (Tile_Index > (NUMB_TILES - (NUMB_TILES_PER_ROW*2) ) || Tile_Index < NUMB_TILES_PER_ROW*2 ) {
        return FALSE;
    }



    // Checks if path goes off the map on the X axis or if it overflows/underflows the tilemap
    switch (Direction)
    {
    case 0:
    if (Tile_Index + (Length * NUMB_TILES_PER_ROW) >= (NUMB_TILES - (NUMB_TILES_PER_ROW*2))){
        return FALSE;
    }
        break;
    case 1:
    Tile_Edge_Row_Index = (Tile_Index - (Length+1)) / NUMB_TILES_PER_ROW;
    if (Tile_Row_Index!=Tile_Edge_Row_Index){
        return FALSE;
    }
        break;
    case 2:
    Tile_Edge_Row_Index = (Tile_Index + (Length+1)) / NUMB_TILES_PER_ROW;
    if (Tile_Row_Index!=Tile_Edge_Row_Index){
        return FALSE;
    }
        break;
    case 3:
    if (Tile_Index - (Length * NUMB_TILES_PER_ROW) <= (NUMB_TILES_PER_ROW*2)){
        return FALSE;
    }
        break;
    
    default:
        break;
    }


    // Checks if the OG tile is already visited
    if (Background_Tiles[Tile_Index].visited == 1){
        return FALSE;
    }  

    // Checks if the tiles surrounding it are visited

    // if(Background_Tiles[Tile_Index + 1].visited == 1 || Background_Tiles[Tile_Index-1].visited == 1){
    //     return FALSE;
    // }

    // if (Background_Tiles[Tile_Index - NUMB_TILES_PER_ROW].visited == 1 || Background_Tiles[Tile_Index+ NUMB_TILES_PER_ROW].visited == 1){
    //     return FALSE;
    // }
    
    int32_t NextTile = Tile_Index;

    // Iterates through each tile in the pathway to check validity 

    for (int i = 0; i < Length+1; i++){
    
    switch (Direction)
            {
            case 0:
                NextTile += NUMB_TILES_PER_ROW; 
                break;
            case 1:
                NextTile += 1; 
                break;
            case 2:
                NextTile -= 1; 
                break;
            case 3:
                NextTile -= NUMB_TILES_PER_ROW; 
                break;
            
            default:
                break;
            }
            



    

    if (i > 0){

    switch (Direction)
            {
            case 0:
                // Check if next tile, or next next tiles right and left have been visited
                if(Background_Tiles[NextTile + 2].visited == 1 || Background_Tiles[NextTile-2].visited == 1 || Background_Tiles[NextTile - 1].visited == 1 || Background_Tiles[NextTile + 1].visited == 1){
                    return FALSE;
                    }

                if(Background_Tiles[NextTile].visited == 1 || Background_Tiles[NextTile + NUMB_TILES_PER_ROW].visited == 1){
                        return FALSE;
                    }
                break;
            case 1:
                // Check if next tile, or next next tiles up and down have been visited
                if(Background_Tiles[NextTile + 2*(NUMB_TILES_PER_ROW)].visited == 1 || Background_Tiles[NextTile - 2*(NUMB_TILES_PER_ROW)].visited == 1 || Background_Tiles[NextTile + 1*(NUMB_TILES_PER_ROW)].visited == 1 || Background_Tiles[NextTile - 1*(NUMB_TILES_PER_ROW)].visited == 1){
                    return FALSE;
                    }
                if(Background_Tiles[NextTile].visited == 1 || Background_Tiles[NextTile + 1].visited == 1){
                        return FALSE;
                    }
                break;
            case 2:
                // Check if next tile, or next next tiles up and down have been visited
                if(Background_Tiles[NextTile + 2*(NUMB_TILES_PER_ROW)].visited == 1 || Background_Tiles[NextTile - 2*(NUMB_TILES_PER_ROW)].visited == 1 || Background_Tiles[NextTile + 1*(NUMB_TILES_PER_ROW)].visited == 1 || Background_Tiles[NextTile - 1*(NUMB_TILES_PER_ROW)].visited == 1){
                    return FALSE;
                    }
                if(Background_Tiles[NextTile].visited == 1 || Background_Tiles[NextTile - 1].visited == 1){
                        return FALSE;
                    }
                break;
            case 3:
                // Check if next tile, or next next tiles right and left have been visited
                if(Background_Tiles[NextTile + 2].visited == 1 || Background_Tiles[NextTile-2].visited == 1 || Background_Tiles[NextTile - 1].visited == 1 || Background_Tiles[NextTile + 1].visited == 1){
                    return FALSE;
                    }
                if(Background_Tiles[NextTile].visited == 1 || Background_Tiles[NextTile - NUMB_TILES_PER_ROW].visited == 1){
                        return FALSE;
                    }
                break;
            
            default:
                break;
            }
    }


    if(Background_Tiles[NextTile].visited == 1){
      //  return FALSE;
    }

    if (NextTile%NUMB_TILES_PER_ROW==(NUMB_TILES_PER_ROW-1) || NextTile%NUMB_TILES_PER_ROW==1 || NextTile%NUMB_TILES_PER_ROW==0 || NextTile%NUMB_TILES_PER_ROW==2){
        return FALSE;
    }
    
    if (NextTile > (NUMB_TILES - (NUMB_TILES_PER_ROW*2) ) || NextTile < NUMB_TILES_PER_ROW*2 ) {
        return FALSE;
    }



    }
    

    return TRUE;
    // Check if room isn't with 3 tiles of another room
    // Returns True is all above conditions are met 
}



VOID DrawCorners(TILE *Background_Tiles, TILE* Tile_Type_Array,int8_t Direction, int8_t Previous_Direction, int32_t Tile_Index){
    int32_t Corner_Tile_Index;
    switch (Direction)
    {
        // Down //
    case 0:
        switch (Previous_Direction)
        {
        case 1:
            Corner_Tile_Index = Tile_Index+NUMB_TILES_PER_ROW-1;
            DrawTileOverride(Corner_Tile_Index,CORNER_UP_LEFT,Background_Tiles,Tile_Type_Array);
            break;
        case 2:
            Corner_Tile_Index = Tile_Index+NUMB_TILES_PER_ROW+1;
            DrawTileOverride(Corner_Tile_Index,CORNER_UP_RIGHT,Background_Tiles,Tile_Type_Array);
            break;
        
        default:
            break;
        }
        break;
    
    // Left //
    case 1:
        switch (Previous_Direction)
        {
        case 0:
            Corner_Tile_Index = Tile_Index-NUMB_TILES_PER_ROW+1;
            DrawTileOverride(Corner_Tile_Index,CORNER_DOWN_RIGHT,Background_Tiles,Tile_Type_Array);
            break;
        case 3:
            Corner_Tile_Index = Tile_Index+NUMB_TILES_PER_ROW+1;
            DrawTileOverride(Corner_Tile_Index,CORNER_UP_RIGHT,Background_Tiles,Tile_Type_Array);
            break;
        
        default:
            break;
        }
        break;

        // Right //
    case 2:
        switch (Previous_Direction)
        {
        case 0:
            Corner_Tile_Index = Tile_Index-NUMB_TILES_PER_ROW-1;
            DrawTileOverride(Corner_Tile_Index,CORNER_DOWN_LEFT,Background_Tiles,Tile_Type_Array);
            break;
        case 3:
            Corner_Tile_Index = Tile_Index+NUMB_TILES_PER_ROW-1;
            DrawTileOverride(Corner_Tile_Index,CORNER_UP_LEFT,Background_Tiles,Tile_Type_Array);
            break;
        
        default:
            break;
        }
        break;

        // Up //
    case 3:
        switch (Previous_Direction)
        {
        case 1:
            Corner_Tile_Index = Tile_Index-NUMB_TILES_PER_ROW-1;
            DrawTileOverride(Corner_Tile_Index,CORNER_DOWN_LEFT,Background_Tiles,Tile_Type_Array);
            break;
        case 2:
            Corner_Tile_Index = Tile_Index-NUMB_TILES_PER_ROW+1;
            DrawTileOverride(Corner_Tile_Index,CORNER_DOWN_RIGHT,Background_Tiles,Tile_Type_Array);
            break;
        
        default:
            break;
        }
        break;
    
    default:
        break;
    }
}
*/