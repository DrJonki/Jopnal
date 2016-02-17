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

#ifndef JOP_LAYER_HPP
#define JOP_LAYER_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Core/SubSystem.hpp>
#include <memory>
#include <vector>

//////////////////////////////////////////////


namespace jop
{
    class Drawable;
    class Camera;
    class RenderTexture;

    class JOP_API Layer : public Subsystem
    {
    private:

        JOP_DISALLOW_COPY_MOVE(Layer);

        friend class StateLoader;

    public:
    
        /// \brief Constructor
        ///
        /// \param ID Identifier of this layer
        ///
        Layer(const std::string& ID);

        /// \brief Virtual destructor
        ///
        virtual ~Layer() override;


        /// \brief Base draw function
        ///
        void drawBase();

        /// \brief Draw function
        ///
        /// The default version of this function simply iterates through the
        /// local & bound draw lists and calls draw() on the drawables.
        ///
        virtual void draw(const Camera& camera);


        /// \brief Method to send messages
        ///
        /// Forwards messages to this object's components
        ///
        /// \param message String holding the message
        ///
        MessageResult sendMessage(const std::string& message);

        /// \brief Method to send messages
        ///
        /// Forwards messages to this object's components
        ///
        /// \param message String holding the message
        /// \param returnWrap Pointer to hold extra data
        ///
        MessageResult sendMessage(const std::string& message, Any& returnWrap);

        /// \brief Function to handle messages
        ///
        /// \param message The message
        ///
        MessageResult sendMessage(const Message& message);


        /// \brief Add a new drawable component to the draw list
        ///
        /// \param drawable Reference to the drawable to be added
        ///
        void addDrawable(Drawable& drawable);

        ///
        ///
        void removeDrawable(const std::string& id);

        /// \brief Bind a layer's draw list into this layer
        ///
        /// \param layer Reference to the layer to be bound
        ///
        void bindOtherLayer(Layer& layer);

        /// \brief Unbind a layer's draw list from this layer
        ///
        /// \param ID The layer's id
        ///
        void unbindOtherLayer(const std::string& ID);

        /// \brief Set the camera
        ///
        /// \param camera Reference to the camera to be set
        ///
        void setCamera(const Camera& camera);

        /// \brief Set a RenderTexture
        ///
        /// The render texture will be created and taken ownership of
        ///
        void setRenderTexture(const glm::ivec2& size, const unsigned int depth, const unsigned int stencil);

        const RenderTexture* getRenderTexture() const;

        /// \brief Sweep the drawables & bound layers that no longer exist
        ///
        void sweepRemoved();


    private:

        void handleDrawableAddition(const Drawable& drawable);

        void handleDrawableRemoval(const Drawable& drawable);

    protected:

        std::vector<std::weak_ptr<Drawable>> m_drawList;  ///< The local draw list
        std::vector<std::weak_ptr<Layer>> m_boundLayers;  ///< Bound layers
        std::weak_ptr<const Camera> m_camera;             ///< Bound camera
        std::unique_ptr<RenderTexture> m_renderTexture;   ///< Bound RenderTexture
        bool m_drawablesRemoved;                          ///< Have any drawables been removed?

    };
}

#endif

/// \class Layer
/// \ingroup graphics
///
/// 