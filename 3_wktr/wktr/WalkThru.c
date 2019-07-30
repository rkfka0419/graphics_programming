#include <windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include "sgi.h"

#define	WIRE 0		
#define	SHADE 1		
#define	TEX 2		

#define PI  3.14159265359
#define	fRadius 10			/* 회전 각도 */
#define Wall_HEIGHT 50
#define Wall_count 30
#define WALL_MARGIN 5

GLfloat mKa[4] = {0.11f, 0.06f, 0.11f, 1.0f}; /* Object : Ambient */
GLfloat mKd[4] = {0.43f, 0.47f, 0.54f, 1.0f}; /* Object : Diffuse */
GLfloat mKs[4] = {0.33f, 0.33f, 0.52f, 1.0f}; /* Object : Specular */
GLfloat mKe[4] = {0.0f, 0.0f, 0.0f, 0.0f}; /* Object : Emission */
GLfloat shininess[1] = {50}; /* Object : Shininess */

typedef struct {
	int x1;
	int z1;
	int x2;
	int z2;
} Margin;

Margin MakeGL_Wall(int x1, int x2, int z1, int z2, int y);

typedef struct {
	float x;
	float y;
	float z;
} Point;

typedef struct {
	unsigned long ip[3];
} Face;

int pnum;
int fnum;
Point *mpoint;
Face *mface;
Margin wall[Wall_count];

int jumping = 0;
int sitFlag = 0;



int status=2;           // WIRE or SHADE  or TEXTURE
int cull=0;             // CULLING toggle 
char *fname="chair.dat";

Point Pos, Dir;
double rads=0.0;
int IsLoad=0;
int BirdEye=0;
int IsFull=0;

unsigned char * image = NULL;
int iwidth, iheight, idepth;

GLuint textureId[4];

/// 흔들거리기
double Wa = 0.0;
double WaAmount = 0.07;
int	Waf = 1;



void InitWalk(void)
{
	Pos.x=0;
	Pos.y=25;
	Pos.z=0;
}

