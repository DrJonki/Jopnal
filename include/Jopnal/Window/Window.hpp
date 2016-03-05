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

#ifndef JOP_WINDOW_HPP
#define JOP_WINDOW_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Core/Subsystem.hpp>
#include <Jopnal/Window/Mouse.hpp>
#include <Jopnal/Window/WindowHandle.hpp>
#include <glm/vec2.hpp>
#include <memory>
#include <string>

//////////////////////////////////////////////


struct GLFWwindow;

namespace jop
{
    class WindowEventHandler;

    namespace detail
    {
        class WindowImpl;
    }

    class JOP_API Window final : public Subsystem
    {
    private:

        JOP_DISALLOW_COPY_MOVE(Window);

    public:

        /// Display modes
        ///
        enum class DisplayMode
        {
            Windowed,   ///< Windowed with decorations
            Borderless, ///< Windowed without decorations
            Fullscreen  ///< Full screen
        };

        /// Window settings
        ///
        struct JOP_API Settings
        {
            Settings(const bool loadSettings);

            glm::uvec2 size;
            std::string title;
            DisplayMode displayMode;
            unsigned int samples;
            bool visible;
            bool vSync;
        };

    public:

        /// \brief Default constructor
        ///
        /// Does not open a window.
        ///
        Window();

        /// \brief Overloaded constructor
        ///
        /// Calls open() using the given settings
        ///
        /// \param settings The window settings
        ///
        Window(const Settings& settings);

        /// \brief Destructor
        ///
        ~Window() override;

        
        /// \brief The pre-update function
        ///
        /// This will simply set an internal boolean flag so
        /// we know when a new frame has begun.
        ///
        void preUpdate(const float deltaTime) override;

        /// \brief The post-update function
        ///
        /// This clears the OpenGL front buffer
        ///
        void postUpdate(const float deltaTime) override;

        /// \brief The post-draw function
        ///
        /// This swaps the OpenGL buffers
        ///
        void draw() override;

        /// \brief Open this window
        ///
        /// This will create a new window. If one already exists, it will be replaced.
        ///
        /// \param settings The window settings
        ///
        void open(const Settings& settings);

        /// \brief Destroy this window
        ///
        void close();

        /// \brief Check if this window is open
        ///
        /// \return True if window is open
        ///
        bool isOpen() const;

        /// \brief Set the clear color
        ///
        /// \param color The new color
        ///
        void setClearColor(const Color& color);

        /// \brief Set the event handler
        ///
        /// This function will automatically pass a reference of this
        /// window into the handler's constructor.
        ///
        /// \param args The arguments to use with construction
        ///
        template<typename T, typename ... Args>
        T& setEventHandler(Args&&... args);

        /// \brief Get the event handler
        ///
        /// \return Pointer to the event handler. NUllptr if none exists
        ///
        WindowEventHandler* getEventHandler();

        /// \brief Remove the event handler
        ///
        void removeEventHandler();

        /// \brief Get the window library handle handle
        ///
        /// On desktop operating systems this is GLFWwindow*.
        ///
        /// \return A pointer to the handle. Nullptr if window hasn't been created
        ///
        WindowLibHandle getLibraryHandle();

        /// \brief Get the native window handle
        ///
        /// \return The native window handle
        ///
        WindowHandle getNativeHandle();

        /// \brief Poll the events of all open windows
        ///
        /// This will poll all events and invoke the appropriate callbacks.
        ///
        static void pollEvents();

        /// \brief Set absolute Viewport for window
        ///
        /// \param x The upper left x coordinate
        /// \param y The upper left y coordinate
        /// \param width Width of the view port in pixels
        /// \param height Height of the view port in pixels
        ///
        void setViewport(const int x, const int y, const unsigned int width, const unsigned int height);

        /// \brief Sets relative Viewport for window
        ///
        /// \param x The upper left relative x coordinate
        /// \param y The upper left relative y coordinate
        /// \param width Relative width of the view port in pixels
        /// \param height Relative height of the view port in pixels
        ///
        void setViewportRelative(const float x, const float y, const float width, const float height);

        /// \brief Sets mouse mode
        ///
        /// Mouse modes: Visible, Hidden, Frozen (Defined in mouse class)
        ///
        /// \param mode Enum mouse mode.
        ///
        void setMouseMode(const Mouse::Mode mode);

        void setPosition(const int x, const int y);

        glm::ivec2 getPosition() const;

        void setSize(const int width, const int height);

        glm::ivec2 getSize(const bool includeFrame = false) const;

        glm::ivec2 getFrameSize() const;

    private:

        Color m_clearColor;                                 ///< The color to use when clearing the buffer
        std::unique_ptr<detail::WindowImpl> m_impl;         ///< The implementation object
        std::unique_ptr<WindowEventHandler> m_eventHandler; ///< The event handler
        bool m_colorChanged;                                ///< Has the clear color changed?

    };

    // Include the template implementation file
    #include <Jopnal/Window/Inl/Window.inl>
}

#endif

/// \class Window
/// \ingroup window
///
/// #TODO Detailed description