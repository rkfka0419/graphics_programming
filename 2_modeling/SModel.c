#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>

#define	WIRE 0		
#define	SHADE 1		
#define PI 3.141592

GLfloat light_diffuse[] = {1.0, 0.0, 0.0, 1.0};  /* Red diffuse light. */
GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};  /* Infinite light location. */
GLfloat	light_specular[] = {1.0, 1.0, 1.0, 1.0}; /* specular light */
GLfloat	light_ambient[] = {0.3, 0.3, 0.3, 1.0};  /* ambient light */

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

GLfloat angle1 = -150;   /* in degrees */
GLfloat angle2 = -150;
GLfloat light_angle1 = -150;   /* in degrees */
GLfloat light_angle2 = -150;   /* in degrees */

float lookat_x, lookat_y, lookat_z = 1500;

GLfloat xloc = 0, yloc = 0, zloc = 0;
int moving, beginx, beginy;
int light_moving;
float scalefactor = 1.0;
int scaling = 0;         
int status=0;           // WIRE or SHADE
int cull=0;             // CULLING toggle 
char *fname="cube.dat";

void DrawWire(void)
{
//	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //배경색
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 픽셀별로 깊이 값 저장
	//3D 를 2D로 표현하기 위해 사용
	glEnable (GL_DEPTH_TEST); // 깊이 활성화
	glEnable (GL_LIGHTING); // 조명 활성화
	glEnable (GL_LIGHT0); // 조명 갯수
	glEnable(GL_COLOR_MATERIAL); // 색 활성화
	if (cull) glEnable(GL_CULL_FACE); // 뒷면 활성화
	else glDisable(GL_CULL_FACE); // 뒷면 비활성화

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // 앞면과 뒷면의 선 표현
	glCallList(1); // 만들어진 리스트 호출
	glutSwapBuffers(); // 출력 버퍼와 백버퍼 스왑
}

void DrawShade(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable (GL_DEPTH_TEST);
	glEnable (GL_LIGHTING);
	glEnable (GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	if (cull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCallList(1);
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

void MakeGL_Model(void)
{
	int i;
	Point norm;	//폴리곤의 노말 벡터
	glShadeModel(GL_SMOOTH);  // 부드럽게 쉐이딩

	glPushMatrix();	// 행렬 스택에 푸쉬
	glRotatef(light_angle1, 0.0, 1.0, 0.0); //y축으로 조명 회전
	glRotatef(light_angle2, 1.0, 0.0, 0.0); //x축으로 조명 회전

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); //난반사 설정
	glLightfv(GL_LIGHT0, GL_POSITION, light_specular);// 정반사 설정
	glLightfv(GL_LIGHT0, GL_POSITION, light_ambient);// 주변광 설정
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);//조명 위치 설정
	glEnable(GL_LIGHT0);// 설정한 LIGHT0 사용 가능
	glEnable(GL_LIGHTING);//조명을 켬
	glPopMatrix();//행렬 스택에서 POP

	if (glIsList(1)) glDeleteLists(1, 1); // 모델 리스트 설정
	glNewList(1, GL_COMPILE);
	glPushMatrix();
	glTranslatef(xloc, yloc, zloc); //물체 이동 변환

	glRotatef(angle1, 0.0, 1.0, 0.0);//물체 회전 변환 y축
	glRotatef(angle2, 1.0, 0.0, 0.0);//물체 회전 변환 x축
    glScalef(scalefactor, scalefactor, scalefactor);//물체 크기 변환
	for (i = 0; i < fnum; i++) {
		norm = cnormal(mpoint[mface[i].ip[2]], mpoint[mface[i].ip[1]],
			mpoint[mface[i].ip[0]]);//면 normal 계산
		glBegin(GL_TRIANGLES); // 삼각형으로 면 생상
		glNormal3f(norm.x, norm.y, norm.z); // 면 normal 설정
		glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y,
			mpoint[mface[i].ip[0]].z); // 1번째 점 설정
		glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y,
			mpoint[mface[i].ip[1]].z); // 2번째 점 설정
		glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y,
			mpoint[mface[i].ip[2]].z); // 3번째 점 설정
		glEnd();
	}

	glPopMatrix(); // 행렬 스택에서 POP
	glEndList();
}

