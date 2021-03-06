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

    #include <Jopnal/Graphics/OpenGL/OpenGL.hpp>
    #include <unordered_map>
    #include <cstring>

    #ifdef JOP_OPENGL_ES
        #include <EGL/egl.h>
    #endif

#endif

//////////////////////////////////////////////


namespace jop 
{
#ifdef JOP_OPENGL_ES

    namespace detail
    {
        bool checkExtensionBase(const char* exts, const char* ext)
        {
            static std::unordered_map<const char*, bool> extMap;

            auto itr = extMap.find(ext);

            if (itr != extMap.end())
                return itr->second;

            const bool available = strstr(exts, ext) != NULL;

            extMap[ext] = available;

            return available;
        }

        //////////////////////////////////////////////

        bool checkGLESExtension(const char* ext)
        {
            return checkExtensionBase(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)), ext);
        }

        //////////////////////////////////////////////

        bool checkEGLExtension(const char* ext)
        {
            return checkExtensionBase(eglQueryString(eglGetDisplay(EGL_DEFAULT_DISPLAY), EGL_EXTENSIONS), ext);
        }

        //////////////////////////////////////////////

        const std::string& getVersionString()
        {
            static std::string version;

            if (version.empty())
            {
                const std::string esVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));

                const std::size_t numPos = esVersion.find_first_of("0123456789");

                version += esVersion[numPos];
                version += '.';
                version += esVersion[numPos + 2];
            }

            return version;
        }
    }

#endif

    namespace gl
    {
        unsigned int getVersionMajor()
        {
        #ifdef JOP_OPENGL_ES

            #ifdef JOP_OPENGL_ES3
                return detail::getVersionString()[0] - '0';
            #else   
                return 2;
            #endif

        #else
            return ogl_GetMajorVersion();

        #endif
        }

        //////////////////////////////////////////////

        unsigned int getVersionMinor()
        {
        #ifdef JOP_OPENGL_ES

            #ifdef JOP_OPENGL_ES3
                return detail::getVersionString()[2] - '0';
            #else
                return 0;
            #endif

        #else
            return ogl_GetMinorVersion();

        #endif
        }

        //////////////////////////////////////////////

        unsigned int getGLSLVersion()
        {
            static unsigned int version = 0;

            if (!version)
            {
                const char* str = std::strpbrk(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)), "0123456789");

                version = std::stoul(std::string(&str[0], 1) + str[2] + str[3]);
            }

            return version;
        }

        //////////////////////////////////////////////

        const bool es =
        #ifdef JOP_OPENGL_ES
            true;
        #else
            false;
        #endif
    }
}