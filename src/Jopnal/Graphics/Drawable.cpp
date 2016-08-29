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
#include JOP_PRECOMPILED_HEADER_FILE

#ifndef JOP_PRECOMPILED_HEADER

    #include <Jopnal/Graphics/Drawable.hpp>

    #include <Jopnal/Core/SettingManager.hpp>
    #include <Jopnal/Core/Serializer.hpp>
    #include <Jopnal/Graphics/LightSource.hpp>
    #include <Jopnal/Graphics/Material.hpp>
    #include <Jopnal/Graphics/Mesh/Mesh.hpp>
    #include <Jopnal/Graphics/Model.hpp>
    #include <Jopnal/Graphics/Renderer.hpp>
    #include <Jopnal/Graphics/ShaderProgram.hpp>
    #include <Jopnal/Graphics/ShaderAssembler.hpp>
    #include <Jopnal/Graphics/OpenGL/OpenGL.hpp>
    #include <Jopnal/Graphics/OpenGL/GlCheck.hpp>
    #include <Jopnal/Utility/CommandHandler.hpp>
    #include <glm/gtc/type_ptr.hpp>

#endif

//////////////////////////////////////////////


namespace jop
{
    JOP_REGISTER_COMMAND_HANDLER(Drawable)

        JOP_BIND_MEMBER_COMMAND(&Drawable::setModel, "setModel");
        JOP_BIND_MEMBER_COMMAND(&Drawable::setRenderGroup, "setRenderGroup");

    JOP_END_COMMAND_HANDLER(Drawable)
}

namespace jop
{
    Drawable::ProjectionInfo::ProjectionInfo(const glm::mat4 & view, const glm::mat4& proj, const glm::vec3 & camPos)
        : viewMatrix        (view),
          projectionMatrix  (proj),
          cameraPosition    (camPos)
    {}

    //////////////////////////////////////////////

    Drawable::Drawable(Object& object, Renderer& renderer, const RenderPass::Pass pass)
        : Component         (object, 0),
          m_color           (),
          m_model           (Mesh::getDefault(), Material::getDefault()),
          m_shader          (),
          m_attributes      (0),
          m_rendererRef     (renderer),
          m_pass            (pass),
          m_flags           (ReceiveLights | ReceiveShadows | CastShadows | Reflected),
          m_renderGroup     (0),
          m_updateShader    (true)
    {
        renderer.bind(this, pass);
    }

    Drawable::Drawable(Object& object, RenderPass& pass)
        : Component         (object, 0),
          m_color           (),
          m_model           (Mesh::getDefault(), Material::getDefault()),
          m_shader          (),
          m_attributes      (0),
          m_rendererRef     (pass.getRenderer()),
          m_pass            (pass.getPass()),
          m_flags           (ReceiveLights | ReceiveShadows | CastShadows | Reflected),
          m_renderGroup     (0),
          m_updateShader    (true)
    {
        pass.bind(this);
    }

    Drawable::Drawable(const Drawable& other, Object& newObj)
        : Component         (other, newObj),
          m_color           (other.m_color),
          m_model           (other.m_model),
          m_shader          (other.m_shader),
          m_rendererRef     (other.m_rendererRef),
          m_pass            (other.m_pass),
          m_renderGroup     (other.m_renderGroup),
          m_flags           (other.m_flags),
          m_updateShader    (other.m_updateShader)
    {
        m_rendererRef.bind(this, m_pass);
    }

    Drawable::~Drawable()
    {
        m_rendererRef.unbind(this, m_pass);
    }

    //////////////////////////////////////////////

    void Drawable::draw(const ProjectionInfo& proj, const LightContainer& lights) const
    {
        if (!m_model.isValid())
            return;

        auto& mesh = *getModel().getMesh();
        auto& mat = *getModel().getMaterial();

        // Uniforms
        {
            auto& shdr = getShader();
            auto& modelMat = getObject()->getTransform().getMatrix();

            shdr.setUniform("u_PMatrix", proj.projectionMatrix);
            shdr.setUniform("u_VMatrix", proj.viewMatrix);

            const auto MM = Mesh::VertexIndex::ModelMatrix;

            glCheck(glVertexAttrib4fv(MM + 0, glm::value_ptr(modelMat[0])));
            glCheck(glVertexAttrib4fv(MM + 1, glm::value_ptr(modelMat[1])));
            glCheck(glVertexAttrib4fv(MM + 2, glm::value_ptr(modelMat[2])));
            glCheck(glVertexAttrib4fv(MM + 3, glm::value_ptr(modelMat[3])));

            if (mat.hasAttribute(Material::Attribute::__Lighting))
                lights.sendToShader(shdr, *this);

            mat.sendToShader(shdr, &proj.cameraPosition);

            if (!mesh.hasVertexComponent(Mesh::Color))
            {
                // Default vertex color
                // Used if the mesh itself doesn't have colors
                glCheck(glVertexAttrib4fv(Mesh::VertexIndex::Color, &getColor().colors[0]));
            }

        #ifdef JOP_DEBUG_MODE

            {
                static const DynamicSetting<bool> validateSetting("engine@Debug|bValidateShaders", false);

                if (validateSetting.value && !shdr.validate())
                    return;
            }

        #endif
        }

        mesh.draw(mat.getAttributes());
    }

