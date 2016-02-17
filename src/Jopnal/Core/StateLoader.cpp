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
#include <Jopnal/Precompiled.hpp>

//////////////////////////////////////////////


namespace
{
    using namespace jop;

    const char* const ns_fileVersion = "0.1";
    const char* const ns_sceneField = "scene";
    const char* const ns_versionField = "version";
    const char* const ns_typeField = "type";
    const char* const ns_sharedSceneField = "sharedscene";
    const char* const ns_layerField = "layers";
    const char* const ns_objectField = "objects";
    const char* const ns_dataField = "data";

    enum
    {
        CompID,
        LayerID,
        SceneID,
        SubID,
        CustomID,

        LoadID = 0,
        SaveID
    };
}

namespace jop
{
    StateLoader& StateLoader::getInstance()
    {
        static StateLoader instance;
        return instance;
    }

    //////////////////////////////////////////////

    bool StateLoader::saveState(const std::string& path, const bool scene, const bool sharedScene, const bool subsystems)
    {
        path;
        scene;
        sharedScene;
        subsystems;
        return false;
    }

    //////////////////////////////////////////////

    bool StateLoader::loadState(const std::string& path, const bool scene, const bool sharedScene, const bool subsystems)
    {
        std::string buf;
        if (!FileLoader::read(path + ".jop", buf))
            return false;

        json::Document doc;
        doc.ParseInsitu<0>(&buf[0]);

        if (!json::checkParseError(doc))
            return false;

        // Handle version
        if (!doc.HasMember(ns_versionField))
            JOP_DEBUG_WARNING("State file doesn't have the version defined, assuming latest (" << ns_fileVersion << ")")
        else if (std::strcmp(doc[ns_versionField].GetString(), ns_fileVersion) != 0)
            JOP_DEBUG_WARNING("The state file version doesn't match the current library version. Attempting to load anyway...");

        // Load subsystems?
        if (subsystems)
        {
            // TODO Implement, remember: a complete success is required, otherwise original state must be returned
        }

        std::unique_ptr<Scene> sharedScenePtr;

        // Load the shared scene?
        if (sharedScene)
        {
            if (!doc.HasMember(ns_sharedSceneField) || !doc[ns_sharedSceneField].IsObject() || !Engine::m_engineObject || !loadScene(sharedScenePtr, doc[ns_sharedSceneField], path))
                return false;
        }

        std::unique_ptr<Scene> scenePtr;

        // Load scene?
        if (scene)
        {
            if (!doc.HasMember(ns_sceneField) || !doc[ns_sceneField].IsObject() || !Engine::m_engineObject || !loadScene(scenePtr, doc[ns_sceneField], path))
                return false;
        }

        // Finally assign the pointers
        if (subsystems)
        {

        }
        if (sharedScene)
        {
            if (sharedScenePtr)
                Engine::m_engineObject->m_sharedScene = std::move(sharedScenePtr);
            else
                return false;
        }
        if (scene)
        {
            if (scenePtr)
                Engine::m_engineObject->m_currentScene = std::move(scenePtr);
            else
                return false;
        }

        return true;
    }

    //////////////////////////////////////////////