void GLSetupRC(void) 
{
  /* Enable a single OpenGL light. */
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); // 난반사 설정
  glLightfv(GL_LIGHT0, GL_POSITION, light_specular); // 정반사 설정
  glLightfv(GL_LIGHT0, GL_POSITION, light_ambient);  // 주변광 설정
  glLightfv(GL_LIGHT0, GL_POSITION, light_position); // 조명의 위치 설정
  glEnable(GL_LIGHT0);	// 설정한 LIGHT0 사용 가능
  glEnable(GL_LIGHTING);// 설정한 조명들을 켬


  /* Use depth buffering for hidden surface elimination. */
  glEnable(GL_DEPTH_TEST);	// 깊이 테스트 켬

  /* Setup the view */
  glMatrixMode(GL_PROJECTION);		// 프로젝션 설정
  glLoadIdentity();					//프로젝션 행렬 초기화 -> 단위 행렬로 대치
  
  gluPerspective(  40.0, // field of view in degree 
  1.0, // aspect ratio 
  1.0, // Z near 
  2000.0); // Z far 
  glMatrixMode(GL_MODELVIEW);	// 시각좌표계 UVN 설정
  glLoadIdentity();
  gluLookAt(400.0, 400.0, 400.0,  // eye is at (0,0,5) 
    0.0, 0.0, 0.0,      // center is at (0,0,0) 
    0.0, 1.0, 0.);      // up is in positive Y direction 
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	MakeGL_Model();
	if (status==WIRE) DrawWire();
	else DrawShade();
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
	fscanf(f1,"%s",s);     printf( "%s", s ); // VERTEX
	fscanf(f1,"%s",s);     printf( "%s", s ); // =
	fscanf(f1,"%d",&pnum);     printf( "%d\n", pnum); //vertex의 갯수

    mpoint = (Point*)malloc(sizeof(Point)*pnum); //vertex 갯수만큼 Point 할당

	for (i=0; i<pnum; i++){
		fscanf(f1,"%f",&mpoint[i].x); //x좌표 읽음
		fscanf(f1,"%f",&mpoint[i].y); //y좌표 읽음
		fscanf(f1,"%f",&mpoint[i].z); //z좌표 읽음
	    printf( "%f %f %f\n", mpoint[i].x, mpoint[i].y,mpoint[i].z);
	}

	fscanf(f1,"%s",s);     printf( "%s", s ); // FACE
	fscanf(f1,"%s",s);     printf( "%s", s ); // =
	fscanf(f1,"%d",&fnum);     printf( "%d\n", fnum); // FACE 갯수 읽음

	mface = (Face*)malloc(sizeof(Face)*fnum); // FACE 갯수로 Face 할당
	for (i=0; i<fnum; i++){
		fscanf(f1,"%d",&mface[i].ip[0]); // 1번째 점의 인덱스 읽음
		fscanf(f1,"%d",&mface[i].ip[1]); // 2번째 점의 인덱스 읽음
		fscanf(f1,"%d",&mface[i].ip[2]); // 3번째 점의 인덱스 읽음
	    printf("%d %d %d\n", mface[i].ip[0], mface[i].ip[1], mface[i].ip[2]);
	}
	fclose(f1);
}

void mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    moving = 1;
	beginx = x;
	beginy = y;
  }
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    moving = 0;
  }

  if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
	scaling = 1;
	beginx = x;
	beginy = y;
  }

  if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
      scaling = 0;
  }
  if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
	  scaling = 1; // 부드럽게 1씩 증감
	  beginx = x;
	  beginy = y;
  }

  if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
	  scaling = 0;
  }

  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
	scaling=0;
	light_moving = 1;
	beginx = x;
	beginy = y;
  }
  if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
	  light_moving = 0;
  }
}