    //////////////////////////////////////////////

    Renderer& Drawable::getRendrer()
    {
        return m_rendererRef;
    }

    //////////////////////////////////////////////

    const Renderer& Drawable::getRenderer() const
    {
        return m_rendererRef;
    }

    //////////////////////////////////////////////

    Drawable& Drawable::setRenderGroup(const uint8 group)
    {
        m_renderGroup = std::min(static_cast<uint8>(31), group);
        return *this;
    }

    //////////////////////////////////////////////

    uint8 Drawable::getRenderGroup() const
    {
        return m_renderGroup;
    }

    //////////////////////////////////////////////

    Drawable& Drawable::setModel(const Model& model)
    {
        m_model = model;
        m_updateShader = true;

        return *this;
    }

    //////////////////////////////////////////////

    const Model& Drawable::getModel() const
    {
        return m_model;
    }

    //////////////////////////////////////////////

    Model& Drawable::getModel()
    {
        m_updateShader = true;

        return m_model;
    }

    //////////////////////////////////////////////

    Drawable& Drawable::setColor(const Color& color)
    {
        m_color = color;
        return *this;
    }

    //////////////////////////////////////////////

    const Color& Drawable::getColor() const
    {
        return m_color;
    }

    //////////////////////////////////////////////

    const std::pair<glm::vec3, glm::vec3>& Drawable::getLocalBounds() const
    {
        if (getModel().getMesh())
            return getModel().getMesh()->getBounds();

        static const auto dummy = std::make_pair(glm::vec3(), glm::vec3());

        return dummy;
    }

    //////////////////////////////////////////////

    std::pair<glm::vec3, glm::vec3> Drawable::getGlobalBounds() const
    {
        if (getModel().getMesh())
        {
            auto bounds = getLocalBounds();
            getObject()->getTransform().transformBounds(bounds.first, bounds.second);

            return bounds;
        }

        return std::make_pair(glm::vec3(), glm::vec3());
    }

    //////////////////////////////////////////////

    Drawable& Drawable::setFlags(const uint32 flags)
    {
        m_flags = flags;
        return *this;
    }

    //////////////////////////////////////////////

    bool Drawable::hasFlag(const uint32 flag) const
    {
        return (m_flags & flag) != 0;
    }

    //////////////////////////////////////////////

    Drawable& Drawable::setAttributes(const uint64 attributes)
    {
        m_attributes = attributes;
        m_updateShader = true;

        return *this;
    }

    //////////////////////////////////////////////

    Drawable& Drawable::addAttributes(const uint64 attributes)
    {
        m_attributes |= attributes;
        m_updateShader = true;

        return *this;
    }

    //////////////////////////////////////////////

    uint64 Drawable::getAttributes() const
    {
        return m_attributes;
    }

    //////////////////////////////////////////////

    bool Drawable::hasAttribute(const uint64 attribute) const
    {
        return (m_attributes & attribute) != 0;
    }

    //////////////////////////////////////////////

    void Drawable::getShaderPreprocessorDef(const uint64 attribs, std::string& str)
    {
        if (attribs & Attribute::__SkyBox)
            str += "#define JDRW_SKYBOX\n";

        if (attribs & Attribute::__SkySphere)
            str += "#define JDRW_SKYSPHERE\n";
    }

    //////////////////////////////////////////////

    Message::Result Drawable::receiveMessage(const Message& message)
    {
        if (JOP_EXECUTE_COMMAND(Drawable, message.getString(), this) == Message::Result::Escape)
            return Message::Result::Escape;

        return Component::receiveMessage(message);
    }

    //////////////////////////////////////////////

    ShaderProgram& Drawable::getShader() const
    {
        if (m_updateShader)
        {
            if (!getModel().isValid())
                m_shader = static_ref_cast<ShaderProgram>(ShaderProgram::getDefault().getReference());
            else
                m_shader = static_ref_cast<ShaderProgram>(ShaderAssembler::getShader(getModel().getMaterial()->getAttributes(), getAttributes()).getReference());

            m_updateShader = false;
        }

        return *m_shader;
    }
}