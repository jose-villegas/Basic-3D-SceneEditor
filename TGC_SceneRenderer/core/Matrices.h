#pragma once
#include "glm/detail/type_mat.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
namespace core {
    // Elemental Matrices
    class Matrices {
        public:
            glm::mat4 model;
            glm::mat4 view;
            glm::mat4 projection;
            glm::mat4 modelViewProjection;
            glm::mat4 modelView;
            glm::mat3 normal;
            Matrices(void);
    };
}
