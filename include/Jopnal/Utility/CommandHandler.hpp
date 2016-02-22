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

#ifndef JOP_COMMANDHANDLER_HPP
#define JOP_COMMANDHANDLER_HPP

// Headers
#include <Jopnal/Header.hpp>
#include <Jopnal/Utility/Message.hpp>
#include <unordered_map>
#include <functional>
#include <string>
#include <tuple>

//////////////////////////////////////////////


namespace jop
{
    namespace detail
    {
        JOP_API std::tuple<std::string, std::string> splitFirstArguments(const std::string& args);

        // Include the helper inl file
        #include <Jopnal/Utility/Inl/CommandParser.inl>
    }

    class CommandHandler final
    {
    public:

        /// \brief Bind a new free function using a custom parser
        ///
        /// \param command The command name
        /// \param func The function to bind
        /// \param parser The parser to use
        ///
        template<typename Func, typename Parser>
        void bind(const std::string& command, const Func& func, const Parser& parser);

        /// \brief Bind a new free function
        ///
        /// \param command The command name
        /// \param func The function object to bind
        ///
        template<typename Ret, typename ... FuncArgs>
        void bind(const std::string& command, const std::function<Ret(FuncArgs...)>& func);

        /// \brief Bind a new free function
        ///
        /// \param command The command name
        /// \param func The function pointer to bind
        ///
        template<typename Ret, typename ... FuncArgs>
        void bind(const std::string& command, Ret (*func)(FuncArgs...));


        /// \brief Bind a new member function
        ///
        /// \param command The command name
        /// \param func The function to bind
        /// \param parser The parser to use
        ///
        template<typename Func, typename Parser>
        void bindMember(const std::string& command, const Func& func, const Parser& parser);

        /// \brief Bind a new member function
        /// 
        /// \param command The command name
        /// \param func The function object to bind
        ///
        template<typename Ret, typename Class, typename ... FuncArgs>
        void bindMember(const std::string& command, const std::function<Ret(Class&, FuncArgs...)>& func);

        /// \brief Bind a new member function
        ///
        /// \param command The command name
        /// \param func The function pointer to bind
        ///
        template<typename Ret, typename Class, typename ... FuncArgs>
        void bindMember(const std::string& command, Ret (Class::*func)(FuncArgs...));

        /// \brief Bind a new const member function
        ///
        /// \param command The command name
        /// \param func The function pointer to bind
        /// 
        template<typename Ret, typename Class, typename ... FuncArgs>
        void bindMember(const std::string& command, Ret (Class::*func)(FuncArgs...) const);


        /// \brief Execute a command
        ///
        /// You must use the other overload with the three arguments of you wish to get a return value, regardless
        /// of whether you mean to call a free function or a member.
        ///
        /// \param command The command name
        /// \param instance The class instance to call the command on. Can be nullptr to only consider free functions
        ///
        void execute(const std::string& command, Any& instance);

        /// \brief Execute a command and get the return value
        ///
        /// \param command The command name
        /// \param instance The class instance to call the command on. Can be nullptr to only consider free functions
        /// \param returnWrap PtrWrapper to hold the return value
        ///
        void execute(const std::string& command, Any& instance, Any& returnWrap);


    private:

        std::unordered_map<std::string, std::function<void(const std::string&, Any&, Any&)>> m_memberParsers; ///< Parsers for member functions
        std::unordered_map<std::string, std::function<void(const std::string&, Any&)>> m_funcParsers;         ///< Parsers for free functions

    };

    // Include the template implementation file
    #include <Jopnal/Utility/Inl/CommandHandler.inl>
}

/// \brief Register a new command handler
///
/// This macro must be followed with JOP_END_COMMAND_HANDLER
///
#define JOP_REGISTER_COMMAND_HANDLER(handlerName) jop::CommandHandler& ns_##handlerName##_getCommandHandler(){ \
                                                  static jop::CommandHandler instance; return instance;}       \
                                                  struct ns_##handlerName##_registrar{                         \
                                                  ns_##handlerName##_registrar(){                              \
                                                  jop::CommandHandler& handler = ns_##handlerName##_getCommandHandler();

/// \brief Register a derived command handler
///
/// This doesn't create a new handler, but rather fetches an existing one.
/// This is primarily used to allow binding of derived class' methods to their
/// base class' command handlers. This is the case with jop::Component and jop::Camera,
/// for instance.
///
#define JOP_DERIVED_COMMAND_HANDLER(handlerName, className) extern jop::CommandHandler& ns_##handlerName##_getCommandHandler(); \
                                                            struct ns_##className##_registrar{                                  \
                                                            ns_##className##_registrar(){                                       \
                                                            jop::CommandHandler& handler = ns_##handlerName##_getCommandHandler();

/// \brief End a command handler registration
///
#define JOP_END_COMMAND_HANDLER(className) }}; static ns_##className##_registrar ns_##className##_reg;

/// \brief Bind a member command
///
#define JOP_BIND_MEMBER_COMMAND(function, funcName) handler.bindMember(funcName, function)

/// brief Bind a free function command
///
#define JOP_BIND_COMMAND(function, funcName) handler.bind(funcName, function)

/// \brief Execute a free function command
///
#define JOP_EXECUTE_COMMAND(handlerName, command, instance, returnPtr) ns_##handlerName##_getCommandHandler().execute(command, instance, returnPtr)


#endif

/// \class CommandHandler
/// \ingroup utility
///
/// #TODO Detailed description