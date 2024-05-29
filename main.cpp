/*******************************************************************
		   CPS511 A3 Zeppelin
		   Kevin Zhao
		   500964932
********************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gl/glut.h>
#include <utility>
#include <vector>
#include "VECTOR3D.h"
#include "RGBpixmap.h"

GLuint textureId;

const int vWidth = 1500;    // Viewport width in pixels
const int vHeight = 700;    // Viewport height in pixels

//FOR TEXTURE MAPPING
RGBpixmap pix1[6];

//globals for zep
float zepWidth = 1.3f;
float zepLength = 0.8f;
float zepDepth = 2.0f;

//movement globals
float speed = 0.02;
float x = 0.0f;
float yZep = 0.0f;
float z = 0.0f;
bool pVis = true;

//enemy x&z variables
float x2 = 0.0f;
float z2 = -30.0f;
bool eVis = true;


// Control Robot body rotation on base
float robotAngle = 0;
float enemyAngle = -90.0;
// Control arm rotation
float propAngle = -40.0;
float gunAngle = -25.0;

//missile variables
//player missile
float pmAngle = robotAngle;
float xMissile = x;
float zMissile = z;

//enemy missile
float enemyMissleAngle = enemyAngle;
float xMissile2 = x2;
float zMissile2 = z2;

bool coords = true;
float slope = 0.0;
float enemyMissileAngle = 0;
// Light properties
GLfloat light_position0[] = { -4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_position1[] = { 4.0F, 8.0F, 8.0F, 1.0F };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[] = { 0.2F, 0.2F, 0.2F, 1.0F };


// Mouse button
int currentButton;

bool fpv = false;
bool pmissile = false;
bool emissile = false;
bool Eaggro = false;

// Structure defining a bounding box, currently unused
typedef struct BoundingBox {
	float xmin;
	float xmax;
	float zmin;
	float zmax;
} BBox;

struct BoundingBox player = { x - 2.5, x + 2.5, z - 2.5, z + 2.5 };
struct BoundingBox enemy = { x2 - 2.5, x2 + 2.5, z2 - 2.5, z2 + 2.5 };
struct BoundingBox aggro = { x2 - 7.5, x2 + 7.5, z2 - 7.5, z2 + 7.5 };

// Default Mesh Size
int meshSize = 16;

// Prototypes for functions in this module
void initOpenGL(int w, int h);
void display(void);
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void mouseMotionHandler(int xMouse, int yMouse);
void keyboard(unsigned char key, int x, int y);
void functionKeys(int key, int x, int y);
void animationHandler(int param);
void movementHandler(int param);
void drawZep();
void drawBody();
void drawEnv();
void drawEnemy();
void drawEnemyBody();
void drawMissile();

void enemyMovement(int param) {
	enemyAngle = enemyAngle + 0.2;
	x2 += 0.05 * sinf(enemyAngle * (3.141592 / 180.0));
	z2 += 0.05 * cosf(enemyAngle * (3.141592 / 180.0));
	//bounding box
	enemy.xmin = x2 - 2.5;
	enemy.xmax = x2 + 2.5;
	enemy.zmin = z2 - 2.5;
	enemy.zmax = z2 + 2.5;

	aggro.xmin = x2 - 7.5;
	aggro.xmax = x2 + 7.5;
	aggro.zmin = z2 - 7.5;
	aggro.zmax = z2 + 7.5;

}


void setTexture(RGBpixmap *p, GLuint textureID)
{
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, p->nCols, p->nRows, 0, GL_RGB,
		GL_UNSIGNED_BYTE, p->pixel);
}

int main(int argc, char **argv)
{
	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(vWidth, vHeight);
	glutInitWindowPosition(200, 30);
	glutCreateWindow("3D Hierarchical Example");

	// Initialize GL
	initOpenGL(vWidth, vHeight);

	// Register callback functions
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotionHandler);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(functionKeys);

	// Start event loop, never returns
	glutMainLoop();

	return 0;
}


// Set up OpenGL. For viewport and projection setup see reshape(). 
void initOpenGL(int w, int h)
{
	// Set up and enable lighting
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
	glLightfv(GL_LIGHT1, GL_POSITION, light_position1);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);   // This second light is currently off

	// Other OpenGL setup
	glEnable(GL_DEPTH_TEST);   // Remove hidded surfaces
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);   // Use smooth shading, makes boundaries between polygons harder to see 
	glClearColor(0.0f, 0.5f, 1.0f, 0.0f);  // Color and depth for glClear
	glClearDepth(1.0f);
	glEnable(GL_NORMALIZE);    // Renormalize normal vectors 
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   // Nicer perspective

	//Texture code
	pix1[0].readBMPFile("tiles01.bmp");
	setTexture(&pix1[0], 2000);
	pix1[1].readBMPFile("building.bmp");
	setTexture(&pix1[1], 2001);
	pix1[2].readBMPFile("road.bmp");
	setTexture(&pix1[2], 2002);
	pix1[3].readBMPFile("popeyes.bmp");
	setTexture(&pix1[3], 1337);
	pix1[4].readBMPFile("kfc.bmp");
	setTexture(&pix1[4], 1338);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


}
void checkRange(int param) {
	if (x > aggro.xmin && x < aggro.xmax && Eaggro == false)
	{
		if (z > aggro.zmin && z < aggro.zmax) {
			Eaggro = true;
			xMissile2 = x2;
			zMissile2 = z2;
			//enemyMissileAngle = enemyAngle;
		}
	}

}

void drawMissile() {

	glPushMatrix();
	glTranslatef(xMissile, yZep, zMissile);
	glutSolidSphere(1, 20, 20);
	glPopMatrix();
}

void drawEmissile() {
	glPushMatrix();
	glTranslatef(xMissile2, yZep, zMissile2);
	glutSolidSphere(1, 20, 20);
	glPopMatrix();
}


void missileTracker(int param) {
	xMissile += 0.5 * sinf(pmAngle * (3.141592 / 180.0));
	zMissile += 0.5 * cosf(pmAngle * (3.141592 / 180.0));

	if (xMissile > enemy.xmin && xMissile < enemy.xmax)
	{
		if (zMissile > enemy.zmin && zMissile < enemy.zmax) {
			eVis = false;
			pmissile = false;
			xMissile = x;
			zMissile = z;
			pmAngle = robotAngle;

		}
	}
	else if (xMissile > 60 || zMissile > 60 || xMissile < -60 || zMissile < -60) {
		pmissile = false;
		pmAngle = robotAngle;
		xMissile = x;
		zMissile = z;

	}
}

void EmissileTracker(int param) {
	xMissile2 += 0.5 * sinf(enemyMissileAngle * (3.141592 / 180.0));
	zMissile2 += 0.5 * cosf(enemyMissileAngle * (3.141592 / 180.0));

	if (xMissile2 > player.xmin && xMissile2 < player.xmax)
	{
		if (zMissile2 > player.zmin && zMissile2 < player.zmax) {
			pVis = false;
			xMissile2 = x2;
			zMissile2 = z2;
			coords = false;
			Eaggro = false;

		}
	}
	else if (xMissile2 > 60 || zMissile2 > 60 || xMissile2 < -60 || zMissile2 < -60) {
		coords = false;
		xMissile2 = x2;
		zMissile2 = z2;
		Eaggro = false;
	}
}

// Callback, called whenever GLUT determines that the window should be redisplayed
// or glutPostRedisplay() has been called.
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 1.0);

	glLoadIdentity();
	// Create Viewing Matrix V
	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	if (fpv) //changes between fpv and overhead view depending on bool variable
	{
		float xRef = x + 50 * sinf(robotAngle * (3.141592 / 180.0));
		float zRef = z + 50 * cosf(robotAngle * (3.141592 / 180.0));
		float xeye = x + 6 * sinf(robotAngle * (3.141592 / 180.0));
		float zeye = z + 6 * cosf(robotAngle * (3.141592 / 180.0));
		gluLookAt(xeye, yZep, zeye, xRef, yZep, zRef, 0, 1, 0);
	}
	else
	{
		gluLookAt(0.0, 20.0, -50.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}


	// Draw Robot

	// Apply modelling transformations M to move robot
	// Current transformation matrix is set to IV, where I is identity matrix
	// CTM = IV


	//adding code for movement here
	if (pVis)
	{
		glPushMatrix();
		glTranslatef(x, yZep, z);
		drawZep();
		glPopMatrix();

	}


	if (eVis)
	{
		glPushMatrix();
		glTranslatef(x2, yZep, z2);
		drawEnemy();
		glPopMatrix();
	}

	// Draw ground
	glPushMatrix();
	glTranslatef(0.0, -20.0, 0.0);
	//groundMesh->DrawMesh(meshSize);
	drawEnv();
	glPopMatrix();

	if (Eaggro)
	{
		if (coords)
		{

			slope = (x - x2) / (z - z2);
			enemyMissileAngle += tanf(slope * (3.141592 / 180.0));
			xMissile2 = x2;
			zMissile2 = z2;
			enemyAngle = enemyMissileAngle;
			coords = false;

		}
		enemyAngle = enemyMissileAngle;
		drawEmissile();
		glutTimerFunc(10, EmissileTracker, 0);

	}


	if (pmissile)
	{
		drawMissile();
		glutTimerFunc(10, missileTracker, 0);
	}

	glutSwapBuffers();   // Double buffering, swap buffers
	glutTimerFunc(10, enemyMovement, 0);
	glutTimerFunc(30, checkRange, 0);
}
//zeplin parts below
void drawProp()
{

	//draw the blades

	glPushMatrix();

	glTranslatef(0, 0, 2.3*zepDepth);
	glRotatef(propAngle, 0.0, 0.0, 1.0);
	glScalef(0.5*zepWidth, 5 * zepLength, 0.3*zepDepth);
	glutSolidSphere(0.2, 20, 20);
	glPopMatrix();

	//draw the driveshaft

	glPushMatrix();
	glTranslatef(0, 0, 1.4*zepDepth);
	glScalef(0.1*zepWidth, 0.1*zepWidth, 2 * zepDepth);
	glutSolidCube(1.0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, 2.3*zepDepth);
	glutSolidSphere(0.3, 20, 20);
	glPopMatrix();
}

void drawCabin()
{

	glPushMatrix();
	glTranslatef(0, -1.8 * zepLength, 0.0); // this will be done last
	glScalef(zepWidth, zepLength, 1.5*zepDepth);
	glutSolidCube(1.0);
	glPopMatrix();


}
void drawFin()
{

	glPushMatrix();

	glTranslatef(0, 0, -0.35*zepDepth); // this will be done last
	glScalef(3 * zepWidth, 0.1*zepLength, 3.3*zepDepth);
	glutSolidCube(1.0);
	glPopMatrix();

	glPopMatrix();
}

void drawZep()
{
	glPushMatrix();
	// spin Zepplin on base. 
	glRotatef(robotAngle, 0.0, 1.0, 0.0);
	drawCabin();
	drawBody();


	glPopMatrix();
}

void drawEnemy() {
	glPushMatrix();
	// spin Zepplin on base. 
	glRotatef(enemyAngle, 0.0, 1.0, 0.0);
	drawCabin();
	drawEnemyBody();

	glPopMatrix();
}

//main zepplin body
void drawBody()
{
	drawProp();
	glBindTexture(GL_TEXTURE_2D, 1337);
	glPushMatrix();
	glScalef(zepWidth, zepLength, zepDepth);
	GLUquadric *quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluQuadricNormals(quadric, GLU_SMOOTH);
	gluQuadricTexture(quadric, GL_TRUE);
	gluSphere(quadric, 2.0, 20, 20);
	gluDeleteQuadric(quadric);
	glPopMatrix();
}

void drawEnemyBody() {
	drawProp();
	glBindTexture(GL_TEXTURE_2D, 1338);
	glPushMatrix();
	glScalef(zepWidth, zepLength, zepDepth);
	GLUquadric *quadric = gluNewQuadric();
	gluQuadricDrawStyle(quadric, GLU_FILL);
	gluQuadricNormals(quadric, GLU_SMOOTH);
	gluQuadricTexture(quadric, GL_TRUE);
	gluSphere(quadric, 2.0, 20, 20);
	gluDeleteQuadric(quadric);
	glPopMatrix();
}



// Callback, called at initialization and whenever user resizes the window.
void reshape(int w, int h)
{
	// Set up viewport, projection, then change to modelview matrix mode - 
	// display function will then set up camera and do modeling transforms.
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / h, 0.2, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set up the camera at position (0, 6, 22) looking at the origin, up along positive y axis
	gluLookAt(0.0, 6.0, 22.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

bool stop = false;

// Callback, handles input from the keyboard, non-arrow keys
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'f':
		pmAngle = robotAngle;
		pmissile = true;

		break;
	case 'r':
		robotAngle += 2.0;
		break;
	case 'p':
		pVis = true;
		eVis = true;
	case 'a':
		propAngle += 2.0;
		break;
	case 'A':
		propAngle -= 2.0;
		break;
	case 'g':
		if (fpv)
		{
			fpv = false;
		}
		else
		{
			fpv = true;
		}
		break;
	case 'G':
		gunAngle -= 2.0;
		break;
	case 's':
		stop = false;
		glutTimerFunc(10, animationHandler, 0);
		glutTimerFunc(10, movementHandler, 0);
		break;
	case 'S':
		stop = true;
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}

//Add Movement handler
void movementHandler(int param)
{
	if (!stop)
	{
		x += speed * sinf(robotAngle * (3.141592 / 180.0)); //zepplin movement in the angle it is facing
		z += speed * cosf(robotAngle * (3.141592 / 180.0));
		player.xmin = x - 2.5;
		player.xmax = x + 2.5;
		player.zmin = z - 2.5;
		player.zmax = z + 2.5;

		if (robotAngle > 360)
		{
			robotAngle = 0;
		}
		else if (robotAngle < 0)
		{
			robotAngle = 360;
		}
		if (pmissile == false)
		{
			xMissile = x;
			zMissile = z;
		}
		glutPostRedisplay();
		glutTimerFunc(10, movementHandler, 0);

	}
}

void animationHandler(int param)
{
	if (!stop)
	{
		propAngle += 1.0;
		glutPostRedisplay();
		glutTimerFunc(10, animationHandler, 0);
	}
}



// Callback, handles input from the keyboard, function and arrow keys
void functionKeys(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		yZep += 0.05;
		break;
	case GLUT_KEY_DOWN:
		yZep -= 0.05;
		break;
	case GLUT_KEY_LEFT:
		robotAngle -= 2.0;
		break;
	case GLUT_KEY_RIGHT:
		robotAngle += 2.0;
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse button callback - use only if you want to 
void mouse(int button, int state, int x, int y)
{
	currentButton = button;

	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;

		}
		break;
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_DOWN)
		{
			;
		}
		break;
	default:
		break;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}


// Mouse motion callback - use only if you want to 
void mouseMotionHandler(int xMouse, int yMouse)
{
	if (currentButton == GLUT_LEFT_BUTTON)
	{
		;
	}

	glutPostRedisplay();   // Trigger a window redisplay
}

//test env
struct color3f
{
	float r, g, b;
	color3f() { r = 0; g = 0; b = 0; }
	color3f(float r, float g, float b) { this->r = r; this->g = g; this->b = b; }
};

void grid()
{
	glPushMatrix();
	glColor3ub(0, 113, 0);
	glBindTexture(GL_TEXTURE_2D, 2000);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-50, 0, -50);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(50, 0, -50);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(50, 0, 50);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-50, 0, 50);
	glEnd();
	glFlush();
	glPopMatrix();
}

//a road in middle of the scene
void road()
{
	//the road
	glPushMatrix();
	glColor3ub(15, 15, 15);
	glBindTexture(GL_TEXTURE_2D, 2002);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-2, 0.005, 50);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(2, 0.005, 50);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(2, 0.005, -50);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-2, 0.005, -50);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3ub(15, 15, 15);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-27.5, 0.005, 50);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-22.5, 0.005, 50);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-22.5, 0.005, -50);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-27.5, 0.005, -50);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3ub(15, 15, 15);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(27.5, 0.005, 50);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(22.5, 0.005, 50);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(22.5, 0.005, -50);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(27.5, 0.005, -50);
	glEnd();
	glFlush();
	glPopMatrix();

}


void house(color3f color, float x, float y, float z)
{
	glPushMatrix();
	glTranslatef(x, y, z);
	glScalef(3, 10, 3);
	//the body	
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, 2000); // top face of cube
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 2001); // right face of cube
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 2001); // left face of cube
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 2001); // bottom face of cube
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 2001); // back face of cube
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(1.0f, -1.0f, -1.0f);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, 2001); // front face of cube
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(0.0, 1.0);
	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0, 1.0);
	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0, 0.0);
	glVertex3f(1.0f, -1.0f, 1.0f);
	glEnd();

	glFlush();
	glPopMatrix();
	glPopMatrix();
}


void drawEnv() {
	glPushMatrix();
	house(color3f(15, 20, 200), 9, 0, 2);
	house(color3f(15, 150, 150), 9, 0, 12);
	house(color3f(20, 150, 200), 9, 0, 6);
	house(color3f(150, 20, 200), 9, 0, -6);
	house(color3f(15, 200, 20), 9, 0, -10);
	house(color3f(200, 15, 20), 9, 0, 19);
	road();
	grid();
	glPopMatrix();
}
