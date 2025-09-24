// Build: -lglfw -lGL -lm -ldl -lpthread -lX11 -lXrandr -lXi
#include <GLFW/glfw3.h>
#include <algorithm>
#include <string>
#include <chrono>
#include <cstdio>

static int fbW=800, fbH=480;
static const int sqSize=50;
static int sqX=100, sqY=100;
static bool dragging=false;
static int dragDX=0, dragDY=0;

static bool mirrorX=false; // press H to toggle
static bool mirrorY=false; // press U to toggle
static bool vsyncOn=true;

static inline int clampi(int v,int lo,int hi){ return std::max(lo,std::min(v,hi)); }

static void setPixelOrtho(){
    glViewport(0,0,fbW,fbH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0,(double)fbW,0.0,(double)fbH,-1.0,1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Apply view-space flips so visuals correct on mirrored screens
    if(mirrorX){ glTranslatef((float)fbW,0.f,0.f); glScalef(-1.f,1.f,1.f); }
    if(mirrorY){ glTranslatef(0.f,(float)fbH,0.f); glScalef(1.f,-1.f,1.f); }
}

static void remapForMirrors(int& x, int& y){
    if(mirrorX) x = fbW - 1 - x;
    if(mirrorY) y = fbH - 1 - y;
}

static void drawText5x7(int x, int y, const std::string& s, int scale=2){
    // 5x7 pixel font for needed chars
    auto rowsOf=[&](char c)->const unsigned char*{
        static const unsigned char SPC[7]={0,0,0,0,0,0,0};
        static const unsigned char COLON[7]={0,0b00100,0,0,0b00100,0,0};
        static const unsigned char D_[7]={0b11110,0b10001,0b10001,0b10001,0b10001,0b10001,0b11110};
        static const unsigned char E_[7]={0b11111,0b10000,0b10000,0b11110,0b10000,0b10000,0b11111};
        static const unsigned char F_[7]={0b11111,0b10000,0b10000,0b11110,0b10000,0b10000,0b10000};
        static const unsigned char M_[7]={0b10001,0b11011,0b10101,0b10101,0b10001,0b10001,0b10001};
        static const unsigned char O_[7]={0b01110,0b10001,0b10001,0b10001,0b10001,0b10001,0b01110};
        static const unsigned char P_[7]={0b11110,0b10001,0b10001,0b11110,0b10000,0b10000,0b10000};
        static const unsigned char S_[7]={0b01111,0b10000,0b10000,0b01110,0b00001,0b00001,0b11110};
        static const unsigned char e_[7]={0b00000,0b00000,0b01110,0b10001,0b11111,0b10000,0b01111};
        static const unsigned char m_[7]={0b00000,0b00000,0b11010,0b10101,0b10101,0b10101,0b10101};
        static const unsigned char o_[7]={0b00000,0b00000,0b01110,0b10001,0b10001,0b10001,0b01110};
        static const unsigned char N0[7]={0b01110,0b10001,0b10011,0b10101,0b11001,0b10001,0b01110};
        static const unsigned char N1[7]={0b00100,0b01100,0b00100,0b00100,0b00100,0b00100,0b01110};
        static const unsigned char N2[7]={0b01110,0b10001,0b00001,0b00010,0b00100,0b01000,0b11111};
        static const unsigned char N3[7]={0b11110,0b00001,0b00001,0b01110,0b00001,0b00001,0b11110};
        static const unsigned char N4[7]={0b00010,0b00110,0b01010,0b10010,0b11111,0b00010,0b00010};
        static const unsigned char N5[7]={0b11111,0b10000,0b11110,0b00001,0b00001,0b10001,0b01110};
        static const unsigned char N6[7]={0b00110,0b01000,0b10000,0b11110,0b10001,0b10001,0b01110};
        static const unsigned char N7[7]={0b11111,0b00001,0b00010,0b00100,0b01000,0b01000,0b01000};
        static const unsigned char N8[7]={0b01110,0b10001,0b10001,0b01110,0b10001,0b10001,0b01110};
        static const unsigned char N9[7]={0b01110,0b10001,0b10001,0b01111,0b00001,0b00010,0b01100};
        switch(c){
            case ' ': return SPC; case ':': return COLON;
            case 'D': return D_; case 'E': return E_; case 'F': return F_;
            case 'M': return M_; case 'O': return O_; case 'P': return P_; case 'S': return S_;
            case 'e': return e_; case 'm': return m_; case 'o': return o_;
            case '0': return N0; case '1': return N1; case '2': return N2; case '3': return N3; case '4': return N4;
            case '5': return N5; case '6': return N6; case '7': return N7; case '8': return N8; case '9': return N9;
            default:  return SPC;
        }
    };

    glBegin(GL_QUADS);
    for(char c : s){
        const unsigned char* rows = rowsOf(c);
        for(int ry=0; ry<7; ++ry){
            unsigned char bits = rows[ry];
            for(int rx=0; rx<5; ++rx){
                if(bits & (1<<(4-rx))){
                    int px=x+rx*2, py=y+ry*2, sc=2;
                    glVertex2i(px,py); glVertex2i(px+sc,py); glVertex2i(px+sc,py+sc); glVertex2i(px,py+sc);
                }
            }
        }
        x += 12; // advance (5*2 + spacing)
    }
    glEnd();
}

int main(){
    if(!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
    glfwWindowHint(GLFW_DOUBLEBUFFER,GLFW_TRUE);

    GLFWmonitor* mon=glfwGetPrimaryMonitor();
    const GLFWvidmode* mode=glfwGetVideoMode(mon);
    fbW=mode->width; fbH=mode->height;

    GLFWwindow* win=glfwCreateWindow(fbW,fbH,"Waterbutt GLFW Demo",mon,nullptr);
    if(!win){glfwTerminate();return -1;}
    glfwMakeContextCurrent(win);

    glfwSetInputMode(win,GLFW_CURSOR,GLFW_CURSOR_HIDDEN);
    glfwSwapInterval(1);

    glDisable(GL_DEPTH_TEST);
    glClearColor(0.f,0.f,0.f,1.f);

    glfwSetFramebufferSizeCallback(win,[](GLFWwindow*,int w,int h){fbW=w;fbH=h;});
    glfwSetKeyCallback(win,[](GLFWwindow* w,int key,int,int action,int){
        if(action!=GLFW_PRESS) return;
        if(key==GLFW_KEY_V){ vsyncOn=!vsyncOn; glfwSwapInterval(vsyncOn?1:0); }
        if(key==GLFW_KEY_H){ mirrorX=!mirrorX; }
        if(key==GLFW_KEY_U){ mirrorY=!mirrorY; }
        if(key==GLFW_KEY_ESCAPE){ glfwSetWindowShouldClose(w,1); }
    });

    glfwSetMouseButtonCallback(win,[](GLFWwindow* w,int button,int action,int){
        if(button!=GLFW_MOUSE_BUTTON_LEFT) return;
        double cx,cy; glfwGetCursorPos(w,&cx,&cy);
        int x=(int)cx, y=fbH-1-(int)cy;
        remapForMirrors(x,y);
        if(action==GLFW_PRESS){
            if(x>=sqX && x<=sqX+sqSize && y>=sqY && y<=sqY+sqSize){
                dragging=true; dragDX=x-sqX; dragDY=y-sqY;
            }
        } else if(action==GLFW_RELEASE){ dragging=false; }
    });
    glfwSetCursorPosCallback(win,[](GLFWwindow*,double cx,double cy){
        if(!dragging) return;
        int x=(int)cx, y=fbH-1-(int)cy;
        remapForMirrors(x,y);
        sqX=clampi(x - dragDX, 0, fbW - sqSize);
        sqY=clampi(y - dragDY, 0, fbH - sqSize);
    });

    auto lastT=std::chrono::high_resolution_clock::now();
    int frames=0; double fps=0.0;

    while(!glfwWindowShouldClose(win)){
        auto now=std::chrono::high_resolution_clock::now();
        frames++;
        double sec=std::chrono::duration<double>(now-lastT).count();
        if(sec>=1.0){ fps=frames/sec; frames=0; lastT=now; }

        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);
        setPixelOrtho();

        // Square
        glColor3f(1.f,0.f,0.f);
        int x0=sqX,y0=sqY,x1=sqX+sqSize,y1=sqY+sqSize;
        glBegin(GL_TRIANGLES);
            glVertex2i(x0,y0); glVertex2i(x1,y0); glVertex2i(x1,y1);
            glVertex2i(x0,y0); glVertex2i(x1,y1); glVertex2i(x0,y1);
        glEnd();

        // Text
        glColor3f(1.f,1.f,1.f);
        drawText5x7(10, fbH-20, "Demo");
        char buf[32]; std::snprintf(buf,sizeof(buf),"FPS: %d",(int)(fps+0.5));
        int w = (int)std::string(buf).size() * 12;
        drawText5x7(fbW - w - 10, 10, buf);

        glfwSwapBuffers(win);
    }

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}