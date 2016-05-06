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
    ShaderAssembler::ShaderAssembler()
        : Subsystem ("shaderassembler"),
          m_shaders (),
          m_uber    (),
          m_mutex   ()
    {
        JOP_ASSERT(m_instance == nullptr, "There must only be one ShaderAssembler instance!");
        m_instance = this;

        std::vector<unsigned char> buf;

        JOP_ASSERT_EVAL(FileLoader::readResource(JOP_RES_UBER_SHADER_VERT, buf), "Failed to read default vertex uber shader source!");
        m_uber[0].assign(reinterpret_cast<const char*>(buf.data()), buf.size());

        JOP_ASSERT_EVAL(FileLoader::readResource(JOP_RES_DEPTH_RECORD_SHADER_POINT_GEOM, buf), "Failed to read default geometry uber shader source!");
        m_uber[1].assign(reinterpret_cast<const char*>(buf.data()), buf.size());

        JOP_ASSERT_EVAL(FileLoader::readResource(JOP_RES_UBER_SHADER_FRAG, buf), "Failed to read default fragment uber shader source!");
        m_uber[2].assign(reinterpret_cast<const char*>(buf.data()), buf.size());
    }

    ShaderAssembler::~ShaderAssembler()
    {
        m_instance = nullptr;
    }

    //////////////////////////////////////////////

    Shader& ShaderAssembler::getShader(const Material::AttribType attributes)
    {
        JOP_ASSERT(m_instance != nullptr, "Couldn't load shader, no ShaderAssembler instance!");

        std::lock_guard<std::recursive_mutex> lock(m_instance->m_mutex);

        auto& cont = m_instance->m_shaders;

        auto itr = cont.find(attributes);
        if (itr != cont.end() && !itr->second.expired())
            return *itr->second;

        const auto& uber = m_instance->m_uber;
        const std::string shaderName = "jop_shader_" + std::to_string(attributes);

        if (ResourceManager::resourceExists<Shader>(shaderName))
            return ResourceManager::getExistingResource<Shader>(shaderName);

        std::string pp;
        getPreprocessDef(attributes, pp);

        auto& s = ResourceManager::getNamedResource<Shader>(shaderName, uber[0], (attributes & Material::Attribute::__RecordEnv) ? uber[1] : "", uber[2], pp);

        if (&s != &Shader::getError())
        {
            s.setShouldSerialize(false);

            cont[attributes] = static_ref_cast<Shader>(s.getReference());

            // Needed so that different samplers don't all point to zero
            if ((attributes & Material::Attribute::__Lighting) != 0)
            {
                static const int maxUnits = Texture::getMaxTextureUnits();

                for (std::size_t i = 0; i < LightSource::getMaximumLights(LightSource::Type::Point); ++i)
                    s.setUniform("u_PointLightShadowMaps[" + std::to_string(i) + "]", maxUnits - 1);
            }
        }
        else
            JOP_DEBUG_ERROR("Failed to load shader with attributes: " << attributes);

        return s;
    }

    //////////////////////////////////////////////

    void ShaderAssembler::getPreprocessDef(const Material::AttribType attrib, std::string& str)
    {
        using m = Material::Attribute;

        std::lock_guard<std::recursive_mutex> lock(m_instance->m_mutex);

        str += "#version 330 core\n";

        // Material
        if ((attrib & m::__Lighting) != 0)
            str += "#define JMAT_MATERIAL\n";
        
        // Diffuse map
        if ((attrib & m::DiffuseMap) != 0)
        {
            str += "#define JMAT_DIFFUSEMAP\n";

            if ((attrib & m::DiffuseAlpha) != 0)
                str += "#define JMAT_DIFFUSEALPHA\n";
        }

        // Specular map
        if ((attrib & m::SpecularMap) != 0)
            str += "#define JMAT_SPECULARMAP\n";

        // Emission map
        if ((attrib & m::EmissionMap) != 0)
            str += "#define JMAT_EMISSIONMAP\n";

        // Environment map
        if ((attrib & m::EnvironmentMap) != 0)
            str += "#define JMAT_ENVIRONMENTMAP\n";

        // Reflection map
        if ((attrib & m::ReflectionMap) != 0)
            str += "#define JMAT_REFLECTIONMAP\n";

        // Reflection map
        if ((attrib & m::OpacityMap) != 0)
            str += "#define JMAT_OPACITYMAP\n";

        // Gloss map
        if ((attrib & m::GlossMap) != 0)
            str += "#define JMAT_GLOSSMAP\n";

        // Lighting
        {
            static const std::string maxLights =
                "#define JMAT_MAX_POINT_LIGHTS "            + std::to_string(LightSource::getMaximumLights(LightSource::Type::Point)) +
                "\n#define JMAT_MAX_DIRECTIONAL_LIGHTS "    + std::to_string(LightSource::getMaximumLights(LightSource::Type::Directional)) +
                "\n#define JMAT_MAX_SPOT_LIGHTS "           + std::to_string(LightSource::getMaximumLights(LightSource::Type::Spot)) + "\n";

            // Phong model
            if ((attrib & m::Phong) != 0)
                str += "#define JMAT_PHONG\n" + maxLights;
        }

        // Environment map record
        if ((attrib & m::__RecordEnv) != 0)
            str += "#define JMAT_ENVIRONMENT_RECORD\n";

        // Skybox/sphere
        if ((attrib & m::__SkyBox) != 0)
            str += "#define JMAT_SKYBOX\n";
        else if ((attrib & m::__SkySphere) != 0)
            str += "#define JMAT_SKYSPHERE\n";
    }

    //////////////////////////////////////////////

    const ShaderAssembler::ShaderMap& ShaderAssembler::getShaderMap()
    {
        return m_instance->m_shaders;
    }

    //////////////////////////////////////////////

    ShaderAssembler* ShaderAssembler::m_instance = nullptr;
}