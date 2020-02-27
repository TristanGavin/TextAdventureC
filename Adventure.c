//Tristan Gavin
//gavintr.adventure.c
//2/7/2020
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <dirent.h>
#include <regex.h>
#include <time.h>
#include <sys/stat.h>

//struct room to hold room data
struct Room {
  char name[10];
  int connectionCount;
  int endRoom; //0 if midroom 1 if endroom
  int startRoom; //0 if midroom 1 if startroom
  char connections[6][10];
};

//function declerations
void FileRead(char* x, int j);
void PrintGame(struct Room x);
void checkWin(struct Room x);

//global variables
struct Room gameArr[7]; //just like graph in buildrooms 
int roomCounter = 1;
int currentRoom;    //index of current room
int winGame = 0;   //win game =1 
int numSteps = 0;  //count numsteps user took
int vPath[50]; //array of gameArr index numbers of th path the user took. 

int startRoom;
int endRoom;
//to get room names
char roomArr[10][10] = {
    "basement", 
    "kitchen",
    "library",
    "bedroom",
    "livingr",
    "bathr",
    "diningr",
    "patio",
    "hallway",
    "atic" 
};



int main(){
    struct stat stats;
    char dirName[100];
    memset(dirName, '\0', 100);
    char path[50] = "./";
    struct dirent *de; 
    DIR *d = opendir("."); //opens current dir
    //cant open dir
    if (d == NULL){ 
        printf("Could not open current directory" ); 
        return 0; 
    } 
    //check every entry in the directory.
    while ((de = readdir(d)) != NULL){
        //don't know how to do regexpressions so i guess this will work... 
        //gets the rooms dir name
        if(strcmp("gavintr.buildrooms", de->d_name) != 0 && strcmp("gavintr.buildrooms.c", de->d_name) != 0 && strcmp("gavintr.adventure", de->d_name) != 0 && strcmp("gavintr.adventure.c", de->d_name) && strcmp(".", de->d_name) != 0 && strcmp("..", de->d_name) != 0){
            sprintf(dirName, "%s", de->d_name);
            break;
        }
    }
    free(d); //free maloc

    //now that we have the latest rooms dir we can switch into it and start working with the files 
    strcat(path, dirName);
    //printf("%s\n", path);
    chdir(path);
    //print names of files dir and then put them in a string array.
    struct dirent *ce; 
    DIR *c = opendir("."); //opens current dir 
    int i = 0;
    char rooms[7][10]; //string array to hold room file names
    while ((ce = readdir(c)) != NULL){
        if (strcmp(".", ce->d_name) != 0 && strcmp("..", ce->d_name) != 0){
            //puts each room name into the rooms array
            sprintf(rooms[i], "%s", ce->d_name);
            //printf("%s\n", rooms[i]);
            i++;
        }
    }
    free(c); // free maloc

    //now that we have the names of all the rooms we can open the files and read them.
    char roomContents[600]; //string contents of room files
    long length;
    //loop through files to fill get name of each room and start and end rooms.
    for (i = 0; i < 7; i++){
        memset(roomContents, '\0', 600); //reset the contents to 0
        FILE *fp = fopen(rooms[i], "r");
        //make a string with entire contents of file
        fseek(fp, 0, SEEK_END);
        length = ftell(fp); //returns the length of the file.
        fseek (fp, 0, SEEK_SET); //set pointer to first line again
        
        if(roomContents){
            fread(roomContents, 1, length, fp); //stores string in roomContents
        }
        //flush buffers.
        fclose(fp);
        //check if substrings exist to in roomContents to find roomTypes
        //also start to store information (name type end room start room) in gameArray (graph)
        if(strstr(roomContents, "START_ROOM") != NULL){
            startRoom = i;
            //printf("%i", startRoom);
            strcpy(gameArr[0].name, rooms[i]); 
            gameArr[0].startRoom = 1;//set start room flag
            gameArr[0].endRoom = 0;
        }
        else if(strstr(roomContents, "END_ROOM") != NULL){
            endRoom = i;
            strcpy(gameArr[6].name, rooms[i]);
            gameArr[6].startRoom = 0;
            gameArr[6].endRoom = 1;//set end room flag
        }
        else{
            strcpy(gameArr[roomCounter].name, rooms[i]);
            gameArr[roomCounter].startRoom = 0;
            gameArr[roomCounter].endRoom = 0;
            roomCounter++;
        }
    }

    
    //now loop every room and build their connections.
    int j;
    for(j = 0; j < 7; j++){
        char roomPath[] = "./";
        strcat(roomPath, gameArr[j].name); //creats a file pathname to send to readfile function
        FileRead(roomPath, j);
    }

    currentRoom = 0; //start room
    //keep printing prompt until user finds end room (winGame !=0)
    while(winGame == 0){
        PrintGame(gameArr[currentRoom]);
    }

    //print victory message num steps and list of rooms traveled stored in vPath array
    printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    printf("YOU TOOK %i STEPS. YOUR PATH TO VICTORY WAS: \n", numSteps);
    for(i = 0; i < numSteps; i++){
        printf("%s\n", gameArr[vPath[i]].name);
    }

    //this is trying to remove a directory with a file remove function. might need to remove all files before deleting directory
    // sprintf(path, "%s", "./");
    // strcat(path, dirName);
    // printf("%s\n", path);
    // chdir("../");
    // if(remove(path)==0){
    //     printf("pathremoved");
    // }
    // else{
    //     printf("pathnotremeoved");
    // }

    //exit with code 0    
    return 0;
}

