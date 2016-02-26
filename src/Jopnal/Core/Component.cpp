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
    JOP_REGISTER_COMMAND_HANDLER(Component)

        JOP_BIND_MEMBER_COMMAND(&Component::setID, "setID");

    JOP_END_COMMAND_HANDLER(Component)
}

namespace jop
{
    Component::Component(Object& object, const std::string& ID)
        : SafeReferenceable<Component>   (this),
          m_objectRef                               (object),
          m_ID                                      (ID)
    {}

    Component::Component(const Component& other)
        : SafeReferenceable<Component>(this),
        m_objectRef(other.m_objectRef),
        m_ID(other.m_ID)
    {}

    Component::~Component()
    {}

    //////////////////////////////////////////////

    Message::Result Component::sendMessage(const std::string& message)
    {
        Any wrap;
        return sendMessage(message, wrap);
    }

    //////////////////////////////////////////////

    Message::Result Component::sendMessage(const std::string& message, Any& returnWrap)
    {
        const Message msg(message, returnWrap);
        return sendMessage(msg);
    }

    Message::Result Component::sendMessage(const Message& message)
    {
        if (message.passFilter(Message::Component, getID()))
        {
            if (message.passFilter(Message::Command))
            {
                Any instance(this);
                if (JOP_EXECUTE_COMMAND(Component, message.getString(), instance, message.getReturnWrapper()) == Message::Result::Escape)
                    return Message::Result::Escape;
            }

            if (message.passFilter(Message::Custom))
                return sendMessageImpl(message);
        }

        return Message::Result::Continue;
    }

    //////////////////////////////////////////////

    void Component::update(const float)
    {}

    //////////////////////////////////////////////

    void Component::fixedUpdate(const float)
    {}

    //////////////////////////////////////////////

    const std::string& Component::getID() const
    {
        return m_ID;
    }

    //////////////////////////////////////////////

    void Component::setID(const std::string& ID)
    {
        m_ID = ID;
    }

    //////////////////////////////////////////////

    Object& Component::getObject()
    {
        return *m_objectRef;
    }

    //////////////////////////////////////////////

    const Object& Component::getObject() const
    {
        return *m_objectRef;
    }

    Message::Result Component::sendMessageImpl(const Message&)
    {
        return Message::Result::Continue;
    }
}