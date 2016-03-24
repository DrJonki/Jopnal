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
#include <Jopnal/Graphics/Camera.hpp>
#include <Jopnal/Graphics/Drawable.hpp>
#include <Jopnal/Graphics/Color.hpp>
#include <Jopnal/Graphics/Font.hpp>
#include <Jopnal/Graphics/Transform.hpp>
#include <Jopnal/Graphics/CubemapDepth.hpp>
#include <Jopnal/Graphics/Texture2D.hpp>
#include <Jopnal/Graphics/Texture2DDepth.hpp>
#include <Jopnal/Graphics/RenderTexture.hpp>
#include <Jopnal/Graphics/TextureSampler.hpp>
#include <Jopnal/Graphics/Shader.hpp>
#include <Jopnal/Graphics/Vertex.hpp>
#include <Jopnal/Graphics/Buffer.hpp>
#include <Jopnal/Graphics/VertexBuffer.hpp>
#include <Jopnal/Graphics/LightSource.hpp>
#include <Jopnal/Graphics/Mesh.hpp>
#include <Jopnal/Graphics/BoxMesh.hpp>
#include <Jopnal/Graphics/SphereMesh.hpp>
#include <Jopnal/Graphics/GenericDrawable.hpp>
#include <Jopnal/Graphics/GlState.hpp>
#include <Jopnal/Graphics/Material.hpp>
#include <Jopnal/Graphics/Model.hpp>
#include <Jopnal/Graphics/RectangleMesh.hpp>
#include <Jopnal/Graphics/ShaderManager.hpp>
#include <Jopnal/Graphics/Text.hpp>
#include <Jopnal/Graphics/Renderer.hpp>
#include <Jopnal/Graphics/Cubemap.hpp>
#include <Jopnal/Graphics/EnvironmentRecorder.hpp>

//////////////////////////////////////////////

/// \defgroup graphics Graphics
///
/// #TODO Detailed decription