void InitTexture(void)
{

	glGenTextures(1, &textureId[0]);
	glBindTexture(GL_TEXTURE_2D, textureId[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image =read_bmp("wall1.bmp", &iwidth, &iheight, &idepth);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glBindTexture(GL_TEXTURE_2D, 0);

	//모래바닥 질감
	glGenTextures(1, &textureId[1]);
	glBindTexture(GL_TEXTURE_2D, textureId[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image =read_bmp("sand.bmp", &iwidth, &iheight, &idepth);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);    
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &textureId[2]);
	glBindTexture(GL_TEXTURE_2D, textureId[2]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image =read_bmp("wood2.bmp", &iwidth, &iheight, &idepth);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glBindTexture(GL_TEXTURE_2D, 0);

	//넝쿨벽 질감
	glGenTextures(1, &textureId[3]);
	glBindTexture(GL_TEXTURE_2D, textureId[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	image = read_bmp("vinewall.bmp", &iwidth, &iheight, &idepth);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void DrawWire(void)
{
//	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_LIGHT0);
	glEnable (GL_LIGHT1);
	glEnable (GL_LIGHT2);
	glEnable (GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	if (cull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glCallList(1);
	glCallList(2);
	glutSwapBuffers();
}

void DrawShade(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_LIGHT0);
	glEnable (GL_LIGHT1);
	glEnable (GL_LIGHT2);
	glEnable (GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	if (cull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCallList(1);
	glCallList(2);
	glutSwapBuffers();
}

void DrawTexture(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_LIGHT0);
	glEnable (GL_LIGHT1);
	glEnable (GL_LIGHT2);
	glEnable (GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);

	if (cull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCallList(1);
	glCallList(2);
	glutSwapBuffers();
}


Point cnormal(Point a, Point b, Point c)
{
    Point p, q, r;
    double val;
    p.x = a.x-b.x; p.y = a.y-b.y; p.z = a.z-b.z;
    q.x = c.x-b.x; q.y = c.y-b.y; q.z = c.z-b.z;

    r.x = p.y*q.z - p.z*q.y;
    r.y = p.z*q.x - p.x*q.z;
    r.z = p.x*q.y - p.y*q.x;

    val = sqrt(r.x*r.x + r.y*r.y + r.z*r.z);
    r.x = r.x / val;
    r.y = r.y / val;
    r.z = r.z / val;
    return r;
}


void MakeGL_Model1(void) // 모델 만들기
{
	int i;
	Point norm;


	glMaterialfv(GL_FRONT, GL_AMBIENT, mKa);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mKd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mKs);
    glMaterialfv(GL_FRONT, GL_EMISSION, mKe);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	glShadeModel(GL_SMOOTH);

	if (glIsList(2)) glDeleteLists(2,2);			
	glNewList(2, GL_COMPILE);
	glPushMatrix();

	glBindTexture(GL_TEXTURE_2D, textureId[2]);

	glTranslatef(50, 17.0, -160.0);
	glScalef(0.1,0.1,0.1);
	for (i = 0; i < fnum; i++) {
		norm = cnormal(mpoint[mface[i].ip[2]], mpoint[mface[i].ip[1]], 
			mpoint[mface[i].ip[0]]);
		glBegin(GL_TRIANGLES);
		//glColor3f(1.0f, 1.0f, 1.0f);

		glNormal3f(norm.x, norm.y, norm.z);
		glTexCoord2f(0.0, 0.0);
		glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y,
			mpoint[mface[i].ip[0]].z);
		if (i%2==0) glTexCoord2f(1.0, 0.0);
		else glTexCoord2f(0.0, 1.0);
		glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y,
			mpoint[mface[i].ip[1]].z);
		glTexCoord2f(1.0, 1.0);
		glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y,
			mpoint[mface[i].ip[2]].z);
		glEnd();
	}
	glPopMatrix();
	glEndList();
}


void MakeGL_SimpleModel(void)
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, mKa);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mKd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mKs);
    glMaterialfv(GL_FRONT, GL_EMISSION, mKe);
    glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	glShadeModel(GL_SMOOTH);

	if (glIsList(1)) glDeleteLists(1,1);
	glNewList(1, GL_COMPILE);
	glPushMatrix();


	glBindTexture(GL_TEXTURE_2D, textureId[0]);

	//오른쪽 벽

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
		glNormal3d(0,0,-1);
		glTexCoord2d(0,0);
   		glVertex3d(-200,0,200);  
		glTexCoord2d(0,1);
		glVertex3d(-200,100, 200);
		glTexCoord2d(1,1);
		glVertex3d(200,100, 200);
		glTexCoord2d(1,0);
		glVertex3d(200,0, 200);
	glEnd();


	//왼쪽 벽
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
		glNormal3d(0,0,1);
		glTexCoord2d(0,0);
   		glVertex3d(200,0,-200);
		glTexCoord2d(0,1);
		glVertex3d(200,100,-200);
		glTexCoord2d(1,1);
		glVertex3d(-200,100,-200);
		glTexCoord2d(1,0);
		glVertex3d(-200,0,-200);
	glEnd();



	//뒤쪽 
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
		glNormal3d(-1,0,0);
		glTexCoord2d(0,0);
   		glVertex3d(-200,0,-200);
		glTexCoord2d(0,1);
		glVertex3d(-200,100,-200);
		glTexCoord2d(1,1);
		glVertex3d(-200,100, 200);
		glTexCoord2d(1,0);
		glVertex3d(-200,0, 200);
	glEnd();


	//앞쪽 벽
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
		glNormal3d(1,0,0);
		glTexCoord2d(0,0);
		glVertex3d(200,0, 200);
		glTexCoord2d(0,1);
		glVertex3d(200,100, 200);
		glTexCoord2d(1,1);
		glVertex3d(200,100,-200);
		glTexCoord2d(1,0);
   		glVertex3d(200,0,-200);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, textureId[1]);


	//바닥
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
		glNormal3d(0,1,0);
		glTexCoord2d(0,0);
		glVertex3d(200,0, 200);
		glTexCoord2d(0,5);
		glVertex3d(200,0,-200);
		glTexCoord2d(5,5);
		glVertex3d(-200,0,-200);
		glTexCoord2d(5,0);
   		glVertex3d(-200,0, 200);
	glEnd();



	wall[0] = MakeGL_Wall(140, -200, 140, -80, Wall_HEIGHT);
	wall[1] = MakeGL_Wall(80, -160, 80, -120, Wall_HEIGHT);
	wall[2] = MakeGL_Wall(80, -120, 20, -120, Wall_HEIGHT);
	wall[3] = MakeGL_Wall(20, -160, 20, -120, Wall_HEIGHT);
	wall[4] = MakeGL_Wall(20, -160, -140, -160, Wall_HEIGHT);
	wall[5] = MakeGL_Wall(-20, -120, -100, -120, Wall_HEIGHT);
	wall[6] = MakeGL_Wall(140, -80, -80, -80, Wall_HEIGHT);
	wall[7] = MakeGL_Wall(140, -40, -40, -40, Wall_HEIGHT);
	wall[8] = MakeGL_Wall(80, 0, 40, 0, Wall_HEIGHT);
	wall[9] = MakeGL_Wall(40, 40, -40, 40, Wall_HEIGHT);
	wall[10] = MakeGL_Wall(80, 80, -80, 80, Wall_HEIGHT);
	wall[11] = MakeGL_Wall(140, 140, -140, 140, Wall_HEIGHT);
	wall[12] = MakeGL_Wall(140, -40, 140, 140, Wall_HEIGHT);
	wall[13] = MakeGL_Wall(80, 0, 80, 80, Wall_HEIGHT);
	wall[14] = MakeGL_Wall(40, 0, 40, 40, Wall_HEIGHT);
	wall[15] = MakeGL_Wall(-40, -40, -40, 0, Wall_HEIGHT);
	wall[16] = MakeGL_Wall(-80, -80, -80, 80, Wall_HEIGHT);
	wall[17] = MakeGL_Wall(-140, -160, -140, 140, Wall_HEIGHT);


	glPopMatrix();
	glEndList();

}

Margin MakeGL_Wall(int x1, int z1, int x2, int z2, int y)
{
	Margin wall = { x1, z1, x2, z2 };

	glBindTexture(GL_TEXTURE_2D, textureId[3]);

	//벽 생성


	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POLYGON);
	glNormal3d(1, 0, 0);
	glTexCoord2d(0, 0);
	glVertex3d(x1, 0, z1);
	glTexCoord2d(0, 1);
	glVertex3d(x1, y, z1);
	glTexCoord2d(1, 1);
	glVertex3d(x2, y, z2);
	glTexCoord2d(1, 0);
	glVertex3d(x2, 0, z2);
	glEnd();

	return wall;
}