void motion(int x, int y)
{
  if (scaling) {
	  scalefactor = scalefactor * (1.0 + (beginx - x)*0.0001);
	  scalefactor = scalefactor * (1.0 + (beginy - y)*0.0001);
    glutPostRedisplay();
  }
  if (moving) {
	  angle1 = angle1 + (x - beginx);
	  angle2 = angle2 + (beginy - y);
	  beginx = x;
	  beginy = y;
    glutPostRedisplay();
  }
  if (light_moving) {
	  light_angle1 = light_angle1 + (x - beginx);
	  light_angle2 = light_angle2 + (beginy - y);
	  beginx = x;
	  beginy = y;
    glutPostRedisplay();
  }
}


void transLocation(int key, int x, int y)  //방향키입력을 위한 함수
{
	switch (key) {
	case GLUT_KEY_RIGHT:
		xloc += 10;
		glutPostRedisplay();
		break;
	case GLUT_KEY_UP:
		zloc -= 10;
		glutPostRedisplay();
		break;
	case GLUT_KEY_DOWN:
		zloc += 10;
		glutPostRedisplay();
		break;
	case GLUT_KEY_PAGE_UP:
		yloc += 10;
		glutPostRedisplay();
		break;
	case GLUT_KEY_PAGE_DOWN:
		yloc -= 10;
		glutPostRedisplay();
		break;
	case GLUT_KEY_LEFT:
		xloc -= 10;
		glutPostRedisplay();
		break;
	}

}



void SORModel()
{
	int i;
	int IncAngle = 1;

	pnum = (360 / IncAngle) * 2;
	mpoint = (Point*)malloc(sizeof(Point)*pnum);

	mpoint[0].x = 100;
	mpoint[0].y = -100;
	mpoint[0].z = 100;
	mpoint[1].x = 100;
	mpoint[1].y = 100;
	mpoint[1].z = 100;

	for (i = 0; i<pnum - 2; i++) {
		mpoint[i + 2].x = cos(IncAngle*PI / 180)*mpoint[i].x - sin(IncAngle*PI / 180)*mpoint[i].z;
		mpoint[i + 2].y = mpoint[i].y;
		mpoint[i + 2].z = sin(IncAngle*PI / 180)*mpoint[i].x + cos(IncAngle*PI / 180)*mpoint[i].z;
	}

	fnum = 2 * 360 / IncAngle;
	mface = (Face*)malloc(sizeof(Face)*fnum);
	for (i = 0; i<pnum; i += 2) {
		mface[i].ip[0] = i%pnum;
		mface[i].ip[1] = (i + 1) % pnum;
		mface[i].ip[2] = (i + 1 + 2) % pnum;

		mface[i + 1].ip[0] = i%pnum;
		mface[i + 1].ip[1] = (i + 1 + 2) % pnum;
		mface[i + 1].ip[2] = (i + 2) % pnum;

	}
}

