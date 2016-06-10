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

#if defined(JOP_OS_WINDOWS)

#include <Jopnal/Window/Desktop/WindowImpl.hpp>

//////////////////////////////////////////////


namespace
{
    std::unordered_map<GLFWwindow*, jop::Window*> ns_windowRefs;
    GLFWwindow* ns_shared;

    void errorCallback(int code, const char* message)
    {
        const std::string msgStr(message);

        switch (code)
        {
            case GLFW_NOT_INITIALIZED:
                JOP_ASSERT(false, "GLFW not initialized!\n\nDetails: " + msgStr);
                break;
            case GLFW_API_UNAVAILABLE:
                JOP_ASSERT(false, "The requested OpenGL API is not available!\n\nDetails: " + msgStr);
                break;
            case GLFW_VERSION_UNAVAILABLE:
                JOP_ASSERT(false, "The requested OpenGL version is not available on this system!\n\nDetails: " + msgStr);
                break;
            case GLFW_PLATFORM_ERROR:
                JOP_ASSERT(false, "GLFW reported a platform error!\n\nDetails: " + msgStr);
        }

        JOP_DEBUG_ERROR(msgStr);
    }

    void initExtensions()
    {
        static bool init = false;

        if (!init)
        {
            const int result = ogl_LoadFunctions();

            JOP_ASSERT(result != ogl_LOAD_FAILED, "Failed to load OpenGL extensions!");

            const int failed = result - ogl_LOAD_SUCCEEDED;

            if (failed > 0)
                JOP_DEBUG_WARNING("Some requested OpenGL extensions failed to load. Missing extensions: " << failed);

            init = true;
        }
    }

    #ifdef JOP_OPENGL_ES
        #define JOP_DEF_OGL_MINOR 0
    #else
        #define JOP_DEF_OGL_MINOR 3
    #endif

    void initialize()
    {
        if (ns_windowRefs.empty())
        {
            glfwSetErrorCallback(errorCallback);
            glfwInit();

            glfwWindowHint(GLFW_RED_BITS, 0);
            glfwWindowHint(GLFW_GREEN_BITS, 0);
            glfwWindowHint(GLFW_BLUE_BITS, 0);
            glfwWindowHint(GLFW_ALPHA_BITS, 0);

            glfwWindowHint(GLFW_DEPTH_BITS, 0);
            glfwWindowHint(GLFW_STENCIL_BITS, 0);

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, jop::SettingManager::get<unsigned int>("engine@Graphics|OpenGL|uVersionMajor", 3));
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, jop::SettingManager::get<unsigned int>("engine@Graphics|OpenGL|uVersionMinor", JOP_DEF_OGL_MINOR));
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_CLIENT_API, 

            #ifdef JOP_OPENGL_ES
                GLFW_OPENGL_ES_API
            #else
                GLFW_OPENGL_API
            #endif
            );

            glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
            glfwWindowHint(GLFW_DECORATED, GL_FALSE);

            ns_shared = glfwCreateWindow(1, 1, "", NULL, NULL);
            glfwMakeContextCurrent(ns_shared);

            initExtensions();

            glfwMakeContextCurrent(NULL);
        }
    }

    void deInitialize()
    {
        if (ns_windowRefs.empty())
        {
            glfwDestroyWindow(ns_shared);
            glfwTerminate();
        }
    }
}

namespace jop { namespace detail
{
    WindowImpl::WindowImpl(const Window::Settings& settings, Window& windowPtr)
        : m_window      (nullptr),
          m_vertexArray (0)
    {
        initialize();

        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        glfwWindowHint(GLFW_VISIBLE, settings.visible);
        glfwWindowHint(GLFW_SAMPLES, settings.samples);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, settings.debug);

        // Pixel format
        glfwWindowHint(GLFW_RED_BITS, 8);
        glfwWindowHint(GLFW_GREEN_BITS, 8);
        glfwWindowHint(GLFW_BLUE_BITS, 8);
        glfwWindowHint(GLFW_ALPHA_BITS, 0);
        glfwWindowHint(GLFW_SRGB_CAPABLE, GL_TRUE);

