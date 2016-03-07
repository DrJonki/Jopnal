// Jopnal Engine C++ Library
// Copyright (c) 2016 Team Jopnal
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgement in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

//////////////////////////////////////////////

// Headers
#include <Jopnal/Precompiled.hpp>

//////////////////////////////////////////////


namespace jop
{
    ShaderManager::ShaderManager()
        : Subsystem("Shader Manager"),
          m_shaders(),
          m_uber()
    {
        JOP_ASSERT(m_instance == nullptr, "There must only be one ShaderManager instance!");
        m_instance = this;

        std::vector<unsigned char> buf;

        JOP_ASSERT_EVAL(FileLoader::readFromDll(IDR_UBERVERT, buf), "Failed to read default vertex uber shader source!");
        m_uber[0].assign(reinterpret_cast<const char*>(buf.data()), buf.size());

        JOP_ASSERT_EVAL(FileLoader::readFromDll(IDR_UBERFRAG, buf), "Failed to read default fragment uber shader source!");
        m_uber[2].assign(reinterpret_cast<const char*>(buf.data()), buf.size());
    }

    //////////////////////////////////////////////

    Shader& ShaderManager::getShader(const Material::AttribType attributes)
    {
        JOP_ASSERT(m_instance != nullptr, "Couldn't load shader, no ShaderManager instance!");

        const auto& uber = m_instance->m_uber;
        const std::string shaderName = "jop_shader_" + std::to_string(attributes);

        if (ResourceManager::resourceExists(shaderName))
            return ResourceManager::getExistingResource<Shader>(shaderName);

        std::string pp;
        getPreprocessDef(attributes, pp);

        auto& s = ResourceManager::getNamedResource<Shader>(shaderName, uber[0], "", uber[2], pp);
        //s.setManaged(true);

        return s;
    }

    //////////////////////////////////////////////

    void ShaderManager::getPreprocessDef(const Material::AttribType attrib, std::string& str)
    {
        using m = Material::Attribute;

        str += "#version 330 core\n";

        // Ambient constant
        if ((attrib & m::AmbientConstant) != 0)
        {
            static const auto colors = Color(SettingManager::getString("uAmbientConstant", "111111FF")).asRGBFloatVector();
            static const std::string ambConst = "#define JMAT_AMBIENT vec3("
                                              + std::to_string(colors.r) + ","
                                              + std::to_string(colors.g) + ","
                                              + std::to_string(colors.b) + ")\n";
            str += ambConst;
        }

        // Material
        if ((attrib & m::Material) != 0)
            str += "#define JMAT_MATERIAL\n";
        
        // Diffuse map
        if ((attrib & m::Diffusemap) != 0)
            str += "#define JMAT_DIFFUSEMAP\n";

        // Specular map
        if ((attrib & m::Specularmap) != 0)
            str += "#define JMAT_SPECULARMAP\n";

        // Emission map
        if ((attrib & m::Emissionmap) != 0)
            str += "#define JMAT_EMISSIONMAP\n";

        // Lighting
        {
            static const std::string maxLights =
                "#define JMAT_MAX_POINT_LIGHTS " + std::to_string(LightSource::getMaximumLights(LightSource::Type::Point)) +
                "\n#define JMAT_MAX_DIRECTIONAL_LIGHTS " + std::to_string(LightSource::getMaximumLights(LightSource::Type::Directional)) +
                "\n#define JMAT_MAX_SPOT_LIGHTS " + std::to_string(LightSource::getMaximumLights(LightSource::Type::Spot)) + "\n";

            // Phong model
            if ((attrib & m::Phong) != 0)
                str += "#define JMAT_PHONG\n" + maxLights;
        }
    }

    ShaderManager::~ShaderManager()
    {
        m_instance = nullptr;
    }

    //////////////////////////////////////////////

    ShaderManager* ShaderManager::m_instance = nullptr;
}