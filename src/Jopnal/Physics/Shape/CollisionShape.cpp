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

    #include <Jopnal/Physics/Shape/CollisionShape.hpp>

    #pragma warning(push)
    #pragma warning(disable: 4127)

    #include <btBulletCollisionCommon.h>

    #pragma warning(pop)

#endif

//////////////////////////////////////////////


namespace jop
{
    CollisionShape::CollisionShape(const std::string& name)
        : Resource  (name),
          m_shape   ()
    {}

    CollisionShape::CollisionShape(const CollisionShape& other, const std::string& newName)
        : Resource  (other, newName),
          m_shape   ()
    {}

    CollisionShape::~CollisionShape()
    {}

    //////////////////////////////////////////////

    void CollisionShape::setMargin(const float margin)
    {
        m_shape->setMargin(margin);
    }

    //////////////////////////////////////////////

    float CollisionShape::getMargin() const
    {
        return m_shape->getMargin();
    }

    //////////////////////////////////////////////

    void CollisionShape::setLocalScale(const glm::vec3& scale)
    {
        m_shape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
    }

    //////////////////////////////////////////////

    glm::vec3 CollisionShape::getLocalScale() const
    {
        auto& s = m_shape->getLocalScaling();
        return glm::vec3(s.x(), s.y(), s.z());
    }
}