        // Depth & stencil exist in the renderer frame buffer
        glfwWindowHint(GLFW_DEPTH_BITS, 0);
        glfwWindowHint(GLFW_STENCIL_BITS, 0);

        // Frame rate limit
        glfwWindowHint(GLFW_REFRESH_RATE, settings.maxFrameRate == 0 ? GLFW_DONT_CARE : settings.maxFrameRate);
        
        // Decorated window
        glfwWindowHint(GLFW_DECORATED, settings.displayMode == Window::DisplayMode::Windowed);
        
        m_window = glfwCreateWindow(settings.size.x, settings.size.y, settings.title.c_str(), settings.displayMode != Window::DisplayMode::Fullscreen ? NULL : glfwGetPrimaryMonitor(), ns_shared);

        JOP_ASSERT(m_window != nullptr, "Failed to create window! Title: " + settings.title);

        ns_windowRefs[m_window] = &windowPtr;

        if (settings.displayMode != Window::DisplayMode::Fullscreen)
        {
            auto vm = glfwGetVideoMode(glfwGetPrimaryMonitor());
            
            int width = 0, height = 0;
            glfwGetWindowSize(m_window, &width, &height);
            glfwSetWindowPos(m_window, vm->width / 2 - width / 2, vm->height / 2 - height / 2);
        }

        glfwMakeContextCurrent(m_window);
        glfwSwapInterval(static_cast<int>(settings.vSync));
        
        glCheck(glGenVertexArrays(1, &m_vertexArray));
        glCheck(glBindVertexArray(m_vertexArray));

        GlState::setDepthTest(true);
        GlState::setFaceCull(true);
        GlState::setSeamlessCubemap(true);
        GlState::setBlendFunc(true);
        GlState::setFramebufferSrgb(true);

        glCheck(glDisable(GL_DITHER));
    }

    WindowImpl::~WindowImpl()
    {
        glCheck(glBindVertexArray(0));
        glCheck(glDeleteVertexArrays(1, &m_vertexArray));

        // There should always be a valid window
        ns_windowRefs.erase(m_window);
        glfwDestroyWindow(m_window);
        deInitialize();
    }

    //////////////////////////////////////////////

    void WindowImpl::swapBuffers()
    {
        glfwSwapBuffers(m_window);
    }

    //////////////////////////////////////////////

    GLFWwindow* WindowImpl::getLibraryHandle()
    {
        return m_window;
    }

    //////////////////////////////////////////////

    WindowHandle WindowImpl::getNativeHandle()
    {
    #ifdef JOP_OS_WINDOWS
        return glfwGetWin32Window(m_window);
    #endif
    }

    //////////////////////////////////////////////

    void WindowImpl::pollEvents()
    {
        glfwPollEvents();
    }

    void WindowImpl::setMouseMode(const Mouse::Mode mode)
    {
        static const int modes[] = 
        {
            GLFW_CURSOR_NORMAL,
            GLFW_CURSOR_HIDDEN,
            GLFW_CURSOR_DISABLED
        };

        glfwSetInputMode(m_window, GLFW_CURSOR, modes[static_cast<int>(mode)]);
    }

    //////////////////////////////////////////////

    void WindowImpl::setPosition(const int x, const int y)
    {
        glfwSetWindowPos(m_window, x, y);
    }

    //////////////////////////////////////////////

    glm::ivec2 WindowImpl::getPosition() const
    {
        glm::ivec2 p;
        glfwGetWindowPos(m_window, &p.x, &p.y);

        return p;
    }

    //////////////////////////////////////////////

    void WindowImpl::setSize(const int width, const int height)
    {
        glfwSetWindowSize(m_window, width, height);
    }

    //////////////////////////////////////////////

    glm::ivec2 WindowImpl::getSize() const
    {
        glm::ivec2 s;
        glfwGetFramebufferSize(m_window, &s.x, &s.y);

        return s;
    }

    //////////////////////////////////////////////

    Window* WindowImpl::getCurrentContextWindow()
    {
        auto itr = ns_windowRefs.find(glfwGetCurrentContext());

        if (itr != ns_windowRefs.end())
            return itr->second;

        return nullptr;
    }
}}

#endif