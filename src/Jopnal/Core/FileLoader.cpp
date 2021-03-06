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

    #include <Jopnal/Core/FileLoader.hpp>

    #include <Jopnal/Core/Engine.hpp>
    #include <Jopnal/Core/DebugHandler.hpp>
    #include <physfs.h>

    #ifdef JOP_OS_ANDROID
        #include <Jopnal/Core/Android/ActivityState.hpp>
        #include <android/asset_manager.h>
    #endif

#endif

//////////////////////////////////////////////


namespace
{
    const char* const ns_resourceDir = "Resources";
    bool ns_errorChecksEnabled = true;
    std::mutex ns_mutex;

    void checkError(const std::string& info)
    {
        std::lock_guard<std::mutex> lock(ns_mutex);

        if (ns_errorChecksEnabled)
        {
            const char* error = PHYSFS_getLastError();

            if (error)
                JOP_DEBUG_ERROR("Filesystem error: " << error << " (" << info << ")");

        #if JOP_CONSOLE_VERBOSITY < 0
            info;
        #endif
        }
    }

    void createNeededDirs()
    {
        const std::string prefDir = jop::FileLoader::getDirectory(jop::FileLoader::Directory::User);
        const std::string resDir = (PHYSFS_getBaseDir() + std::string(ns_resourceDir));

        if (!PHYSFS_setWriteDir(prefDir.c_str()) || !PHYSFS_mkdir("Saves") || !PHYSFS_mkdir("Log") || !PHYSFS_mkdir("Config"))
        {
            checkError("Create user dir");
            JOP_ASSERT(false, "Failed to create user directory!");
        }
        
    #ifndef JOP_OS_ANDROID

        if (!PHYSFS_setWriteDir(PHYSFS_getBaseDir()) || !PHYSFS_mkdir(ns_resourceDir))
        {
            checkError("Create resource dir");
            JOP_ASSERT(false, "Failed to create resource directory!");
        }

        if (!PHYSFS_mount(ns_resourceDir, NULL, false) || !PHYSFS_mount(resDir.c_str(), NULL, false))
        {
            checkError("Mount resource directory");
            JOP_ASSERT(false, "Failed to mount resource directory!");
        }

    #endif

        if (!PHYSFS_mount(jop::FileLoader::getDirectory(jop::FileLoader::Directory::User).c_str(), NULL, true))
        {
            checkError("Mount user directory");
            JOP_ASSERT(false, "Failed to mount user directory!");
        }
    }
}

namespace jop
{
    namespace detail
    {
        FileSystemInitializer::FileSystemInitializer(const char* arg)
            : Subsystem(0)
        {
            JOP_ASSERT(!PHYSFS_isInit(), "You can only have a single jop::FileSystemInitializer sub system instance!");

            JOP_ASSERT_EVAL(PHYSFS_init(arg) != 0, "Failed to initialize file system!");
            createNeededDirs();

            DebugHandler::getInstance().openFileHandles();
        }

        FileSystemInitializer::~FileSystemInitializer()
        {
            DebugHandler::getInstance().closeFileHandles();

            if (!PHYSFS_deinit())
                checkError("Filesystem deinit");
        }
    }


    //////////////////////////////////////////////


    FileLoader::FileLoader()
        : m_file    (nullptr),
          m_isAsset (false)
    {}

    FileLoader::FileLoader(const std::string& path)
        : m_file    (nullptr),
          m_isAsset (false)
    {
        open(path);
    }

    FileLoader::FileLoader(const Directory dir, const std::string& path, const bool append)
        : m_file    (nullptr),
          m_isAsset (false)
    {
        open(dir, path, append);
    }

    FileLoader::FileLoader(FileLoader&& other)
        : m_file    (other.m_file),
          m_isAsset (other.m_isAsset)
    {
        other.m_file = nullptr;
        other.m_isAsset = false;
    }

    FileLoader& FileLoader::operator =(FileLoader&& other)
    {
        m_file              = other.m_file;
        m_isAsset           = other.m_isAsset;
        other.m_file        = nullptr;
        other.m_isAsset     = false;

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
        #ifdef JOP_OS_ANDROID

            m_asset = AAssetManager_open(detail::ActivityState::get()->nativeActivity->assetManager, path.c_str(), AASSET_MODE_STREAMING);

            return (m_isAsset = (m_asset != nullptr)) == true;

        #else

            checkError(path);
            return false;

        #endif
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
        #ifdef JOP_OS_ANDROID

            if (m_isAsset)
            {
                AAsset_close(m_asset);
                m_isAsset = false;

                return;
            }

        #endif

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
        {
        #ifdef JOP_OS_ANDROID

            if (m_isAsset)
                return static_cast<int64>(AAsset_read(m_asset, data, static_cast<size_t>(size)));

        #endif

            return PHYSFS_readBytes(m_file, data, size);
        }

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
        {
        #ifdef JOP_OS_ANDROID

            if (m_isAsset)
                return AAsset_seek64(m_asset, position, SEEK_SET) != -1;

        #endif

            return PHYSFS_seek(m_file, position) != 0;
        }

        return false;
    }

