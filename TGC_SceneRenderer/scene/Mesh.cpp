#include "Mesh.h"
#include "..\core\Data.h"
using namespace scene;

Mesh::Mesh(void)
{
    _texCollection = collections::TexturesCollection::Instance();
    this->base = new bases::BaseObject("Mesh");
}

Mesh::~Mesh(void)
{
    clear();
}

bool Mesh::loadMesh(const std::string &sFileName)
{
    // Release the previously loaded mesh (if it exists)
    clear();
    bool bRtrn = false;
    Assimp::Importer Importer;
    // read filename with assimp importer
    const aiScene *pScene = Importer.ReadFile(sFileName.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);

    if (pScene) {
        std::cout << "Mesh(" << this << ") " << "Loading asset " << sFileName << std::endl;
        bRtrn = initFromScene(pScene, sFileName);
    } else {
        std::cout << "Mesh(" << this << ") " << "Error parsing '" << sFileName << "': '" << Importer.GetErrorString() << std::endl;
    }

    if (bRtrn) { std::cout << "Mesh(" << this << ") " << "Asset " << sFileName << " loaded successfully" << std::endl; }
    else { std::cout << "Mesh(" << this << ") " << "An error occured loading asset " << sFileName << std::endl; }

    return bRtrn;
}

bool Mesh::initFromScene(const aiScene *pScene, const std::string &sFilename)
{
    _meshEntries.resize(pScene->mNumMeshes);

    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0 ; i < _meshEntries.size() ; i++) {
        const aiMesh *paiMesh = pScene->mMeshes[i];
        initMesh(i, paiMesh);
    }

    // Load associated materials
    return initMaterials(pScene, sFilename);
}

void Mesh::initMesh(unsigned int index, const aiMesh *paiMesh)
{
    std::vector<types::Vertex> vertices;
    std::vector<unsigned int> indices;
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {
        const aiVector3D *pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D *pNormal   = &(paiMesh->mNormals[i]);
        const aiVector3D *pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
        const aiVector3D *pTangent =  paiMesh->HasTangentsAndBitangents() ? &paiMesh->mTangents[i] : &Zero3D;
        const aiVector3D *pBitangent =  paiMesh->HasTangentsAndBitangents() ? &paiMesh->mBitangents[i] : &Zero3D;
        types::Vertex v(
            glm::vec3(pPos->x, pPos->y, pPos->z),
            glm::vec2(pTexCoord->x, pTexCoord->y),
            glm::vec3(pNormal->x, pNormal->y, pNormal->z),
            glm::vec3(pTangent->x, pTangent->y, pTangent->z),
            glm::vec3(pBitangent->x, pBitangent->y, pBitangent->z)
        );
        vertices.push_back(v);
    }

    for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) {
        const aiFace &face = paiMesh->mFaces[i];
        assert(face.mNumIndices == 3); // Verify triangulation preprocess
        indices.push_back(face.mIndices[0]);
        indices.push_back(face.mIndices[1]);
        indices.push_back(face.mIndices[2]);
    }

    _meshEntries[index] = new MeshEntry(vertices, indices);
    _meshEntries[index]->materialIndex = paiMesh->mMaterialIndex;
    vertices.clear();
    indices.clear();
}

