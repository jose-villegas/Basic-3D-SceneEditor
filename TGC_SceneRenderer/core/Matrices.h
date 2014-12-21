#pragma once
#include "../types/ShaderProgram.h"
#include "../bases/ShaderLinks.h"
#include "../Collections/stored/Shaders.h"
#include "glm/detail/type_mat.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "Data.h"

namespace core {
    // Elemental Matrices
    class Matrices : public bases::ShaderLinkBlock {
        private:
            glm::mat4 model;
            glm::mat4 view;
            glm::mat4 projection;
            glm::mat4 modelViewProjection;
            glm::mat4 modelView;
            glm::mat4 normal;

        public:
            Matrices(void);
            ~Matrices();
            void calculateMatrices();
            void setViewMatrix(const glm::mat4 &value);
            void setModelMatrix(const glm::mat4 &value);
            void setProjectionMatrix(const glm::mat4 &value);
            // only use this if uniformBlockInfo is set
            void setUniformBlock();
            // sets the class holder for uniform info and saves the uniform block info indices and offsets
            void setUniformBlockInfo();
    };
}

