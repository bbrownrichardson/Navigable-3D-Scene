#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <windows.h>
#include <GL/glut.h>
#endif

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "glm.h"
#include "tgaClass.h"

#define PI 3.14159

// Pointers to models
GLMmodel* houseModel;
GLMmodel* houseModel2;
GLMmodel* slat;
GLMmodel* foundation;
GLMmodel* leftColumn;
GLMmodel* lowerRight;
GLMmodel* upperRight;
GLMmodel* crowsNest;
GLMmodel* balcony;
GLMmodel* peugeot;
GLMmodel* bmw;

// trackball variables
GLfloat trackball_angle = 0;   /* in degrees */
GLfloat trackball_angle2 = 0;   /* in degrees */
int trackball_moving_x = 0, trackball_moving_y = 0, trackball_startx=0, trackball_starty=0;
float navigation_damper = 0.3f;
GLint win_width = 400;			// Window dimensions
GLint win_height = 300;

// current position of the observer
GLfloat eyex = 0.0;
GLfloat eyey = 1.0;
GLfloat eyez = 10.0;

// direction vector of the observer; used as the center parameters in gluLookAt
GLfloat center_x;
GLfloat center_y;
GLfloat center_z;

/* State vars */
bool drawGuides = 0;

// store vertices of invisible bounding boxes used for collision detection
// An array of 9 bounding boxes, each with 3 components (x, y, z), each consisting of a min and a max (2)
// while not a perfect solution, manually calculating and hard-coding these was the most practical implementation in this case, especially given the small number of collidable objects
float boundingBoxVertices[9][3][2] = {
  {{-4.825, -0.175}, {0, 4.5},{-3.9, 3}},
  {{-1.2, 3.1}, {0, 4.5}, {-3.225, 2.5}},
  {{2.545, 4.3}, {0, 4.5}, {-3.5, -0.35}},
  {{5.65, 8.35}, {0, 1.0}, {10.4, 11.6}},
  {{-4.5, -3.5}, {0, 1.0}, {5.9, 8.1}},
  {{-13.5, 24.5}, {0, 1.2}, {-10.0, -9.5}},
  {{-13.5, 24.5}, {0, 1.2}, {24.5, 25.0}},
  {{-14.0, -13.5}, {0, 1.2}, {-10, 25.0}},
  {{24.5, 25.0}, {0, 1.2}, {-10, 25.0}},
};

/* Texture Vars */
#define MAX_TEXTURES 100
#define GRASS_ID 0
#define PEUGEOT_ID 1
#define DRYWALL_ID 2
#define CONCRETE_ID 3
#define WOOD_ID 4
#define BRICK_ID 5
#define FENCE_ID 6
unsigned int textureArray[MAX_TEXTURES] = {0};

using namespace std;

void createTexture(unsigned int textureArray[], char * strFileName, int textureID);
void set_projection ();
void origin();
bool isCollidedWithObstacle();
void drawGrass();
void drawHouse();
void drawPeugeot();
void drawBmw();
void drawBoundingBoxes();
void drawDriveway();
void drawFence();

