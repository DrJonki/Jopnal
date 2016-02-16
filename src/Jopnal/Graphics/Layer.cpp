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
    JOP_REGISTER_COMMAND_HANDLER(Layer)



    JOP_END_COMMAND_HANDLER(Layer)
}

namespace jop
{
    Layer::Layer(const std::string& ID)
        : Subsystem                             (ID),
          m_drawList                            (),
          m_boundLayers                         (),
          m_camera                              (),
          m_renderTexture                       (),
          m_drawablesRemoved                    (false)
    {}

    Layer::~Layer()
    {}

    //////////////////////////////////////////////

    void Layer::drawBase()
    {
        if (isActive())
        {
            if (m_camera.expired())
                setCamera(Camera::getDefault());

            if (!m_renderTexture.expired())
                m_renderTexture.lock()->bind();
            else
                RenderTexture::unbind();

            draw(*m_camera.lock());

            sweepRemoved();
        }
    }

    //////////////////////////////////////////////

    void Layer::draw(const Camera& camera)
    {
        GlState::setDepthTest(true);
        GlState::setFaceCull(true);

        // TODO Does expired() have to be called, since drawable dtor removes itself from layers?

        for (auto& i : m_boundLayers)
        {
            if (!i.expired())
            {
                for (auto& j : i.lock()->m_drawList)
                {
                    if (!j.expired())
                        j.lock()->draw(camera);
                    else
                        m_drawablesRemoved = true;
                }
            }
        }

        for (auto& i : m_drawList)
        {
            if (!i.expired())
                i.lock()->draw(camera);
            else
                m_drawablesRemoved = true;
        }
    }

    //////////////////////////////////////////////

    MessageResult Layer::sendMessage(const std::string& message)
    {
        Any wrap;
        return sendMessage(message, wrap);
    }

    //////////////////////////////////////////////

    MessageResult Layer::sendMessage(const std::string& message, Any& returnWrap)
    {
        const Message msg(message, returnWrap);
        return sendMessage(msg);
    }

    //////////////////////////////////////////////

    MessageResult Layer::sendMessage(const Message& message)
    {
        if (message.passFilter(Message::Layer, getID()))
        {
            if (message.passFilter(Message::Command))
            {
                Any instance(this);
                JOP_EXECUTE_COMMAND(Layer, message.getString(), instance, message.getReturnWrapper());
            }

            if (message.passFilter(Message::Custom))
                return sendMessageImpl(message);
        }

        return MessageResult::Continue;
    }

    //////////////////////////////////////////////

    void Layer::addDrawable(Drawable& drawable)
    {
        m_drawList.emplace_back(std::static_pointer_cast<Drawable>(drawable.shared_from_this()));
        m_drawList.back().lock()->m_boundToLayers.emplace(std::static_pointer_cast<Layer>(shared_from_this()));
    }

    //////////////////////////////////////////////

    void Layer::removeDrawable(const std::string& id)
    {
        for (auto itr = m_drawList.begin(); itr != m_drawList.end(); ++itr)
        {
            if (!itr->expired() && itr->lock()->getID() == id)
            {
                itr->lock()->m_boundToLayers.erase(std::static_pointer_cast<Layer>(shared_from_this()));
                m_drawList.erase(itr);
                return;
            }
        }
    }

    //////////////////////////////////////////////

    void Layer::bindOtherLayer(Layer& layer)
    {
        m_boundLayers.emplace_back(std::static_pointer_cast<Layer>(layer.shared_from_this()));
    }

    //////////////////////////////////////////////

    void Layer::unbindOtherLayer(const std::string& ID)
    {
        for (auto itr = m_boundLayers.begin(); itr != m_boundLayers.end(); ++itr)
        {
            if (!itr->expired() && itr->lock()->getID() == ID)
            {
                m_boundLayers.erase(itr);
                return;
            }
        }
    }

    //////////////////////////////////////////////

    void Layer::setCamera(std::reference_wrapper<const Camera> camera)
    {
        m_camera = std::static_pointer_cast<const Camera>(camera.get().shared_from_this());
    }

    //////////////////////////////////////////////

    void Layer::setRenderTexture(RenderTexture* renderTexture)
    {
        if (renderTexture)
            m_renderTexture = std::static_pointer_cast<RenderTexture>(renderTexture->shared_from_this());
        else
            m_renderTexture.reset();
    }

    //////////////////////////////////////////////

    void Layer::sweepRemoved()
    {
        if (m_drawablesRemoved)
        {
            m_drawList.erase(std::remove_if(m_drawList.begin(), m_drawList.end(), [](const std::weak_ptr<Drawable>& drawable)
            {
                return drawable.expired();

            }), m_drawList.end());

            m_boundLayers.erase(std::remove_if(m_boundLayers.begin(), m_boundLayers.end(), [](const std::weak_ptr<Layer>& layer)
            {
                return layer.expired();

            }), m_boundLayers.end());

            m_drawablesRemoved = false;
        }
    }
}