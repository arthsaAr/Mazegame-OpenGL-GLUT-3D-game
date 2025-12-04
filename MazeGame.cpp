/******************************************************************************
*  File Name:    MazeGame.cpp
*  Author:       Raashtra KC
*  Date:         2025-10-15
*  Language:     C++
*  Notes:        Make sure the required libraries are installed
******************************************************************************/

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
#include <windows.h>
#define M_PI 3.14159265358979323846 // Window doesn't seem to define this
#endif
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif
#include <cmath>
#include <iostream>

//Initializations for window of game(game window)
const int windowX = 800;          //fixed length of the game window
const int windowY = 800;          //fixed breadth of the game window

//Initializations for defining the size of our world in 3D
float lef = -1.0f;          //storing the lower bound of the world in x plane
float rig = 1.0f;           //storing the upper bound of the world in x plane
float top = 1.0f;           //storing the upper bound of the world in y plane
float bot = -1.0f;          //storing the lower bound of the world in y plane

//Initializations for placing the camera in the screen
float dire1 = -8.0f;               //face towards z direction
float dire2 = 0.0f;                //
float cameraSpeed = 0.0058f;        //defining the cameraspeed(in our case it determines each next step of the camera when moved forward or backward)
float dir_X, dir_Y, dir_Z;          //defining the directions in x, y and z directions

//Initializations for the toggel features in the game
bool makeLines = false;             //initializing the bool variable for enabling and disabling the lines toggle feature in our game(triggered by pressing o/O)
bool moveThrough = false;           //initializing the bool variable for enabling and disabling the wallhack/pass through the walls/boxes in our maze(triggered by pressing w/W)

//Initializations for the blue box thrown by the player/camera
bool blueAlive = false;             //setting the bluebox status false by default
float blueX, blueY, blueZ;          //defining the position of blue box in x,y and z directions
float speedX, speedY, speedZ;       //defining the speed of blue box in x,y and z directions
float blueRot = 0.0f;               //defininf the angle of rotation of the blue box
float blueBounce = 0.05f;           //initializing the bounce to start from 0.05f, center of the screen in our case
int blueTimeSpan = 0;               //initializing the timespan for the blue box as 0 

