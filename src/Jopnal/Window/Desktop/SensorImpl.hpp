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

#ifndef JOP_SENSORIMPL_HPP
#define JOP_SENSORIMPL_HPP

// Headers
#include <Jopnal/Header.hpp>

#ifdef JOP_OS_DESKTOP

#include <Jopnal/Window/Sensor.hpp>
#include <glm/vec3.hpp>

//////////////////////////////////////////////


namespace jop
{
    class SensorImpl
    {
    public:

        static void init();

        static void deInit();

    public:

        SensorImpl(const Sensor::Type type);

        bool isAvailable() const;

        glm::vec3 getData() const;

        void setEnabled(const bool);

        bool isEnabled() const;
    };
}

#endif
#endif