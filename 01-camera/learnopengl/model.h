#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <sstream>
#include <fstream>
#include <ostream>
#include <iostream>
#include <vector>
#include <string>
#include <map>

#include "shader.h"
#include "mesh.h"

using namespace std;

unsigned int TextureFromFile(const char *path,const string &directory,bool gama=false);

class Model
{
public:
    vector<Texture> texture_loaded;
    vector<Mesh> meshes;
    string directory;
    bool gammaCorrection;

    Model(string const &path,bool gamma=false):gammaCorrection(gamma){
        this->loadModel(path);
    }
private:
    void loadModel(string const &path){
        Assimp::Importer importer;
        const aiScene *scene=importer.ReadFile(path,aiProcess_Triangulate|aiProcess_FlipUVs|aiProcess_CalcTangentSpace);
        if(!scene||scene->mFlags &AI_SCENE_FLAGS_INCOMPLETE||!scene->mRootNode){
            count<<"ERROR::ASSIMP:: "<<importer.GetErrorString()<<endl;
            return;
        }
        directory=path.substr(0,path.find_last_of('/'));
        this->processNode(scene->mRootNode,scene);

    }

    void processNode(aiNode *node,const aiScene *scene){
        for(unsigned int i=0;i<node->mNumMeshes;i++){
            aiMesh*mesh=scene->mMeshes[node->mMeshes[i]];
            this->meshes.push_back(this->processMesh(mesh,scene));
        }

        for(unsigned int i=0;i<node->mNumChildren;i++){
            this->processNode(node->mChildren[i],scene);
        }
    }

    Mesh processMesh(aiMesh *mesh,const aiScene *scene){
        vector<vector> vertices;
        vector<unsigned int > indices;
        vector<Texture> textures;
        for(unsigned int i=0;i<mesh->mNumVertices;i++){
            Vertex vertex;
            glm::vec3 tmp;
            tmp.x=mesh->mVertices[i].x;
            tmp.y=mesh->mVertices[i].y;
            tmp.z=mesh->mVertices[i].z;
            vertex.Position=tmp;

            tmp.x=mesh->mNormals[i].x;
            tmp.y=mesh->mNormals[i].y;
            tmp.z=mesh->mNormals[i].z;
            vertex.Normal=tmp;

            if(mesh->mTextureCoords[0]){
                glm::vec2 tmp;
                tmp.x=mesh->mTextureCoords[0][i].x;
                tmp.y=mesh->mTextureCoords[0][i].y;
                vertex.TexCoords=tmp;
            }else {
                vertex.TexCoords=glm::vec2(0.0f,0.0f);
            }

            tmp.x=mesh->mTangents[i].x;
            tmp.y=mesh->mTangents[i].y;
            tmp.z=mesh->mTangents[i].z;
            vertex.Tangent=tmp;

            tmp.x=mesh->mBitangents[i].x;
            tmp.y=mesh->mBitangents[i].y;
            tmp.z=mesh->mBitangents[i].z;
            vertex.Bitangent=tmp;

            vertices.push_back(vertex);
        }


        for(unsigned int i=0;i<mesh->mNumFaces;i++){
            aiFace face=mesh->mFaces[i];
            for(unsigned int j=0;j<face.mNumIndices;j++){
                indices.push_back(face.mIndices[j]);
            }
        }

        aiMaterial *material=scene->mMaterials[mesh->mMaterialIndex];

        vector<Texture> diffuseMaps=this->loadMaterialTextures(material,aiTextureType_DIFFUSE,"texture_diffuse");
        textures.insert(textures.end(),diffuseMaps.begin(),diffuseMaps.end());

        vector<Texture> specularMaps=this->loadMaterialTextures(material,aiTextureType_SPECULAR,"texture_specular");
        textures.insert((textures.end(),specularMaps.begin(),specularMaps.end());

        vector<Texture> normalMaps=this->loadMaterialTextures(material,aiTextureType_NORMALS,"texture_normal");
        textures.insert(textures.end(),normalMaps.begin(),normalMaps.end());

        vector<Texture> heightMaps=this->loadMaterialTextures(material,aiTextureType_HEIGHT,"texture_height");
        textures.insert(textures.end(),heightMaps.begin(),heightMaps.end());


        return Mesh(vertices,indices,textures);

    }

    vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,string typeName){
        vector<Texture> textures;
        for(unsigned int i=0;i<mat->GetTextureCount(type);i++){
            aiString str;
            mat->GetTexture(type,i,&str);
            bool skip=false;
            for(unsigned int j=0;j<texture_loaded.size();j++){
                if(std::strcmp(texture_loaded[j].path.data(),str.C_Str())==0){
                    textures.push_back((texture_loaded[j]));
                    skip=true;
                    break;
                }
            }
            if(!skip){
                Texture texture;
                texture.id=TextureFromFile(str.C_Str(),this->directory);
                texture.type=typeName;
                texture.path=str.C_Str();
                textures.push_back(texture);
                texture_loaded.push_back(texture);
            }
        }

        return textures;
    }

};


unsigned int TextureFromFile(const char *path, const string &directory, bool gama){
    string filename=string(path);
    filename=directory+'/'+filename;

    unsigned int textureID;
    glGenTextures(1,&textureID);
    int width,height,nrComponents;
    unsigned char *data=stbi_load(filename.c_str(),&width,&height,&nrComponents,0);
    if(data){
        GLenum format;
        if(nrComponents==1)
            format=GL_RED;
        else if(nrComponents==3)
            format=GL_RGB;
        else if(nrComponents==4)
            format=GL_RGBA;
        glBindTexture(GL_TEXTURE_2D,textureID);
        glTexImage2D(GL_TEXTURE_2D,0,format,width,height,0,format,GL_UNSIGNED_BYTE,data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

        stbi_image_free(data);
    }else {
        std::count<<"Texture failed to load at path: "<<path<<std::endl;
        stbi_image_free(data);
    }

    return textureID;


}


#endif // MODEL_H