//Initializations for maze size
const int mapRows = 20;           //number of rows to put boxes in the maze
const int mapCols = 20;           //number of columns to put boxes in the maze
int map[mapRows][mapCols] = {       //out main map array for maze, where every 1 is a wall, 0 is a free space where player/camera can move and 2 is red boxes in different places.
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,1,0,1,1,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,1,0,0,0,0,2,0,1,1,1,1,0,1,0,1},
    {1,0,0,0,1,1,1,1,0,1,1,1,1,0,0,1,0,1,0,1},
    {1,0,0,1,1,0,0,0,0,1,0,0,1,0,1,1,0,1,0,1},
    {1,1,0,1,1,1,1,1,0,1,0,1,1,0,0,1,0,1,1,1},
    {1,0,0,0,1,0,0,0,0,0,0,1,1,1,0,1,0,0,0,1},
    {1,0,0,0,1,1,0,1,0,1,1,0,1,0,0,1,0,1,0,1},
    {1,0,1,0,1,0,0,0,0,2,0,0,1,1,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,1,0,0,1,0,1,1,0,1,0,1,0,1},
    {1,0,2,0,0,0,0,0,0,1,1,0,0,0,0,1,0,1,0,1},
    {1,0,0,0,1,0,0,1,0,0,0,0,1,1,1,1,1,1,2,1},
    {1,0,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1},
    {1,0,1,0,0,0,1,0,1,1,1,0,1,0,0,1,1,1,1,1},
    {1,0,0,0,1,0,1,0,1,0,0,0,1,0,0,0,0,1,0,1},
    {1,1,1,0,1,0,1,0,1,0,1,0,1,0,1,1,1,1,0,1},
    {1,0,1,0,1,0,1,0,1,1,0,0,0,0,0,0,0,1,0,1},
    {1,0,1,0,0,0,1,0,0,0,1,0,1,0,1,1,0,0,0,1},
    {1,0,0,0,1,0,0,0,1,0,2,0,1,0,0,1,1,1,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

//Initialization for Red rotating box in maze
float rotAng = 0.0f;            //angle by which the red box rotates repeatedly

//Initialization for the total number of Hittable box in the game
int boxCounter = 5;                 //initializing the total number of box in our maze as 5

//Initializing the breadth and height of each box/wall to place in our world
float cellBreadth = (rig - lef) / mapCols;
float cellHeight = (top - bot) / mapRows;

//Initialize the camera position
float cameraX = lef + cellBreadth / 2.0f + 2 * cellBreadth;     //Initializing the location to place our camera in x direction(slightly inside the world by 2 cells)
float cameraY = 0.049f;                                         //placing the camera at fixed height, around center of a cell
float cameraZ = bot + cellHeight / 2.0f + 2 * cellHeight;       //Initializing the location to place our camera in z direction(slightly inside the world by 2 cells)

float cameraSpacing = cellBreadth * 0.99f;               //initializing cameraspacing for camera movement(each step)

const int TEXTURE_SIZE = 64;
//defining texture array for storing texture details for the brick pattern
unsigned char brickTexture[TEXTURE_SIZE][TEXTURE_SIZE][3]; ///size three for assigning r,g,b(3 color values while generating texture)
//defining texture array for storing texture details for the food that are being spawned in map(total of 5 in our case)
unsigned char foodTexture[TEXTURE_SIZE][TEXTURE_SIZE][3];

GLuint brickID, foodID;

void generateFoodTexture() {
    int brickWidth = 22;   // pixels per brick horizontally
    int brickHeight = 13;   // pixels per brick vertically
    int gap = 1;         // thickness of mortar in pixels

    //looping over every pixel of our defined 64 * 64 image texutre, which becomes our texture for the walls
    for (int col = 0; col < TEXTURE_SIZE; col++) {
        for (int row = 0; row < TEXTURE_SIZE; row++) {

            //initializing which row of the bricks we current are in,
            //dividing the current column with the brick height gives us specific row position in that column
            //in our case we will get,
            //for columns 0 to 13: row will be 0
            //for column 14-27
            int curRow = col / brickHeight;

            //// Apply half-brick horizontal offset on every other row
            int xShifted = row;
            if (curRow % 2 == 1) {
                xShifted = row + brickWidth / 2;
            }

            // Determine brick boundaries
            int boundaryX = xShifted % brickWidth;
            int boundaryY = col % brickHeight;

            // Check if current pixel is the gap between the bricks or the acctual brick
            //we assign if the current pixel is the gap, by checking if any of the boundary variable of the brick is less than the defined gap size
            bool isGap = false;
            if ((boundaryY < gap) || (boundaryX < gap)) {
                isGap = true;
            }

            if (isGap) {
                // Light gray color for the gap between the bricks(like real mortar color)
                foodTexture[col][row][0] = 0;
                foodTexture[col][row][1] = 255;
                foodTexture[col][row][2] = 0;
            }
            else {
                // Brick color setup with red value to 150
                foodTexture[col][row][0] = 150;      // red
                foodTexture[col][row][1] = 0;    //Green value
                foodTexture[col][row][2] = 0;   //Blue value
            }
        }
    }
}

void generateWallTexture() {
    int brickWidth = 22;   // pixels per brick horizontally
    int brickHeight = 13;   // pixels per brick vertically
    int gap = 1;         // thickness of mortar in pixels

    //looping over every pixel of our defined 64 * 64 image texutre, which becomes our texture for the walls
    for (int col = 0; col < TEXTURE_SIZE; col++) {
        for (int row = 0; row < TEXTURE_SIZE; row++) {

            //initializing which row of the bricks we current are in,
            //dividing the current column with the brick height gives us specific row position in that column
            //in our case we will get,
            //for columns 0 to 13: row will be 0
            //for column 14-27
            int curRow = col / brickHeight;

            //// Apply half-brick horizontal offset on every other row
            int xShifted = row;
            if (curRow % 2 == 1) {
                xShifted = row + brickWidth / 2;
            }

            // Determine brick boundaries
            int boundaryX = xShifted % brickWidth;
            int boundaryY = col % brickHeight;

            // Check if current pixel is the gap between the bricks or the acctual brick
            //we assign if the current pixel is the gap, by checking if any of the boundary variable of the brick is less than the defined gap size
            bool isGap = false;
            if ((boundaryY < gap) || (boundaryX < gap)) {
                isGap = true;
            }

            if (isGap) {
                // Light gray color for the gap between the bricks(like real mortar color)
                brickTexture[col][row][0] = 220;
                brickTexture[col][row][1] = 220;
                brickTexture[col][row][2] = 220;
            }
            else {
                // Brick color setup with red value to 150
                brickTexture[col][row][0] = 150 ;      // red
                brickTexture[col][row][1] = 0;    //Green value
                brickTexture[col][row][2] = 0;   //Blue value
            }
        }
    }
}

//Converts a angle from degrees to radians to use with trignometric functions
//Takes angle in degree and and returns angle in radians
float degtoRadian(float degrees) {
    return degrees * (M_PI / 180.0f);
}

//This method checks for collision between the camera and objects in the maze
//In our implementation it is used to handle collision of camera with walls and collision with the red rotating boxes 
//This method also updates the state of red boxes to green as needed for collision
//Input: takes current camera position in coordinates
//Returns: a boolean true if there is collision and false otherwise
bool collisionDetection(float x, float z) {
    //converting the coordinates of the camera to match the cell sizes
    int mapX = (int)((x + dir_X * cameraSpacing - lef) / cellBreadth);
    int mapZ = (int)((z + dir_Z * cameraSpacing - bot) / cellHeight);

    //check if the coordinates of camera is within the map bounds
    if (mapX >= 0 && mapX < mapCols && mapZ >= 0 && mapZ < mapRows) {

        //Checking if the object in current position of camera is wall, and if yes returning collision is detected
        if (!moveThrough && map[mapZ][mapX] == 1) {
            return true;
        }

        //checking for collision of camera with a red box(by checking if the current position of camera includes a red box)
        if (map[mapZ][mapX] == 2 && boxCounter <= 5) { // red box
            boxCounter--;       //decreasing the number of red boxes count when a collision is detected

            //handling if all the red boxes are already hit and current count is equal to zero
            if (boxCounter <= 0) {
                printf("No more boxes left, resetting boxes back to red and walls to green.\n");
                // Reset all boxes to red
                for (int i = 0; i < mapRows; i++) {
                    for (int j = 0; j < mapCols; j++) {
                        if (map[i][j] == 3) {       //resetting each of the red boxes back to green as per our winning/game completion condition
                            map[i][j] = 2;      //setting each maze with green boxes back to red
                        }
                        // Change walls to green
                        if (map[i][j] == 1) {           //converting all the wall with yellow color to green(winning/game completion as required)
                            map[i][j] = 4;              // assuming 4 = green wall, we converted each wall into green color
                        }
                    }
                }
                boxCounter = 6;                         // reset counter to value greater than 5, to avoid detecting further collision after game is finished
                printf("Game is completed!, Thank you for playing\n");      //game ended printing to the console
            }
            else {
                map[mapZ][mapX] = 3;                    // if there are still more than 0 boxes left in the maze, than changing the current collision box to green
                printf("Box turned green, %d boxes remaining\n", boxCounter);
            }
        }
    }
    return false;               //returning no collision is detected
}

//This method checks for collision between the blue box and objects in the maze
//In our implementation, we handle the collision of blue box with red box, and also with the walls
//This method also changes the state of blue box after collision
//Input: takes current position of blue box in coordinates
//Returns: a boolean true if there is collision and false otherwise
bool bluecollisionDetection(float x, float z) {
    float smallCube = 0.32f;        //length of the sides of red cube is 0.32 as we defined in makeCube function
    float extraThinkness = 0.02f;   //adding small thinkness around the blue box to avoid passing through edges of red box and detect no collision

    //converting the coordinates to the maze coordinates (ranges from 0 to 19)
    //with this conversion we know in which exact cell the blue box is in
    int mapX = (int)((x - lef) / cellBreadth);
    int mapZ = (int)((z - bot) / cellHeight);

    //checking if the converted coordinate of the blue box is within the map bounds
    if (mapX >= 0 && mapX < mapCols && mapZ >= 0 && mapZ < mapRows) {

        //collision with a wall for map array value = 1
        //checking if the current position/cell where the blue box :  has a wall and returning true as a collision is detected
        if (!moveThrough && map[mapZ][mapX] == 1) {
            return true;
        }

        //collision with a red box for map array value = 2
        //checking for collision of blue box with a red box(by checking if the current position of blue box includes a red box)
        if (map[mapZ][mapX] == 2 && boxCounter <= 5) {
            //calculating the center of the red cube as same as we did for the blue box
            //center of the specific red cube that is in the cell mapX, mapZ(where we have our blue box)
            //since the red cube is at the center of cell floating, we calculate the center as :
            // mapX*cellBreadth gives the starting point of cell and adding half gives the midpoint
            // similarly mapZ*cellHeight gives the starting point of height of cell, and adding half gives the midpoint
            float cubeX = lef + mapX * cellBreadth + cellBreadth / 2.0f;
            float cubeZ = bot + mapZ * cellHeight + cellHeight / 2.0f;
            float cubeMid = (cellBreadth / 2.0f) * smallCube;

            //checking if the blue box overlaps the red box by making sure the blue box is within the bound of red box
            if (x + extraThinkness >= cubeX - cubeMid && x - extraThinkness <= cubeX + cubeMid &&
                z + extraThinkness >= cubeZ - cubeMid && z - extraThinkness <= cubeZ + cubeMid)
            {
                boxCounter--;       //decreasing the number of coount for red boxes in the maze after a collision is detected
                map[mapZ][mapX] = 3;        //setting the current cell(the object in current cell to 3, which is a green rotating cube)
                printf("Box turned green, %d boxes remaining\n", boxCounter);
                //handling if all the red boxes are already hit and current count is equal to zero
                if (boxCounter <= 0) {
                    printf("No more boxes left, resetting boxes back to red and walls to green.\n");
                    for (int i = 0; i < mapRows; i++) {
                        for (int j = 0; j < mapCols; j++) {
                            if (map[i][j] == 3) {       //resetting each of the red boxes back to green as per our winning/game completion condition
                                map[i][j] = 2;          //setting each maze with green boxes back to red
                            }
                            if (map[i][j] == 1) {       //converting all the wall with yellow color to green(winning/game completion as required)
                                map[i][j] = 4;          // assuming 4 = green wall, we converted each wall into green color
                            }
                        }
                    }
                    boxCounter = 6;                     // reset counter to value greater than 5, to avoid detecting further collision after game is finished
                    printf("Game is completed!, Thank you for playing!\n");     //game ended printing to the console
                }
                return true;
            }
        }
    }
    return false;       //returning no collision is detected
}

//This method updates the direction of camera for the given angles\
//method also initializes the vector components for the camera for the given angle dire1 and dire2
void cameraDirection() {
    //converting the given angles from degrees to radians for use with trignometric functions
    float dir1_InRadians = degtoRadian(dire1);      //horizontal rotation angle
    float dir2_InRadians = degtoRadian(dire2);      //verticle rotation angle

    //calculating direction vector components for each direction
    dir_X = cosf(dir1_InRadians);
    dir_Z = sinf(dir1_InRadians);
    dir_Y = sinf(dir2_InRadians);

    //now since we have a constant Y component and we move our camera around the XZ plant, we normalize the horizontal components(i.e. XZ plane.
    //this helps camera move at constant speed
    float lengthXZ = sqrtf(dir_X * dir_X + dir_Z * dir_Z);
    if (lengthXZ != 0.0f) {
        dir_X = dir_X / lengthXZ;
        dir_Z = dir_Z / lengthXZ;
    }
}

//This method moves the camera in forward direction in the XZ plane
//This method also makes sure there is no collision and only moves when there is free space to move into
void moveUp() {
    //finding the new/next position where the camera should be based on the direction of camera and speed for the move
    float dx = cameraX + dir_X * cameraSpeed;      //new x position
    float dz = cameraZ + dir_Z * cameraSpeed;      //new z position

    //here we are just calling the function, because i have implemented a color change method inside the collision detection, which doesnot
    //return anything but only changes the color of the red box to green when hit by camera
    //I saw this feature in demo so implemented it in my maze game as well
    collisionDetection(dx, dz);

    //this condition updates the camera to next position only if
    //movethrough is enabled(when w toggle is used by player) OR
    //if there is no collision for the next position of camera
    if (moveThrough || !collisionDetection(dx, dz)) {
        cameraX = dx;
        cameraZ = dz;
    }
}

//This method moves the camera in backward direction in XZ plane
//This method makes sure that there is no collision and only moves the camera if there is free space to move into
void moveDown() {
    //finding the next position of camera by calculating new X and Z position based on camera speed and direction the camera is facing
    float dx = cameraX - dir_X * cameraSpeed;
    float dz = cameraZ - dir_Z * cameraSpeed;

    //we call the function just to make sure we correctly update the color of red box to green if it is collided by the camera(same implementation as moveUp() method)
    collisionDetection(dx, dz);

    //this condition updates the camera to next backward position only if
    //movethrough is enabled(when w toggle is used by player) OR
    //if there is no collision for the next position of camera
    if (moveThrough || !collisionDetection(dx, dz)) {
        cameraX = dx;
        cameraZ = dz;
    }
}

//This method makes cube using triangles for each face
//each face consists of two triangles, so we have total of 12 triangles in each cube
//our method uses glBegin and glEnd to draw triangles
void cubeMaker() {
    glEnable(GL_LIGHTING);      //making sure we enable lighting before starting to draw our triangles
    glCullFace(GL_BACK);        //i had to use this feature for also enabling the lighting inside of the cube(as when the camera goes in the cube it's looked black which seemed like error)
    //So both the surface as well as inner part of the cube is colored with same color
//beggining drawing triangles
    
    glBegin(GL_QUADS);

    // FRONT face (+Z)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f); 
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f); 
    glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f); 
    glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); 
    glVertex3f(-1.0f, 1.0f, 1.0f);

    // BACK face (−Z)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); 
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); 
    glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); 
    glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); 
    glVertex3f(-1.0f, 1.0f, -1.0f);

    // RIGHT face (+X)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); 
    glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f); 
    glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); 
    glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f); 
    glVertex3f(1.0f, 1.0f, 1.0f);

    // LEFT face (−X)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); 
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); 
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); 
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f); 
    glVertex3f(-1.0f, 1.0f, -1.0f);

    // TOP face (+Y)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f); 
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f); 
    glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f); 
    glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f); 
    glVertex3f(-1.0f, 1.0f, -1.0f);

    // BOTTOM face (−Y)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f); 
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f); 
    glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f); 
    glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f); 
    glVertex3f(-1.0f, -1.0f, 1.0f);

    glEnd();
}

