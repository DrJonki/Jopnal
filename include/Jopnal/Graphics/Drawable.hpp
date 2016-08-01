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

#ifndef JOP_DRAWABLE_HPP
#define JOP_DRAWABLE_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Core/Component.hpp>
#include <Jopnal/Graphics/Model.hpp>
#include <Jopnal/Graphics/RenderPass.hpp>
#include <Jopnal/Utility/Json.hpp>
#include <memory>

//////////////////////////////////////////////


namespace jop
{
    class ShaderProgram;
    class LightSource;
    class LightContainer;
    class Renderer;
    class Camera;

    class JOP_API Drawable : public Component
    {
    private:

        JOP_GENERIC_COMPONENT_CLONE(Drawable);

    public:
    
        enum Flag : uint32
        {
            ReceiveLights   = 1,
            ReceiveShadows  = 1 << 1,
            CastShadows     = 1 << 2,
            Reflected       = 1 << 3
        };

    public:

        /// \brief Constructor
        ///
        /// \param object Reference to the object this drawable will be bound to
        /// \param renderer Reference to the renderer
        /// \param ID Component identifier
        ///
        Drawable(Object& object, Renderer& renderer, const RenderPass::Pass pass = RenderPass::Pass::BeforePost);

        Drawable(Object& object, RenderPass& pass);

        /// \brief Virtual destructor
        ///
        virtual ~Drawable() override;


        /// \brief Base draw function
        ///
        /// This will use the shader bound to this drawable.
        /// 
        /// \param camera The camera to use
        /// \param lights The light container
        ///
        virtual void draw(const glm::mat4& view, const glm::mat4& proj, const LightContainer& lights) const;

        /// \brief Get the renderer this drawable is bound to
        ///
        /// \return Reference to the renderer
        ///
        Renderer& getRendrer();

        /// \copydoc getRenderer
        ///
        const Renderer& getRenderer() const;


        /// \brief Set the render group
        ///
        /// The value will be clamped between 0 and 31.
        ///
        /// \param group The new group to set
        ///
        /// \comm setRenderGroup
        ///
        Drawable& setRenderGroup(const uint8 group);

        /// \brief Get the render group
        ///
        /// \return The render group 
        ///
        uint8 getRenderGroup() const;


        /// \brief Set the model
        ///
        /// The model will be copied.
        ///
        /// \param model Reference to the model
        ///
        /// \comm setModel
        ///
        /// \return Reference to self
        ///
        Drawable& setModel(const Model& model);

        /// \brief Get the model
        ///
        /// \return Reference to the model
        ///
        Model& getModel();

        /// \copydoc getModel()
        ///
        const Model& getModel() const;

        Drawable& setFlags(const uint32 flags);

        bool hasFlag(const uint32 flag) const;

    protected:

        Message::Result receiveMessage(const Message& message) override;

    private:

        Model m_model;                          ///< The bound model
        WeakReference<ShaderProgram> m_shader;  ///< The bound shader
        Renderer& m_rendererRef;                ///< Reference to the renderer
        const RenderPass::Pass m_pass;
        uint32 m_flags;                         ///< Property flags
        uint8 m_renderGroup;                    ///< The render group
    };
}

#endif

/// \class Drawable
/// \ingroup graphics
///
/// #TODO Detailed description