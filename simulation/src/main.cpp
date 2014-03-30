/*
 *
 *	Simple example of combining OpenCvs camera interface and OpenGL
 *
 *
 *	@author Gordon Wetzstein, now @ The University of British Columbia [wetzste1@cs.ubc.ca]
 *	@date 10/15/06
 *
 */


//-----------------------------------------------------------------------------
// includes

#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>
//#include <GL/glew.h>
#include <GL/glut.h>

#include <highgui.h>

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// GLUT callbacks and functions

void initGlut(int argc, char **argv);
void displayFunc(void);
void idleFunc(void);
void reshapeFunc(int width, int height);
void mouseFunc(int button, int state, int x, int y);
void mouseMotionFunc(int x, int y);
void keyboardFunc(unsigned char key, int x, int y);
void specialFunc(int key, int x, int y);
//-----------------------------------------------------------------------------

// other [OpenGL] functions
void countFrames(void);
void renderBitmapString(float x, float y, float z, void *font, char *string);

//-----------------------------------------------------------------------------

bool bFullsreen = false;
int nWindowID;

//-----------------------------------------------------------------------------

// parameters for the framecounter
char pixelstring[30];
int cframe = 0;
int ttime = 0;
int ttimebase = 0;

//-----------------------------------------------------------------------------

// OpenCV variables

CvCapture *cvCapture = 0;

GLuint cameraImageTextureID;

//-----------------------------------------------------------------------------

bool bInit = false;

//-----------------------------------------------------------------------------

void displayFunc(void) {

	if(!bInit) {

		// initialize 1st camera on the bus
		cvCapture = cvCreateCameraCapture(-1);

		// initialze OpenGL texture
		glEnable(GL_TEXTURE_RECTANGLE_ARB);

		glGenTextures(1, &cameraImageTextureID);
		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, cameraImageTextureID);

		glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		bInit = true;
	}

	IplImage* newImage = cvQueryFrame( cvCapture );

	if( (newImage->width > 0) && (newImage->height > 0)) {

		// clear the buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glEnable(GL_TEXTURE_RECTANGLE_ARB);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0.0,(GLdouble)newImage->width,0.0,(GLdouble)newImage->height);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glBindTexture(GL_TEXTURE_RECTANGLE_ARB, cameraImageTextureID);

		if(newImage->nChannels == 3)
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, newImage->width, newImage->height, 0, GL_BGR, GL_UNSIGNED_BYTE, newImage->imageData);
		else if(newImage->nChannels == 4)
			glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, newImage->width, newImage->height, 0, GL_BGRA, GL_UNSIGNED_BYTE, newImage->imageData);

		glBegin(GL_QUADS);
			glTexCoord2i(0,0);
			glVertex2i(0,0);
			glTexCoord2i(newImage->width,0);
			glVertex2i(newImage->width,0);
			glTexCoord2i(newImage->width,newImage->height);
			glVertex2i(newImage->width,newImage->height);
			glTexCoord2i(0,newImage->height);
			glVertex2i(0,newImage->height);
		glEnd();

	}

	glDisable(GL_TEXTURE_RECTANGLE_ARB);

	countFrames();

	glutSwapBuffers();
}

//-----------------------------------------------------------------------------

void initGlut(int argc, char **argv) {

	// GLUT Window Initialization:
	glutInit (&argc, argv);
	glutInitWindowSize (640, 480);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode ( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	nWindowID = glutCreateWindow ("simpleGLUT - CvCamera");

	// Register callbacks:
	glutDisplayFunc		(displayFunc);
	glutReshapeFunc		(reshapeFunc);
	glutKeyboardFunc	(keyboardFunc);
	glutSpecialFunc		(specialFunc);
	glutMouseFunc		(mouseFunc);
	glutMotionFunc		(mouseMotionFunc);
	glutIdleFunc		(idleFunc);
}



//-----------------------------------------------------------------------------

void idleFunc(void) {
	glutPostRedisplay();
}

//-----------------------------------------------------------------------------

void reshapeFunc(int width, int height) {
	glViewport(0, 0, width, height);
}

//-----------------------------------------------------------------------------


// mouse callback
void mouseFunc(int button, int state, int x, int y) {

}

//-----------------------------------------------------------------------------

void mouseMotionFunc(int x, int y) {

}

//-----------------------------------------------------------------------------

void keyboardFunc(unsigned char key, int x, int y) {

	switch(key) {

		// -----------------------------------------

#ifdef WIN32
		// exit on escape
		case '\033':

			if(bInit) {
				glDeleteTextures(1, &cameraImageTextureID);
				cvReleaseCapture( &cvCapture );
			}
			exit(0);
			break;
#endif

		// -----------------------------------------

		// switch to fullscreen
		case 'f':

			bFullsreen = !bFullsreen;
			if(bFullsreen)
				glutFullScreen();
			else {
				glutSetWindow(nWindowID);
				glutPositionWindow(100, 100);
				glutReshapeWindow(640, 480);
			}
			break;

		// -----------------------------------------
	}
}

//-----------------------------------------------------------------------------

void specialFunc(int key, int x, int y) {
	//printf("key pressed: %d\n", key);
}

//-----------------------------------------------------------------------------

void countFrames(void)  {

	ttime=glutGet(GLUT_ELAPSED_TIME);
	cframe++;
	if (ttime - ttimebase > 50) {
		sprintf(pixelstring, "fps: %4.2f", cframe*1000.0/(ttime-ttimebase));
		ttimebase = ttime;
		cframe = 0;
	// Draw status text and uni-logo:
	}
	glDisable(GL_LIGHTING);
	glColor4f(1.0,1.0,1.0,1.0);
	glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, 200, 0, 200);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	// render the string
	renderBitmapString(5,5,0.0,GLUT_BITMAP_HELVETICA_10,pixelstring);

	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

//-----------------------------------------------------------------------------

void renderBitmapString(float x, float y, float z, void *font, char *string) {
  char *c;
  glRasterPos3f(x, y,z);
  for (c=string; *c != '\0'; c++) {
    glutBitmapCharacter(font, *c);
  }
}

//-----------------------------------------------------------------------------


int main(int argc, char **argv) {

	initGlut(argc, argv);
	glutMainLoop();

}