void GLSetupRC(void) 
{
	GLfloat lmodel_ambient[] = { 0.0, 0.0, 0.0, 1.0 };   // Ambient of the entire scene
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

	GLfloat	light0_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};  /* Ambient light */
	GLfloat light0_diffuse[] = {1.0, 1.0, 1.0, 1.0};  /* White diffuse light */
	GLfloat	light0_specular[] = {1.0, 1.0, 1.0, 1.0}; /* Specular light */
	GLfloat light0_position[] = {1.0, 1.0, 1.0, 0.0};  /* Infinite light location */

	/* Enable a single OpenGL light. */
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glEnable(GL_LIGHT0);

	GLfloat	light1_ambient[] = {0.3f, 0.3f, 0.3f, 1.0f};  /* Ambient light */
	GLfloat light1_diffuse[] = {1.0, 1.0, 1.0, 1.0};  /* White diffuse light */
	GLfloat	light1_specular[] = {1.0, 1.0, 1.0, 1.0}; /* Specular light */
	GLfloat light1_position[] = {0.0, 1.0, 0.0, 1.0};  /* Point light location */

	/* Enable a single OpenGL light. */
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 1.5); // 감쇠현상  (Constant: ca)
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.5);      // 감쇠현상  (Linear: la)
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.2);   // 감쇠현상  (Quadric: qa)
	glEnable(GL_LIGHT1);

	GLfloat light2_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	GLfloat light2_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light2_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light2_position[] = { -2.0, 2.0, 1.0, 1.0 };
	GLfloat spot_direction[] = { -1.0, -1.0, 0.0 };

	glLightfv(GL_LIGHT2, GL_AMBIENT, light2_ambient);     // 광원 주변광 (Intensity: I0)
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light2_diffuse);       // 광원 난반사  (Intensity: I0)
	glLightfv(GL_LIGHT2, GL_SPECULAR, light2_specular); // 광원 정반사  (Intensity: I0)
	glLightfv(GL_LIGHT2, GL_POSITION, light2_position);   // 광원 위치      (Position: p)

	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 1.5); // 감쇠현상  (Constant: ca)
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.5);      // 감쇠현상  (Linear: la)
	glLightf(GL_LIGHT2, GL_QUADRATIC_ATTENUATION, 0.2);   // 감쇠현상  (Quadric: qa)

	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction); // 집중광원 방향 (Direction: d)
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 2.0);                     // 집중광원 (Falloff: sd)
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 45.0);                        // 집중광원 (Cutoff : sc)

	glEnable(GL_LIGHT2);
    glEnable(GL_LIGHTING);

  /* Use depth buffering for hidden surface elimination. */
	glEnable(GL_DEPTH_TEST);

  /* Setup the view */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
  
	gluPerspective(  40.0, // field of view in degree 
		1.0, // aspect ratio 
		1.0, // Z near 
		2000.0); // Z far 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	Dir.x=Pos.x+(fRadius*cos(rads));
	Dir.y=Pos.y;
	Dir.z=Pos.z-(fRadius*sin(rads));

	if (BirdEye==0)  {
		Wa = Wa + WaAmount;
		if (Wa > 1) WaAmount = -WaAmount;
		if (Wa < 0) WaAmount = -WaAmount;
		printf("Wa = %lf\n", Wa);
		gluLookAt(	Pos.x, Pos.y + Wa, Pos.z,
			Dir.x, Dir.y, Dir.z, 
			0.0f,		1.0f,		0.0f);
	}
	else {
		gluLookAt(	Pos.x, Pos.y+500, Pos.z,
			Dir.x, Dir.y, Dir.z, 
			0.0f,		1.0f,		0.0f);
	}
}

