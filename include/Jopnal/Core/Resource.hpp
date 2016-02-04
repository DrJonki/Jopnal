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

#ifndef JOP_RESOURCE_HPP
#define JOP_RESOURCE_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <string>
#include <memory>

//////////////////////////////////////////////


namespace jop
{
    class JOP_API Resource : public std::enable_shared_from_this<Resource>
    {
    public:

        /// \brief Constructor
        ///
        Resource();

        /// \brief Virtual destructor
        ///
        virtual ~Resource() = 0;
    };
}

#endif

/// \class Resource
/// \ingroup core
///
/// This is the base class for all resources that are loaded from files