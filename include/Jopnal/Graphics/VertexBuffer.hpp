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

#ifndef JOP_VERTEXBUFFER_HPP
#define JOP_VERTEXBUFFER_HPP

// Headers
#include <Jopnal/Jopnal.hpp>
#include <Jopnal\Graphics\Buffer.hpp>

//////////////////////////////////////////////

namespace jop
{

    class VertexBuffer : public Buffer
    {
    public:

        ///\brief default constructor 
        ///
        ///\param type Buffer type 
        ///
        VertexBuffer(BufferType type);

        ///\brief default deconstructor
        ///
        ~VertexBuffer();

        ///\brief constructor to another vertex buffer
        ///
        ///\param other reference to vertex buffer constructor 
        ///
        VertexBuffer(const VertexBuffer& other);

        ///\brief operator to overload reference to vertex buffer
        ///
        ///\param other reference to vertex buffer constructor
        ///
        VertexBuffer& operator =(const VertexBuffer& other);

        ///\brief move constructor 
        ///
        ///\param other 
        ///
        VertexBuffer(VertexBuffer&& other);

        ///\brief operator to overload move constructor
        ///
        ///\param other 
        ///
        VertexBuffer& operator =(VertexBuffer&& other);

        ///\brief method to set data by bytes
        ///
        ///\param data const void pointer to data
        ///\param bytes unsigned int 
        ///
        void setData(const void* data, std::size_t bytes);

        ///\brief method to set sub data by size and and set offset 
        ///
        ///\param data const void pointer to data
        ///\param offset unsigned int 
        ///\param size unsigned int
        ///
        void setSubData(const void* data, std::size_t offset, std::size_t size);

    private:
    };
}
#endif

/// \class VertexBuffer
/// \ingroup Graphics
///
/// Do explanation about the class