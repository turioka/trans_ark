
#include <stdlib.h>  
#include <stdio.h>  
#include <math.h>
#include <GLFW/glfw3.h>
#include <time.h>


#define BALLCY 16  
#define BALLCX  16  
#define BARCY 10
#define BARCX  40
#define BLOCKCX 60
#define BLOCKCY 30
#define PIC_RESIZE 2
#define MOVE_UNIT 0.85
#define ROOT_2 1.41421356f
#define FPS 60.f
#define NBLOCKX 10
#define NBLOCKY 10
typedef struct {
    bool right;
    bool left;
    bool up;
    bool down;
}keyStat;
keyStat G_keying;
bool exist[NBLOCKX][NBLOCKY];
int G_displayCX = 1440;
int G_displayCY = 1040;
int G_ballwindowCX = 100;
int G_ballwindowCY = 100;
int G_barwindowCX = 200;
int G_barwindowCY = 100;

int G_blockwindowCX = NBLOCKX*BLOCKCX;
int G_blockwindowCY = 2*NBLOCKY*BLOCKCY;

int G_start;
float G_rad;


char forDebug[100];

static GLubyte imageBall[BALLCY][BALLCX][4];
static GLubyte imageBar[BARCY][BARCX][4];

static void drawBlock(float rx, float ry) {
    glBegin(GL_QUADS);
    glColor3f(0, 0, 0);
    glVertex2f(-1+rx, 1+ry);
    glVertex2f(-0.8 + rx, 1 + ry);
    glVertex2f(-0.8 + rx, 0.9 + ry);
    glVertex2f(-1 + rx, 0.9 + ry);
    glEnd();

    glBegin(GL_LINE_LOOP);

    glColor3f(0.8, 0.8, 0.8);
    glVertex2f(-0.99 + rx, 0.99 + ry);
    glVertex2f(-0.81 + rx, 0.99 + ry);
    glVertex2f(-0.81 + rx, 0.91 + ry);
    glVertex2f(-0.99 + rx, 0.91 + ry);
    glEnd();
    glBegin(GL_POINTS);

    glColor3f(0.8, 0.8, 0.8);
    glVertex2f(-0.99 + rx, 0.99 + ry);
    glVertex2f(-0.81 + rx, 0.99 + ry);
    glVertex2f(-0.81 + rx, 0.91 + ry);
    glVertex2f(-0.99 + rx, 0.91 + ry);
    glEnd();
}

static void key_callback(GLFWwindow* hiwindow, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)glfwSetWindowShouldClose(hiwindow, GLFW_TRUE);
    if (key == GLFW_KEY_Z && action == GLFW_PRESS)G_start = 1;
    if (key == GLFW_KEY_RIGHT) {
        if (action == GLFW_PRESS) {
            G_keying.right = true;
        }
        if (action == GLFW_RELEASE) {
            G_keying.right = false;
        }
    }
    if (key == GLFW_KEY_LEFT) {
        if (action == GLFW_PRESS) {
            G_keying.left = true;
        }
        if (action == GLFW_RELEASE) {
            G_keying.left = false;
        }
    }
    if (key == GLFW_KEY_UP) {
        if (action == GLFW_PRESS) {
            G_keying.up = true;
        }
        if (action == GLFW_RELEASE) {
            G_keying.up = false;
        }
    }
    if (key == GLFW_KEY_DOWN) {
        if (action == GLFW_PRESS) {
            G_keying.down = true;
        }
        if (action == GLFW_RELEASE) {
            G_keying.down = false;
        }
    }
}

