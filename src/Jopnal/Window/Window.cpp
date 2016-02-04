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

#if defined(JOP_OS_DESKTOP)
    #include <Jopnal/Window/Desktop/WindowImpl.hpp>
#endif

//////////////////////////////////////////////


namespace
{
    bool ns_eventsPolled = false;
}

namespace jop
{
    Window::Settings::Settings(const bool loadSettings)
        : size          (1u, 1u),
          title         ("Window Title"),
          displayMode   (DisplayMode::Windowed),
          visible       (false)
    {
        if (loadSettings)
        {
            size.x = SettingManager::getUint("uDefaultWindowSizeX", 1024); size.y = SettingManager::getUint("uDefaultWindowSizeY", 600);
            title = SettingManager::getString("sDefaultWindowTitle", getProjectName());
            displayMode = static_cast<Window::DisplayMode>(std::min(2u, SettingManager::getUint("uDefaultWindowMode", 0)));
            visible = true;
        }
    }

    //////////////////////////////////////////////

    Window::Window()
        : Subsystem         ("Window"),
          m_impl            (),
          m_eventHandler    (),
          m_colorChanged    (true)
    {}

    Window::Window(const Settings& settings)
        : Subsystem         ("Window"),
          m_impl            (),
          m_eventHandler    (),
          m_colorChanged    (true)
    {
        open(settings);
    }

    Window::~Window()
    {}

    //////////////////////////////////////////////

    void Window::preUpdate(const double)
    {
        ns_eventsPolled = false;
    }

    //////////////////////////////////////////////

    void Window::postUpdate(const double)
    {
        if (isOpen())
        {
            if (m_colorChanged)
            {
                auto c = m_clearColor.asFloatVector();

                glCheck(gl::ClearColor(c.r, c.g, c.b, c.a));
                glCheck(gl::ClearDepth(1.0));
                glCheck(gl::ClearStencil(0));

                m_colorChanged = false;
            }

            glCheck(gl::Clear(gl::COLOR_BUFFER_BIT | gl::DEPTH_BUFFER_BIT | gl::STENCIL_BUFFER_BIT));
        }
    }

    //////////////////////////////////////////////

    void Window::draw()
    {
        if (isOpen())
            m_impl->swapBuffers();

        // Only poll the events if they haven't yet been during this frame.
        // We care about this because we don't want to invoke controller
        // callbacks multiple times.
        if (!ns_eventsPolled)
        {
            static const bool controllers = SettingManager::getBool("bControllerInput", true);

            pollEvents();

            if (controllers && m_eventHandler.operator bool())
                m_eventHandler->handleControllerInput();

            ns_eventsPolled = true;
        }
    }

    //////////////////////////////////////////////

    void Window::open(const Settings& settings)
    {
        m_impl = std::make_unique<detail::WindowImpl>(settings);
    }

    //////////////////////////////////////////////

    void Window::close()
    {
        m_impl.reset();
    }

    //////////////////////////////////////////////

    bool Window::isOpen() const
    {
        return m_impl.operator bool();
    }

    //////////////////////////////////////////////

    void Window::setClearColor(const Color& color)
    {
        m_clearColor = color;
        m_colorChanged = true;
    }

    //////////////////////////////////////////////

    WindowEventHandler* Window::getEventHandler()
    {
        return m_eventHandler.get();
    }

    //////////////////////////////////////////////

    void Window::removeEventHandler()
    {
        m_eventHandler.reset();
    }

    //////////////////////////////////////////////

    GLFWwindow* Window::getLibraryHandle()
    {
    #ifdef JOP_OS_DESKTOP
        if (isOpen())
            return m_impl->getLibraryHandle();
    #endif

        return nullptr;
    }

    //////////////////////////////////////////////

    void Window::pollEvents()
    {
        detail::WindowImpl::pollEvents();
    }
}