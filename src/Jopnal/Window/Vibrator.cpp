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

#include <Jopnal/Window/Vibrator.hpp>

#include <Jopnal/Core/DebugHandler.hpp>
#include <Jopnal/Core/Android/ActivityState.hpp>
#include <Jopnal/Utility/Thread.hpp>

//////////////////////////////////////////////

    
namespace jop
{
    void Vibrator::vibrate(const unsigned int time)
    {
    #ifdef JOP_OS_ANDROID

        auto env = Thread::getCurrentJavaEnv();

        if (!env)
        {
            JOP_DEBUG_ERROR("No current java environment, function \"" << __func__ << "\"");
            return;
        }

        // Class information
        jclass nativeAct = env->FindClass("android/app/NativeActivity");
        jclass context = env->FindClass("android/content/Context");

        // Parameters for getSystemService
        jfieldID fieldID = env->GetStaticFieldID(context, "VIBRATOR_SERVICE", "Ljava/lang/String;");
        jobject serviceStr = env->GetStaticObjectField(context, fieldID);

        // getSystemService
        jmethodID getSysServ = env->GetMethodID(nativeAct, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
        jobject vibrator = env->CallObjectMethod(detail::ActivityState::get()->nativeActivity->clazz, getSysServ, serviceStr);

        // Get class & method
        jclass vibratorClass = env->GetObjectClass(vibrator);
        jmethodID vibrate = env->GetMethodID(vibratorClass, "vibrate", "(J)V");

        // Vibrate!
        jlong jtime = static_cast<jlong>(std::min(static_cast<unsigned int>(INT_MAX), time));
        env->CallVoidMethod(vibrator, vibrate, jtime);

        // Free references
        env->DeleteLocalRef(nativeAct);
        env->DeleteLocalRef(context);
        env->DeleteLocalRef(serviceStr);
        env->DeleteLocalRef(vibrator);
        env->DeleteLocalRef(vibratorClass);

    #else

        time;

    #endif
    }

    void Vibrator::vibrate()
    {
        vibrate(INT_MAX);
    }

    void Vibrator::stop()
    {
    #ifdef JOP_OS_ANDROID

        auto env = Thread::getCurrentJavaEnv();

        if (!env)
        {
            JOP_DEBUG_ERROR("No current java environment, function \"" << __func__ << "\"");
            return;
        }

        // Class information
        jclass nativeAct = env->FindClass("android/app/NativeActivity");
        jclass context = env->FindClass("android/content/Context");

        // Parameters for getSystemService
        jfieldID fieldID = env->GetStaticFieldID(context, "VIBRATOR_SERVICE", "Ljava/lang/String;");
        jobject serviceStr = env->GetStaticObjectField(context, fieldID);

        // getSystemService
        jmethodID getSysServ = env->GetMethodID(nativeAct, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;");
        jobject vibrator = env->CallObjectMethod(detail::ActivityState::get()->nativeActivity->clazz, getSysServ, serviceStr);

        // Get class & method
        jclass vibratorClass = env->GetObjectClass(vibrator);
        jmethodID stop = env->GetMethodID(vibratorClass, "cancel", "()V");

        // Stop vibrating
        env->CallVoidMethod(vibrator, stop);

        // Free references
        env->DeleteLocalRef(nativeAct);
        env->DeleteLocalRef(context);
        env->DeleteLocalRef(serviceStr);
        env->DeleteLocalRef(vibrator);
        env->DeleteLocalRef(vibratorClass);

    #endif
    }
}