//This method draws the map and places walls and red rotating cubes
//It also handles different conditions for changing the state of any map items, like changing wall color, changing the color of red rotating boxes etc.
void mapDraw() {
    //looping over each row and coloumn of our initial array we defined for creating a maze
    for (int i = 0; i < mapRows; i++) {
        for (int j = 0; j < mapCols; j++) {

            //calculating the coordinates for each cell
            float xD = lef + j * cellBreadth;
            float zD = bot + i * cellHeight;

            //when we have 1 in our map array, we place wall in our map construction
            if (map[i][j] == 1) {
                float cubeHeight = 1.0f;    //heaight of each wall for the maze
                glPushMatrix();
                glTranslatef(xD + cellBreadth / 2.0f, cellHeight / 2.0f, zD + cellBreadth / 2.0f);  //make sure the cube is centered in each cell
                glScalef(cellBreadth / 2.0f, cellHeight / 2.0f, cellBreadth / 2.0f);        //we scale each cube to the size of a cell
                //glColor3f(0.3627f, 0.2451f, 0.0176f);       //setting the color to be yellow+brown before we start our drawing to make our walls
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, brickID);
                cubeMaker();        //calling our cubeMaker function, which makes a solid cube using triangles
                glDisable(GL_TEXTURE_2D);
                glPopMatrix();
            }
            //when we have 2 in our map array, we place red rotating cube in our map
            else if (map[i][j] == 2) {
                float cubeHeight = 0.32f;       //length for each side of the red cube
                glPushMatrix();
                glTranslatef(xD + cellBreadth / 2.0f, cellHeight / 2.0f, zD + cellHeight / 2.0f);  //y is floor
                glRotatef(rotAng, 0.0f, 1.0f, 0.0f);        //rotating the cube with some angle rotAng(which is constantly changed using the timer function)
                glScalef((cellBreadth / 2.0f) * cubeHeight, (cellHeight / 2.0f) * cubeHeight, (cellBreadth / 2.0f) * cubeHeight);
                glColor3f(1.0f, 1.0f, 1.0f);
                glEnable(GL_TEXTURE_2D);
                glBindTexture(GL_TEXTURE_2D, foodID);
                cubeMaker();                        //calling our cubeMaker function, which makes a solid cube using triangles
                glDisable(GL_TEXTURE_2D);
                glPopMatrix();
            }
            //when we have 3 in our map array(or if some cell changes to the value of 3), we place green rotating cube in our map
            else if (map[i][j] == 3) {
                float cubeHeight = 0.32f;   //length for each side of the green cube
                glPushMatrix();
                glTranslatef(xD + cellBreadth / 2.0f, cellHeight / 2.0f, zD + cellHeight / 2.0f);  //y is floor
                glRotatef(rotAng, 0.0f, 1.0f, 0.0f);
                glScalef((cellBreadth / 2.0f) * cubeHeight, (cellHeight / 2.0f) * cubeHeight, (cellBreadth / 2.0f) * cubeHeight);
                //glColor3f(0.0f, 0.8f, 0.0f);        //setting the color to green before starting to draw our cube
                cubeMaker();                        //calling our cubeMaker function, which makes a solid cube using triangles
                glPopMatrix();
            }
            //when we have 4 in our map array(this happens in our game end condition), we change all the walls to green color
            else if (map[i][j] == 4) {
                float cubeHeight = 1.0f;
                glPushMatrix();
                glTranslatef(xD + cellBreadth / 2.0f, cellHeight / 2.0f, zD + cellBreadth / 2.0f);  //y is floor
                glScalef(cellBreadth / 2.0f, cellHeight / 2.0f, cellBreadth / 2.0f);
                //glColor3f(0.0f, 0.8f, 0.0f);        //setting the color to green before starting to draw our cube
                cubeMaker();                        //calling our cubeMaker function, which makes a solid cube using triangles
                glPopMatrix();
            }
        }
    }
}