bool Mesh::initMaterials(const aiScene *pScene, const std::string &sFilename)
{
    // Extract the directory part from the file name
    std::string::size_type slashIndex = sFilename.find_last_of("/");
    std::string dir;
    std::string slashDir = "/";
    // Try with \ slashes if / didn't work out

    if (slashIndex == std::string::npos) {
        std::string::size_type tempSlashIndex = sFilename.find_last_of("\\");

        if (tempSlashIndex != std::string::npos) {
            slashIndex = tempSlashIndex;
            slashDir = "\\";
        }
    }

    if (slashIndex == 0) {
        dir = slashDir;
    } else {
        dir = sFilename.substr(0, slashIndex);
    }

    bool bRtrn = true;

    // Initialize the materials
    for (unsigned int i = 0 ; i < pScene->mNumMaterials ; i++) {
        const aiMaterial *pMaterial = pScene->mMaterials[i];
        types::Material *currentMat = new types::Material();
        // Get Texture Count
        int diffuseTextureCount      = pMaterial->GetTextureCount(aiTextureType_DIFFUSE);
        int specularTextureCount     = pMaterial->GetTextureCount(aiTextureType_SPECULAR);
        int ambientextureCount       = pMaterial->GetTextureCount(aiTextureType_AMBIENT);
        int emissiveTextureCount     = pMaterial->GetTextureCount(aiTextureType_EMISSIVE);
        int heightTextureCount       = pMaterial->GetTextureCount(aiTextureType_HEIGHT);
        int normalTextureCount       = pMaterial->GetTextureCount(aiTextureType_NORMALS);
        int shininessTextureCount    = pMaterial->GetTextureCount(aiTextureType_SHININESS);
        int ocapacityTextureCount    = pMaterial->GetTextureCount(aiTextureType_OPACITY);
        int displacementTextureCount = pMaterial->GetTextureCount(aiTextureType_DISPLACEMENT);
        int lightmapTextureCount     = pMaterial->GetTextureCount(aiTextureType_LIGHTMAP);
        int reflectionTextureCount   = pMaterial->GetTextureCount(aiTextureType_REFLECTION);

        for (int tIndex = 0; tIndex < diffuseTextureCount; tIndex++) {
            aiString textureFilename;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, tIndex, &textureFilename) == AI_SUCCESS) {
                std::string fullPath = dir + slashDir + textureFilename.data;

                if (!_texCollection->addTexture(fullPath, _texCollection->textureCount(), types::Texture::Diffuse)) {
                    bRtrn = false;
                    currentMat->addTexture(_texCollection->getDefaultTexture());
                } else {
                    currentMat->addTexture(_texCollection->getTexture(_texCollection->textureCount() - 1));
                }
            }
        }

        for (int tIndex = 0; tIndex < heightTextureCount; tIndex++) {
            aiString textureFilename;

            if (pMaterial->GetTexture(aiTextureType_HEIGHT, tIndex, &textureFilename) == AI_SUCCESS) {
                std::string fullPath = dir + slashDir + textureFilename.data;

                if (!_texCollection->addTexture(fullPath, _texCollection->textureCount(), types::Texture::Height)) {
                    bRtrn = false;
                    currentMat->addTexture(_texCollection->getDefaultTexture());
                } else {
                    currentMat->addTexture(_texCollection->getTexture(_texCollection->textureCount() - 1));
                }
            }
        }

        for (int tIndex = 0; tIndex < normalTextureCount; tIndex++) {
            aiString textureFilename;

            if (pMaterial->GetTexture(aiTextureType_NORMALS, tIndex, &textureFilename) == AI_SUCCESS) {
                std::string fullPath = dir + slashDir + textureFilename.data;

                if (!_texCollection->addTexture(fullPath, _texCollection->textureCount(), types::Texture::Normals)) {
                    bRtrn = false;
                    currentMat->addTexture(_texCollection->getDefaultTexture());
                } else {
                    currentMat->addTexture(_texCollection->getTexture(_texCollection->textureCount() - 1));
                }
            }
        }

        if (currentMat->textureCount() == 0) { currentMat->addTexture(_texCollection->getDefaultTexture()); }

        // Material Properties
        aiColor4D ambient(0.0f, 0.0f, 0.0f, 0.0f);
        aiColor4D diffuse(0.0f, 0.0f, 0.0f, 0.0f);
        aiColor4D specular(0.0f, 0.0f, 0.0f, 0.0f);
        int shadingModel = 0;
        float shininess = 0;
        aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_AMBIENT, &ambient);
        aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
        aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_SPECULAR, &specular);
        aiGetMaterialFloat(pMaterial, AI_MATKEY_SHININESS, &shininess);
        aiGetMaterialInteger(pMaterial, AI_MATKEY_SHADING_MODEL, &shadingModel);
        // Save properties to current material
        currentMat->ambient = glm::vec3(ambient.r, ambient.g, ambient.b);
        currentMat->diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
        currentMat->specular = glm::vec3(specular.r, specular.g, specular.b);
        // Guess the shader type based on textures supplied
        currentMat->guessMaterialShader();
        // Save current mat to mesh materials
        _materials.push_back(currentMat);
    }

    return bRtrn;
}

void Mesh::clear()
{
    for each(types::Material * var in _materials) {
        delete var;
    }

    _materials.clear();

    for each(MeshEntry * var in _meshEntries) {
        delete var;
    }
}

void Mesh::render()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    for (unsigned int i = 0 ; i < _meshEntries.size() ; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, _meshEntries[i]->VB);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(types::Vertex), 0);						// Vertex Position
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(types::Vertex), (const GLvoid *)12);		// Vertex UVS
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(types::Vertex), (const GLvoid *)20);		// Vertex Normals
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(types::Vertex), (const GLvoid *)32);		// Vertex Tangents
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(types::Vertex), (const GLvoid *)44);		// Vertex BiTangets
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshEntries[i]->IB);
        const unsigned int materialIndex = _meshEntries[i]->materialIndex;
        // Binds the mesh material textures for shader use and set shaders material
        // uniforms, the material shadeprogram has to be set for the uniforms
        this->_materials[materialIndex]->useMaterialShader();
        this->_materials[materialIndex]->bindTextures();
        this->_materials[materialIndex]->setUniforms();
        // Draw mesh
        glDrawElements(GL_TRIANGLES, _meshEntries[i]->indicesCount, GL_UNSIGNED_INT, 0);
    }

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
}

void Mesh::MeshEntry::generateBuffers()
{
    glGenBuffers(1, &VB);
    glGenBuffers(1, &IB);
}

Mesh::MeshEntry::MeshEntry()
{
    this->VB            = core::EngineData::Commoms::INVALID_VALUE;
    this->IB            = core::EngineData::Commoms::INVALID_VALUE;
    this->materialIndex = core::EngineData::Commoms::INVALID_MATERIAL;
    this->verticesCount = this->indicesCount = 0;
}

scene::Mesh::MeshEntry::MeshEntry(const std::vector<types::Vertex> &vertices, const std::vector<unsigned int> &indices)
{
    this->VB            = core::EngineData::Commoms::INVALID_VALUE;
    this->IB            = core::EngineData::Commoms::INVALID_VALUE;
    this->materialIndex = core::EngineData::Commoms::INVALID_MATERIAL;
    this->verticesCount = this->indicesCount = 0;
    this->generateBuffers();
    this->setBuffersData(vertices, indices);
}

void scene::Mesh::MeshEntry::setBuffersData(const std::vector<types::Vertex> &vertices, const std::vector<unsigned int> &indices)
{
    this->verticesCount = vertices.size();
    this->indicesCount  = indices.size();
    this->facesCount    = indices.size() / 3; // Valid since mesh is triangulated before processing it
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(types::Vertex) * verticesCount, &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicesCount, &indices[0], GL_STATIC_DRAW);
}

Mesh::MeshEntry::~MeshEntry()
{
    if (VB != core::EngineData::Commoms::INVALID_VALUE) {
        glDeleteBuffers(1, &VB);
    }

    if (IB != core::EngineData::Commoms::INVALID_VALUE) {
        glDeleteBuffers(1, &IB);
    }
}
