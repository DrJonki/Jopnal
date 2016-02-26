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


namespace jop
{
    JOP_REGISTER_LOADABLE(jop, GenericDrawable)[](Object& obj, const Scene& scene, const json::Value& val) -> bool
    {
        return Drawable::loadStateBase(obj.createComponent<GenericDrawable>(""), scene, val);
    }
    JOP_END_LOADABLE_REGISTRATION(GenericDrawable)

    JOP_REGISTER_SAVEABLE(jop, GenericDrawable)[](const Component& comp, json::Value& val, json::Value::AllocatorType& alloc) -> bool
    {
        return Drawable::saveStateBase(static_cast<const GenericDrawable&>(comp), val, alloc);
    }
    JOP_END_SAVEABLE_REGISTRATION(GenericDrawable)
}

namespace jop
{
    GenericDrawable::GenericDrawable(Object& object, const std::string& ID)
        : Drawable(object, ID)
    {}

    //////////////////////////////////////////////

    GenericDrawable* GenericDrawable::clone() const
    {
        return new GenericDrawable(*this);
    }

    //////////////////////////////////////////////

    void GenericDrawable::draw(const Camera& camera)
    {
        if (getShader().expired() || getModel().getMesh().expired())
            return;

        auto& s = *getShader();
        auto& mod = getModel();
        auto& msh = *mod.getMesh();

        msh.getVertexBuffer().bind();

        s.setUniform("u_PVMMatrix", camera.getProjectionMatrix() * camera.getViewMatrix() * getObject().getMatrix());
        s.setAttribute(0, gl::FLOAT, 3, sizeof(Vertex), false, (void*)Vertex::Position);

        mod.getMaterial().sendToShader(s);
        s.setAttribute(1, gl::FLOAT, 2, sizeof(Vertex), false, (void*)Vertex::TexCoords);

        msh.getIndexBuffer().bind();
        glCheck(gl::DrawElements(gl::TRIANGLES, mod.getElementAmount(), gl::UNSIGNED_INT, (void*)0));
    }
}