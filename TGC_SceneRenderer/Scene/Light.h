#pragma once
#include "../bases/BaseComponent.h"
#include "glm/detail/type_mat.hpp"
#include "glm/detail/type_vec.hpp"
#include "glm/gtc/matrix_transform.hpp"
namespace scene {

    class Light : public bases::BaseComponent {
        public:

            enum LightType {
                Directional,
                Spot,
                Point
            };

            glm::vec3 specular;
            glm::vec3 diffuse;
            glm::vec3 attenuation;
            float intensity;
            // Spot Light Params
            float innerConeAngle;
            float outerConeAngle;
            LightType lightType;

            Light(void);
    };
}