//This method updates the blue box after it is thrown from the camera/player
//It updates the position, and bouncing physics of the blue box along with implementing a lifespan
void bounceBlueBox() {

    //If the box is not initialized by user(space bar not pressed), than exiting early from the function
    if (!blueAlive || blueTimeSpan <= 0) {
        return;
    }

    //initializing the position of blue box to a seperate variable for checking any collisions 
    float oldX = blueX;
    float oldZ = blueZ;

    //now we calculate next position of the blue box which is determined by it's speed in specific direction
    blueX = blueX + speedX;
    blueZ = blueZ + speedZ;

    float t = (float)(200 - blueTimeSpan);      //calculating a value for bouncing motion of our cube, it is time based so ultimately reduces such that sine function gives a decreasing height for bounce each time

    //updating the y position using sine function to create a bouncing effect as sine has a parabolic/curve motion
    //the use of fabs() is to make sure we always have the sine function to return a position value so that our cube doesnot bounce from below the surface of our maze
    blueY = fabs(sin(t * 0.17f)) * blueBounce;

    //decreasing the value for bounce by 99.2% each time
    blueBounce *= 0.992f;

    //checking the collision between the old and new position of blue box for 5 different possible points.
    //i had to implement this way, as when checking for collision directly it was not always accurate
    const int checkCollisionSteps = 5;      //along the path
    for (int i = 1; i <= checkCollisionSteps; i++) {
        //calculating the factor for finding each point(lies between 0 to 1)
        float factor = i / (float)checkCollisionSteps;
        //finding the x and z coordinates along the path determined by old and next/new positions
        float checkX = oldX + factor * (blueX - oldX);
        float checkZ = oldZ + factor * (blueZ - oldZ);

        //checking for collision of blue box with red box or wall for specific point
        if (bluecollisionDetection(checkX, checkZ)) {
            // Stop the box at collision
            //stopping the box and disabling it immediately at the exact position for collision
            blueX = checkX;
            blueZ = checkZ;
            speedX = 0;
            speedZ = 0;
            blueAlive = false;
            blueTimeSpan = 0;
            return;
        }
    }

    //decreasing the lifespan of the blue box
    blueTimeSpan--;
    if (blueTimeSpan <= 0) {        //in current implementation the box stays on screen for around 4.5-5.2 seconds
        blueAlive = false;      //when the lifespan value reaces zero ultimately making the blue box disappear from screen
    }
}