void init()
{
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /* Setup initial OpenGL rendering state. */
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    // light0 is the positional light that follows the observer
    glEnable(GL_LIGHTING);
    GLfloat ambient[4] = {0.45, 0.45, 0.45, 0.0};
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, ambient);
    glLightfv(GL_LIGHT0, GL_SPECULAR, ambient);

    // extra lights, just to lighten the scene a bit
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
    GLfloat light1_pos[4] = {10.0, 0.0, 0.0, -1.0};
    glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);

    glEnable(GL_LIGHT2);
    glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
    GLfloat light2_pos[4] = {-10.0, 0.0, 0.0, 1.0};
    glLightfv(GL_LIGHT2, GL_POSITION, light2_pos);

    /* Load textures */
    createTexture(textureArray, "grass2.tga", GRASS_ID);
    createTexture(textureArray, "assets/fenceTexture.tga", FENCE_ID);
    createTexture(textureArray, "assets/car.tga", PEUGEOT_ID);
    createTexture(textureArray, "assets/drywallTexture.tga", DRYWALL_ID);
    createTexture(textureArray, "assets/concreteTexture.tga", CONCRETE_ID);
    createTexture(textureArray, "assets/woodTexture.tga", WOOD_ID);
    createTexture(textureArray, "assets/brickTexture.tga", BRICK_ID);

    // Init models
    // foundation
    foundation = (GLMmodel*)malloc(sizeof(GLMmodel));
    foundation = glmReadOBJ("models/foundation.obj");
    glmLinearTexture(foundation);
    // leftColumn
    leftColumn = (GLMmodel*)malloc(sizeof(GLMmodel));
    leftColumn = glmReadOBJ("models/leftColumn.obj");
    glmLinearTexture(leftColumn);
    // lowerRight
    lowerRight = (GLMmodel*)malloc(sizeof(GLMmodel));
    lowerRight = glmReadOBJ("models/lowerRight.obj");
    glmLinearTexture(lowerRight);
    // upperRight
    upperRight = (GLMmodel*)malloc(sizeof(GLMmodel));
    upperRight = glmReadOBJ("models/upperRight.obj");
    glmLinearTexture(upperRight);
    // crowsNest
    crowsNest = (GLMmodel*)malloc(sizeof(GLMmodel));
    crowsNest = glmReadOBJ("models/crowsNest.obj");
    glmLinearTexture(crowsNest);
    // balcony
    balcony = (GLMmodel*)malloc(sizeof(GLMmodel));
    balcony = glmReadOBJ("models/balcony.obj");
    glmLinearTexture(balcony);
    // slat
    slat = (GLMmodel*)malloc(sizeof(GLMmodel));
    slat = glmReadOBJ("models/slat.obj");
    glmLinearTexture(slat);
    // peugeot car
    peugeot = (GLMmodel*)malloc(sizeof(GLMmodel));
    peugeot = glmReadOBJ("models/peugeot.obj");
    glmLinearTexture(peugeot);
    // bmw car
    bmw = (GLMmodel*)malloc(sizeof(GLMmodel));
    bmw = glmReadOBJ("models/bmw.obj");
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    set_projection();

    // position light0 at the observer's location
    GLfloat light_pos[4] = {eyex, eyey, eyez, 1};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    // set gluLookAt to create a first-person navigation effect
    // eyex, eyey, and eyez are the observer's current location
    // the center variables give a direction vector for the direction the observer is facing. This vector added to the current position give the point the observer is looking at
    gluLookAt(eyex, eyey, eyez, eyex + center_x, eyey + center_y, eyez + center_z, 0.0, 1.0, 0.0);

    // if true, draw axes and gridlines (for dev purposes)
    if (drawGuides) {
      origin();
      drawBoundingBoxes();
    }

    glEnable(GL_TEXTURE_2D);
    // draw and texture the grass
    drawGrass();

    // draw and texture the driveway
    drawDriveway();

    // draw and texture the fences
    drawFence();

    // draw and texture the first house at the origin
    drawHouse();

    // draw and texture the second house after transformations
    glPushMatrix();
      glTranslatef(15.0, 0.0, 15.0);
      glRotatef(-90.0, 0.0, 1.0, 0.0);
      drawHouse();
    glPopMatrix();

    // draw the Peugeot car in front of the first house
    glPushMatrix();
      glTranslatef(-4.0, 0.5, 7.0);
      glRotatef(-90.0, 0.0, 1.0, 0.0);
      glScalef(2.0, 2.0, 2.0);
      drawPeugeot();
    glPopMatrix();

    // draw the BMW car in front of the second house
    glPushMatrix();
      glTranslatef(7.0, 0.0, 11.0);
      glRotatef(180.0, 0.0, 1.0, 0.0);
      glScalef(0.015, 0.015, 0.015);
      drawBmw();
    glPopMatrix();

    // checks if observer is touching any of the obstacles; if so, prints a message to the console (observer is also prevented from continuing to move forward; that is handled in the keyboard function)
    if (isCollidedWithObstacle()) {
      printf("Collision Detected; eyex: %f\n", eyex);
    }

    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

