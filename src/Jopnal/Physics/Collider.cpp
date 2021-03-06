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

    #include <Jopnal/Physics/Collider.hpp>

    #include <Jopnal/Core/Object.hpp>
    #include <Jopnal/Physics/World.hpp>
    #include <Jopnal/Physics/Detail/WorldImpl.hpp>
    #include <Jopnal/Physics/ContactListener.hpp>
    #include <Jopnal/Physics/Shape/CollisionShape.hpp>
    #include <Jopnal/Core/DebugHandler.hpp>

    #pragma warning(push)
    #pragma warning(disable: 4127)

    #include <btBulletCollisionCommon.h>

    #pragma warning(pop)

#endif

//////////////////////////////////////////////

namespace jop
{
    Collider::Collider(Object& object, World& world, const uint32 ID)
        : Component                     (object, ID),
          SafeReferenceable<Collider>   (this),
          m_body                        (),
          m_worldRef                    (world),
          m_detached                    (false),
          m_allowSleep                  (true)
    {}

    Collider::Collider(const Collider& other, Object& newObj)
        : Component                     (other, newObj),
          SafeReferenceable<Collider>   (this),
          m_body                        (),
          m_worldRef                    (other.m_worldRef),
          m_detached                    (other.m_detached),
          m_allowSleep                  (other.m_allowSleep)
    {}

    Collider::~Collider()
    {
        for (auto& i : m_listeners)
            i->m_collider = nullptr;
    }

    //////////////////////////////////////////////

    void Collider::update(const float)
    {
        if (m_detached)
            return;

        const bool active = isActive();

        if (m_body->isActive() != active)
        {
            if (m_body->isKinematicObject() || !m_allowSleep)
                m_body->setActivationState(active ? DISABLE_DEACTIVATION : DISABLE_SIMULATION);

            else
                m_body->setActivationState(active ? ACTIVE_TAG : DISABLE_SIMULATION);
        }
    }

    //////////////////////////////////////////////

    void Collider::setAllowSleep(const bool allow)
    {
        m_allowSleep = allow;
    }

    //////////////////////////////////////////////

    bool Collider::isSleepAllowed() const
    {
        return m_allowSleep;
    }

    //////////////////////////////////////////////

    bool Collider::checkOverlap(const Collider& other) const
    {
        if (m_detached)
            return false;

        struct Callback : btBroadphaseAabbCallback
        {
            const void* m_against;
            bool hit;

            explicit Callback(const void* against)
                : m_against (against),
                  hit       (false)
            {}

            bool process(const btBroadphaseProxy* proxy) override
            {
                return (hit = (m_against == proxy->m_clientObject)) != true;
            }

        } cb(m_body->getBroadphaseHandle()->m_clientObject);

        auto& bp = *m_worldRef.m_worldData->world->getBroadphase();

        btVector3 min, max;
        bp.getAabb(other.m_body->getBroadphaseHandle(), min, max);
        bp.aabbTest(min, max, cb);
        
        return cb.hit;
    }

    //////////////////////////////////////////////

    bool Collider::checkContact(const Collider& other) const
    {
        if (m_detached)
            return false;

        struct Callback : btCollisionWorld::ContactResultCallback
        {
            bool hit;

            Callback()
                : hit(false)
            {}

            btScalar addSingleResult(btManifoldPoint&, const btCollisionObjectWrapper*, int, int, const btCollisionObjectWrapper*, int, int) override
            {
                hit = true;
                return 0.f;
            }

        } cb;

        m_worldRef.m_worldData->world->contactPairTest(m_body.get(), other.m_body.get(), cb);
        
        return cb.hit;
    }

    //////////////////////////////////////////////

    bool Collider::checkRay(const glm::vec3& start, const glm::vec3& ray) const
    {
        if (m_detached)
            return false;

        struct Callback : btCollisionWorld::RayResultCallback
        {
            const void* m_against;
            bool hit;
            
            explicit Callback(const void* against)
                : m_against (against),
                  hit       (false)
            {}
            
            btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool) override
            {
                hit = rayResult.m_collisionObject->getUserPointer() == m_against;
                return 0.f;
            }

        } cb(this);

        btTransform from;
        from.setOrigin(btVector3(start.x, start.y, start.z));
        btTransform to;
        const glm::vec3 fromTo(start + ray);
        to.setOrigin(btVector3(fromTo.x, fromTo.y, fromTo.z));

        m_worldRef.m_worldData->world->rayTestSingle(from, to, m_body.get(), m_body->getCollisionShape(), m_body->getWorldTransform(), cb);
        
        return cb.hit;
    }

    //////////////////////////////////////////////

    World& Collider::getWorld()
    {
        return m_worldRef;
    }

    //////////////////////////////////////////////

    const World& Collider::getWorld() const
    {
        return m_worldRef;
    }

    //////////////////////////////////////////////

    void Collider::registerListener(ContactListener& listener)
    {
        if (listener.m_collider != this)
        {
            if (listener.m_collider)
                listener.m_collider->m_listeners.erase(&listener);

            // Replace old collider with this
            listener.m_collider = this;

            // Check if this listener is already registered
            std::pair<std::set<ContactListener*>::iterator, bool> ret;
            ret = m_listeners.insert(&listener);

            if (ret.second == false)
            {
                // Erase the old and replace it with new
                m_listeners.erase(ret.first);
                m_listeners.insert(&listener);
            }
        }
        else
        {
            JOP_DEBUG_INFO("Could not register listener for Collider2D: Listener is already registered for collider");
            return;
        }
    }

    //////////////////////////////////////////////

    void Collider::detachFromWorld()
    {
        if (!m_detached)
        {
            m_worldRef.m_worldData->world->removeCollisionObject(m_body.get());
            m_detached = true;
        }
    }

    //////////////////////////////////////////////

    void Collider::attachToWorld()
    {
        if (m_detached)
        {
            m_worldRef.m_worldData->world->addCollisionObject(m_body.get());
            m_detached = false;
        }
    }

    //////////////////////////////////////////////

    bool Collider::isDetachedFromWorld() const
    {
        return m_detached;
    }

    //////////////////////////////////////////////

    void Collider::updateWorldBounds()
    {
        m_worldRef.m_worldData->world->updateSingleAabb(m_body.get());
    }

    //////////////////////////////////////////////

    void Collider::setCollisionShape(CollisionShape& shape)
    {
        m_body->setCollisionShape(shape.m_shape.get());
    }

    //////////////////////////////////////////////

    const CollisionShape* Collider::getCollisionShape() const
    {
        if (m_body->getCollisionShape())
            return static_cast<const CollisionShape*>(m_body->getCollisionShape()->getUserPointer());

        return nullptr;
    }
}