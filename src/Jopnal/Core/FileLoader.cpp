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
#ifdef JOP_OS_WINDOWS

    HINSTANCE ns_resourceDll = nullptr;

#endif

    const char* const ns_resourceDir = "Resources";
    bool ns_errorChecksEnabled = true;
    std::mutex ns_mutex;

    void checkError(const std::string& info)
    {
        std::lock_guard<std::mutex> lock(ns_mutex);

        if (ns_errorChecksEnabled)
        {
            const char* error = PHYSFS_getLastError();

            if (error && jop::SettingManager::checkInit())
                JOP_DEBUG_ERROR("Filesystem error: " << error << " (" << info << ")");
        }
    }

    bool createNeededDirs()
    {
        const std::string prefDir = jop::FileLoader::getDirectory(jop::FileLoader::Directory::User);

        if (!PHYSFS_setWriteDir(prefDir.c_str()) || !PHYSFS_mkdir("Saves") || !PHYSFS_mkdir("Log"))
        {
            checkError("Create user dir");
            return false;
        }

        if (!PHYSFS_setWriteDir(PHYSFS_getBaseDir()) || !PHYSFS_mkdir(ns_resourceDir))
        {
            checkError("Create resource dir");
            return false;
        }

        if (!PHYSFS_mount(ns_resourceDir, NULL, true) || !PHYSFS_mount(jop::FileLoader::getDirectory(jop::FileLoader::Directory::User).c_str(), NULL, true))
        {
            checkError("Mount directories");
            return false;
        }

        return true;
    }
}

namespace jop
{
    std::unique_ptr<Assimp::Importer> ns_importer;

    //////////////////////////////////////////////

    FileSystemInitializer::FileSystemInitializer(const char* arg)
        : Subsystem("filesysteminitializer")
    {
        JOP_ASSERT(!PHYSFS_isInit(), "You can only have a single jop::FileSystemInitializer sub system instance!");

        JOP_ASSERT_EVAL(PHYSFS_init(arg) != 0, "Failed to initialize file system!");
        JOP_ASSERT_EVAL(createNeededDirs(), "Failed to create user directory!");

        checkError("Init");
        
        struct Logger : Assimp::Logger
        {
            Logger()
                : Assimp::Logger(Assimp::Logger::LogSeverity::NORMAL)
            {}

            bool attachStream(Assimp::LogStream*, unsigned int)
            {
                return true;
            }
            bool detatchStream(Assimp::LogStream*, unsigned int)
            {
                return true;
            }
            void OnDebug(const char* message) override
            {
                JOP_DEBUG_INFO(message);
            }
            void OnInfo(const char* message) override
            {
                JOP_DEBUG_INFO(message);
            }
            void OnWarn(const char* message) override
            {
                JOP_DEBUG_WARNING(message);
            }
            void OnError(const char* message) override
            {
                JOP_DEBUG_ERROR(message);
            }
        };
        Assimp::DefaultLogger::set(new Logger);
    }

    FileSystemInitializer::~FileSystemInitializer()
    {
        ns_importer.reset();
        Assimp::DefaultLogger::kill();

        if (!PHYSFS_deinit())
            checkError("Filesystem deinit");
    }

    Assimp::Importer& FileSystemInitializer::getImporter()
    {
        if (!ns_importer)
        {
            ns_importer = std::make_unique<Assimp::Importer>();

            struct Streamer : Assimp::IOSystem
            {
                struct Stream : Assimp::IOStream
                {
                private:

                    FileLoader* m_loader;

                public:

                    explicit Stream(FileLoader& loader)
                        : m_loader(&loader)
                    {}

                    size_t Read(void* pvBuffer, size_t pSize, size_t pCount) override
                    {
                        return static_cast<size_t>(m_loader->read(pvBuffer, pSize * pCount));
                    }
                    size_t Write(const void* pvBuffer, size_t pSize, size_t pCount) override
                    {
                        return static_cast<size_t>(m_loader->write(pvBuffer, pSize * pCount));
                    }
                    aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override
                    {
                        return static_cast<aiReturn>(aiReturn_FAILURE - m_loader->seek(pOrigin + pOffset));
                    }
                    size_t Tell() const override
                    {
                        return static_cast<size_t>(m_loader->tell());
                    }
                    size_t FileSize() const override
                    {
                        return static_cast<size_t>(m_loader->getSize());
                    }
                    void Flush() override
                    {
                        m_loader->flush();
                    }
                };

                FileLoader m_loader;

                bool Exists(const char* pFile) const override
                {
                    return FileLoader::fileExists(pFile);
                }
                char getOsSeparator() const override
                {
                    return FileLoader::getDirectorySeparator();
                }

                Assimp::IOStream* Open(const char* pFile, const char* pMode) override
                {
                    if (std::string(pMode).find('r') != std::string::npos)
                        m_loader.open(pFile);

                    else if (std::string(pMode).find('w') != std::string::npos)
                        m_loader.open(FileLoader::Directory::Resource, pFile, false);

                    return new Stream(m_loader);
                }

                void Close(Assimp::IOStream* stream) override
                {
                    delete stream;
                    m_loader.close();
                }
            };

            ns_importer->SetIOHandler(new Streamer);
        }

        return *ns_importer;
    }

