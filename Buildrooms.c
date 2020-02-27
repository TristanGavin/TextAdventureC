//Tristan Gavin
//gavintr.buildrooms.c 
//2/7/2020
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>

//room struct to hold room data
struct Room {
  char* name;
  int connectionCount;
  char* type;
  char connections[6][10];
};

//function declaration
struct Room GetRandomRoom();
int IsGraphFull();
void AddRandomConnection();
int CanAddConnectionFrom(struct Room x);
int ConnectionAlreadyExists(struct Room x, struct Room y);
int IsSameRoom(struct Room x, struct Room y);
void ConnectRoom(struct Room x, struct Room y);
void RoomTypes();
void GraphCheck();
void RoomFiles();
void MakeFileContents(struct Room x);

//global vars
struct Room roomA; //use for temp room storage
struct Room roomB;
int pid;
char spid[50]; //pid to string
char dirStr[] = "./gavintr.rooms.";
int a;
int B;
time_t t;
char fileContents[1000];

  //create 10 rooms with names and connection count = 0 no type or connections though.
  struct Room roomArr[10] = {
    {"basement", 0}, 
    {"kitchen", 0},
    {"library", 0},
    {"bedroom", 0},
    {"livingr", 0},
    {"bathr", 0},
    {"diningr", 0},
    {"patio", 0},
    {"hallway", 0},
    {"atic", 0} 
  };

  //the connected graph of rooms we will be building
  int graphPointer = 0;
  struct Room graph[10];

main(){
  pid = getpid();
  sprintf(spid, "%i", pid);
  strcat(dirStr, spid);
  //make dir gavintr.rooms.PID and change into it.
  mkdir(dirStr, 0700);
  chdir(dirStr);
  //random seed generator.
  srand((unsigned) time(&t));
  //initialize room types


  //when graph pointer = 7 graph is full
  //if graph is not full (1) add more connections.
  while (IsGraphFull() == 1){
    AddRandomConnection();
  }

  //assing room types
  RoomTypes();

  // fill out files like this:
  // ROOM NAME: <room name>
  // CONNECTION 1: <connection1>
  // ...
  // ROOM TYPE: <room type> 
  RoomFiles();

  //GraphCheck();

}

// Returns true (0) if all rooms have 3 to 6 outbound connections, false otherwise
int IsGraphFull()
{
  //7 rooms??
  if(graphPointer == 7){
    int i;
    //check each room for > 3 connections.
    for (i = 0; i < 7; i++){
      if(!(graph[i].connectionCount > 3)){
        //return true
        return 1;
      }
    }
    return 0;
  }
  //return false if not full
  return 1;
}


struct Room GetRandomRoom()
{
  //assigns a random room to return.
  char *roomName;
  int i;
  struct Room returnRoom;
  a = rand() % 10;
  //if room is already in graph return the graph[room] else just return random room
  returnRoom = roomArr[a];
  roomName = roomArr[a].name;
  for (i = 0; i < graphPointer; i++){
    if (strcmp(roomName, graph[i].name) == 0){
      returnRoom = graph[i];
    }
  }
  return returnRoom;
}

//adds a connection to a random roomA and random roomB
void AddRandomConnection()  
{
  //while true (1)
  while(1)
  {
    //loop until we find a room that can add a connection.
    roomA = GetRandomRoom();

    if (CanAddConnectionFrom(roomA) == 0)//0=true
      break;
  }
  //loop for a roomB that canAddconnection, is not the sameroom as A and no connection exists between a and b
  do
  {
    roomB = GetRandomRoom();
  }
  //one of these is causing infinite loop. ?
  while(CanAddConnectionFrom(roomB) == 1 || IsSameRoom(roomA, roomB) == 0 || ConnectionAlreadyExists(roomA, roomB) == 0 || PointerCheck(roomA, roomB) == 8);

  //connect rooms and add them to the graph
  ConnectRoom(roomA, roomB);  //  Add this connection, 
  ConnectRoom(roomB, roomA);  //  because this A and B will be destroyed when this function terminates
}

// make sure graph pointer doesn't go to 8, if at 6 and both rooms have zero connections return 8
int PointerCheck(struct Room x, struct Room y){
  if (graphPointer == 6 && x.connectionCount == 0 && y.connectionCount == 0){
    return 8;  
  }
  else
  {
    return 0;
  }
}