void waterBottle()
{
	int i, j;
	int IncAngle = 5;			// 회전 증가 변수 (5도씩 회전)
	pnum = (360 / IncAngle) * 17; 		// 점의 개수
	mpoint = (Point*)malloc(sizeof(Point)*pnum); // 점의 개수 할당
	mpoint[0].x = 0; mpoint[0].y = 100; mpoint[0].z = 0;    // 1번째 점의 초기 위치
	mpoint[1].x = 15;	mpoint[1].y = 100; mpoint[1].z = 0; // 2번째 
	mpoint[2].x = 20;	mpoint[2].y = 95;	mpoint[2].z = 10;  // 3번째 
	mpoint[3].x = 20;	mpoint[3].y = 75;	mpoint[3].z = 10;  // 4번째 
	mpoint[4].x = 15;	mpoint[4].y = 75; mpoint[4].z = 0;  // 5번째
	mpoint[5].x = 15;	mpoint[5].y = 70; mpoint[5].z = 0;  // 6번째
	mpoint[6].x = 20; mpoint[6].y = 35; mpoint[6].z = 10;   // 7번째
	mpoint[7].x = 23; mpoint[7].y = 30; mpoint[7].z = 20;  // 8번째 
	mpoint[8].x = 28; mpoint[8].y = 25; mpoint[8].z = 25;  // 9번째
	mpoint[9].x = 35; mpoint[9].y = 0; mpoint[9].z = 35;  // 10번째
	mpoint[10].x = 40; mpoint[10].y = -40;	mpoint[10].z = 40; // 11번째 
	mpoint[11].x = 40; mpoint[11].y = -90;	mpoint[11].z = 35; // 12번째 
	mpoint[12].x = 35; mpoint[12].y = -190;	mpoint[12].z = 40; // 13번째 
	mpoint[13].x = 32; mpoint[13].y = -193;	mpoint[13].z = 35; // 14번째
	mpoint[14].x = 28; mpoint[14].y = -197;	mpoint[14].z = 30; // 15번째
	mpoint[15].x = 26; mpoint[15].y = -200;	mpoint[15].z = 28; // 16번째 
	mpoint[16].x = 0; mpoint[16].y = -200;	mpoint[16].z = 0; // 17번째 


	for (i = 0; i<pnum - 17; i++) {		// 회전에 의한 점 위치 계산
		mpoint[i + 17].x = cos(IncAngle*PI / 180)*mpoint[i].x - sin(IncAngle*PI / 180)*mpoint[i].z;
		mpoint[i + 17].y = mpoint[i].y;
		mpoint[i + 17].z = sin(IncAngle*PI / 180)*mpoint[i].x + cos(IncAngle*PI / 180)*mpoint[i].z;
	}
	fnum = 32 * 360 / IncAngle;		// 면의 개수
	mface = (Face*)malloc(sizeof(Face)*fnum);      // 면의 개수 할당
	for (i = 0, j = 0; i<pnum - 1; i++, j += 2) {		// 계산된 점들로 면 구성
		if (i % 17 == 16) i++;
		mface[j].ip[0] = i%pnum;
		mface[j].ip[1] = (i + 1) % pnum;
		mface[j].ip[2] = (i + 1 + 17) % pnum;
		mface[j + 1].ip[0] = i%pnum;
		mface[j + 1].ip[1] = (i + 1 + 17) % pnum;
		mface[j + 1].ip[2] = (i + 17) % pnum;
	}

}

void cupModel()
{
	int i, j;
	int IncAngle = 5;			// 회전 증가 변수 (5도씩 회전)
	pnum = (360 / IncAngle) * 7; 		// 점의 개수
	mpoint = (Point*)malloc(sizeof(Point)*pnum); // 점의 개수 할당
	mpoint[0].x = 0;	mpoint[0].y = 25;	mpoint[0].z = 0;
	mpoint[1].x = 100;	mpoint[1].y = 25;	mpoint[1].z = 0;
	mpoint[2].x = 100;	mpoint[2].y = 150;	mpoint[2].z = 0;
	mpoint[3].x = 110;	mpoint[3].y = 165;	mpoint[3].z = 0; //컵의 벽면 시작
	mpoint[4].x = 110;	mpoint[4].y = 25;	mpoint[4].z = 0;
	mpoint[5].x = 105;	mpoint[5].y = 0;	mpoint[5].z = 0;
	mpoint[6].x = 0;	mpoint[6].y = 0;	mpoint[6].z = 0;


	for (i = 0; i<pnum - 7; i++) {		// 회전에 의한 점 위치 계산
		mpoint[i + 7].x = cos(IncAngle*PI / 180)*mpoint[i].x - sin(IncAngle*PI / 180)*mpoint[i].z;
		mpoint[i + 7].y = mpoint[i].y;
		mpoint[i + 7].z = sin(IncAngle*PI / 180)*mpoint[i].x + cos(IncAngle*PI / 180)*mpoint[i].z;
	}
	fnum = 12 * 360 / IncAngle;		// 면의 개수
	mface = (Face*)malloc(sizeof(Face)*fnum);      // 면의 개수 할당
	for (i = 0, j = 0; i<pnum - 1; i++, j += 2) {		// 계산된 점들로 면 구성
		if (i % 7 == 6) i++;
		mface[j].ip[0] = i%pnum;
		mface[j].ip[1] = (i + 1) % pnum;
		mface[j].ip[2] = (i + 1 + 7) % pnum;
		mface[j + 1].ip[0] = i%pnum;
		mface[j + 1].ip[1] = (i + 1 + 7) % pnum;
		mface[j + 1].ip[2] = (i + 7) % pnum;
	}



}