//this timer function is called repeatedly for handlign the repeated/infinite events(like rotation of cube in our case)
void timerF(int val) {

    //update rotation angle for red box
    rotAng = rotAng + 2.5f;     //increasing angle by 2.5 degrees each time
    if (rotAng > 360.0f) {      //if angle goes beyong 360.0f resetting it back to 0 and starting over again
        rotAng = rotAng - 360.0f;
    }

    //using a seperate rotation for our blue box, to make it rotate a bit more than the red box(looks like a realistic throwing of a cube)
    blueRot = blueRot + 10.0f;
    if (blueRot > 360.0f) {
        blueRot = 0.0f;
    }

    bounceBlueBox();  // update blue box position for changed angle of rotation every 40ms

    glutPostRedisplay();        //everyrun of timer, says to redraw window now(timer->glurpostredisplay->display->glrotate

    //calling the timer function again after 40miliseconds
    //this helps create a loop that updates the scene around 25 frames per second making smooth appearance in screen
    glutTimerFunc(40, timerF, 0);       //calls the function every 15ms
}

//This method handles events when specific keys are pressed.
//In our current implementation the function handles events for UP, LEFT, RIGHT and DOWN arrow keys that is present in any standard keyboard
//On different keys, the function handles camera movement and rotation as needed
void specialKeys(int key, int x, int y) {
    float turning = 2.0f;       //initializing the amount by which our camera rotates per key press
    if (key == GLUT_KEY_RIGHT) {
        //Rotate the camera to the right by turning
        dire1 = dire1 + turning;
        //as the camera direction is changed, we also change the direction vector for placing the camera
        cameraDirection();
    }
    else if (key == GLUT_KEY_LEFT) {
        dire1 = dire1 - turning;       //rotate the camera to left by turning
        cameraDirection();     //updating the camera direction vectors for z,y and x directions
    }
    else if (key == GLUT_KEY_UP) {
        moveUp();     //calling our move up method which handles camera movement in forward direction
    }
    else if (key == GLUT_KEY_DOWN) {
        moveDown();       //calling our move back method which handles camera movement in backward direction
    }
    glutPostRedisplay(); //refresh after a move
}