// createTexture function taken from skybox example code. Original documentation left in.
void createTexture(unsigned int textureArray[], char * strFileName, int textureID)
{
	char buffer[30];	//filename holder
	int status = 0;		//error codes for file read
	TGA myTGAImage;

	if(!strFileName)									// Return from the function if no file name was passed in
		return;

	sprintf (buffer,strFileName);		//load buffer with filename
	status = myTGAImage.readTGA(buffer);// Load into a TGA object
	if(status > 1) {
		cout <<"Error occurred = " << status <<strFileName<< endl;
		exit(0);
	}


	// Now that we have the texture data, we need to register our texture with OpenGL
	// To do this we need to call glGenTextures().  The 1 for the first parameter is
	// how many texture we want to register this time (we could do a bunch in a row).
	// The second parameter is the array index that will hold the reference to this texture.

	// Generate a texture with the associative texture ID stored in the array
	glGenTextures((GLsizei)1, (GLuint *) &textureArray[textureID]);

	// This sets the alignment requirements for the start of each pixel row in memory.
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	// Now that we have a reference for the texture, we need to bind the texture
	// to tell OpenGL this is the reference that we are assigning the bitmap data too.
	// The first parameter tells OpenGL we want are using a 2D texture, while the
	// second parameter passes in the reference we are going to assign the texture too.
	// We will use this function later to tell OpenGL we want to use this texture to texture map.

	// Bind the texture to the texture arrays index and init the texture
	glBindTexture(GL_TEXTURE_2D, textureArray[textureID]);

	// Now comes the important part, we actually pass in all the data from the bitmap to
	// create the texture. Here is what the parameters mean in gluBuild2DMipmaps():
	// (We want a 2D texture, 3 channels (RGB), bitmap width, bitmap height, It's an RGB format,
	//  the data is stored as unsigned bytes, and the actuall pixel data);

	// What is a Mip map?  Mip maps are a bunch of scaled pictures from the original.  This makes
	// it look better when we are near and farther away from the texture map.  It chooses the
	// best looking scaled size depending on where the camera is according to the texture map.
	// Otherwise, if we didn't use mip maps, it would scale the original UP and down which would
	// look not so good when we got far away or up close, it would look pixelated.

	// Build Mipmaps (builds different versions of the picture for distances - looks better)
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3,  myTGAImage.width,
					  myTGAImage.height, myTGAImage.format, GL_UNSIGNED_BYTE, myTGAImage.data);

	// Lastly, we need to tell OpenGL the quality of our texture map.  GL_LINEAR is the smoothest.
	// GL_NEAREST is faster than GL_LINEAR, but looks blochy and pixelated.  Good for slower computers though.
	// Read more about the MIN and MAG filters at the bottom of main.cpp

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	// The default GL_TEXTURE_WRAP_S and ""_WRAP_T property is GL_REPEAT.
	// We need to turn this to GL_CLAMP, otherwise it creates ugly seems
	// in our sky box.  GL_CLAMP does not repeat when bound to an object.
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

}

