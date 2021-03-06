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

#ifndef JOP_TEXTURESAMPLER_HPP
#define JOP_TEXTURESAMPLER_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Core/Resource.hpp>
#include <Jopnal/Graphics/Color.hpp>
#include <memory>

//////////////////////////////////////////////


namespace jop
{
    class JOP_API TextureSampler final : public Resource
    {
    private:

        JOP_DISALLOW_COPY_MOVE(TextureSampler);

    public:

        /// The filtering mode
        ///
        enum class Filter
        {
            None,       ///< No filtering
            Bilinear,   ///< Bilinear (linear) filtering
            Trilinear,  ///< Trilinear filtering, requires mipmaps

            /// Anisotropic filtering, requires mipmaps
            ///
            /// \warning Only supported with the extension EXT_texture_filter_anisotropic
            ///
            Anisotropic
        };

        /// The repeat mode
        ///
        /// \warning Repeat modes (Basic, Mirrored) on GLES 2.0 are only supported with
        ///          the extension OES_texture_npot, when using non-power-of-two textures
        ///
        enum class Repeat
        {
            Basic,      ///< Repeat
            Mirrored,   ///< Mirrored repeat
            ClampEdge,  ///< Clamp to edge

            /// Clamp to border
            ///
            /// \warning On GLES 3.1 and lesser, only supported with one of these extensions:
            ///          NV_texture_border_clamp, EXT_texture_border_clamp, OES_texture_border_clamp
            ///
            ClampBorder
        };

    public:

        /// \brief Constructor
        /// 
        /// This will create a sampler object with default settings.
        ///
        /// \param name Name of the resource
        ///
        TextureSampler(const std::string& name);

        /// \brief Constructor
        ///
        /// \param name Name of the resource
        /// \param filter Initial filtering mode
        /// \param repeat Initial repeating mode
        /// \param param Anisotropic filtering level
        ///
        TextureSampler(const std::string& name, const Filter filter, const Repeat repeat, const float param = 1.f);

        /// \brief Destructor
        ///
        ~TextureSampler();


        /// \brief Bind this sampler
        ///
        /// \param textureUnit The texture unit to bind this sample into
        ///
        void bind(const unsigned int textureUnit) const;

        /// \brief Unbind a sampler
        ///
        /// \param textureUnit The texture unit to unbind a sampler from
        ///
        static void unbind(const unsigned int textureUnit);

        /// \brief Delete and recreate this sampler, clearing all the settings
        ///
        /// \return Reference to self
        ///
        TextureSampler& reset();

        /// \brief Set the filtering mode
        ///
        /// \param mode The filtering mode
        /// \param param Possible anisotropic filtering level
        ///
        /// \return Reference to self
        ///
        TextureSampler& setFilterMode(const Filter mode, const float param = 1.f);

        /// \brief Set the repeating mode
        ///
        /// \param repeat The repeating mode
        ///
        /// \return Reference to self
        ///
        TextureSampler& setRepeatMode(const Repeat repeat);

        /// \brief Set the border color
        ///
        /// \warning On GLES 3.1 and lesser, only supported with one of these extensions:
        ///          EXT_texture_border_clamp, OES_texture_border_clamp
        ///
        /// \param color The border color
        ///
        /// \return Reference to self
        ///
        TextureSampler& setBorderColor(const Color& color);

        /// \brief Get the OpenGL handle
        ///
        /// \return The OpenGL handle
        ///
        unsigned int getHandle() const;

        /// \brief Get the filtering mode
        ///
        /// \return The filtering mode
        ///
        Filter getFilterMode() const;

        /// \brief Get the repeating mode
        ///
        /// \return The repeating mode
        ///
        Repeat getRepeatMode() const;

        /// \brief Get the anisotropy level
        ///
        /// \return The anisotropy level
        ///
        float getAnisotropyLevel() const;

        /// \brief Get the border color
        ///
        /// \return The border color
        ///
        const Color& getBorderColor() const;

        /// \brief Get the maximum anisotropy level supported by the system
        ///
        /// \return The maximum anisotropy level. Zero if not supported
        ///
        static float getMaxAnisotropy();

    private:

        Filter m_filter;        ///< The filtering mode
        Repeat m_repeat;        ///< The repeating mode
        float m_anisotropic;    ///< The anisotropy level
        Color m_borderColor;    ///< The border color
        unsigned int m_sampler; ///< The OpenGL sampler handle
    };
}

/// \class jop::TextureSampler
/// \ingroup graphics

#endif