//This is a seperate method for handling the key presses specifically for Toggle feature in our game
//it handles keyboard input for letter O, w and space bar and implements different behaviours
void keyboardpress(unsigned char key, int x, int y) {

    //Detecting of user pressed the letter o/O in keyboard
    //This keypress triggers the GL_LINE mode, which when pressed first it changes the GL_FILL mode for all the polygons in our maze to GL_LINE
    //the GL_LINE can be reverted back to GL_FILL by pressing the key for second time
    if (key == 'o' || key == 'O') {
        makeLines = !makeLines;         //setting our state of the maze game(i.e. if lines mode is enabled or disabled)
        if (makeLines) {
            glDisable(GL_LIGHTING);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);      //changing the mode to GL_LINE which shows lines for each polygon in the screen
        }
        else {
            glEnable(GL_LIGHTING);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);  //changing the mode to GL_FILL which makes all the polyggons to their regular mode with solid color in all of their face
        }
    }
    //Detecting if user pressed letter w/W in keyboard
    //This keypress enables or disables the ability for user to pass through walls of the maze(wall hack feature for our maze game)
    else if (key == 'w' || key == 'W') {
        moveThrough = !moveThrough;     //the movethrough toggle is handled in collision detections, where if movethrough is enabled than no collision is detected
    }
    //Detecting if user pressed letter space bar
    //This keypress triggers drawing a blue box into the screen which is thrown from the camera position and bounces towards the direction our camera is facing
    else if (key == ' ') {
        if (!blueAlive) {
            blueX = cameraX + dir_X * 0.1f;  // setting the x position for blue box equal to camera and adding a small padding/space so that it starts a bit in front of the camera
            blueY = 0.07f;                    // setting the starting height of the cube to be 0.07f above the ground
            blueZ = cameraZ + dir_Z * 0.1f;     //setting the z position for blue box and also adding a small padding equal to blueX

            //setting initial speed of the box thrown from the player/camera, these are relative to the direction where the camera is actually facing so that box is thrown straight forward
            speedX = dir_X * 0.002f;
            speedY = 0.0008f;
            speedZ = dir_Z * 0.0015f;

            blueRot = 0.0f;             //setting the rotation of box
            blueBounce = 0.08f;         //starting height for bouncing the red box
            blueTimeSpan = 150;         //around 4-5  seconds on screen if no collision
            blueAlive = true;           //seting the blue box is alive, so that our other methods triggers and creates bouncing box
        }
    }
}