//function to read file
//reads file like we did before with fopen and fread
void FileRead(char* x, int j){
    char roomContents[1000];
    memset(roomContents, '\0', 1000);
    long length; 
    //open the file
    FILE *fp = fopen(x, "r");
    fseek(fp, 0, SEEK_END);
    length = ftell(fp); //returns the length of the file.
    fseek (fp, 0, SEEK_SET); //set pointer to first line again
    if(roomContents){
        fread(roomContents, 1, length, fp);   //make a string with entire contents of file
    }
    //flush buffers.
    fclose(fp);

    //loop and use substring to find connections.
    int c = 0;
    int i;
    for(i = 0; i < 7; i++){
        if(strstr(roomContents, gameArr[i].name) && i != j){
            strcpy(gameArr[j].connections[c], gameArr[i].name); // if connection is found copy name into gamearray connections for the current room
            c++;
            gameArr[j].connectionCount++;     
        }
    }

}

void PrintGame(struct Room x){
    //CURRENT LOCATION:
    //POSSIBLE CONNECTIONS:
    //WHERE TO? >
    char currentLoc[100] = "CURRENT LOCATION: ";
    char possibleCons[300] = "POSSIBLE CONNECTIONS: ";
    char whereTo[50] = "WHERE TO? >";
    char nextRoom[20];
    int i;
    int quit = 0;

    strcat(currentLoc, x.name); //concatonate name with currentloc and print to user
    printf("%s\n", currentLoc);
    for(i = 0; i < x.connectionCount; i++){ //loop all connections and concat with possibleCons
        strcat(possibleCons, x.connections[i]);
        if(i == x.connectionCount-1){
            strcat(possibleCons, "."); //if last connection use period
        }
        else
        {
            strcat(possibleCons, ", "); //middle connections get commas
        }    
    }
    printf("%s\n", possibleCons);
    
    //get next room from user with scanf
    printf("%s", whereTo);
    scanf("%s", nextRoom);
    printf("\n");
    //checks connection exists.
    for(i = 0; i < x.connectionCount; i++){
        if(strcmp(nextRoom, x.connections[i]) == 0){
            //printf("%s", x.connections[i]);
            quit = 1;
        }
    }

    //if connection doesn't exist quit and have user start again
    if(quit == 0){
        printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
        return;
    }

    //get the array index of the next room, inc numSteps, ADD TO VICTORY PATH, check win 
    for(i = 0; i < 7; i++){
        if(strcmp(nextRoom, gameArr[i].name) == 0){
            vPath[numSteps] = i; //adds room loation to victory path
            numSteps++;
            //check if end room
            if(gameArr[i].endRoom == 1){
                winGame = 1; //if endRoom exit daddy loop.
            }
            currentRoom = i;
        }
    }

}