// make bitmap directly
void initBarTexture(void) 
{
    int i, j, c;
    for (i = 0; i < BARCY; i++) {
        for (j = 0; j < BARCX; j++) {
            int color = 0;

            switch (i) {

            case 0:
            case 9:
                color = 4 <= j && j <= BARCX-5 ? 1 : color;
                break;
            case 1:
            case 8:
                color = 2 <= j && j <= BARCX - 3 ? 1 : color;
                color = 4 <= j && j <= BARCX - 5 ? 192 : color;
                break;
            case 2:
            case 7:
            case 3:
            case 6:
                color = j == 1 || j == BARCX - 2 ? 1 : color;
                color = 2 <= j && j <= BARCX - 3 ? 192 : color;
                break;
            
               
            case 4:
            case 5:
                color = j == 0 || j == BARCX - 1 ? 1 : color;
                color = 1 <= j && j <= BARCX - 2 ? 192 : color;
                color = 4 <= j && j <= BARCX - 5 ? 255 : color;

                break;
            default:
                color = 0;
                break;
            }
            imageBar[i][j][0] = imageBar[i][j][1] = imageBar[i][j][2] = color;
            int alpha = color != 0 ? 255 : 0;
            imageBar[i][j][3] = alpha;



        }
    }
}
// make bitmap directly
void initBallTexture(void)
{
    int i, j, c;
    for (i = 0; i < BALLCY; i++) {
        for (j = 0; j < BALLCX; j++) {
            int color = 0;
            
            switch (i) {
                
            case 0:   
            case 15:
                color = 5 <= j && j <= 10 ? 1 : color;                
                break;
            case 1:    
            case 14:
                color = 3 <= j && j <= 12 ? 1 : color;
                color = 5 <= j && j <= 10 ? 128 : color;
                break;
            case 2:
            case 13:
                color = j == 2 || j == 13 ? 1 : color;
                color = 3 <= j && j <= 12 ? 128 : color;
                color = 5 <= j && j <= 10 ? 192 : color;
                break;
            case 3:
            case 12:
                color = j == 1 || j == 14 ? 1 : color;
                color = 2 <= j && j <= 13 ? 128 : color;
                color = 4 <= j && j <= 11 ? 192 : color;
                break;
            case 4:
            case 11:
                color = j == 1 || j == 14 ? 1 : color;
                color = j == 2 || j == 13 ? 128 : color;
                color = 3 <= j && j <= 12 ? 192 : color;
                
                break;
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
                color = j == 0 || j == 15 ? 1 : color;
                color = j == 1 || j == 14 ? 128 : color;
                color = 2 <= j && j <= 13 ? 192 : color;
                break;
            }
            imageBall[i][j][0] = imageBall[i][j][1] = imageBall[i][j][2] = color;
            int alpha = color != 0 ? 255 : 0;
            imageBall[i][j][3] = alpha;
        }
    }
    imageBall[11][4][0] = imageBall[11][4][1] = imageBall[11][4][2] = 255;
    imageBall[10][4][0] = imageBall[10][4][1] = imageBall[10][4][2] = 255;
    imageBall[10][3][0] = imageBall[10][3][1] = imageBall[10][3][2] = 255;
    imageBall[9][3][0] = imageBall[9][3][1] = imageBall[9][3][2] = 255;
}

void initBar(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BARCX, BARCY,
        0, GL_RGBA, GL_UNSIGNED_BYTE, imageBar);
}

void initBall(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, BALLCX, BALLCY,
        0, GL_RGBA, GL_UNSIGNED_BYTE, imageBall);
}

