//Jopnal Engine C++ Library
//Copyright(c) 2016 Team Jopnal
//
//This software is provided 'as-is', without any express or implied
//warranty.In no event will the authors be held liable for any damages
//arising from the use of this software.
//
//Permission is granted to anyone to use this software for any purpose,
//including commercial applications, and to alter it and redistribute it
//freely, subject to the following restrictions :
//
//1. The origin of this software must not be misrepresented; you must not
//claim that you wrote the original software.If you use this software
//in a product, an acknowledgement in the product documentation would be
//appreciated but is not required.
//2. Altered source versions must be plainly marked as such, and must not be
//misrepresented as being the original software.
//3. This notice may not be removed or altered from any source distribution.

//////////////////////////////////////////////

#ifndef JOP_TEXT_HPP
#define JOP_TEXT_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Graphics/Drawable.hpp>
#include <Jopnal/Graphics/Font.hpp>
#include <Jopnal/Graphics/Texture2D.hpp>
#include <Jopnal/Graphics/Mesh.hpp>
#include <Jopnal/Graphics/Material.hpp>

//////////////////////////////////////////////

namespace jop
{
    class JOP_API Text
        :public GenericDrawable, private Mesh
    {
    public:
        Text(Object& object, Renderer& renderer);
        
        void setString(const std::string &string); //generates vertices
        //void draw(const Camera&, const LightContainer&)override;
        void setPosition(const glm::vec2 position);
        void setFont(Font& font);
        void setScale(const float x, const float y);
        void setScale(glm::vec2 scale);
    private:
        WeakReference<Font> m_font;
        Material m_material;
        glm::vec2 m_position;
        glm::vec2 m_size;
        std::string m_string;
    };
}

#endif

/// \class Text
/// \ingroup graphics
///
/// Class for rendering text
/// 
///