void ReadModel()
{
	FILE *f1;
	char s[81];

	int i;

	if ((f1 = fopen(fname, "rt"))== NULL) {
		printf("No file\n");
		exit(0);
	}
	fscanf(f1,"%s",s);     printf( "%s", s );
	fscanf(f1,"%s",s);     printf( "%s", s );
	fscanf(f1,"%d",&pnum);     printf( "%d\n", pnum);

    mpoint = (Point*)malloc(sizeof(Point)*pnum);

	for (i=0; i<pnum; i++){
		fscanf(f1,"%f",&mpoint[i].x);
		fscanf(f1,"%f",&mpoint[i].y);
		fscanf(f1,"%f",&mpoint[i].z);
	    printf( "%f %f %f\n", mpoint[i].x, mpoint[i].y,mpoint[i].z);
	}

	fscanf(f1,"%s",s);     printf( "%s", s );
	fscanf(f1,"%s",s);     printf( "%s", s );
	fscanf(f1,"%d",&fnum);     printf( "%d\n", fnum);

	mface = (Face*)malloc(sizeof(Face)*fnum);
	for (i=0; i<fnum; i++){
		fscanf(f1,"%d",&mface[i].ip[0]);
		fscanf(f1,"%d",&mface[i].ip[1]);
		fscanf(f1,"%d",&mface[i].ip[2]);
	    printf("%d %d %d\n", mface[i].ip[0], mface[i].ip[1], mface[i].ip[2]);
	}
	fclose(f1);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLSetupRC();
	if (IsLoad==0) {
		MakeGL_SimpleModel();
		ReadModel();
		MakeGL_Model1();
		IsLoad=1;
	}

	if (status==WIRE) DrawWire();
	else if (status==SHADE) DrawShade();
	else DrawTexture();
}