// // Returns true if a connection can be added from Room x (< 6 outbound connections), false if graph pointer =7 and connection count == 0 
int CanAddConnectionFrom(struct Room x) 
{
  if(x.connectionCount == 0 && graphPointer == 7)
    return 1;
  else if(x.connectionCount < 6)
    return 0;
  else
    return 1;
}

// Returns true (0) if Rooms x and y are the same Room, false otherwise
int IsSameRoom(struct Room x, struct Room y) 
{
  int result;
  result = strcmp(x.name, y.name);
  if(result == 0)
    return 0;
  else 
    return 1;
}

// Returns true (0) if a connection from Room x to Room y already exists, false otherwise
int ConnectionAlreadyExists(struct Room x, struct Room y)
{
  int i;
  //loop connections if room x = room y
  for(i = 0; i < 6; i++){
    if (strcmp(x.name, y.connections[i]) == 0)
      return 0;
  }
  return 1;
}

//add room y to room x connections in graph also increment connectionCount and graphPointer
void ConnectRoom(struct Room x, struct Room y) 
{
  int exit = 0;
  int i;
  //if x already exists in graph need to just add the connection (don't increment graphpointer dont add new room to graph)
  for (i = 0; i < graphPointer; i++){
    if (strcmp(x.name, graph[i].name) == 0){
      strcpy(x.connections[x.connectionCount], y.name); //x is temp struct room
      x.connectionCount ++; 
      graph[i] = x; //echange temp struct room with actual room
      exit = 1;
      break;
    }
  }
  //if x doesn't already exist in graph add it to the next point in graph pointer and exit.
  if(exit == 0){
    strcpy(x.connections[x.connectionCount], y.name);
    x.connectionCount ++;
    graph[graphPointer] = x;
    graphPointer = graphPointer + 1;
  }
}

//walks through graph and assigns room typse.
void RoomTypes(){
  int i;
  for(i = 0; i < 7; i++){
    if(i == 0){
      graph[i].type = "START_ROOM";
    }
    else if(i == 6){
      graph[i].type = "END_ROOM";
    }
    else{
      graph[i].type = "MID_ROOM";
    }
  }
}

// fill out files like this:
// ROOM NAME: <room name>
// CONNECTION 1: <connection1>
// ...
// ROOM TYPE: <room type> 
void RoomFiles(){
  int i;
  char *fileName;
  int fileDescriptor;
  size_t nwritten;
  //loop through graph
  for (i = 0; i < 7; i++){
    char filePath[12] = "./";
    fileName = graph[i].name;
    strcat(filePath, fileName);
    //for each room in graph create a file ./<name>
    fileDescriptor = open(filePath, O_RDWR | O_CREAT | O_TRUNC, 0600);
    //catch file creation errors
    if(fileDescriptor == -1){
      printf("Hull breach - open() failed on \"%s\"\n", filePath);
      perror("In RoomFiles()");
      exit(1);
    }
    else{
      //create a string with file contents in above format.
      MakeFileContents(graph[i]);
      nwritten = write(fileDescriptor, fileContents, strlen(fileContents) * sizeof(char));
    }

  }
}

void MakeFileContents(struct Room x){
  //fileContents
  char fileRoomConnections[300]= "";
  char roomConnection[50];
  int i;
  //for each connection creat a string CONNECTION #: <connection>
  for(i = 0; i < x.connectionCount; i++){
    sprintf(roomConnection, "CONNECTION %i: %s\n", i+1, x.connections[i]);  
    strcat(fileRoomConnections, roomConnection);
  }
  //create entire string of file contents store in fileContents
  sprintf(fileContents, "ROOM NAME: %s\n%sROOM TYPE: %s\n", x.name, fileRoomConnections, x.type);

}

//just an extra function to check if the graph is working!
void GraphCheck(){
  int i;
  int k;
  for(i = 0; i < 7; i++){
    printf("%s\n", graph[i].name);
    printf("%s\n", graph[i].type);
    printf("connection count: %i\n", graph[i].connectionCount);
    for(k = 0; k < graph[i].connectionCount; k++){
      printf("%s\n", graph[i].connections[k]);
    }
    printf("\n\n");
  }
}