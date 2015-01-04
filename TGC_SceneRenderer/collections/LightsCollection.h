#pragma once
#include "../bases/ShaderLinks.h"
#include "../core/Data.h"
#include "GL/gl_core_4_4.h"
#include "../Scene/Light.h"
#include <vector>

namespace scene {
    class Light; // Forward Declare to solve circular dependency
}

namespace collections {

    class LightsCollection : public bases::ShaderLinkBlock {
        private:
            // Class Instance
            static LightsCollection *instance;
            std::vector<scene::Light *> lights;
            LightsCollection(void);
            LightsCollection(const LightsCollection &lightc);

        public:
            ~LightsCollection();
            static LightsCollection *Instance();
            void setUniformBlock();
            void setUniformBlockInfo();
            void removeLight(const unsigned int &lightIndex);
            void removeLight(scene::Light *lght);
            const unsigned int lightCount() const;
            scene::Light *createLight(const unsigned int lightType);
            scene::Light *getLight(const unsigned int &lightIndex) const;
            std::vector<scene::Light *> getLights() const { return lights; }

    };
}