    bool StateLoader::loadScene(std::unique_ptr<Scene>& scene, const json::Value& data, const std::string& path)
    {
        const auto& sceneCont = std::get<SceneID>(m_loaderSavers);

        // Does the scene object have a type?
        if (data.HasMember(ns_typeField) && data[ns_typeField].IsString())
        {
            auto itr = sceneCont.find(data[ns_typeField].GetString());

            // Has the load function been registered for this scene type?
            if (itr != sceneCont.end())
            {
                json::Value v(json::kObjectType);

                // Attempt to create the scene
                if (std::get<LoadID>(itr->second)(scene, data.HasMember(ns_dataField) && data[ns_dataField].IsObject() ? data[ns_dataField] : v))
                {
                    // Attempt to load layers
                    if (data.HasMember(ns_layerField) && data[ns_layerField].IsArray() && !data[ns_layerField].Empty())
                    {
                        if (!loadLayers(scene, data[ns_layerField], path))
                        {
                            JOP_DEBUG_ERROR("Couldn't load scene state, a layer reported loading failure: " << path);
                            return false;
                        }
                    }

                    // Attempt to load objects
                    if (data.HasMember(ns_objectField) && data[ns_objectField].IsArray() && !data[ns_objectField].Empty())
                    {
                        if (!loadObjects(scene, data[ns_objectField], path))
                        {
                            JOP_DEBUG_ERROR("Couldn't load scene state, an object reported loading failure: " << path);
                            return false;
                        }
                    }
                }
                else
                {
                    JOP_DEBUG_ERROR("Couldn't load scene state, the registered load function reported failure: " << path);
                    return false;
                }
            }
            else
            {
                JOP_DEBUG_ERROR("Couldn't load scene state, scene type (\"" << data[ns_typeField].GetString() << "\") not registered: " << path);
                return false;
            }
        }
        else
        {
            JOP_DEBUG_ERROR("Couldn't load scene state, missing type info: " << path);
            return false;
        }

        return true;
    }

    //////////////////////////////////////////////

    bool StateLoader::loadLayers(std::unique_ptr<Scene>& scene, const json::Value& data, const std::string& path)
    {
        const auto& layerCont = std::get<LayerID>(m_loaderSavers);

        // Load layers
        for (auto& i : data)
        {
            auto& obj = i.value;

            if (!obj.IsObject())
                continue;

            if (obj.HasMember(ns_typeField) && obj[ns_typeField].IsString())
            {
                auto itr = layerCont.find(obj[ns_typeField].GetString());

                if (itr != layerCont.end())
                {
                    std::unique_ptr<Layer> ptr;

                    if (obj.HasMember(ns_dataField) && obj[ns_dataField].IsObject() && std::get<LoadID>(itr->second)(ptr, obj[ns_dataField]))
                        scene->m_layers.push_back(std::shared_ptr<Layer>(ptr.release()));
                    else
                    {
                        JOP_DEBUG_ERROR("Couldn't load layer state, data object missing or the registered load function reported failure: " << path);
                        return false;
                    }
                }
                else
                {
                    JOP_DEBUG_ERROR("Couldn't load layer state, layer type (\"" << obj[ns_typeField].GetString() << "\") not registered: " << path);
                    return false;
                }
            }
            else
            {
                JOP_DEBUG_ERROR("Couldn't create layer, no type specified: " << path);
                return false;
            }
        }

        // Link layers, we can assume all the objects are valid since they went through the previous loop
        const char* const boundLayersField = "boundlayers";

        for (auto& i : data)
        {
            auto& obj = i.value;

            if (!obj.IsObject())
                continue;

            // It can be assumed that the id exists and is of right type
            std::weak_ptr<Layer> weakCurrLayer = scene->getLayer(obj["id"].GetString());

            if (weakCurrLayer.expired())
            {
                JOP_DEBUG_WARNING("Layer with id \"" << obj["id"].GetString() << "\" expired while loading scene: " << path);
                continue;
            }

            if (obj.HasMember(boundLayersField) && obj[boundLayersField].IsArray())
            {
                Layer& currLayer = *weakCurrLayer.lock();

                for (auto& j : obj[boundLayersField])
                {
                    std::weak_ptr<Layer> bound;
                    if (j.value.IsString() && !(bound = scene->getLayer(j.value.GetString())).expired())
                        currLayer.bindOtherLayer(*std::static_pointer_cast<Layer>(bound.lock()));
                    else
                        JOP_DEBUG_WARNING("Couldn't bind layer to \"" << currLayer.getID() << "\". Array element not a string or layer couldn't be found: " << path);
                }
            }
        }

        return true;
    }

    //////////////////////////////////////////////