    //////////////////////////////////////////////


    FileLoader::FileLoader()
        : m_file(nullptr)
    {}

    FileLoader::FileLoader(const std::string& path)
        : m_file(nullptr)
    {
        open(path);
    }

    FileLoader::FileLoader(const Directory dir, const std::string& path, const bool append)
        : m_file(nullptr)
    {
        open(dir, path, append);
    }

    FileLoader::FileLoader(FileLoader&& other)
        : m_file(other.m_file)
    {
        other.m_file = nullptr;
    }

    FileLoader& FileLoader::operator =(FileLoader&& other)
    {
        m_file = other.m_file;
        other.m_file = nullptr;

        return *this;
    }

    FileLoader::~FileLoader()
    {
        close();
    }

    //////////////////////////////////////////////

    bool FileLoader::open(const std::string& path)
    {
        close();

        if (path.empty())
            return false;

        m_file = PHYSFS_openRead(path.c_str());

        if (!isValid())
        {
            checkError(path);
            return false;
        }

        return true;
    }

    //////////////////////////////////////////////

    bool FileLoader::open(const Directory dir, const std::string& path, const bool append)
    {
        close();

        if (path.empty())
            return false;

        if (!path.empty())
        {
            if (!PHYSFS_setWriteDir(getDirectory(dir).c_str()))
            {
                checkError("Set write dir");
                return false;
            }

            m_file = append ? PHYSFS_openAppend(path.c_str()) : PHYSFS_openWrite(path.c_str());

            if (isValid())
                return true;
            else
                checkError(path);
        }

        return false;
    }

    //////////////////////////////////////////////

    void FileLoader::flush()
    {
        if (isValid())
        {
            if (PHYSFS_flush(m_file) == 0)
                checkError("File flush");
        }
    }

    //////////////////////////////////////////////

    void FileLoader::close()
    {
        if (isValid())
        {
            if (PHYSFS_close(m_file) == 0)
                checkError("File close");
            else
                m_file = nullptr;
        }
    }

    //////////////////////////////////////////////

    bool FileLoader::isValid() const
    {
        return m_file != nullptr;
    }

    //////////////////////////////////////////////

    int64 FileLoader::read(void* data, const uint64 size)
    {
        if (isValid() && size)
            return PHYSFS_readBytes(m_file, data, size);

        return -1;
    }

    //////////////////////////////////////////////

    int64 FileLoader::write(const void* data, const uint64 size)
    {
        if (isValid() && size && data)
            return PHYSFS_writeBytes(m_file, data, size);

        return -1;
    }

    //////////////////////////////////////////////

    bool FileLoader::seek(const uint64 position)
    {
        if (isValid())
            return PHYSFS_seek(m_file, position) != 0;

        return false;
    }

    //////////////////////////////////////////////

    int64 FileLoader::tell()
    {
        if (isValid())
            return PHYSFS_tell(m_file);

        return -1;
    }

    //////////////////////////////////////////////

    int64 FileLoader::getSize()
    {
        if (isValid())
            return PHYSFS_fileLength(m_file);

        return -1;
    }

    //////////////////////////////////////////////

    FileLoader::operator bool() const
    {
        return isValid();
    }

    //////////////////////////////////////////////

    bool FileLoader::fileExists(const std::string& path)
    {
        return PHYSFS_exists(path.c_str()) != 0;
    }

    //////////////////////////////////////////////

    void FileLoader::listFiles(const std::string& path, std::vector<std::string>& list)
    {
        PHYSFS_enumerateFilesCallback(path.c_str(), [](void* list, const char* dir, const char* file)
        {
            std::string fileString((std::string(dir).empty() ? "" : (std::string(dir) + "/")) + std::string(file));

            if (!PHYSFS_isDirectory(fileString.c_str()))
                static_cast<std::vector<std::string>*>(list)->emplace_back(std::move(fileString));

        }, &list);
    }

    //////////////////////////////////////////////

