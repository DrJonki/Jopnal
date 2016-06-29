# Jopnal Physics CMakeLists

set(__INCDIR_PHYSICS ${PROJECT_SOURCE_DIR}/include/Jopnal/Physics)
set(__SRCDIR_PHYSICS ${PROJECT_SOURCE_DIR}/src/Jopnal/Physics)

# Include
set(__INC_PHYSICS
    ${__INCDIR_PHYSICS}/Collider.hpp
    ${__INCDIR_PHYSICS}/ContactInfo.hpp
    ${__INCDIR_PHYSICS}/PhantomBody.hpp
    ${__INCDIR_PHYSICS}/RayInfo.hpp
    ${__INCDIR_PHYSICS}/RigidBody.hpp
    ${__INCDIR_PHYSICS}/World.hpp
)
source_group("Physics\\Headers" FILES ${__INC_PHYSICS})
list(APPEND SRC ${__INC_PHYSICS})

# Include - Shape
set(__INC_PHYSICS_SHAPE
    ${__INCDIR_PHYSICS}/Shape/BoxShape.hpp
    ${__INCDIR_PHYSICS}/Shape/CapsuleShape.hpp
    ${__INCDIR_PHYSICS}/Shape/CollisionShape.hpp
    ${__INCDIR_PHYSICS}/Shape/CompoundShape.hpp
    ${__INCDIR_PHYSICS}/Shape/ConeShape.hpp
    ${__INCDIR_PHYSICS}/Shape/ConvexHullShape.hpp
    ${__INCDIR_PHYSICS}/Shape/CylinderShape.hpp
    ${__INCDIR_PHYSICS}/Shape/FrustumShape.hpp
    ${__INCDIR_PHYSICS}/Shape/InfinitePlaneShape.hpp
    ${__INCDIR_PHYSICS}/Shape/RectangleShape.hpp
    ${__INCDIR_PHYSICS}/Shape/SphereShape.hpp
    ${__INCDIR_PHYSICS}/Shape/TerrainShape.hpp
)
source_group("Physics\\Headers\\Shape" FILES ${__INC_PHYSICS_SHAPE})
list(APPEND SRC ${__INC_PHYSICS_SHAPE})

# Source
set(__SRC_PHYSICS
    ${__SRCDIR_PHYSICS}/Collider.cpp
    ${__SRCDIR_PHYSICS}/ContactInfo.cpp
    ${__SRCDIR_PHYSICS}/PhantomBody.cpp
    ${__SRCDIR_PHYSICS}/RayInfo.cpp
    ${__SRCDIR_PHYSICS}/RigidBody.cpp
    ${__SRCDIR_PHYSICS}/World.cpp
)
source_group("Physics\\Source" FILES ${__SRC_PHYSICS})
list(APPEND SRC ${__SRC_PHYSICS})

# Source - Shape
set(__SRC_PHYSICS_SHAPE
    ${__SRCDIR_PHYSICS}/Shape/BoxShape.cpp
    ${__SRCDIR_PHYSICS}/Shape/CapsuleShape.cpp
    ${__SRCDIR_PHYSICS}/Shape/CollisionShape.cpp
    ${__SRCDIR_PHYSICS}/Shape/CompoundShape.cpp
    ${__SRCDIR_PHYSICS}/Shape/ConeShape.cpp
    ${__SRCDIR_PHYSICS}/Shape/ConvexHullShape.cpp
    ${__SRCDIR_PHYSICS}/Shape/CylinderShape.cpp
    ${__SRCDIR_PHYSICS}/Shape/FrustumShape.cpp
    ${__SRCDIR_PHYSICS}/Shape/InfinitePlaneShape.cpp
    ${__SRCDIR_PHYSICS}/Shape/RectangleShape.cpp
    ${__SRCDIR_PHYSICS}/Shape/SphereShape.cpp
    ${__SRCDIR_PHYSICS}/Shape/TerrainShape.cpp
)
source_group("Physics\\Source\\Shape" FILES ${__SRC_PHYSICS_SHAPE})
list(APPEND SRC ${__SRC_PHYSICS_SHAPE})

# Source - Detail
set(__SRC_PHYSICS_DETAIL
    ${__SRCDIR_PHYSICS}/Detail/WorldImpl.cpp
    ${__SRCDIR_PHYSICS}/Detail/WorldImpl.hpp
)
source_group("Physics\\Source\\Detail" FILES ${__SRC_PHYSICS_DETAIL})
list(APPEND SRC ${__SRC_PHYSICS_DETAIL})