void keyboard(unsigned char key, int x, int y)
{
	printf("key %d\n", key);
	switch (key)
	{
	case 'w':
		status = WIRE;
		glutPostRedisplay();
		break;
	case 's':
		status = SHADE;
		glutPostRedisplay();
		break;
	case 'c':
		if (cull) cull = 0;
		else cull = 1;
		glutPostRedisplay();
		break;
	case '1':
		fname = "cube.dat";
		ReadModel();
		glutPostRedisplay();
		break;
	case '2':
		fname = "sphere.dat";
		ReadModel();
		glutPostRedisplay();
		break;
	case '3':
		fname = "teapot.dat";
		ReadModel();
		glutPostRedisplay();
		break;
	case '4':
		fname = "plane.dat";
		ReadModel();
		glutPostRedisplay();
		break;
	case '5':
		SORModel();
		glutPostRedisplay();
		break;
	case '6':
		waterBottle();
		glutPostRedisplay();
		break;
	case '7':
		fname = "chair.dat";
		ReadModel();
		glutPostRedisplay();
		break;
	case '8':
		cupModel();
		glutPostRedisplay();
		break;
	case 'h':
		lookat_x -= 100;
		glLoadIdentity();
		gluLookAt(lookat_x, lookat_y, lookat_z, 0, 0, 0, 0, 1, 0);
		glutPostRedisplay();
		break;
	case 'k':
		lookat_x += 100;
		glLoadIdentity();
		gluLookAt(lookat_x, lookat_y, lookat_z, 0, 0, 0, 0, 1, 0);
		glutPostRedisplay();
		break;
	case 'u':
		lookat_z -= 100;
		glLoadIdentity();
		gluLookAt(lookat_x, lookat_y, lookat_z, 0, 0, 0, 0, 1, 0);
		glutPostRedisplay();
		break;
	case 'j':
		lookat_z += 100;
		glLoadIdentity();
		gluLookAt(lookat_x, lookat_y, lookat_z, 0, 0, 0, 0, 1, 0);
		glutPostRedisplay();
		break;
	case 'y':
		lookat_y -= 100;
		glLoadIdentity();
		gluLookAt(lookat_x, lookat_y, lookat_z, 0, 0, 0, 0, 1, 0);
		glutPostRedisplay();
		break;
	case 'i':
		lookat_y += 100;
		glLoadIdentity();
		gluLookAt(lookat_x, lookat_y, lookat_z, 0, 0, 0, 0, 1, 0);
		glutPostRedisplay();
		break;

	}
}



int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(400,400);
	glutInitWindowPosition(400,400);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // 더블 버퍼 사용
	glutCreateWindow("Simple Modeling");
	glutDisplayFunc(display);	// display() : Rendering Loop 에서 계속 불림
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutSpecialFunc(transLocation);
	ReadModel();
	GLSetupRC();
	glutMainLoop();
	return 0;             /* ANSI C requires main to return int. */
}

