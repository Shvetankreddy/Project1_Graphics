#include <iostream>
#include <vector>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

const int GRID_SIZE=5;
float cubeColor[3]={1.0f,0.2f,0.2f};
int cubeX=2,cubeY=2,cubeZ=2;
bool filled[GRID_SIZE][GRID_SIZE][GRID_SIZE]={false};
glm::vec3 cellColors[GRID_SIZE][GRID_SIZE][GRID_SIZE];
float rotateX=0.0f,rotateY=0.0f;

struct Action{int type;int x,y,z;bool oldFilled;glm::vec3 oldColor;glm::vec3 oldCubeColor;};
vector<Action> history;

const char* vertexShaderSource=R"(
#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aColor;
uniform mat4 transform;
out vec3 ourColor;
void main(){gl_Position=transform*vec4(aPos,1.0);ourColor=aColor;}
)";

const char* fragmentShaderSource=R"(
#version 330 core
in vec3 ourColor;
out vec4 FragColor;
void main(){FragColor=vec4(ourColor,1.0);}
)";

unsigned int createShaderProgram(){
    unsigned int vertexShader=glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram=glCreateProgram();
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

float cubeVertices[]={
    0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,
    0,0,0,1,1,1,0,1,0,1,1,1,1,1,0,1,1,1,1,0,0,1,1,1,
    0,0,0,1,1,1,0,0,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,
    1,0,0,1,1,1,1,1,0,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,
    0,1,0,1,1,1,0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,
    0,0,0,1,1,1,1,0,0,1,1,1,1,0,1,1,1,1,0,0,1,1,1,1
};

unsigned int cubeIndices[]={
    0,1,2,2,3,0,
    4,5,6,6,7,4,
    8,9,10,10,11,8,
    12,13,14,14,15,12,
    16,17,18,18,19,16,
    20,21,22,22,23,20
};

void createColoredCube(unsigned int& VAO,unsigned int& VBO,unsigned int& EBO){
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(cubeVertices),cubeVertices,GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(cubeIndices),cubeIndices,GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

vector<float> gridVertices;

void addLine(glm::vec3 a,glm::vec3 b,glm::vec3 color){
    gridVertices.push_back(a.x);gridVertices.push_back(a.y);gridVertices.push_back(a.z);
    gridVertices.push_back(color.r);gridVertices.push_back(color.g);gridVertices.push_back(color.b);
    gridVertices.push_back(b.x);gridVertices.push_back(b.y);gridVertices.push_back(b.z);
    gridVertices.push_back(color.r);gridVertices.push_back(color.g);gridVertices.push_back(color.b);
}

void generateGrid(){
    gridVertices.clear();
    float start=-GRID_SIZE/2.0f,end=GRID_SIZE/2.0f;
    glm::vec3 lineColor(0.85f,0.85f,0.85f);
    for(int y=0;y<=GRID_SIZE;y++){
        for(int z=0;z<=GRID_SIZE;z++){
            float yy=start+y,zz=start+z;
            addLine(glm::vec3(start,yy,zz),glm::vec3(end,yy,zz),lineColor);
        }
    }
    for(int x=0;x<=GRID_SIZE;x++){
        for(int z=0;z<=GRID_SIZE;z++){
            float xx=start+x,zz=start+z;
            addLine(glm::vec3(xx,start,zz),glm::vec3(xx,end,zz),lineColor);
        }
    }
    for(int x=0;x<=GRID_SIZE;x++){
        for(int y=0;y<=GRID_SIZE;y++){
            float xx=start+x,yy=start+y;
            addLine(glm::vec3(xx,yy,start),glm::vec3(xx,yy,end),lineColor);
        }
    }
}

unsigned int gridVAO,gridVBO;

void createGrid(){
    generateGrid();
    glGenVertexArrays(1,&gridVAO);
    glGenBuffers(1,&gridVBO);
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER,gridVBO);
    glBufferData(GL_ARRAY_BUFFER,gridVertices.size()*sizeof(float),gridVertices.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

bool keyPressed(GLFWwindow* window,int key){
    static bool previous[512]={};
    int state=glfwGetKey(window,key);
    bool pressed=state==GLFW_PRESS&&!previous[key];
    previous[key]=state==GLFW_PRESS;
    return pressed;
}

int main(){
    if(!glfwInit()){
        cout<<"Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window=glfwCreateWindow(1000,800,"Project 1",NULL,NULL);
    if(window==NULL){
        cout<<"Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental=GL_TRUE;

    if(glewInit()!=GLEW_OK){
        cout<<"Failed to initialize GLEW\n";
        glfwTerminate();
        return -1;
    }

    unsigned int shaderProgram=createShaderProgram();
    createGrid();

    unsigned int cubeVAO,cubeVBO,cubeEBO;
    createColoredCube(cubeVAO,cubeVBO,cubeEBO);

    glEnable(GL_DEPTH_TEST);
    glLineWidth(2.5f);
    cout<<"LEFT / RIGHT : Move cube along X-axis\n";
    cout<<"UP / DOWN    : Move cube along Y-axis\n";
    cout<<"U / B        : Move cube along Z-axis\n";
    cout<<"C            : Change cube color\n";
    cout<<"F            : Fill  grid cell with colour \n";
    cout<<"W            : Clear grid cell of the colour \n";
    cout<<"P            : Undo last fill/clear/color change\n";
    cout<<"X            : Clear all grid cells\n";
    cout<<"L / R        : Rotate grid left/right\n";
    cout<<"T / D        : Rotate grid up/down\n";
    cout<<"ESC          : Exit\n";

    for(int x=0;x<GRID_SIZE;x++)
        for(int y=0;y<GRID_SIZE;y++)
            for(int z=0;z<GRID_SIZE;z++)
                cellColors[x][y][z]=glm::vec3(1.0f,1.0f,1.0f);

    while(!glfwWindowShouldClose(window)){
        if(keyPressed(window,GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window,true);

        if(keyPressed(window,GLFW_KEY_LEFT))
            if(cubeX>0) cubeX--;

        if(keyPressed(window,GLFW_KEY_RIGHT))
            if(cubeX<GRID_SIZE-1) cubeX++;

        if(keyPressed(window,GLFW_KEY_DOWN))
            if(cubeY>0) cubeY--;

        if(keyPressed(window,GLFW_KEY_UP))
            if(cubeY<GRID_SIZE-1) cubeY++;

        if(keyPressed(window,GLFW_KEY_U))
            if(cubeZ<GRID_SIZE-1) cubeZ++;

        if(keyPressed(window,GLFW_KEY_B))
            if(cubeZ>0) cubeZ--;

        if(keyPressed(window,GLFW_KEY_C)){
            history.push_back({1,0,0,0,false,glm::vec3(0.0f),glm::vec3(cubeColor[0],cubeColor[1],cubeColor[2])});
            cout<<"\nEnter RGB values (0-1): ";
            cin>>cubeColor[0]>>cubeColor[1]>>cubeColor[2];
            for(int i=0;i<3;i++){
                if(cubeColor[i]<0.0f) cubeColor[i]=0.0f;
                if(cubeColor[i]>1.0f) cubeColor[i]=1.0f;
            }
            cout<<"Cube color changed to RGB ("<<cubeColor[0]<<", "<<cubeColor[1]<<", "<<cubeColor[2]<<")\n";
        }

        if(keyPressed(window,GLFW_KEY_F)){
            history.push_back({0,cubeX,cubeY,cubeZ,filled[cubeX][cubeY][cubeZ],cellColors[cubeX][cubeY][cubeZ],glm::vec3(0.0f)});
            filled[cubeX][cubeY][cubeZ]=true;
            cellColors[cubeX][cubeY][cubeZ]=glm::vec3(cubeColor[0],cubeColor[1],cubeColor[2]);
        }

        if(keyPressed(window,GLFW_KEY_W)){
            history.push_back({0,cubeX,cubeY,cubeZ,filled[cubeX][cubeY][cubeZ],cellColors[cubeX][cubeY][cubeZ],glm::vec3(0.0f)});
            filled[cubeX][cubeY][cubeZ]=false;
        }

        if(keyPressed(window,GLFW_KEY_P)){
            if(!history.empty()){
                Action last=history.back();
                history.pop_back();
                if(last.type==0){
                    filled[last.x][last.y][last.z]=last.oldFilled;
                    cellColors[last.x][last.y][last.z]=last.oldColor;
                }else{
                    cubeColor[0]=last.oldCubeColor.r;
                    cubeColor[1]=last.oldCubeColor.g;
                    cubeColor[2]=last.oldCubeColor.b;
                }
                cout<<"Undo applied\n";
            }else{
                cout<<"Nothing to undo\n";
            }
        }

        if(keyPressed(window,GLFW_KEY_X)){
            for(int x=0;x<GRID_SIZE;x++)
                for(int y=0;y<GRID_SIZE;y++)
                    for(int z=0;z<GRID_SIZE;z++)
                        filled[x][y][z]=false;
            cout<<"Grid cleared\n";
        }

        if(keyPressed(window,GLFW_KEY_L))
            rotateY-=glm::radians(15.0f);

        if(keyPressed(window,GLFW_KEY_R))
            rotateY+=glm::radians(15.0f);

        if(keyPressed(window,GLFW_KEY_T))
            rotateX-=glm::radians(15.0f);

        if(keyPressed(window,GLFW_KEY_D))
            rotateX+=glm::radians(15.0f);

        int width,height;
        glfwGetFramebufferSize(window,&width,&height);
        glViewport(0,0,width,height);
        glClearColor(0.04f,0.04f,0.07f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glm::mat4 model(1.0f);
        model=glm::rotate(model,rotateX,glm::vec3(1.0f,0.0f,0.0f));
        model=glm::rotate(model,rotateY,glm::vec3(0.0f,1.0f,0.0f));

        glm::mat4 view=glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,-10.0f));
        glm::mat4 projection=glm::perspective(glm::radians(45.0f),(float)width/(float)height,0.1f,100.0f);
        glm::mat4 VP=projection*view*model;

        glUseProgram(shaderProgram);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"transform"),1,GL_FALSE,glm::value_ptr(VP));
        glBindVertexArray(gridVAO);
        glDrawArrays(GL_LINES,0,gridVertices.size()/6);

        for(int x=0;x<GRID_SIZE;x++){
            for(int y=0;y<GRID_SIZE;y++){
                for(int z=0;z<GRID_SIZE;z++){
                    if(!filled[x][y][z]) continue;

                    glm::mat4 cellModel(1.0f);
                    cellModel=glm::translate(cellModel,glm::vec3(x-GRID_SIZE/2.0f,y-GRID_SIZE/2.0f,z-GRID_SIZE/2.0f));
                    glm::mat4 transform=projection*view*model*cellModel;

                    glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"transform"),1,GL_FALSE,glm::value_ptr(transform));

                    vector<float> coloredVertices;
                    for(int i=0;i<24;i++){
                        coloredVertices.push_back(cubeVertices[i*6+0]);
                        coloredVertices.push_back(cubeVertices[i*6+1]);
                        coloredVertices.push_back(cubeVertices[i*6+2]);
                        coloredVertices.push_back(cellColors[x][y][z].r);
                        coloredVertices.push_back(cellColors[x][y][z].g);
                        coloredVertices.push_back(cellColors[x][y][z].b);
                    }

                    glBindBuffer(GL_ARRAY_BUFFER,cubeVBO);
                    glBufferSubData(GL_ARRAY_BUFFER,0,coloredVertices.size()*sizeof(float),coloredVertices.data());
                    glBindVertexArray(cubeVAO);
                    glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);
                }
            }
        }

        glm::mat4 cubeModel(1.0f);
        cubeModel=glm::translate(cubeModel,glm::vec3(cubeX-GRID_SIZE/2.0f,cubeY-GRID_SIZE/2.0f,cubeZ-GRID_SIZE/2.0f));
        glm::mat4 cubeTransform=projection*view*model*cubeModel;

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram,"transform"),1,GL_FALSE,glm::value_ptr(cubeTransform));

        vector<float> currentCubeVertices;
        for(int i=0;i<24;i++){
            currentCubeVertices.push_back(cubeVertices[i*6+0]);
            currentCubeVertices.push_back(cubeVertices[i*6+1]);
            currentCubeVertices.push_back(cubeVertices[i*6+2]);
            currentCubeVertices.push_back(cubeColor[0]);
            currentCubeVertices.push_back(cubeColor[1]);
            currentCubeVertices.push_back(cubeColor[2]);
        }

        glBindBuffer(GL_ARRAY_BUFFER,cubeVBO);
        glBufferSubData(GL_ARRAY_BUFFER,0,currentCubeVertices.size()*sizeof(float),currentCubeVertices.data());
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&gridVAO);
    glDeleteBuffers(1,&gridVBO);
    glDeleteVertexArrays(1,&cubeVAO);
    glDeleteBuffers(1,&cubeVBO);
    glDeleteBuffers(1,&cubeEBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();

    return 0;
}