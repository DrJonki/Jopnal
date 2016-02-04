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

#ifndef JOP_COLOR_HPP
#define JOP_COLOR_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <glm/glm.hpp>
#include <string>

//////////////////////////////////////////////


namespace jop
{
    class JOP_API Color
    {
    public:

        /// \brief Default constructor
        ///
        /// Initializes this color as white.
        ///
        Color();

        /// \brief Overloaded constructor
        ///
        /// \param _r The red value
        /// \param _g The green value
        /// \param _b The blue value
        /// \param _a The alpha value
        ///
        Color(const jop::uint8 _r, const jop::uint8 _g, const jop::uint8 _b, const jop::uint8 _a = 255);

        /// \brief Overloaded constructor
        ///
        /// \param colors A 32-bit unsigned integer to represent the colors,
        ///               as in 0xFFFFFFFF
        ///
        Color(const jop::uint32 colors);

        /// \brief Overloaded constructor
        ///
        /// \param hexString A string in hex form to describe the color, as
        ///                  in "FFFFFFFF"
        ///
        Color(const std::string& hexString);


        /// \brief Get this color as a float vector
        ///
        /// The precision of the values is 32 bits each.
        ///
        /// \return A float vector describing the color
        ///
        glm::fvec4 asFloatVector() const;


    public:

        jop::uint8   r, ///< The red component
                     g, ///< The green component
                     b, ///< The blue component
                     a; ///< The alpha component

        static const Color Black;
        static const Color White;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color Cyan;
        static const Color Yellow;
        static const Color Magenta;
        static const Color Purple;
        static const Color Orange;
        static const Color Gray;
        static const Color Brown;
        static const Color Transparent;


    public:

        /// \brief Equality operator
        ///
        bool operator ==(const Color& other) const;

        /// \brief Inequality operator
        ///
        bool operator !=(const Color& other) const;

    };
}

#endif

/// \class Color
/// \ingroup graphics
///
/// #TODO Detailed description