    bool StateLoader::loadObjects(std::unique_ptr<Scene>& scene, const json::Value& data, const std::string& path)
    {
        for (auto& i : data)
        {
            const auto& val = i.value;

            if (!val.IsObject() || val.Empty())
                continue;

            const char* id = (val.HasMember("id") && val["id"].IsString() ? val["id"].GetString() : "");

            scene->createObject(id);
            if (!loadObject(*scene->m_objects.back(), *scene, val, path))
            {
                JOP_DEBUG_ERROR("Failed to load object with id \"" << id << "\": " << path);
                return false;
            }
        }

        return true;
    }

    //////////////////////////////////////////////

    bool StateLoader::loadObject(Object& obj, const Scene& scene, const json::Value& data, const std::string& path)
    {
        const char* const activeField = "active";
        const char* const componentsField = "components";
        const char* const childrenField = "children";
        const char* const transformField = "transform";

        if (data.HasMember(activeField) && data[activeField].IsBool())
            obj.setActive(data[activeField].GetBool());

        if (data.HasMember(transformField) && data[transformField].IsArray() && data[transformField].Size() >= 10)
        {
            auto& val = data[transformField];

            for (auto& i : val)
            {
                if (!i.value.IsDouble())
                {
                    JOP_DEBUG_WARNING("Encountered unexpected transform value(s) while loading object with id \"" << obj.getID() << "\": " << path);
                    goto SkipTransform;
                }
            }

            obj.setPosition(static_cast<float>(val[0u].GetDouble()),
                            static_cast<float>(val[1u].GetDouble()),
                            static_cast<float>(val[2u].GetDouble()));

            obj.setScale(static_cast<float>(val[3u].GetDouble()),
                         static_cast<float>(val[4u].GetDouble()),
                         static_cast<float>(val[5u].GetDouble()));

            obj.setRotation(glm::quat(static_cast<float>(val[6u].GetDouble()),
                                      static_cast<float>(val[7u].GetDouble()),
                                      static_cast<float>(val[8u].GetDouble()),
                                      static_cast<float>(val[9u].GetDouble())));
        }

        SkipTransform:

        if (data.HasMember(componentsField) && data[componentsField].IsArray())
        {
            auto& compCont = std::get<CompID>(m_loaderSavers);

            for (auto& i : data[componentsField])
            {
                auto& val = i.value;

                if (val.IsObject() && val.HasMember(ns_typeField) && val[ns_typeField].IsString())
                {
                    auto itr = compCont.find(val[ns_typeField].GetString());

                    if (itr != compCont.end())
                    {
                        if (!val.HasMember(ns_dataField) || !val[ns_dataField].IsObject() || std::get<LoadID>(itr->second)(obj, scene, val[ns_dataField]))
                        {
                            JOP_DEBUG_ERROR("Couldn't load component state, registered load function reported failure: " << path);
                            return false;
                        }
                    }
                    else
                    {
                        JOP_DEBUG_ERROR("Couldn't load component state, component type (\"" << val[ns_typeField].GetString() << "\") not registered: " << path);
                        return false;
                    }
                }
                else
                {
                    JOP_DEBUG_ERROR("Failed to load component, no type specified: " << path);
                    return false;
                }
            }
        }

        if (data.HasMember(childrenField) && data[childrenField].IsArray())
        {
            for (auto& i : data[childrenField])
            {
                auto& val = i.value;

                const char* id = (val.HasMember("id") && val["id"].IsString() ? val["id"].GetString() : "");

                obj.createChild(id);
                if (!loadObject(*obj.m_children.back(), scene, val, path))
                {
                    JOP_DEBUG_ERROR("Failed to load child object with id \"" << id << "\": " << path);
                    return false;
                }
            }
        }

        return true;
    }
}

//{
//    "version" : "0.1",
//    "subsystems" : [
//        subsystem datas here
//    ],
//    "sharedScene" : {
//        shared scene data here, same as "scene"
//    },
//    "scene" : {
//        "type" : "jop::Scene",
//        "data" : {
//            this object is passed to the registered scene load function
//        },
//        "layers" : [
//            Layer datas here, first is the default layer
//        ],
//        "objects" : [
//            {
//                "id" : "someObject",
//                transform data
//                active
//                children array
//                component array
//            }
//        ]
//    }
//}