//This method initializes camera positions, modes for the maze, sets lighting as well as creates a blue box into the screen when needed
//this function is called everytime the screen needs to be redrawn/refreshed
void display(void)
{
    //calculating the aspect ratio of our screen size
    float ratio = (float)windowX / (float)windowY;
    glClearColor(0.3f, 0.3f, 0.3f, 0.3f); // background colour of black(little grey)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the background for each next draw

    //setting up the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, ratio, 0.1, 100.0); // (FOV, aspect ratio, near plane, far plane)

    //setting up modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //placing our camera into the screen
    gluLookAt(
        cameraX, cameraY + 0.015f, cameraZ,   // eye position (camera position)
        cameraX + dir_X, cameraY + dir_Y, cameraZ + dir_Z,    // center (where the camera looks at)
        0.0, 1.0, 0.0     // up vector (usually Y-up)
    );
    //lights positions
    //enabling two sided lighting for both front and back faces of shapes drawn into the screen
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    GLfloat lightAmbient[] = { 0.01f, 0.01f, 0.01f, 1.0f };        //softness of light// so that all the object at least reflects something(no black)
    GLfloat lightDiffuse[] = { 0.13f, 0.13f, 0.13f, 1.0f };        //faces straight are brighter and away are darker
    GLfloat lightSpecular[] = { 0.3f, 0.3f, 0.3f, 1.0f };       //depends on viewer direction
    GLfloat lightPosition[] = { cameraX, cameraY, cameraZ, 1.0f };      //positioninig the light at camera position

    //applying the lighting properties we set to our LIGHT0 
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);     //setting the ambient component
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);     //setting the diffuse component
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);       //setting specular component
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);       //setting position of the light relative to camera position

    //drawing our maze after all the lightings are setup
    mapDraw();

    //drawing the blue box if it's active triggered when user presses space bar key
    if (blueAlive && blueTimeSpan > 0) {
        glPushMatrix();
        glTranslatef(blueX, blueY, blueZ);

        // rotating the blue box as it is created on the screen
        glRotatef(blueRot, 1.0f, 0.5f, 0.2f);
        //setting the color for the box to blue
        //glColor3f(0.0f, 0.0f, 0.8f);
        glutSolidCube(0.015f);      //using glut inbuilt function to create a solid cube, as we wanted a smaller cube 
        glPopMatrix();
    }

    //swapping front and back buffering
    glutSwapBuffers();
}

