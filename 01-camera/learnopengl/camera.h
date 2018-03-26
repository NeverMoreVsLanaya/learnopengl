#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum Camera_Movement{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW =-90.0f;
const float PITCH=0.0f;
const float SPEED=2.5f;
const float SENSITIVITY=0.1f;
const float ZOOM=45.0f;


class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    Camera(glm::vec3 position=glm::vec3(0.0f,0.0f,0.0f),glm::vec3 worldup=(0.0f,1.0f,0.0f),float yaw=YAW,float pitch=PITCH):
        Front(0.0f,0.0f,-1.0f),MovementSpeed(SPEED),MouseSensitivity(SENSITIVITY),Zoom(ZOOM)
    {
        this->Position=position;
        this->WorldUp=worldup;
        this->Yaw=yaw;
        this->Pitch=pitch;
        this->updateCameraVectors();
    }

    Camera(float posX,float posY,float posZ,float worldupX,float worldupY,float worldupZ,float yaw,float pitch):Front(0.0f,0.0f,-1.0f),MovementSpeed(SPEED),MouseSensitivity(SENSITIVITY),Zoom(ZOOM){
        this->Position=glm::vec3(posX,posY,posZ);
        this->WorldUp=glm::vec3(worldupX,worldupY,worldupZ);
        this->Pitch=pitch;
        this->Yaw=yaw;
        this->updateCameraVectors();
    }

    glm::mat4 GetViewMatrix(){
        return glm::lookAt(this->Position,this->Position+this->Front,this->Up);
    }

    void ProcessKeyboard(Camera_Movement direction,float deltaTime){
        float velocity=MovementSpeed*deltaTime;
        if(direction==FORWARD)
            this->Position+=this->Front*velocity;
        if(direction==BACKWARD)
            this->Position-=this->Front*velocity;
        if(direction==LEFT)
            this->Position-=this->Right*velocity;
        if(direction==Right)
            this->Position+=this->Right*velocity;
    }

    void ProcessMouseMoveMent(float xoffset,float yoffset,GLboolean constranPitch=true){
        xoffset*=MouseSensitivity;
        yoffset*=MouseSensitivity;
        this->Yaw+=xoffset;
        this->Pitch+=yoffset;

        if(constranPitch){
            if(this->Pitch>89.0f)
                this->Pitch=89.0f;
            if(this->Pitch<-89.0f)
                this->Pitch=-89.0f;
        }

        this->updateCameraVectors();
    }


    void ProcessMouseScroll(float yoffset){
        if(this->Zoom>=1.0f&&this->Zoom<=45.0f)
            this->Zoom-=yoffset;
        if(this->Zoom<=1.0f)
            this->Zoom=1.0f;
        if(this->Zoom>=45.0f)
            this->Zoom=45.0f;
    }


private:
    void updateCameraVectors(){
        glm::vec3 front;
        front.x=cos(glm::radians(this->Yaw))*cos(glm::radians(this->Pitch));
        front.y=sin(glm::radians(this->Pitch));
        front.z=sin(glm::radians(this->Yaw))*cos(glm::radians(this->Pitch));
        this->Front=glm::normalize(front);
        this->Right=glm::normalize(glm::cross(this->Front,this->WorldUp));
        this->Up=glm::normalize(glm::cross(this->Right,this->Front));
    }

};

#endif // CAMERA_H