void MoveViewer(GLdouble dStep)
{
	GLdouble xDelta, zDelta;
	xDelta = dStep*cos(rads);
	zDelta = -dStep*sin(rads);
	Pos.x += (float)xDelta;
	Pos.z += (float)zDelta;

}
void MoveStep(GLdouble dStep)
{
	GLdouble xDelta, zDelta;
	double frads;

	frads = PI*0.5 - rads;

	xDelta = dStep*cos(frads);
	zDelta = dStep*sin(frads);
	Pos.x += (float)xDelta;
	Pos.z += (float)zDelta;
}


void jump()
{
	int pos = Pos.y;
	int velo = 1;
	while(Pos.y >= pos)
	{
		Pos.y += velo;		
		if (Pos.y > 50) velo = -velo;
		display();
	}
}


void sit()
{
	if (sitFlag == 1)
	{
		Pos.y = Pos.y * 3;
		sitFlag = 0;
	}
	else if (sitFlag == 0)
	{
		Pos.y = Pos.y / 3;
		sitFlag = 1;
	}
}



void keyboard(unsigned char key, int x, int y)
{
	printf("key %d\n",key);
	switch (key)
	{
		case 'w':
			status=WIRE;
		    glutPostRedisplay();
			break;
		case 's':
			status=SHADE;
		    glutPostRedisplay();
			break;
		case 't':
			status=TEX;
		    glutPostRedisplay();
			break;
		case 'c':
			if (cull) cull=0;
			else cull=1;
		    glutPostRedisplay();
			break;
		case 'f':
			if (!IsFull) {
				glutFullScreen();
				printf("FullScreen\n");
				IsFull=1;
			}
			else {
				glutPositionWindow(400, 400);
				glutReshapeWindow(400,400);
				printf("Window\n");
				IsFull=0;
			}
			break;

		case '0':
			Pos.x=Pos.y=Pos.z=0;
			Dir.x=Dir.z=0;
		    glutPostRedisplay();
			break;
		case '1':
			fname="cube.dat";
			ReadModel();
			IsLoad=0;
		    glutPostRedisplay();
			break;
		case '2':
			fname="sphere.dat";	
			ReadModel();
			IsLoad=0;
		    glutPostRedisplay();
			break;
		case '3':
			fname="teapot.dat";
			ReadModel();
			IsLoad=0;
		    glutPostRedisplay();
			break;
		case '4':
			fname = "chair.dat";
			ReadModel();
			IsLoad = 0;
			glutPostRedisplay();
			break;
		case 'v':
			if (!BirdEye) BirdEye=1;
			else BirdEye=0;
		    glutPostRedisplay();
			break;
		case 27:
			exit(EXIT_SUCCESS);
			break;

	}
}