//This method is the main entry point for our maze game, 
// It sets the window sizes
// Sets the window title 
// Hides the cursor when it's inside thw window
// Initializes camera direction by calling cameraDirection function
int main(int argc, char** argv)
{
    // First set up GLUT
    glutInit(&argc, argv);

    //setting up the display mode as GLUT_DOUBLE for double buffering, GLUT_RGB for color mode and GLUT_DEPTH for 3D rendering
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // two buffers to prevent flicker, and red, green blue

    //setting the initial window size
    glutInitWindowSize(windowX, windowY);

    //setting the initial window position on the screen
    glutInitWindowPosition(100, 100);

    // Make the window with a title
    int windowHandle = glutCreateWindow("Maze Game 3d");
    glutSetWindow(windowHandle);

    //initializing camera direction before rendering anything in screen
    cameraDirection();

    //calling our display function which is called everytime the screen is refreshed
    glutDisplayFunc(display);
    //disabling the cursor when it's inside the game window(just a extra implementation)
    glutSetCursor(GLUT_CURSOR_NONE);
    //making sure to keep calling display function 
    glutIdleFunc(display);

    //calling special key handling methods(arrow keys in our case)
    glutSpecialFunc(specialKeys);
    //handling standard keyboard keys handling method like (w,o and space bar in our case)
    glutKeyboardFunc(keyboardpress);

    //seting up the timer function inside the main so that it is called repeatedly for smooth rotation of our maze objects
    glutTimerFunc(0, timerF, 0);

    //Enabling epth testing so that objects close to camera look a bit bigger/closer than the one that is far(real 3D effect)
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);      //enabling lighting(firstly switching to lighting mode)
    glEnable(GL_LIGHT0);        //and turning on a first light LIGHT0
    glEnable(GL_COLOR_MATERIAL);        //enablind color material so that our cube colors interact with the light coming our of LIGHT0
    //glEnable(GL_TEXTURE_2D);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glGenTextures(1, &brickID);
    glBindTexture(GL_TEXTURE_2D, brickID);
    generateWallTexture();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_SIZE, TEXTURE_SIZE, 0,
        GL_RGB, GL_UNSIGNED_BYTE, brickTexture);

    
    glGenTextures(1, &foodID);
    glBindTexture(GL_TEXTURE_2D, foodID);
    generateFoodTexture();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_SIZE, TEXTURE_SIZE, 0,
        GL_RGB, GL_UNSIGNED_BYTE, foodTexture);

    glutMainLoop();

    return 0; // required since main returns an int, but this line will never be reached
}