    void FileLoader::listFilesRecursive(const std::string& path, std::vector<std::string>& list)
    {
        PHYSFS_enumerateFilesCallback(path.c_str(), [](void* list, const char* dir, const char* file)
        {
            std::string fileString(std::string(dir) + "/" + std::string(file));

            if (!PHYSFS_isDirectory(fileString.c_str()))
                static_cast<std::vector<std::string>*>(list)->emplace_back(std::move(fileString));
            else
                listFilesRecursive(fileString, *static_cast<std::vector<std::string>*>(list));

        }, &list);
    }

    //////////////////////////////////////////////

    bool FileLoader::deleteFile(const std::string& file)
    {
        if (PHYSFS_delete(file.c_str()) == 0)
        {
            checkError(file);
            return false;
        }

        return true;
    }

    //////////////////////////////////////////////

    bool FileLoader::readTextfile(const std::string& path, std::string& file)
    {
        FileLoader fs(path);

        if (fs)
        {
            const uint32 size = static_cast<uint32>(fs.getSize());

            if (size > 0)
            {
                std::vector<char> buffer(size + 1);
                fs.read(buffer.data(), size);
                buffer.back() = '\0';
                file = buffer.data();

                return true;
            }
        }

        checkError(path);

        return false;
    }

    //////////////////////////////////////////////

    bool FileLoader::readBinaryfile(const std::string& path, std::vector<uint8>& buffer)
    {
        FileLoader fs(path);

        if (fs)
        {
            const uint32 size = static_cast<uint32>(fs.getSize());

            if (size > 0)
            {
                buffer.resize(size);
                fs.read(buffer.data(), size);

                return true;
            }
        }

        checkError(path);

        return false;
    }

    //////////////////////////////////////////////

    bool FileLoader::writeTextfile(const Directory dir, const std::string& path, const std::string& text, const bool append)
    {
        return writeBinaryfile(dir, path, text.data(), text.size(), append);
    }

    //////////////////////////////////////////////

    bool FileLoader::writeBinaryfile(const Directory dir, const std::string& path, const void* data, const std::size_t bytes, const bool append)
    {
        FileLoader fs(dir, path, append);

        if (fs)
        {
            if (fs.write(data, bytes) < bytes)
            {
                checkError(path);
                return false;
            }

            return true;
        }

        return false;
    }

    //////////////////////////////////////////////

    bool FileLoader::makeDirectory(const std::string& path)
    {
        if (PHYSFS_mkdir(path.c_str()) == 0)
        {
            checkError(path);
            return false;
        }

        return true;
    }

    //////////////////////////////////////////////

    const std::string& FileLoader::getDirectory(const Directory dir)
    {
        static const std::string dirArr[] =
        {
            std::string(PHYSFS_getBaseDir()),
            std::string(std::string(PHYSFS_getBaseDir()) + ns_resourceDir),
            std::string(PHYSFS_getPrefDir("Jopnal", getProjectName().c_str()))
        };

        return dirArr[static_cast<int>(dir)];
    }

    //////////////////////////////////////////////

    char FileLoader::getDirectorySeparator()
    {
        return PHYSFS_getDirSeparator()[0];
    }

    //////////////////////////////////////////////

    bool FileLoader::readResource(const int id, std::vector<uint8>& buffer)
    {
        static std::mutex mutex;
        std::lock_guard<std::mutex> lock(mutex);

        if (!ns_resourceDll)
        {
            ns_resourceDll = LoadLibrary("Jopnal Resources.dll");
            JOP_ASSERT(ns_resourceDll != nullptr, "Failed to load the Jopnal resource dll!");
        }

        bool success = false;

        auto handle = FindResource(ns_resourceDll, MAKEINTRESOURCE(id), RT_RCDATA);

        if (handle)
        {
            auto res = LoadResource(ns_resourceDll, handle);

            if (res)
            {
                buffer.clear();

                auto size = SizeofResource(ns_resourceDll, handle);
                auto locked = LockResource(res);

                if (locked && size)
                {
                    buffer.resize(size);
                    std::memcpy(buffer.data(), locked, size);
                    success = true;
                }

                UnlockResource(locked);

                FreeResource(res);
            }
        }

        return success;
    }

    //////////////////////////////////////////////

    void FileLoader::enableErrorChecks(const bool enable)
    {
        std::lock_guard<std::mutex> lock(ns_mutex);

        ns_errorChecksEnabled = enable;
    }

    //////////////////////////////////////////////

    bool FileLoader::errorChecksEnabled()
    {
        std::lock_guard<std::mutex> lock(ns_mutex);

        return ns_errorChecksEnabled;
    }
}