int Collision() //  충돌
{
	printf("Pos %f %f %f\n",Pos.x, Pos.y, Pos.z);
	if (Pos.x> 195.0 || Pos.x <-195.0) return 1;
	if (Pos.z> 195.0 || Pos.z <-195.0) return 1;
	// 가로 벽이면 > < 순, 세로 벽이면 < > 순
	if (Pos.x > wall[0].x1 - WALL_MARGIN && Pos.x < wall[0].x2 + WALL_MARGIN)	if (Pos.z > wall[0].z1 - WALL_MARGIN && Pos.z < wall[0].z2 + WALL_MARGIN)	return 1; // 0번 벽
	if (Pos.x > wall[1].x1 - WALL_MARGIN && Pos.x < wall[1].x2 + WALL_MARGIN)	if (Pos.z > wall[1].z1 - WALL_MARGIN && Pos.z < wall[1].z2 + WALL_MARGIN)	return 1; // 1번 벽
	if (Pos.x < wall[2].x1 - WALL_MARGIN && Pos.x > wall[2].x2 + WALL_MARGIN)	if (Pos.z > wall[2].z1 - WALL_MARGIN && Pos.z < wall[2].z2 + WALL_MARGIN)	return 1; // 2번 벽
	if (Pos.x > wall[3].x1 - WALL_MARGIN && Pos.x < wall[3].x2 + WALL_MARGIN)	if (Pos.z > wall[3].z1 - WALL_MARGIN && Pos.z < wall[3].z2 + WALL_MARGIN)	return 1; // 3번 벽
	if (Pos.x < wall[4].x1 - WALL_MARGIN && Pos.x > wall[4].x2 + WALL_MARGIN)	if (Pos.z > wall[4].z1 - WALL_MARGIN && Pos.z < wall[4].z2 + WALL_MARGIN)	return 1; // 4번 벽
	if (Pos.x < wall[5].x1 - WALL_MARGIN && Pos.x > wall[5].x2 + WALL_MARGIN)	if (Pos.z > wall[5].z1 - WALL_MARGIN && Pos.z < wall[5].z2 + WALL_MARGIN)	return 1; // 5번 벽
	if (Pos.x < wall[6].x1 - WALL_MARGIN && Pos.x > wall[6].x2 + WALL_MARGIN)	if (Pos.z > wall[6].z1 - WALL_MARGIN && Pos.z < wall[6].z2 + WALL_MARGIN)	return 1; // 6번 벽
	if (Pos.x < wall[7].x1 - WALL_MARGIN && Pos.x > wall[7].x2 + WALL_MARGIN)	if (Pos.z > wall[7].z1 - WALL_MARGIN && Pos.z < wall[7].z2 + WALL_MARGIN)	return 1; // 7번 벽
	if (Pos.x < wall[8].x1 - WALL_MARGIN && Pos.x > wall[8].x2 + WALL_MARGIN)	if (Pos.z > wall[8].z1 - WALL_MARGIN && Pos.z < wall[8].z2 + WALL_MARGIN)	return 1; // 8번 벽
	if (Pos.x < wall[9].x1 - WALL_MARGIN && Pos.x > wall[9].x2 + WALL_MARGIN)	if (Pos.z > wall[9].z1 - WALL_MARGIN && Pos.z < wall[9].z2 + WALL_MARGIN)	return 1; // 9번 벽
	if (Pos.x < wall[10].x1 - WALL_MARGIN && Pos.x > wall[10].x2 + WALL_MARGIN)	if (Pos.z > wall[10].z1 - WALL_MARGIN && Pos.z < wall[10].z2 + WALL_MARGIN)	return 1; // 10번 벽
	if (Pos.x < wall[11].x1 - WALL_MARGIN && Pos.x > wall[11].x2 + WALL_MARGIN)	if (Pos.z > wall[11].z1 - WALL_MARGIN && Pos.z < wall[11].z2 + WALL_MARGIN)	return 1; // 11번 벽
	if (Pos.x > wall[12].x1 - WALL_MARGIN && Pos.x < wall[12].x2 + WALL_MARGIN)	if (Pos.z > wall[12].z1 - WALL_MARGIN && Pos.z < wall[12].z2 + WALL_MARGIN)	return 1; // 12번 벽
	if (Pos.x > wall[13].x1 - WALL_MARGIN && Pos.x < wall[13].x2 + WALL_MARGIN)	if (Pos.z > wall[13].z1 - WALL_MARGIN && Pos.z < wall[13].z2 + WALL_MARGIN)	return 1; // 13번 벽
	if (Pos.x > wall[14].x1 - WALL_MARGIN && Pos.x < wall[14].x2 + WALL_MARGIN)	if (Pos.z > wall[14].z1 - WALL_MARGIN && Pos.z < wall[14].z2 + WALL_MARGIN)	return 1; // 14번 벽
	if (Pos.x > wall[15].x1 - WALL_MARGIN && Pos.x < wall[15].x2 + WALL_MARGIN)	if (Pos.z > wall[15].z1 - WALL_MARGIN && Pos.z < wall[15].z2 + WALL_MARGIN)	return 1; // 15번 벽
	if (Pos.x > wall[16].x1 - WALL_MARGIN && Pos.x < wall[16].x2 + WALL_MARGIN)	if (Pos.z > wall[16].z1 - WALL_MARGIN && Pos.z < wall[16].z2 + WALL_MARGIN)	return 1; // 16번 벽
	if (Pos.x > wall[17].x1 - WALL_MARGIN && Pos.x < wall[17].x2 + WALL_MARGIN)	if (Pos.z > wall[17].z1 - WALL_MARGIN && Pos.z < wall[17].z2 + WALL_MARGIN)	return 1; // 17번 벽

	return 0;
}

