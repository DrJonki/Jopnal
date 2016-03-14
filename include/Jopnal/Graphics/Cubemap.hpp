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

#ifndef JOP_CUBEMAP_HPP
#define JOP_CUBEMAP_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Graphics/Texture.hpp>

//////////////////////////////////////////////


namespace jop
{
    class JOP_API Cubemap final : public Texture
    {
    public:

        Cubemap(const std::string& name);


        bool load(const std::string& right, const std::string& left,
                  const std::string& top, const std::string& bottom,
                  const std::string& back, const std::string& front);

        bool load(const int width, const int height, const int bpp);

    private:

        int m_width;
        int m_height;
        int m_bytesPerPixel;

    };
}

#endif