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

#ifndef JOP_RESOURCEMANAGER_HPP
#define JOP_RESOURCEMANAGER_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Core/Subsystem.hpp>
#include <unordered_map>
#include <memory>

//////////////////////////////////////////////


namespace jop
{
    class Resource;

    class JOP_API ResourceManager : public Subsystem
    {
    public:

        /// \brief Default constructor
        ///
        ResourceManager();

        /// \brief Destructor
        ///
        ~ResourceManager() override;


        /// \brief Load and get a resource
        ///
        /// If resource is not found this creates a new one. The first argument must be convertible
        /// into std::string, as it's used as a hash map key.
        ///
        /// \param args Arguments passed to resource's constructor
        ///
        /// \return Pointer to the resource. Empty if loading failed
        ///
        template<typename T, typename ... Args>
        static std::weak_ptr<T> getResource(const Args&... args);

        /// \brief Get a named resource
        ///
        /// This is primarily used when the resource is not loaded from a file. After the resource
        /// has been loaded once, it can be retrieved by using getResource().
        ///
        /// \param name Name for the resource
        /// \param args Arguments passed to resource's constructor
        ///
        /// \return Pointer to the resource. Empty if loading failed
        ///
        template<typename T, typename ... Args>
        static std::weak_ptr<T> getNamedResource(const std::string& name, const Args&... args);


        /// \brief Deletes resource from memory
        ///
        /// \param Name or path for wanted resource
        ///
        static void unloadResource(const std::string& path);

        /// \brief Deletes all resources from memory
        ///
        static void unloadResources();

    private:

        static ResourceManager* m_instance;                                     ///< Pointer to the single instance


        std::unordered_map<std::string, std::shared_ptr<Resource>> m_resources; ///< Container holding resources

    };

    // Include the template implementation file
    #include <Jopnal/Core/Inl/ResourceManager.inl>
}

#endif

/// \class ResourceManager
/// \ingroup core