// draws axes and grid lines as developer guides
void origin(){
    glPushAttrib(GL_CURRENT_BIT);

    glBegin(GL_LINES);
        glColor3f(1.0, 0.0, 0.0);
        GLfloat x_mat[3] = {1.0, 0.0, 0.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, x_mat);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, x_mat);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, x_mat);
          glVertex3f(-100.0, 0, 0);
          glVertex3f(100.0, 0, 0);
        glColor3f(0.0, 1.0, 0.0);
        GLfloat y_mat[3] = {0.0, 1.0, 0.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, y_mat);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, y_mat);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, y_mat);
          glVertex3f(0, -100.0, 0);
          glVertex3f(0, 100.0, 0);
        glColor3f(0.0, 0.0, 1.0);
        GLfloat z_mat[3] = {0.0, 0.0, 1.0};
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, z_mat);
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, z_mat);
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, z_mat);
          glVertex3f(0, 0, -100.0);
          glVertex3f(0, 0, 100.0);
      glEnd();

      // draw grid lines
      GLfloat x_grid_mat[3] = {3.333, 3.333, 3.333};
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, x_grid_mat);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, x_grid_mat);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, x_grid_mat);
      glBegin(GL_LINES);
        for (int k=1; k < 50; k++) {
          glVertex3f(k, 0, -50.0);
          glVertex3f(k, 0, 50.0);

          glVertex3f(-k, 0, -50.0);
          glVertex3f(-k, 0, 50.0);
        }
      glEnd();

      GLfloat z_grid_mat[3] = {3.333, 3.333, 3.333};
      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, z_grid_mat);
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, z_grid_mat);
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, z_grid_mat);
      glBegin(GL_LINES);
        for (int k=1; k < 50; k++) {
          glVertex3f(-50.0, 0, k);
          glVertex3f(50.0, 0, k);

          glVertex3f(-50.0, 0, -k);
          glVertex3f(50.0, 0, -k);
        }
      glEnd();

    glPopAttrib();
}

// draw and texture grass
void drawGrass() {
  // bind texture
  glBindTexture(GL_TEXTURE_2D, textureArray[GRASS_ID]);

  // repeat 80 copies of texture image over 200 x 200 plane
  glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-100, 0, -100);

    glTexCoord2f(80.0, 0.0);
    glVertex3f(100, 0, -100.0);

    glTexCoord2f(80.0, 80.0);
    glVertex3f(100, 0, 100.0);

    glTexCoord2f(0.0, 80.0);
    glVertex3f(-100, 0, 100.0);
  glEnd();
}

// draw and texture driveway
void drawDriveway () {
  glEnable(GL_TEXTURE_2D);
  // bind previously intialized concrete texture
  glBindTexture(GL_TEXTURE_2D, textureArray[CONCRETE_ID]);
  // draw 5 copies of asphalt texture
  glBegin(GL_QUADS);
    glTexCoord2d(0.0, 5.0);
    glVertex3f(-6.0, 0.0001, 4.0);

    glTexCoord2d(0.0, 0.0);
    glVertex3f(-6.0, 0.0001, 10.0);

    glTexCoord2d(5.0, 0.0);
    glVertex3f(-3.0, 0.0001, 10.0);

    glTexCoord2d(5.0, 5.0);
    glVertex3f(-3.0, 0.0001, 4.0);
  glEnd();
  glDisable(GL_TEXTURE_2D);
}

// draw and texture fence
void drawFence () {
  glEnable(GL_TEXTURE_2D);
  // bind previously initialized fence texture
  glBindTexture(GL_TEXTURE_2D, textureArray[FENCE_ID]);
  // draw four sides of fence, each with 20 copies of fence texture
  glBegin(GL_QUADS);
    // side one
    glTexCoord2d(0.0, 0.0);
    glVertex3f(-14, 0.0, -10.0);

    glTexCoord2d(20.0, 0.0);
    glVertex3f(25, 0.0, -10.0);

    glTexCoord2d(20.0, 20.0);
    glVertex3f(25, 1.2, -10.0);

    glTexCoord2d(0.0, 20.0);
    glVertex3f(-14, 1.2, -10.0);

    // side two
    glTexCoord2d(0.0, 0.0);
    glVertex3f(25, 0.0, -10.0);

    glTexCoord2d(20.0, 0.0);
    glVertex3f(25, 0.0, 25.0);

    glTexCoord2d(20.0, 20.0);
    glVertex3f(25, 1.2, 25.0);

    glTexCoord2d(0.0, 20.0);
    glVertex3f(25, 1.2, -10.0);

    // side three
    glTexCoord2d(0.0, 0.0);
    glVertex3f(-14, 0.0, 25.0);

    glTexCoord2d(20.0, 0.0);
    glVertex3f(25, 0.0, 25.0);

    glTexCoord2d(20.0, 20.0);
    glVertex3f(25, 1.2, 25.0);

    glTexCoord2d(0.0, 20.0);
    glVertex3f(-14, 1.2, 25.0);

    // side four
    glTexCoord2d(0.0, 0.0);
    glVertex3f(-14, 0.0, -10.0);

    glTexCoord2d(20.0, 0.0);
    glVertex3f(-14, 0.0, 25.0);

    glTexCoord2d(20.0, 20.0);
    glVertex3f(-14, 1.2, 25.0);

    glTexCoord2d(0.0, 20.0);
    glVertex3f(-14, 1.2, -10.0);

  glEnd();
  glDisable(GL_TEXTURE_2D);
}

