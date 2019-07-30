#include <GL/glut.h> // (or others, depending on the system in use)
#include "sgi.h"


GLsizei w,h;
int width, height, depth;
unsigned char *image = NULL;
int Status;

void init (void);
void reshape(int w, int h);
void lineSegment (void);
void mainSelect(int);
void fileSelect(int);
void displaySelect(int);
void NegativeImage();


void init (void)  {
	glClearColor (1.0, 1.0, 1.0, 0.0);       // Set display-window color to white.
	glMatrixMode (GL_PROJECTION); // Set projection parameters.
	gluOrtho2D (0.0, 200.0, 0.0, 150.0);
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h);
   height = (GLint) h;
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D(0.0, (GLdouble) w, 0.0, (GLdouble) h);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
}


void display(void)
{

   glClear(GL_COLOR_BUFFER_BIT);
   glRasterPos2i(0, 0);

   glDrawPixels(width, height, GL_RGB, 
                GL_UNSIGNED_BYTE, image);
   glFlush();
}

void mainSelect(int value)
{
	Status=value;
	switch (value) {
		case 666:
			printf("exit\n");
			exit(0);
			break;
	}
}

void fileSelect(int value)
{
	char *filename;
	Status=value;
	switch (value) {
		case 0:
			filename="sample.rgb";
			image = read_sgi(filename, &width, &height, &depth);
			glutReshapeWindow(width,height);
			display();
			break;
		case 1:
			filename="train.bmp";
			image = read_bmp(filename, &width, &height, &depth);
			glutReshapeWindow(width,height);
			display();
			break;
	}
}

void displaySelect(int value)
{
	Status=value;
	switch (value) {
		case 10:
			NegativeImage();
			display();
			break;
	}
}

void NegativeImage() {
   int i,j;
   for (j=0;j<height; j++)  {
	   for (i=0;i<width*3; i=i+3) {
			image[j*width*3+i+0]=255-image[j*width*3+i+0];
			image[j*width*3+i+1]=255-image[j*width*3+i+1];
			image[j*width*3+i+2]=255-image[j*width*3+i+2];
	   }
   }
}

void main (int argc, char** argv)   {

	int fileMenu, displayMenu, mainMenu;

	glutInit (&argc, argv); 				// Initialize GLUT.
	glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB); 	// Set display mode.
//	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);


	glutInitWindowPosition (50, 100); 	// Set top-left display-window position.
	glutInitWindowSize (300, 300); 	// Set display-window width and height.
	glutCreateWindow ("An Example OpenGL Program"); // Create display window.

	init ( ); 					// Execute initialization procedure.
	glutDisplayFunc (display); 	// Send graphics to display window.
    glutReshapeFunc(reshape);

	fileMenu = glutCreateMenu(fileSelect);
	glutAddMenuEntry("sample.rgb",0);
	glutAddMenuEntry("train.bmp",1);

	displayMenu = glutCreateMenu(displaySelect);
	glutAddMenuEntry("Negative Image",10);

	mainMenu = glutCreateMenu(mainSelect);
	glutAddSubMenu("File", fileMenu);
	glutAddSubMenu("Display", displayMenu);

	glutAddMenuEntry("Quit",666);
	glutSetMenu(mainMenu);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutMainLoop ( ); 		// Display everything and wait.
}