    //////////////////////////////////////////////

    int64 FileLoader::tell() const
    {
        if (isValid())
        {
        #ifdef JOP_OS_ANDROID

            if (m_isAsset)
                return AAsset_getLength64(m_asset) - AAsset_getRemainingLength64(m_asset);

        #endif

            return PHYSFS_tell(m_file);
        }

        return -1;
    }

    //////////////////////////////////////////////

    int64 FileLoader::getSize() const
    {
        if (isValid())
        {
        #ifdef JOP_OS_ANDROID

            if (m_isAsset)
                return AAsset_getLength64(m_asset);

        #endif

            return PHYSFS_fileLength(m_file);
        }

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
    #ifdef JOP_OS_ANDROID

        if (AAsset* asset = AAssetManager_open(detail::ActivityState::get()->nativeActivity->assetManager, path.c_str(), AASSET_MODE_STREAMING))
        {
            AAsset_close(asset);
            return true;
        }
        else

    #endif

            return PHYSFS_exists(path.c_str()) != 0;
    }

    //////////////////////////////////////////////

    void FileLoader::listFiles(const std::string& path, std::vector<std::string>& list)
    {
        PHYSFS_enumerateFilesCallback(path.c_str(), [](void* list, const char* dir, const char* file)
        {
            std::string fileString((std::string(dir).empty() ? "" : (std::string(dir) + "/")) + std::string(file));

            PHYSFS_Stat stat;
            PHYSFS_stat(fileString.c_str(), &stat);

            if (stat.filetype != PHYSFS_FILETYPE_DIRECTORY)
                static_cast<std::vector<std::string>*>(list)->emplace_back(std::move(fileString));

        }, &list);

    #ifdef JOP_OS_ANDROID

        AAssetManager* am = detail::ActivityState::get()->nativeActivity->assetManager;
        AAssetDir* dir = AAssetManager_openDir(am, path.c_str());

        while (const char* fileName = AAssetDir_getNextFileName(dir))
            list.emplace_back(path + "/" + fileName);

        AAssetDir_close(dir);

    #endif
    }

    //////////////////////////////////////////////

    void FileLoader::listFilesRecursive(const std::string& path, std::vector<std::string>& list)
    {
        PHYSFS_enumerateFilesCallback(path.c_str(), [](void* list, const char* dir, const char* file)
        {
            std::string fileString(std::string(dir) + "/" + std::string(file));

            PHYSFS_Stat stat;
            PHYSFS_stat(fileString.c_str(), &stat);

            if (stat.filetype != PHYSFS_FILETYPE_DIRECTORY)
                static_cast<std::vector<std::string>*>(list)->emplace_back(std::move(fileString));
            else
                listFilesRecursive(fileString, *static_cast<std::vector<std::string>*>(list));

        }, &list);

    #ifdef JOP_OS_ANDROID

        AAssetManager* am = detail::ActivityState::get()->nativeActivity->assetManager;
        AAssetDir* dir = AAssetManager_openDir(am, path.c_str());

        while (const char* fileName = AAssetDir_getNextFileName(dir))
            list.emplace_back(path + "/" + fileName);

        AAssetDir_close(dir);

    #endif
    }

    //////////////////////////////////////////////

    bool FileLoader::deleteFile(const Directory dir, const std::string& file)
    {
        if (!PHYSFS_setWriteDir(getDirectory(dir).c_str()) || PHYSFS_delete(file.c_str()) == 0)
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

    bool FileLoader::makeDirectory(const Directory dir, const std::string& path)
    {
        if (!PHYSFS_setWriteDir(getDirectory(dir).c_str()) || !PHYSFS_mkdir(path.c_str()))
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
        #ifdef JOP_OS_ANDROID

            std::string(detail::ActivityState::get()->nativeActivity->internalDataPath) + "/",
            std::string(detail::ActivityState::get()->nativeActivity->externalDataPath) + "/",
            std::string(detail::ActivityState::get()->nativeActivity->internalDataPath) + "/"

        #else

            std::string(PHYSFS_getBaseDir()),
            std::string(std::string(PHYSFS_getBaseDir()) + ns_resourceDir),
            std::string(PHYSFS_getPrefDir("Jopnal", getProjectName().c_str()))

        #endif
        };

        return dirArr[static_cast<int>(dir)];
    }

    //////////////////////////////////////////////

    char FileLoader::getDirectorySeparator()
    {
        return PHYSFS_getDirSeparator()[0];
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