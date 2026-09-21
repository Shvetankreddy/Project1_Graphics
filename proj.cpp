#include <iostream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

int GRID_SIZE=5,cubeX=2,cubeY=2,cubeZ=2;

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

float vertices[]={
0,0,1,1,0,0,1,0,1,1,0,0,1,1,1,1,0,0,0,1,1,1,0,0,
0,0,0,1,0,0,1,0,0,1,0,0,1,1,0,1,0,0,0,1,0,1,0,0
};

unsigned int indices[]={0,1,2,2,3,0,4,5,6,6,7,4};

unsigned int shader(){
    unsigned int v=glCreateShader(GL_VERTEX_SHADER),f=glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(v,1,&vertexShaderSource,NULL);
    glShaderSource(f,1,&fragmentShaderSource,NULL);
    glCompileShader(v);glCompileShader(f);
    unsigned int p=glCreateProgram();
    glAttachShader(p,v);glAttachShader(p,f);glLinkProgram(p);
    glDeleteShader(v);glDeleteShader(f);
    return p;
}

bool key(GLFWwindow* w,int k){
    static bool old[512]={};
    bool p=glfwGetKey(w,k)==GLFW_PRESS&&!old[k];
    old[k]=glfwGetKey(w,k)==GLFW_PRESS;
    return p;
}

int main(){
    if(!glfwInit())return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* w=glfwCreateWindow(1000,800,"Project 1",NULL,NULL);
    if(!w)return -1;

    glfwMakeContextCurrent(w);
    glewExperimental=GL_TRUE;
    if(glewInit()!=GLEW_OK)return -1;

    unsigned int program=shader(),VAO,VBO,EBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glEnable(GL_DEPTH_TEST);

    while(!glfwWindowShouldClose(w)){
        if(key(w,GLFW_KEY_ESCAPE))glfwSetWindowShouldClose(w,true);
        if(key(w,GLFW_KEY_LEFT)&&cubeX>0)cubeX--;
        if(key(w,GLFW_KEY_RIGHT)&&cubeX<GRID_SIZE-1)cubeX++;
        if(key(w,GLFW_KEY_DOWN)&&cubeY>0)cubeY--;
        if(key(w,GLFW_KEY_UP)&&cubeY<GRID_SIZE-1)cubeY++;
        if(key(w,GLFW_KEY_B)&&cubeZ>0)cubeZ--;
        if(key(w,GLFW_KEY_U)&&cubeZ<GRID_SIZE-1)cubeZ++;

        int width,height;
        glfwGetFramebufferSize(w,&width,&height);
        glViewport(0,0,width,height);
        glClearColor(0.04f,0.04f,0.07f,1);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        glm::mat4 model=glm::translate(glm::mat4(1.0f),glm::vec3(cubeX-2.5f,cubeY-2.5f,cubeZ-2.5f));
        glm::mat4 view=glm::lookAt(glm::vec3(8,7,8),glm::vec3(0),glm::vec3(0,1,0));
        glm::mat4 projection=glm::perspective(glm::radians(45.0f),(float)width/height,0.1f,100.0f);
        glm::mat4 transform=projection*view*model;

        glUseProgram(program);
        glUniformMatrix4fv(glGetUniformLocation(program,"transform"),1,GL_FALSE,glm::value_ptr(transform));
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);

        glfwSwapBuffers(w);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    glDeleteBuffers(1,&EBO);
    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}