// draw house imported from obj
void drawHouse() {
  glPushMatrix();
  glEnable(GL_TEXTURE_2D);

  // -- Draw and texture each part of the house
  // draw leftColumn
  glBindTexture(GL_TEXTURE_2D, textureArray[WOOD_ID]);
  glmDraw(leftColumn, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);
  // draw lowerRight
  glBindTexture(GL_TEXTURE_2D, textureArray[BRICK_ID]);
  glmDraw(lowerRight, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);
  // draw upperRight
  glBindTexture(GL_TEXTURE_2D, textureArray[BRICK_ID]);
  glmDraw(upperRight, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);
  // draw slat
  glBindTexture(GL_TEXTURE_2D, textureArray[DRYWALL_ID]);
  glmDraw(slat, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);
  // draw crowsNest
  glBindTexture(GL_TEXTURE_2D, textureArray[DRYWALL_ID]);
  glmDraw(crowsNest, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);
  // draw balcony
  glBindTexture(GL_TEXTURE_2D, textureArray[BRICK_ID]);
  glmDraw(balcony, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);
  // draw foundation
  glBindTexture(GL_TEXTURE_2D, textureArray[CONCRETE_ID]);
  glmDraw(foundation, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL);

  glDisable(GL_TEXTURE_2D);

  glPopMatrix();
}

// draw and texture Peugeot car, loaded from OBJ
void drawPeugeot() {
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, textureArray[PEUGEOT_ID]);
  glmDraw(peugeot, GLM_SMOOTH | GLM_MATERIAL | GLM_TEXTURE);
  glDisable(GL_TEXTURE_2D);
}

// draw and texture BMW car, loaded from OBJ
void drawBmw () {
  glmDraw(bmw, GLM_SMOOTH | GLM_MATERIAL);
}

// iteratively check if the observer has collided with any of the 9 invisible bounding boxes
bool isCollidedWithObstacle () {
  for (int k=0; k < 9; k++) {
    if ((eyex >= boundingBoxVertices[k][0][0] && eyex <= boundingBoxVertices[k][0][1]) && (eyey >= boundingBoxVertices[k][1][0] && eyey <= boundingBoxVertices[k][1][1]) && (eyez >= boundingBoxVertices[k][2][0] && eyez <= boundingBoxVertices[k][2][1])) {
      return true;
    }
  }
  return false;
}

// optionally draw graphical representations of the invisible bounding boxes used for collision detection
void drawBoundingBoxes () {
  GLfloat box1_mat[3] = {1.0, 1.0, 0.0};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, box1_mat);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, box1_mat);

  glPushMatrix();
    glTranslatef(-2.5, 1.5, 0.0);
    glScalef(-2.65, 3.0, 6.0);
    glutSolidCube(1.0);
  glPopMatrix();

  GLfloat box2_mat[3] = {1.0, 0.5, 0.0};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, box2_mat);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, box2_mat);
  glPushMatrix();
    glTranslatef(0.75, 1.5, -0.5);
    glScalef(3.9, 3.0, 5.45);
    glutSolidCube(1.0);
  glPopMatrix();

  GLfloat box3_mat[4] = {0.0, 1.0, 0.0, 0.0};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, box3_mat);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, box3_mat);
  glPushMatrix();
    glTranslatef(3.2, 1.5, -1.7);
    glScalef(1.31, 3.0, 2.7);
    glutSolidCube(1.0);
  glPopMatrix();

  GLfloat box4_mat[4] = {0.0, 1.0, 1.0, 0.0};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, box4_mat);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, box4_mat);
  glPushMatrix();
    glTranslatef(-4.0, 0.5, 7.0);
    glScalef(1.0, 1.0, 2.2);
    glutSolidCube(1.0);
  glPopMatrix();

  GLfloat box5_mat[4] = {0.0, 1.0, 1.0, 0.0};
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, box5_mat);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, box5_mat);
  glPushMatrix();
    glTranslatef(7.0, 0.5, 11.0);
    glScalef(2.7, 1.0, 1.2);
    glutSolidCube(1.0);
  glPopMatrix();

}

