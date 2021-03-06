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

#ifndef JOP_RIGIDBODY2D_HPP
#define JOP_RIGIDBODY2D_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Physics/RigidBody.hpp>
#include <Jopnal/Physics2D/Collider2D.hpp>
#include <Jopnal/Physics2D/Joint/Joint2D.hpp>
#include <unordered_set>
#include <memory>

//////////////////////////////////////////////


class b2Shape;

namespace jop
{
    class CollisionShape2D;

    class JOP_API RigidBody2D : public Collider2D
    {
    private:

        JOP_GENERIC_COMPONENT_CLONE(RigidBody2D);

    public:

        /// Rigid body construction info
        ///
        class JOP_API ConstructInfo2D
        {
        private:

            JOP_DISALLOW_COPY_MOVE(ConstructInfo2D);

            friend class RigidBody2D;

        public:

            /// \brief Constructor
            ///
            /// \param shape The collision shape
            /// \param type Body type
            /// \param mass Mass, will default to 0 when type is static or kinematic
            ///
            ConstructInfo2D(const CollisionShape2D& shape, const RigidBody::Type type = RigidBody::Type::Static, const float mass = 0.f);

            int16 group;            ///< Collision filter group
            int16 mask;             ///< Collision filter mask

            float friction;         ///< Friction
            float restitution;      ///< Restitution

        private:

            const CollisionShape2D& m_shape;    ///< Collision shape
            const RigidBody::Type m_type;       ///< Body type
            const float m_mass;                 ///< Mass
        };

    public:

        /// \brief Constructor
        ///
        /// \param object Reference to the object
        /// \param world The physics world
        /// \param info Construction info
        ///
        RigidBody2D(Object& object, World2D& world, const ConstructInfo2D& info);

        /// \brief Destructor
        ///
        virtual ~RigidBody2D() override;


        /// \brief Set gravity scaling to the rigid body object
        ///
        /// \param scale Scale of the gravity to be applied as float
        ///
        /// \return Reference to self
        ///
        /// \comm setGravityScale
        ///
        RigidBody2D& setGravityScale(const float scale);

        /// \brief Get the gravity scale
        ///
        /// \return float
        ///
        float getGravityScale() const;

        /// \brief Sets the linear velocity for rigid body
        ///
        /// \param linearVelocity Unique vector for linear velocity
        ///
        /// \return Reference to self
        ///
        /// \comm setLinearVelocity
        ///
        RigidBody2D& setLinearVelocity(const glm::vec2& linearVelocity);

        /// \brief Get the linear velocity of the body
        ///
        /// \return The linear velocity
        ///
        glm::vec2 getLinearVelocity() const;

        /// \brief Sets the angular velocity for rigid body
        ///
        /// \param angularVelocity Unique float for angular velocity
        ///
        /// \return Reference to self
        ///
        /// \comm setAngularVelocity
        ///
        RigidBody2D& setAngularVelocity(const float angularVelocity);

        /// \brief Gets the angular velocity as float
        ///
        /// \return The angular velocity
        ///
        float getAngularVelocity() const;

        /// \brief Applies constant force to rigid bodies relative position
        ///
        /// \param force Amount and direction of the force 
        /// \param worldPoint Vector for a point in the world that the force is applied to relative to the rigid body
        ///
        /// \return Reference to self
        ///
        /// \comm applyForce
        ///
        RigidBody2D& applyForce(const glm::vec2& force, const glm::vec2& worldPoint);

        /// \brief Applies force to the rigid body's center 
        ///
        /// \param force Amount and direction of the applied force
        ///
        /// \return Reference to self
        ///
        /// \comm applyCentralForce
        ///
        RigidBody2D& applyCentralForce(const glm::vec2& force);

        /// \brief Applies an impulse to rigid bodies relative position
        ///
        /// \param impulse Amount of the impulse
        ///
        /// \return Reference to self
        ///
        /// \comm applyImpulse
        ///
        RigidBody2D& applyTorqueImpulse(const float impulse);

        /// \brief Applies an impulse to rigid bodies relative position
        ///
        /// \param impulse Amount and direction of the impulse
        /// \param point Vector for a point in world that the impulse is applied to relative to the rigid body
        ///
        /// \return Reference to self
        ///
        /// \comm applyImpulse
        ///
        RigidBody2D& applyLinearImpulse(const glm::vec2& impulse, const glm::vec2& point);

        /// \brief Applies impulse to the rigid body's center
        ///
        /// \param impulse Amount and direction of the impulse
        ///
        /// \return Reference to self
        ///
        /// \comm applyCentralImpulse
        ///
        RigidBody2D& applyCentralImpulse(const glm::vec2& impulse);

        /// \brief Applies torque to the rigid body
        ///
        /// \param torque Amount and direction as vector of the applied torque
        ///
        /// \return Reference to self
        ///
        /// \comm applyTorque
        ///
        RigidBody2D& applyTorque(const float torque);

        /// \brief Sets/unsets the body to constantly rotate
        ///
        /// \param rot Enable/disable body from rotating. Resets the mass of the body.
        ///
        /// \return Reference to self
        ///
        /// \comm setFixedRotation
        ///
        RigidBody2D& setFixedRotation(const bool rot);

        /// \brief Check if fixed rotation has been set
        ///
        /// \return True if set
        ///
        bool hasFixedRotation() const;

        /// \brief Sets the RigidBody2D position to be same as the objects' transform
        ///
        /// \return Reference to self.
        ///
        RigidBody2D& synchronizeTransform();

        /// \brief Clear the affecting forces of this body
        ///
        RigidBody2D& clearForces();

        /// \brief Returns a pointer to a joint on the RigidBody2D whence called from
        ///
        /// User can give an ID of the the joint which to return. If left empty, returns a pointer to the first joint the RigidBody2D has.
        ///
        /// \param ID The joint's identifier
        ///
        /// \return Returns a pointer to the joint. nullptr if not found.
        ///
        template<typename T>
        T* getJoint(unsigned int id = 0);

        /// \brief Breaks a joint from the RigidBody2D whence called from
        ///
        /// User can give an ID of the the joint which to break. If left empty, breaks the first joint the RigidBody2D has.
        ///
        /// \param ID The joint's identifier
        ///
        /// \return Returns true if successful.
        ///
        template<typename T>
        bool breakJoint(unsigned int ID = 0);

        /// \brief Creates a joint between this RigidBody2D and another RigidBody2D
        ///
        /// T is the type of the joint to create. Applicable joints are derived from Joint2D.
        ///
        /// \param body The other body to link
        /// \param args The arguments to pass to the joint's constructor
        ///
        /// \return Returns a reference to the RigidBody2D whence called from.
        ///
        template<typename T, typename ... Args>
        T& link(RigidBody2D& body, Args&&... args);

    protected:

        /// \copydoc Component::receiveMessage()
        ///
        Message::Result receiveMessage(const Message& message) override;

        /// \brief Initializes m_body with a collidable object.
        ///
        /// Handled by the engine.
        ///
        void createCollidable(const ConstructInfo2D& info, const b2Shape& shape);


        /// All the joints that this body has.
        ///
        std::unordered_set<std::shared_ptr<Joint2D>> m_joints;
    };

    // Include the template implementation file
    #include <Jopnal/Physics2D/Inl/RigidBody2D.inl>
}

/// \class jop::RigidBody2D
/// \ingroup physics2d

#endif