void displayTex(float x, float y, float cx, float cy)
{
    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex2f(x, y);
    glTexCoord2f(0.0, 1.0); glVertex2f(x, y+cy);
    glTexCoord2f(1.0, 1.0); glVertex2f(x+cx, y+ cy);
    glTexCoord2f(1.0, 0.0); glVertex2f(x+cx, y);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void display(float x, float y, float cx, float cy)
{
    static float spin = 0.0;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    {
        //glTranslatef(0.0, 0.0, -20.0);
        glColor3f(1.0, 1.0, 1.0);
        displayTex(x,y,cx,cy);
    }
    glPopMatrix();
    glFlush();
}

void reshape(int w, int h)
{
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 500.0);
    glOrtho(0, w / PIC_RESIZE, h / PIC_RESIZE, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

//move window depending on key
static void moveByKey(GLFWwindow* movedWindow, float deltaT) {
    int lx, ly;
    float  lcodeX = 0, lcodeY = 0;
    if (G_keying.left)lcodeX = -1;
    if (G_keying.right)lcodeX = 1;
    if (G_keying.up)lcodeY = -1;
    if (G_keying.down)lcodeY = 1;
    if (fabsf(lcodeY) + fabsf(lcodeX) > 1) {//lcodeX^2+lcodeY^2=1
        lcodeY /= ROOT_2;
        lcodeX /= ROOT_2;
    }
    glfwGetWindowPos(movedWindow, &lx, &ly);
    lx += (int)(lcodeX * MOVE_UNIT * deltaT);
    ly += (int)(lcodeY * MOVE_UNIT * deltaT);

    glfwSetWindowPos(movedWindow, lx, ly);
}

//make focus on window3
static void focusOrder(GLFWwindow* window, GLFWwindow* window2, GLFWwindow* window3) {
    if (glfwGetWindowAttrib(window2, GLFW_FOCUSED) || glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
        glfwFocusWindow(window2);
        glfwFocusWindow(window3);
    }
}
inline float reflectRad(float inrad, float axrad) {
    axrad *= inrad*axrad < 0 ? -1 : 1; //make sign of axrad same as that of inrad to return in (-1,1)
    return 2 * axrad - inrad;
}

void collisionANDmove(GLFWwindow* ballWin, GLFWwindow* barWin, GLFWwindow* blockWin, float deltaT) {
        
        int ballWinX, ballWinY, blockWinX, blockWinY, barWinX, barWinY;
        glfwGetWindowPos(ballWin, &ballWinX, &ballWinY);
        glfwGetWindowPos(barWin, &barWinX, &barWinY);
        glfwGetWindowPos(blockWin, &blockWinX, &blockWinY);

        int xrad = (int)(deltaT * cos(G_rad * 3.14));
        int yrad = -(int)(deltaT * sin(G_rad * 3.14));
        int nowx = ballWinX + xrad;
        int nowy = ballWinY + yrad;
        bool leftbottom = (blockWinX < nowx + BALLCX * PIC_RESIZE && nowy < BLOCKCY*NBLOCKY + blockWinY);
        bool righttop = (nowx < BLOCKCX*NBLOCKX + blockWinX && blockWinY < nowy + BALLCY * PIC_RESIZE);
        bool ballOnBlockArea = leftbottom && righttop;

        //ball vs. block
        if (ballOnBlockArea) {
            int nowi, nowj;
            bool rightAttack = (xrad>0);
            if (rightAttack) {
                nowi = (int)((nowx + BALLCX * PIC_RESIZE - blockWinX) / BLOCKCX);
            }
            else {
                nowi = (int)((nowx - blockWinX) / BLOCKCX);
            }
            
            bool bottomAttack = (yrad>0);
            if (bottomAttack) {
                nowj = (int)((nowy + BALLCY * PIC_RESIZE - blockWinY) / BLOCKCY);
            }
            else {
                nowj = (int)((nowy - blockWinY) / BLOCKCY);
            }

            if (exist[nowi][nowj]) {
                exist[nowi][nowj] = false;
                bool ballOnTheBlock = BLOCKCY * nowj + blockWinY < nowy + BALLCY * PIC_RESIZE && nowy < BLOCKCY* (nowj + 1) + blockWinY && BLOCKCX * nowi + blockWinX < nowx + BALLCX * PIC_RESIZE && nowx < BLOCKCX* (nowi + 1) + blockWinX;
                bool yattack = !(BLOCKCY * nowj + blockWinY < ballWinY + BALLCY * PIC_RESIZE && ballWinY < BLOCKCY* (nowj + 1) + blockWinY);
                if (ballOnTheBlock) {
                    if (yattack) {
                        G_rad = reflectRad(G_rad, 0);
                    }
                    else {
                        G_rad = reflectRad(G_rad, 0.5);
                    }
                }
            }
        }

        //ball vs. bar
        if (barWinY+BARCY*PIC_RESIZE> nowy && nowy + BALLCY*PIC_RESIZE > barWinY) {
            if (barWinX < nowx + BALLCX * PIC_RESIZE && nowx < barWinX + BARCX * PIC_RESIZE) {
                //change axis of reflect
                //nxtrad in [0.15,0.85]
                float nxtrad = -0.7f * (ballWinX - (barWinX - BALLCX * PIC_RESIZE)) / ((BARCX * PIC_RESIZE) - (-BALLCX * PIC_RESIZE)) + 0.85f;
                G_rad = G_rad > 0 ? -nxtrad : nxtrad;

                
                //move along bar
                int barWinX2, barWinY2;
                glfwGetWindowPos(barWin, &barWinX2, &barWinY2);
                ballWinX += barWinX2 - barWinX;
                ballWinY += barWinY2 - barWinY;
            }
        }
        
        //ball vs. display
        if (nowx <0) {
            G_rad = reflectRad(G_rad, 0.5);
            ballWinX = 0;
        }
        else if (nowy < 0) {
            G_rad = reflectRad(G_rad, 0);
            ballWinY = 0;
        }
        if (nowx+BALLCX * PIC_RESIZE > G_displayCX) {
            G_rad = reflectRad(G_rad, 0.5);
            ballWinX = G_displayCX - BALLCX * PIC_RESIZE;
        }
        else if (nowy+BALLCY * PIC_RESIZE > G_displayCY) {
            G_rad = reflectRad(G_rad, 0);
            ballWinY = G_displayCY - BALLCY * PIC_RESIZE;
        }

        //move
        xrad = (int)(deltaT * cos(G_rad * 3.14));
        yrad = -(int)(deltaT * sin(G_rad * 3.14));
        glfwSetWindowPos(ballWin, ballWinX + xrad, ballWinY + yrad);
        
    
}
int main()
{
    G_start = 0;
    G_rad = 0.4;
    GLFWwindow* ballWin, * blockWin, * barWin;
    int windowSizeW = 640, windowSizeH = 600;

    // initialize the library
    if (!glfwInit())
        return -1;

    initBallTexture();
    initBarTexture();
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);
    G_displayCX = mode->width;
    G_displayCY = mode->height;
    int baseY = 550;
    //ball window
    {
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

        glfwWindowHint(GLFW_RELEASE_BEHAVIOR_FLUSH, GLFW_TRUE);
        ballWin = glfwCreateWindow(G_ballwindowCX, G_ballwindowCY, "Ball", NULL, NULL);
        if (!ballWin)
        {
            glfwTerminate();
            return -1;
        }
        glfwSetWindowPos(ballWin, G_displayCX / 2 - BALLCX * PIC_RESIZE / 2, G_displayCY / 2 - BALLCY * PIC_RESIZE);
        glfwSetWindowAttrib(ballWin, GLFW_DECORATED, GLFW_FALSE);
        glfwMakeContextCurrent(ballWin);
    }

    //bar window
    {
        glfwWindowHint(GLFW_RELEASE_BEHAVIOR_FLUSH, GLFW_TRUE);

        barWin = glfwCreateWindow(G_barwindowCX, G_barwindowCY, "Bar ", NULL, NULL);
        if (!barWin)
        {
            glfwTerminate();
            return -1;
        }
        glfwSetWindowPos(barWin, G_displayCX / 2 - BARCX * PIC_RESIZE / 2, G_displayCY / 2);
        glfwSetWindowAttrib(barWin, GLFW_DECORATED, GLFW_FALSE);
        glfwMakeContextCurrent(barWin);
    }
    glfwDefaultWindowHints();

    //block window
    {
        glfwWindowHint(GLFW_RELEASE_BEHAVIOR_FLUSH, GLFW_TRUE);
        
        blockWin = glfwCreateWindow(G_blockwindowCX, G_blockwindowCX, "move window (press escape key to finish)", NULL, NULL);
        if (!blockWin)
        {
            glfwTerminate();
            return -1;
        }
        glfwSetWindowPos(blockWin,G_displayCX/2-300 , G_displayCY / 2 - 450);
        glfwMakeContextCurrent(blockWin);
    }

    glfwDefaultWindowHints();


    // show the window
    glfwShowWindow(blockWin);
    glfwShowWindow(ballWin);
    glfwShowWindow(barWin);
    for (int i = 0; i < NBLOCKX; i++) {
        for (int j = 0; j < NBLOCKY; j++) {
            exist[i][j] = true;
        }

    }
    //init context
    glfwMakeContextCurrent(ballWin);
    initBall();

    glfwMakeContextCurrent(barWin);
    initBar();


    glfwSetKeyCallback(barWin, key_callback);
    double nowtime, lasttime = 0;
    lasttime = glfwGetTime();

    /**************
    *  main loop  *
    **************/
    while (!glfwWindowShouldClose(barWin)&& !glfwWindowShouldClose(blockWin)&& !glfwWindowShouldClose(ballWin))// Loop until the user closes the window
    {
        //wait one frame
        nowtime = glfwGetTime();
        while (nowtime - lasttime < 1.f/FPS)nowtime = glfwGetTime();// wait enough to floor
        float deltaT = (float)((nowtime - lasttime) * 1000);
        lasttime = nowtime;
        
        focusOrder(blockWin, ballWin, barWin);

        moveByKey(barWin, deltaT);
        if (G_start == 0) {
            moveByKey(ballWin, deltaT);
        }
        else {
            collisionANDmove(ballWin, barWin, blockWin, deltaT);
        }


        //block window
        glfwMakeContextCurrent(blockWin);
        {
            //ban maximize window
            if (glfwGetWindowAttrib(blockWin, GLFW_MAXIMIZED)) {
                glfwRestoreWindow(blockWin);
            }
            //draw blocks
            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            glLineWidth(6.0f);
            glPointSize(6.0f);
            for (int i = 0; i < NBLOCKX; i++) {
                for (int j = 0; j < NBLOCKY; j++) {
                    if (exist[i][j])drawBlock(0.2 * i, -0.1 * j);
                   
                }
            }
        }

        //ball window
        glfwMakeContextCurrent(ballWin);
        {
            display(0,0,BALLCX,BALLCY);
            reshape(G_ballwindowCX, G_ballwindowCY);
        }

        //bar window
        glfwMakeContextCurrent(barWin);
        {
            display(0,0,BARCX,BARCY);
            reshape(G_barwindowCX, G_barwindowCY);
        }

        // last
        glfwSwapBuffers(ballWin);
        glfwSwapBuffers(blockWin);
        glfwSwapBuffers(barWin);
        glfwPollEvents();
    }


    
   

    glfwTerminate();
    return 0;
}
