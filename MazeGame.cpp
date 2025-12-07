/******************************************************************************
*  File Name:    MazeGame.cpp
*  Author:       Raashtra KC
*  Date:         2025-10-15
*  Updates: 	 Added textures on 2025-12-05
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

//-------------------------------------------TEXTURING ADDITIONS-------------------------------------------//

//defining a global size for our texture generation, in our case we are defining texture of size 64*64
const int TEXTURE_SIZE = 64;

//creating a texture array for storing texture details for the brick pattern
//this array of size 64*64 stores color, i.e. values of R,G,B for each index/pixel of wall cubes/boxes
unsigned char brickTexture[TEXTURE_SIZE][TEXTURE_SIZE][3]; ///size three for assigning r,g,b(3 color values while generating texture)

//defining texture array for storing texture details for the food that are being spawned in map(total of 5 in our case)
//this array of size 64*64 stores color, i.e. values of R,G,B for each index/pixel of food cube
unsigned char foodTexture[TEXTURE_SIZE][TEXTURE_SIZE][3];

//defining texture array for storing texture details for the shootedCube from the camera(when SPACEBAR is pressed)
//this array of size 64*64 stores color, i.e. values of R,G,B for each index/pixel of shooted ball(fired cube) 
unsigned char fireTexture[TEXTURE_SIZE][TEXTURE_SIZE][3];

//defining an ADDITIONAL texture array for storing texture details for the floors in the map(to cover up the empty walking floor)
//this array of size 64*64 stores color, i.e. values of R,G,B for each index/pixel of floor(a 2D cube placed on the x plane(horizontal axis)
unsigned char floorTexture[TEXTURE_SIZE][TEXTURE_SIZE][3];

//Defining custom texture ID for each type of textures we have and generating them accordingly
//after generating a texture we store it's ID in these static GLuint variables(these are used to identify each textures)
static GLuint brickID, foodID, fireID, floorID;

//This function generates texture for the ball that is shot by the player by pressing SPACE BAR
//It creates circular pattern(like a type of circular illusion of black and white(rings bigger and bigger as it spreads out), but here we have pattern of blue colors)
//the rings/circular pattern created by this method follows the pattern, where it's center has white color, and as the circle becomes bigger we have darker and darker color(going towards dark blue)
//the final look on cube produces a white spot at the center of each side
void generateShootBoxTexture() {
    //Finding the center coordinates of the texture(which in our case is always 32)
    int centerX = TEXTURE_SIZE / 2;
    int centerY = TEXTURE_SIZE / 2;

    //looping through every texel(pixel) in the texture of size 64*64 that we defined and setting color for each pixel manually
    for (int col = 0; col < TEXTURE_SIZE; col++) {
        for (int row = 0; row < TEXTURE_SIZE; row++) {
            // Calculate distance of the current texel from the center point of the face/overall texture
            int dx = row - centerX;             //horizontal distance from the center
            int dy = col - centerY;             //vertical distance from the center
            //now calculating the exact distance using the distance formula
            float distance = sqrt(dx * dx + dy * dy);

            //now based on the distance from the center, we will assign different color to each of the pixel to create the circular ring pattern
            //we have different conditions for assigning colors ranging from bright to dark blue as the distance changes from close to far

            //for all the texels with distance from center less than 10
            //we assign a bright color(in other words is a white color)
            if (distance < 10) {
                //assigning white color to the specific col and row(index) of the array 
                fireTexture[col][row][0] = 255;
                fireTexture[col][row][1] = 255;
                fireTexture[col][row][2] = 255;
            }
            //with distance between 11 to 17
            //we assign a light blue color
            else if (distance < 18) {
                fireTexture[col][row][0] = 0;
                fireTexture[col][row][1] = 150;
                fireTexture[col][row][2] = 255;
            }
            //with distance between 18 to 25
            //we assign a moderate darker blue color
            else if (distance < 26) {
                fireTexture[col][row][0] = 0;
                fireTexture[col][row][1] = 100;
                fireTexture[col][row][2] = 200;
            }
            //with distance between 26 to 33
            //we assign a darker blue color
            else if (distance < 34) {
                fireTexture[col][row][0] = 0;
                fireTexture[col][row][1] = 60;
                fireTexture[col][row][2] = 130;
            }
            //with distance greater than 33,
            //we assign a fixed dark blue color
            else {
                fireTexture[col][row][0] = 0;
                fireTexture[col][row][1] = 30;
                fireTexture[col][row][2] = 80;
            }
        }
    }
}

//This function generates texture for our game/maze floor(Additional texture) to cover the empty floor we had while moving
//it creates a grass like texture for the floor
//for each texel of the texture we assign a green color but with some random noise added to each r,g and b values to make it look like grassy look
void generateFloorTexture() {
    //looping through every texel(pixel) in the texture of size 64*64 that we defined and setting color for each pixel manually
    for (int col = 0; col < TEXTURE_SIZE; col++) {
        for (int row = 0; row < TEXTURE_SIZE; row++) {
            //generating a random integer value to add to our default green color
            //it generates a value between 0 to 29
            int noise = (rand() % 30);

            //we have a default value for rgb as 20,100 and 20 which is dark green starting color and for each texel we add a noise that is generated using the random function
            int r = 20 + noise;
            int g = 100 + noise;
            int b = 20 + noise;

            //making sure we are always within the color range of 0 to 255
            if (r < 0) {
                r = 0;
            }
            if (r > 255) {
                r = 255;
            }
            if (g < 0) {
                g = 0;
            }
            if (g > 255) {
                g = 255;
            }
            if (b < 0) {
                b = 0;
            }
            if (b > 255){
                b = 255;
            }
            
            //assigning the green color with some random noise to make it look like grass surface
            floorTexture[col][row][0] = r;
            floorTexture[col][row][1] = g;
            floorTexture[col][row][2] = b;
        }
    }
}

 //This method creates a coin(yellow coing) on each side of the rotating food(cube), in our maze we have total of 5 cubes using this texture
//It draws a yellow coin(making it look like gold coin, like the one we have in classic running games)
//along with the coin, it also assigns a bit darker yelllow color to the texels outside the coin
void generateFoodTexture() {
    //Finding the center coordinates of the texture(which in our case is always 32)
    int centerX = TEXTURE_SIZE / 2;
    int centerY = TEXTURE_SIZE / 2;

    //defining the fixed radius for our coin(circle)
    int radius = 24; 

    //looping through every texel(pixel) in the texture of size 64*64 that we defined and setting color for each pixel manually
    for (int col = 0; col < TEXTURE_SIZE; col++) {
        for (int row = 0; row < TEXTURE_SIZE; row++) {
            // Calculate distance of the current texel from the center point of the face/overall texture
            int dx = row - centerX;              //horizontal distance from the center
            int dy = col - centerY;              //vertical distance from the center

            //now calculating the exact distance using the distance formula
            float distance = sqrt(dx * dx + dy * dy);

            //now based on the distance from the center, we will assign different color to each of the pixel to create a coin with a small border around it and a darker area outside the coin
            
            //now following the similar idea as generating the shooting ball
            //for when the distance of the current pixel is less than the radius we defines, we will assign a yellow/gold type of color for our coin
            if (distance < radius) {
                //assigning yellow/gold color to the specific col and row(index) of the array 
                foodTexture[col][row][0] = 255;
                foodTexture[col][row][1] = 215;
                foodTexture[col][row][2] = 0;
            }
            //for when the distance lies greater than the radius value and a small fixed value of 3
            //we draw the border for our yellow coin
            //the small +3 in the if condition helps set the border to orange(a bit dark+brighter color than the coin's color)
            else if (distance < radius + 3) {
                //assigning orange(light yellow type) color to the specific col and row(index) of the array 
                foodTexture[col][row][0] = 228;
                foodTexture[col][row][1] = 155;
                foodTexture[col][row][2] = 15;
            }
            //for when the distance is greater than the radius+3(outside the coin'g border) value
            //we set the color to a dark yellow color
            else {
                //assigning dark color to the specific col and row(index) of the array 
                foodTexture[col][row][0] = 139;
                foodTexture[col][row][1] = 128;
                foodTexture[col][row][2] = 0;
            }
        }
    }
}

void generateWallTexture() {
    int brickWidth = 22;   // pixel size per brick horizontally
    int brickHeight = 13;   // pixel size per brick vertically
    int gap = 1;         // thickness of border between two bricks in pixels

    //looping over every pixel of our defined 64 * 64 image texutre, which becomes our texture for the walls
    for (int col = 0; col < TEXTURE_SIZE; col++) {
        for (int row = 0; row < TEXTURE_SIZE; row++) {
            //initializing which row of the bricks we current are in,
            //dividing the current column with the brick height gives us specific row position
            //in our case we will get,
            //for row 0, we will have pixels 0-12
            //for row 1, we will have pixels 13-25
            int curRow = col / brickHeight;

            //Apply offset on every other row to just draw half brick making the pattern look more like wall
            int shiftRow = row;         //assigning the original position, and only adding the offsets for odd rows
            //checking if current row we are in is odd, and adding extra shift in the brick for making it look like real brick wall
            if (curRow % 2 == 1) {   
                shiftRow = row + brickWidth / 2;        //adding half a size of brick which makes odd row bricks to start in center of the normal bricks
            }

            //finding where the specific texel lies within the brick
            //for finding the boundry of brick in horizontal direction we take the modulo of the shifted row with the width of the brick
            //this gives us specific horizontal index brick in which the texel/pixel lies in
            int boundaryX = shiftRow % brickWidth;

            //similarly, we compute the vertical boundary for determining which specific brick the current texel lies vertically
            int boundaryY = col % brickHeight;

            //we have the following additional check to determine where do we have the border between the bricks
            //by calculate if current pixel is the gap between the bricks
            //by checking if any of the boundary variable of the brick is less than the defined gap size
            //which in our case, we defined the gap/border size between the bricks to be 1
            bool isGap = false;
            //checking if the current boundary/brick position is actually the border or the brick
            if ((boundaryY < gap) || (boundaryX < gap)) {
                isGap = true;
            }

            //if the current texel is actually a gap between two bricks, than we assign a different gray color
            if (isGap) {
                // Light gray color for the gap between the bricks(like real mortar color)
                brickTexture[col][row][0] = 220;
                brickTexture[col][row][1] = 220;
                brickTexture[col][row][2] = 220;
            }
            //if the current texel doesnot fall in the gap area between the bricks, than we draw a red bricks 
            else {
                // Brick color setup with red value to 150
                brickTexture[col][row][0] = 150 ;      // red
                brickTexture[col][row][1] = 0;    //Green value
                brickTexture[col][row][2] = 0;   //Blue value
            }
        }
    }
}

//----------------------------------------------------------------------------------------------------------//

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
//each face also has texture coordinates for applying textures into it
void cubeMaker() {
    glEnable(GL_LIGHTING);      //making sure we enable lighting before starting to draw our triangles
    glCullFace(GL_BACK);        //i had to use this feature for also enabling the lighting inside of the cube(as when the camera goes in the cube it's looked black which seemed like error)
    //So both the surface as well as inner part of the cube is colored with same color
//beggining drawing triangles
    
    glBegin(GL_QUADS);
    //mapping the texture that we generated to the corners of the quads(squares)
    
    // FRONT face (+Z)
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);               //0,0 is the bottom left of the texture 
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f);               //1,0 bottom right of the texture
    glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);               //1, 1top right corner
    glVertex3f(1.0f, 1.0f, 1.0f);   
    glTexCoord2f(0.0f, 1.0f);               //0,1 top left corner
    glVertex3f(-1.0f, 1.0f, 1.0f);

    // BACK face (−Z)
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);               //0,0 is the bottom left of the texture 
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);               //0,1 top left corner
    glVertex3f(-1.0f, 1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);               //1, 1top right corner
    glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);               //1,0 bottom right of the texture
    glVertex3f(1.0f, -1.0f, -1.0f);

    // RIGHT face (+X)
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);               //0,0 is the bottom left of the texture 
    glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f);               //1,0 bottom right of the texture
    glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);               //1, 1top right corner
    glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);               //0,1 top left corner
    glVertex3f(1.0f, 1.0f, 1.0f);

    // LEFT face (−X)
    glNormal3f(-1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);          //1,0 bottom right of the texture
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);           //0,0 is the bottom left of the texture 
    glVertex3f(-1.0f, -1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);           //0,1 top left corner
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);           //1, 1top right corner
    glVertex3f(-1.0f, 1.0f, -1.0f);

    // TOP face (+Y)
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);               //0,1 top left corner
    glVertex3f(-1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 1.0f);               //1, 1top right corner
    glVertex3f(1.0f, 1.0f, 1.0f);
    glTexCoord2f(1.0f, 0.0f);               //1,0 bottom right of the texture
    glVertex3f(1.0f, 1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);               //0,0 is the bottom left of the texture 
    glVertex3f(-1.0f, 1.0f, -1.0f);

    // BOTTOM face (−Y)
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);               //0,0 is the bottom left of the texture 
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);               //1,0 bottom right of the texture
    glVertex3f(1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);               //1, 1top right corner
    glVertex3f(1.0f, -1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);               //0,1 top left corner
    glVertex3f(-1.0f, -1.0f, 1.0f);

    glEnd();
}

//This method draws a solid square for the floor in our map.
//it has just one face drawn using GL_QUADS
//the main purpose of drawing the floor is for applying a floor/green texture to the map 
//it follows the same methods as used in drawing the walls of the map, which uses translation and scaling to map the square to our defined world
//This method draws the square, along with applying the green texture to it
void floorDrawer(float xD, float zD) {
    glPushMatrix();
    glTranslatef(xD + cellBreadth / 2.0f, cellHeight / 2.0f, zD + cellBreadth / 2.0f);  //make sure the cube is centered in each cell
    glScalef(cellBreadth / 2.0f, cellHeight / 2.0f, cellBreadth / 2.0f);        //we scale each cube to the size of a cell
    glColor3f(1.0f, 1.0f, 1.0f);  // White to show texture
    glEnable(GL_TEXTURE_2D);                    //enabling 2D texturing
    glBindTexture(GL_TEXTURE_2D, floorID);      //bounding the current matrix drawing with the floor texture which we generated using generateFloorTexture method

    glBegin(GL_QUADS);
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

    glDisable(GL_TEXTURE_2D);   //turning off the texturing
    glPopMatrix();
}

//This method draws the map and places walls and red rotating cubes
// when drawing each items, the method makes sure to properly turn on and off the texturing modes, and also assigns the specific textures for each game objects(in our case we used texture for wall, bouncing box, rotating food/box, and additionally floor)
//It also handles different conditions for changing the state of any map items, like changing wall color, changing the color of red rotating boxes etc.
void mapDraw() {
    //looping over each row and coloumn of our initial array we defined for creating a maze
    for (int i = 0; i < mapRows; i++) {
        for (int j = 0; j < mapCols; j++) {

            //calculating the coordinates for each cell
            float xD = lef + j * cellBreadth;
            float zD = bot + i * cellHeight;
            
            //adding a new condition for drawing the floor(drawing a solid square for when the map has 0)
            if (map[i][j] == 0) {
                //this floor is drawn ensuring no collision with floor is detected(which is handled inside the collision detection fuction, where the collision is only checked for map value fo 1
                floorDrawer(xD, zD);    //calling our floowdrawer method to draw a green looking floor in the map where we have nothing/empty floor space, i.e map value =0
            }
            //when we have 1 in our map array, we place wall in our map construction
            else if (map[i][j] == 1) {
                float cubeHeight = 1.0f;    //heaight of each wall for the maze
                glPushMatrix();
                glTranslatef(xD + cellBreadth / 2.0f, cellHeight / 2.0f, zD + cellBreadth / 2.0f);  //make sure the cube is centered in each cell
                glScalef(cellBreadth / 2.0f, cellHeight / 2.0f, cellBreadth / 2.0f);        //we scale each cube to the size of a cell
                glColor3f(1.0f, 1.0f, 1.0f);        // Defining a starting color as white(because when directly applying the texture, for some reason when food was eaten, the texture on a random single wall was being changed
                //drawing our wall cubes by enabling texture and calling the cube drawing method
                glEnable(GL_TEXTURE_2D);                    //turning on the texturing
                glBindTexture(GL_TEXTURE_2D, brickID);      //using the brickID which stores our texture for brick wall
                cubeMaker();                                //calling our cubeMaker function, which makes a solid cube using triangles
                glDisable(GL_TEXTURE_2D);                   //disabling the texturing after each cubes are drawn
                glPopMatrix();
            }
            //when we have 2 in our map array, we place red rotating cube in our map
            else if (map[i][j] == 2) {
                floorDrawer(xD, zD);        //we are also calling the floor drawing method first to cover up the empty space we have below our rotating box

                float cubeHeight = 0.32f;       //length for each side of the red cube
                glPushMatrix();
                glTranslatef(xD + cellBreadth / 2.0f, cellHeight / 2.0f, zD + cellHeight / 2.0f);  //y is floor
                glRotatef(rotAng, 0.0f, 1.0f, 0.0f);        //rotating the cube with some angle rotAng(which is constantly changed using the timer function)
                glScalef((cellBreadth / 2.0f) * cubeHeight, (cellHeight / 2.0f) * cubeHeight, (cellBreadth / 2.0f) * cubeHeight);
                glColor3f(1.0f, 1.0f, 1.0f);        //defining a starting color for the rotating cube as white, and pasting a texture above it      
                //drawing our eatable cubes by enabling texture and calling the cube drawing method
                glEnable(GL_TEXTURE_2D);                    //turning on the texturing
                glBindTexture(GL_TEXTURE_2D, foodID);       //using the foodID which stores our texture for food(ie. coin type of look)
                cubeMaker();                                //calling our cubeMaker function, which makes a solid cube using triangles
                glDisable(GL_TEXTURE_2D);                   //disabling the texturing after each cubes are drawn
                glPopMatrix();
            }
            //when we have 3 in our map array(or if some cell changes to the value of 3), we place green rotating cube in our map
            else if (map[i][j] == 3) {
                floorDrawer(xD, zD);    //we are calling the floor drawing method first to cover up the empty space we have below our rotating box

                float cubeHeight = 0.32f;   //length for each side of the green cube
                glPushMatrix();
                glTranslatef(xD + cellBreadth / 2.0f, cellHeight / 2.0f, zD + cellHeight / 2.0f);  //y is floor
                glRotatef(rotAng, 0.0f, 1.0f, 0.0f);
                glScalef((cellBreadth / 2.0f) * cubeHeight, (cellHeight / 2.0f) * cubeHeight, (cellBreadth / 2.0f) * cubeHeight);

                glColor3f(0.0f, 1.0f, 0.0f);  //setting initial color to green, to indicate the captured box(texture will be applied on green, making it look like green/dead box
                glEnable(GL_TEXTURE_2D);                    //turning on the texturing
                glBindTexture(GL_TEXTURE_2D, foodID);           //using the foodID which stores our texture for food(ie. coin type of look)
                cubeMaker();                        //calling our cubeMaker function, which makes a solid cube using triangles
                glDisable(GL_TEXTURE_2D);               //disabling the texturing after each cubes are drawn
                glPopMatrix();
            }
            //when we have 4 in our map array(this happens in our game end condition), we change all the walls to green color
            else if (map[i][j] == 4) {
                float cubeHeight = 1.0f;
                glPushMatrix();
                glTranslatef(xD + cellBreadth / 2.0f, cellHeight / 2.0f, zD + cellBreadth / 2.0f);  //y is floor
                glScalef(cellBreadth / 2.0f, cellHeight / 2.0f, cellBreadth / 2.0f);

                glColor3f(0.0f, 1.0f, 0.0f);  //Setting the initial color the green making our red brich wall to appear dark green(indicating game end condition)
                glEnable(GL_TEXTURE_2D);                //turning on the texturing
                glBindTexture(GL_TEXTURE_2D, brickID);  //using the brickID which stores our texture forbrick wall
                cubeMaker();    //calling our cubeMaker function, which makes a solid cube using triangles
                glDisable(GL_TEXTURE_2D);               //disabling the texturing after each cubes are drawn
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
// the method also properly assigns the specific bouncing ball texture to the cube being drawn
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
        //the blue color for the box is applied as a texture where we generate a blue pattern for the box being fired
        glRotatef(blueRot, 1.0f, 0.5f, 0.2f);
        // Scale it down to make it small 
        // Half of 0.015f, in the Maze lab, i actually used the inbuilt method for drawing the cube by defining the size. 
        //here since we need to apply texture i am using the same cubemaker function to draw the cube by scaling down the size
        glScalef(0.0075f, 0.0075f, 0.0075f);  

        glColor3f(1.0f, 1.0f, 1.0f);  // White to show texture
        glEnable(GL_TEXTURE_2D);    //turning on the texturing
        glBindTexture(GL_TEXTURE_2D, fireID);           //using the fireID which stores our texture for the bouncing box that is fired
        cubeMaker();  // Use your custom function instead
        glDisable(GL_TEXTURE_2D);               //disabling the texture 
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
//enables all the main componenets like DEPTH test, lightning, setup all the textures and stores it in a specific textureID
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
    int windowHandle = glutCreateWindow("Maze Game 3D");
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
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    //-----------------------------------TEXTURING SETUP----------------------------------//
    //this specific part makes all our textures before starting the main game
    //we create four different textures that we are using in our maze, i.e. brick texxture, coin texture(for rotating red cube), blue ring pattern(for bouncing box), and green grass type of pattern for our floor

    //generating first texture for Brick pattern in wall
    glGenTextures(1, &brickID);             //creating a texture and generates a ID, which is assigned to brickID since we are passing it by reference
    glBindTexture(GL_TEXTURE_2D, brickID);  //enabling the brickID texture to make any changes to it
    generateWallTexture();                     //calling our method to initialize the texture array with brick pattern(different rgb values to make brick patttern)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);           //here we are using min_filter and gl_linear to smooth our textures that we appied to wall, so when seen from distance, it looks smooth(and looks blending properly with out lighting asw ell)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);           //here we are using mag_filter and gl_linear, to also smooth and blend our texture but in this case, it will make the textures smooth which are near the camera or zoomed in 
    //here we are using glTexImage2D, to make our array into a texture and store it for using it
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_SIZE, TEXTURE_SIZE, 0,
        GL_RGB, GL_UNSIGNED_BYTE, brickTexture);

    //Generating second texture for rotating cube
    glGenTextures(1, &foodID);                      //creating a texture and generates a ID, which is assigned to foodID since we are passing it by reference
    glBindTexture(GL_TEXTURE_2D, foodID);           //enabling the foodID texture to make any changes to it
    generateFoodTexture();               //calling our method to initialize the texture array with coing pattern for our cube, which makes all sides of the cube to make coin shape
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);           //using min_filter and gl_linear to smooth our textures that we appied to wall
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);           //here we are using mag_filter and gl_linear, to also smooth and blend our texture but in this case, it will make the textures smooth which are near the camera or zoomed in 
    //here we are using glTexImage2D, to make our array into a texture and store it for using it
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_SIZE, TEXTURE_SIZE, 0,
        GL_RGB, GL_UNSIGNED_BYTE, foodTexture);

    //Generating third texture for bouncing cube
    glGenTextures(1, &fireID);                  //creating a texture and generates a ID, which is assigned to fireID since we are passing it by reference
    glBindTexture(GL_TEXTURE_2D, fireID);           //enabling the fireID texture to make any changes to it
    generateShootBoxTexture();                  //calling our method to initialize the texture array with increasing circular pattern where for each circle there is different color of blue(inner to outer circle -->>  bright to darker blue
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);           //using min_filter and gl_linear to smooth our textures that we appied to wall
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);               //here we are using mag_filter and gl_linear, to also smooth and blend our textures
    //here we are using glTexImage2D, to make our array into a texture and store it for using it
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_SIZE, TEXTURE_SIZE, 0,
        GL_RGB, GL_UNSIGNED_BYTE, fireTexture);

    //Generating fourth texture for the floor(every position where in our map array we have the value of 0)
    glGenTextures(1, &floorID);                         //creating a texture and generates a ID, which is assigned to floorID since we are passing it by reference
    glBindTexture(GL_TEXTURE_2D, floorID);               //enabling the floorID texture to make any changes to it
    generateFloorTexture();                 //calling our method to initialize the texture array with a simple solid green color(with a random noise to make it look like grass)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);       //using min_filter and gl_linear to smooth our textures that we appied to wall
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);            //here we are using mag_filter and gl_linear, to also smooth and blend our textures
    //here we are using glTexImage2D, to make our array into a texture and store it for using it
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_SIZE, TEXTURE_SIZE, 0,
        GL_RGB, GL_UNSIGNED_BYTE, floorTexture);

    //------------------------------------------------------------------------------------//

    glutMainLoop();

    return 0; // required since main returns an int, but this line will never be reached
}