#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader
{
public:
    unsigned int ID;
    Shader(const char * vertexPath,const char *fragmentPath,const char *geometryPath=nullptr) {
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        vShaderFile.execptions(std::ifstream::failbit|std::ifstream::badbit);
        fShaderFile.execptions(std::ifstream::failbit|std::ifstream::badbit);
        gShaderFile.execptions(std::ifstream::failbit|std::ifstream::badbit);

        try {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            vShaderStream<<vShaderFile.rdbuf();
            fShaderStream<<fShaderFile.rdbuf();
            vShaderFile.close();
            fShaderFile.close();
            vertexCode=vShaderStream.str();
            fragmentCode=fShaderStream.str();

            if(geometryPath!=nullptr){
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream<<gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode=gShaderStream.str();
            }
        } catch (std::ifstream::failure e) {
            std::cout<<"ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ"<<std::endl;
        }
        const char *vShaderCode=vertexCode.c_str();
        const char *fShaderCode=fragmentCode.c_str();
        unsigned int vertex,fragment;
        int success;
        char infoLog[512];
        vertex=glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex,1,&vShaderCode,nullptr);
        glCompileShader(vertex);
        this->checkCompileErrors(vertex,"VERTEX");

        fragment=glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment,1,&fragmentCode,nullptr);
        glCompileShader(fragment);
        this->checkCompileErrors(fragment,"FRAGMENT");

        unsigned int geometry;
        if(geometryPath!=nullptr){
            const char *gShaderCode=geometryCode.c_str();
            geometry=glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry,1,&gShaderCode,nullptr);
            glCompileShader(geometry);
            this->checkCompileErrors(geometry,"GEOMETRY");
        }

        ID=glCreateProgram();
        glAttachShader(ID,vertex);
        glAttachShader(ID,fragment);
        if(geometryPath!=nullptr){
            glAttachShader(ID,geometry);
        }
        glLinkProgram(ID);
        this->checkCompileErrors(ID,"PROGRAM");
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if(geometryPath!=nullptr){
            glDeleteShader(geometry);
        }

    }
    void use(){
        glUseProgram(ID);
    }
private:
    void checkCompileErrors(GLuint shader,std::string type){
        GLuint success;
        GLchar infoLog[1024];
        if(type!="PROGRAM"){
            glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
            if(!success){
                glGetShaderInfoLog(shader,1024,nullptr,infoLog);
                std::cout<<"ERROR::SHADER_COMPILATION_ERROR of type: "
                <<type<<"\n"<<infoLog<<"\n --------------------------"
                <<std::endl;
            }

        }else {
            glGetProgramiv(shader,GL_LINK_STATUS,&success);
            if(!success){
                glGetProgramInfoLog(shader,1024,nullptr,infoLog);
                std::cout<<"ERROR::PROGRAM_LINKING_ERROR of type: "
                <<type<<"\n"<<infoLog<<"\n --------------------------"
                <<std::endl;
            }
        }
    }
};

#endif // SHADER_H
