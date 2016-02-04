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
    std::string ns_projectName;

    int ns_argc;
    char** ns_argv;
}

namespace jop
{
    Engine::Engine(const std::string& name, int argc, char* argv[])
        : m_sharedScene     (std::make_unique<Scene>("SharedScene")),
          m_subsystems      (),
          m_currentScene    (),
          m_running         (true)
    {
        JOP_ASSERT(m_engineObject == nullptr, "Only one jop::Engine object may exist at a time!");
        JOP_ASSERT(!name.empty(), "Project name mustn't be empty!");

        ns_projectName = name;
        m_engineObject = this;

        ns_argc = argc;
        ns_argv = argv;
    }

    Engine::~Engine()
    {
        m_currentScene.reset();
        m_sharedScene.reset();
        m_subsystems.clear();

        ns_projectName = std::string();
        m_engineObject = nullptr;
    }

    //////////////////////////////////////////////

    void Engine::loadDefaultConfiguration()
    {
        // Setting manager
        createSubsystem<SettingManager>();

        // File loader
        createSubsystem<FileLoader>(ns_argv[0]);

        // Resource manager
        createSubsystem<ResourceManager>();

        // Main window
        const Window::Settings wSettings(true);
        createSubsystem<Window>(wSettings);
    }

    //////////////////////////////////////////////

    int Engine::runMainLoop()
    {
        if (!m_currentScene)
            JOP_DEBUG_WARNING("No scene was loaded before entering main loop. Only the shared scene will be used.");

        const double timeStep = 1.0 / SettingManager::getUint("uFixedUpdateFrequency", 30);
        float64 accumulator = 0.0;

        Clock frameClock;

        while (m_running)
        {
            float64 frameTime = frameClock.reset().asSeconds();

            // Clamp the delta time to a certain value. This is to prevent
            // a "spiral of death" if fps goes below 10.
            if (frameTime > 0.1)
                frameTime = 0.1;

            // Fixed update
            {
                accumulator += frameTime;

                while (accumulator >= timeStep)
                {
                    for (auto& i : m_subsystems)
                        i->preFixedUpdate(timeStep);

                    if (m_currentScene)
                        m_currentScene->fixedUpdateBase(timeStep);

                    m_sharedScene->fixedUpdateBase(timeStep);

                    for (auto& i : m_subsystems)
                        i->postFixedUpdate(timeStep);

                    accumulator -= timeStep;
                }
            }

            // Update
            {
                for (auto& i : m_subsystems)
                    i->preUpdate(frameTime);

                if (m_currentScene)
                    m_currentScene->updateBase(frameTime);

                m_sharedScene->updateBase(frameTime);

                for (auto& i : m_subsystems)
                    i->postUpdate(frameTime);
            }
            
            // Draw
            {
                if (m_currentScene)
                    m_currentScene->drawBase();

                m_sharedScene->drawBase();

                for (auto& i : m_subsystems)
                    i->draw();
            }
        }

        // #TODO Threaded event loop

        return EXIT_SUCCESS;
    }

    //////////////////////////////////////////////

    Scene& Engine::getCurrentScene()
    {
        JOP_ASSERT(m_engineObject != nullptr && m_engineObject->m_currentScene, "Tried to get the current scene when it or the engine wasn't loaded!");
        return *m_engineObject->m_currentScene;
    }

    //////////////////////////////////////////////

    Subsystem* Engine::getSubsystem(const std::string& ID)
    {
        if (m_engineObject)
        {
            for (auto& i : m_engineObject->m_subsystems)
            {
                if (i->getID() == ID)
                    return i.get();
            }
        }

        return nullptr;
    }

    //////////////////////////////////////////////

    bool Engine::removeSubsystem(const std::string& ID)
    {
        if (m_engineObject)
        {
            for (auto itr = m_engineObject->m_subsystems.begin(); itr != m_engineObject->m_subsystems.end(); ++itr)
            {
                if ((*itr)->getID() == ID)
                {
                    m_engineObject->m_subsystems.erase(itr);
                    return true;
                }
            }
        }

        return false;
    }

    //////////////////////////////////////////////

    bool Engine::isRunning()
    {
        return m_engineObject && m_engineObject->m_running;
    }

    //////////////////////////////////////////////

    void Engine::exit()
    {
        if (m_engineObject)
            m_engineObject->m_running = false;
    }

    //////////////////////////////////////////////

    MessageResult Engine::sendMessage(const std::string& message, Any returnWrap)
    {
        const Message msg(message, returnWrap);
        return sendMessage(msg);
    }

    //////////////////////////////////////////////

    MessageResult Engine::sendMessage(const Message& message)
    {
        if (m_engineObject)
        {
            //if (message.passFilter(Message::Engine) && message.passFilter(Message::Command))
            //    JOP_EXECUTE_MEMBER_COMMAND(Engine, message.getString(), ns_engineObject, message.getReturnPointer());

            static const unsigned short sceneField = Message::SharedScene |
                                                     Message::Scene |
                                                     Message::Layer |
                                                     Message::Object |
                                                     Message::Component;

            if (message.passFilter(sceneField) && m_engineObject->m_sharedScene->sendMessage(message) == MessageResult::Escape)
                return MessageResult::Escape;

            auto& s = m_engineObject->m_currentScene;

            if (s && message.passFilter(sceneField) && s->sendMessage(message) == MessageResult::Escape)
                return MessageResult::Escape;

            if (message.passFilter(Message::Subsystem))
            {
                for (auto& i : m_engineObject->m_subsystems)
                {
                    if (message.passFilter(i->getID()) && i->sendMessage(message) == MessageResult::Escape)
                        return MessageResult::Escape;
                }
            }
        }

        return MessageResult::Escape;
    }

    //////////////////////////////////////////////

    Scene& Engine::getSharedScene()
    {
        JOP_ASSERT(isRunning(), "There must be a valid jop::Engine object in order to call jop::Engine::getSharedScene()!");
        return *m_engineObject->m_sharedScene;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////

    Engine* Engine::m_engineObject = nullptr;

    //////////////////////////////////////////////

    const std::string& getProjectName()
    {
        return ns_projectName;
    }

    //////////////////////////////////////////////

    MessageResult broadcast(const std::string& message, Any returnWrap)
    {
        const Message msg(message, returnWrap);
        return Engine::sendMessage(msg);
    }

    MessageResult broadcast(const Message& message)
    {
        return Engine::sendMessage(message);
    }
}