void special(int key, int x, int y)
{
	switch (key)
	{
		//동시
		case GLUT_KEY_LEFT && GLUT_KEY_UP:
		if (glutGetModifiers() == GLUT_ACTIVE_CTRL)
		{
			printf("CTRL \n");
			MoveStep(-2.0);
			if (Collision()) MoveStep(2.5);
		}
		else
		{
			rads += PI / 60.0f;
			if (rads > (2.0*PI))
				rads = 0.0;
			printf("동시  key. \n\n");

			MoveViewer(4.0);
			if (Collision()) MoveViewer(-6.0);
		}
		break;

		case GLUT_KEY_LEFT :
			if (glutGetModifiers()==GLUT_ACTIVE_CTRL) 
			{
				printf("CTRL \n");
				MoveStep(-4.0);
				if (Collision()) MoveStep(5.0);
			}
			else 
			{
				rads += PI/60.0f; 
				if(rads > (4.0*PI))		
					rads = 0.0;
				printf("Left directional key. \n\n");
			}
			break;

		case GLUT_KEY_RIGHT :
			if (glutGetModifiers()==GLUT_ACTIVE_CTRL) 
			{
				printf("CTRL \n");
				MoveStep(4.0);
				if(Collision()) MoveStep(-5.0);
			}
			else 
			{
				printf("Right directional key. \n");
				rads -= PI/60.0; 
				if(rads < 0.0)
					rads = (4.0*PI);	
			}
			break;

		case GLUT_KEY_UP :
			if (glutGetModifiers()==GLUT_ACTIVE_CTRL) 
			{
				printf("CTRL \n");
				MoveViewer(8.0);					
				if(Collision()) MoveViewer(-10.0);					
				break;
			}
			else 
			{
				printf("Up directional key. \n");
				MoveViewer(4.0);					
				if(Collision()) MoveViewer(-6.0);					
				break;
			}
			
		case GLUT_KEY_DOWN :
			if (glutGetModifiers()==GLUT_ACTIVE_CTRL) 
			{
				printf("CTRL \n");
				MoveViewer(-8.0);					
				if(Collision()) MoveViewer(10.0);					
				break;
			}
			else 
			{
				printf("Down directional key. \n");
				MoveViewer(-4.0);					
				if(Collision()) MoveViewer(6.0);					
				break;
			}	

		case GLUT_KEY_PAGE_UP:
			jump();

			break;

		case GLUT_KEY_PAGE_DOWN:
			sit();
			break;

		default :
			printf("Function key. \n");
			break;
	}
	display();
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(400,400);
	glutInitWindowPosition(400,400);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("3D Modeling");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	InitWalk();
	InitTexture();
	GLSetupRC();
	glutMainLoop();
	return EXIT_SUCCESS;            /* ANSI C requires main to return int. */
}

