/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_
#define CRIMILD_

#include "Mathematics/Distance.hpp"
#include "Mathematics/Frustum.hpp"
#include "Mathematics/Interpolation.hpp"
#include "Mathematics/Intersection.hpp"
#include "Mathematics/Matrix.hpp"
#include "Mathematics/Numeric.hpp"
#include "Mathematics/Plane.hpp"
#include "Mathematics/Quaternion.hpp"
#include "Mathematics/Ray.hpp"
#include "Mathematics/Rect.hpp"
#include "Mathematics/Root.hpp"
#include "Mathematics/Sphere.hpp"
#include "Mathematics/Time.hpp"
#include "Mathematics/Transformation.hpp"
#include "Mathematics/Vector.hpp"

#include "Foundation/Macros.hpp"
#include "Foundation/NamedObject.hpp"
#include "Foundation/Log.hpp"

#include "Exceptions/Exception.hpp"
#include "Exceptions/FileNotFoundException.hpp"
#include "Exceptions/RuntimeException.hpp"
#include "Exceptions/HasParentException.hpp"

#include "SceneGraph/CameraNode.hpp"
#include "SceneGraph/GeometryNode.hpp"
#include "SceneGraph/GroupNode.hpp"
#include "SceneGraph/Node.hpp"
#include "SceneGraph/SwitchNode.hpp"

#include "Components/CameraComponent.hpp"
#include "Components/MaterialComponent.hpp"
#include "Components/NodeComponent.hpp"
#include "Components/RotationComponent.hpp"

#include "Visitors/FetchCameras.hpp"
#include "Visitors/NodeVisitor.hpp"
#include "Visitors/ComputeVisibilitySet.hpp"
#include "Visitors/SelectNodes.hpp"
#include "Visitors/UpdateComponents.hpp"
#include "Visitors/UpdateWorldState.hpp"

#include "Primitives/Primitive.hpp"
#include "Primitives/ParametricPrimitive.hpp"
#include "Primitives/ConePrimitive.hpp"
#include "Primitives/KleinBottlePrimitive.hpp"
#include "Primitives/MobiusStripPrimitive.hpp"
#include "Primitives/NewellTeapotPrimitive.hpp"
#include "Primitives/QuadPrimitive.hpp"
#include "Primitives/SpherePrimitive.hpp"
#include "Primitives/TorusPrimitive.hpp"
#include "Primitives/TrefoilKnotPrimitive.hpp"

#include "Rendering/BufferObject.hpp"
#include "Rendering/Camera.hpp"
#include "Rendering/Catalog.hpp"
#include "Rendering/FrameBufferObject.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/ImageTGA.hpp"
#include "Rendering/IndexBufferObject.hpp"
#include "Rendering/Material.hpp"
#include "Rendering/Renderer.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/ShaderLocation.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/VertexBufferObject.hpp"
#include "Rendering/VertexFormat.hpp"
#include "Rendering/VisibilitySet.hpp"

#include "Simulation/InputState.hpp"
#include "Simulation/Simulation.hpp"
#include "Simulation/FileSystem.hpp"
#include "Simulation/RunLoop.hpp"
#include "Simulation/Task.hpp"
#include "Simulation/Tasks/BeginRenderTask.hpp"
#include "Simulation/Tasks/EndRenderTask.hpp"
#include "Simulation/Tasks/RenderSceneTask.hpp"
#include "Simulation/Tasks/UpdateSceneTask.hpp"

#endif

