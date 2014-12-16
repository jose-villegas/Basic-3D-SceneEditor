#pragma once
#include "GL/gl_core_4_4.h"
#include "assimp/material.h"
#include "FreeImage/FreeImage.h"
#include <string>
namespace types {
    class Texture {
        public:

            enum TextureType {
                None			=	aiTextureType_NONE,
                Diffuse			=	aiTextureType_DIFFUSE,
                Specular		=	aiTextureType_SPECULAR,
                Ambient			=	aiTextureType_AMBIENT,
                Emissive		=	aiTextureType_EMISSIVE,
                Height			=	aiTextureType_HEIGHT,
                Normals			=	aiTextureType_NORMALS,
                Shininess		=	aiTextureType_SHININESS,
                Ocapacity		=	aiTextureType_OPACITY,
                Displacement	=	aiTextureType_DISPLACEMENT,
                Lightmap		=	aiTextureType_LIGHTMAP,
                Reflection		=	aiTextureType_REFLECTION
            };

            Texture(const std::string &sFilename, const unsigned int &texID, const TextureType &tType);
            Texture(const unsigned int &texID, const TextureType &tType);
            ~Texture();
            void bind() const;
            bool load();
            bool load(const std::string &sFilename);
            void unload() const;
            unsigned int getWidth() { return this->tWidth; };
            unsigned int getHeight() { return this->tHeight; };
            unsigned int getOGLTexID() { return this->oglTexID; };
            unsigned int geTexID() { return this->texID; };
            std::string getFilename() { return this->sFilename; };
        private:
            unsigned int tWidth;
            unsigned int tHeight;
            unsigned int tBitsPerPixel;
            unsigned int oglTexID;
            unsigned int texID;
            TextureType tType;
            std::string sFilename;
    };
}
