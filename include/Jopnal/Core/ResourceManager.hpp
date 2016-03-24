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
#include <Jopnal/Core/Resource.hpp>
#include <Jopnal/Utility/Json.hpp>
#include <unordered_map>
#include <memory>

//////////////////////////////////////////////


namespace jop
{
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
        /// \return Reference to the resource
        ///
        template<typename T, typename ... Args>
        static T& getResource(Args&&... args);

        /// \brief Get a named resource
        ///
        /// This is primarily used when the resource is not loaded from a file. After the resource
        /// has been loaded once, it can be retrieved by using getResource().
        ///
        /// \param name Name for the resource
        /// \param args Arguments passed to resource's constructor
        ///
        /// \return Reference to the resource
        ///
        template<typename T, typename ... Args>
        static T& getNamedResource(const std::string& name, Args&&... args);

        /// \brief Get an empty resource
        ///
        /// This function will replace the resource if one already exists with the same name.
        ///
        /// \param args Arguments to pass to the resource's constructor
        ///
        /// \return Reference to the resource
        ///
        template<typename T, typename ... Args>
        static T& getEmptyResource(Args&&... args);

        /// \brief Get an existing resource
        ///
        /// This function will not attempt to create the resource if it's not found
        /// or is not of the matching type. Instead a default is returned.
        ///
        /// \param name Name of the resource
        ///
        /// \return Reference to the resource
        ///
        template<typename T>
        static T& getExistingResource(const std::string& name);

        /// \brief Check is a resource exists
        ///
        /// You can pass the resource type as a template argument to compare against it.
        /// If only the name should be checked, T should be Resource.
        ///
        /// \param name Name of the resource
        ///
        /// \return True if the resource exists
        /// 
        template<typename T = Resource>
        static bool resourceExists(const std::string& name);


        /// \brief Copy a resource
        ///
        /// This function requires that the resource has a valid copy constructor.
        /// If the resource is not found or is of the wrong type, the default/error
        /// resource will be returned instead.
        ///
        /// \param name Name of the resource to copy
        /// \param newName Name of the copied resource
        ///
        /// \return Reference to the resource
        ///
        template<typename T>
        static T& copyResource(const std::string& name, const std::string& newName);


        /// \brief Deletes resource from memory
        ///
        /// The resource, if found, will be deleted regardless of the persistence flag.
        ///
        /// \param path Name or path for wanted resource
        ///
        static void unloadResource(const std::string& path);

        /// \brief Deletes all resources from memory
        ///
        /// This will only delete the resources not flagged as persistent.
        ///
        static void unloadResources(const unsigned short persistence = 0xFFFF, const bool descending = true);


        /// \brief Load the contents
        ///
        /// This is for internal use only.
        ///
        static bool loadBase(const json::Value& val);

        /// \brief Save the contents
        ///
        /// This is for internal use only.
        ///
        static bool saveBase(const Subsystem& subsys, json::Value& val, json::Value::AllocatorType& alloc);

    private:

        static ResourceManager* m_instance;                                     ///< Pointer to the single instance

        std::unordered_map<std::string, std::unique_ptr<Resource>> m_resources; ///< Container holding resources

    };

    // Include the template implementation file
    #include <Jopnal/Core/Inl/ResourceManager.inl>
}

#endif

/// \class ResourceManager
/// \ingroup core