// callback to pass to glutPassiveMotionFunc
// handles rotation of observer's view
static void mouse_idle (int x, int y)
{
    float delta_x = x - trackball_startx;
    float delta_y = y - trackball_starty;
    trackball_startx = x;
    trackball_starty = y;
    trackball_angle += delta_y * 0.01;
    trackball_angle2 += delta_x * 0.01;
    center_x = sin(trackball_angle2);
    center_y = sin(trackball_angle);
    center_z = -cos(trackball_angle2);
    glutPostRedisplay();
}

// handles mouse motion when one or more mouse buttons is being pressed
// allows the user to re-position the mouse without rotating the view
static void motion(int x, int y)
{
  trackball_startx = x;
  trackball_starty = y;
}

void set_projection () {

   GLfloat aspect = GLfloat(win_width) / GLfloat(win_height);
	// Reset the projection when zoom setting or window shape changes.
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, aspect, 0.1f, 300.0f);

}

void myReshape(int w, int h)
{
	// Window reshaping function.
	win_width = w;
	win_height = h;
	glViewport(0, 0, win_width, win_height);

}

void keyFunction(unsigned char key, int x, int y) {
  switch (key) {
    float yrot_rads;
    case 'w':
    case 'W':
      // if observer has collided with object, prevent forward motion
      // else, increment the x and z components of the observer's position
      if (!isCollidedWithObstacle()) {
        eyex += center_x * navigation_damper;
        eyez += center_z * navigation_damper;
      }
      break;
    case 's':
    case 'S':
        // backward motion is allowed regardless of collision status
        eyex -= center_x * navigation_damper;
        eyez -= center_z * navigation_damper;
      break;
    case 'g':
    case 'G':
      // toggle guides drawn
      drawGuides = !drawGuides;
      break;
    case 'r':
    case 'R':
      // reset to default position
      eyex = 0;
      eyey = 0.5;
      eyez = 10.0;
      break;
    case 'q':
    case 'Q':
      exit(0);
      break;
  }
    glutPostRedisplay();
}

void mySpecialFunc (int key, int x, int y) {

    switch(key)
    {

        case GLUT_KEY_LEFT:
            // rotate view
            trackball_angle2 -= 0.05f;
            center_x = sin(trackball_angle2);
            center_z = -cos(trackball_angle2);
            break;
        case GLUT_KEY_RIGHT:
            // rotate view
            trackball_angle2 += 0.05f;
            center_x = sin(trackball_angle2);
            center_z = -cos(trackball_angle2);
            break;
        case GLUT_KEY_UP:
            eyey++;
            break;
        case GLUT_KEY_DOWN:
            eyey--;
            break;
       }

  glutPostRedisplay();

}

int main(int argc, char **argv)
{

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(900, 700);
    glutCreateWindow("Project 1");

    /* Register assorted GLUT callback routines. */
    init();
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
    // glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutPassiveMotionFunc(mouse_idle);
    glutSpecialFunc(mySpecialFunc);
    glutKeyboardFunc(keyFunction);

    /* Enter GLUT's main loop; callback dispatching begins. */
    glutMainLoop();
    return 0;
}

