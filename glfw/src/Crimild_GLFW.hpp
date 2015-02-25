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

#ifndef CRIMILD_GLFW_
#define CRIMILD_GLFW_

#include "Rendering/GL3/IndexBufferObjectCatalog.hpp"
#include "Rendering/GL3/Renderer.hpp"
#include "Rendering/GL3/OffscreenRenderPass.hpp"
#include "Rendering/GL3/ShaderProgramCatalog.hpp"
#include "Rendering/GL3/TextureCatalog.hpp"
#include "Rendering/GL3/Utils.hpp"
#include "Rendering/GL3/VertexBufferObjectCatalog.hpp"

#include "Rendering/GL3/Programs/ParticleSystemShaderProgram.hpp"
#include "Rendering/GL3/Programs/SignedDistanceFieldShaderProgram.hpp"
#include "Rendering/GL3/Programs/SkinningShaderProgram.hpp"

#include "Rendering/GL3/ImageEffects/DepthOfFieldImageEffect.hpp"
#include "Rendering/GL3/ImageEffects/SSAOImageEffect.hpp"
#include "Rendering/GL3/ImageEffects/EmissiveGlowImageEffect.hpp"
#include "Rendering/GL3/ImageEffects/SepiaToneImageEffect.hpp"
#include "Rendering/GL3/ImageEffects/BloomImageEffect.hpp"

#include "Rendering/GL3/Library/FlatMaterial.hpp"
#include "Rendering/GL3/Library/FlatShaderProgram.hpp"
#include "Rendering/GL3/Library/GouraudMaterial.hpp"
#include "Rendering/GL3/Library/GouraudShaderProgram.hpp"
#include "Rendering/GL3/Library/PhongMaterial.hpp"
#include "Rendering/GL3/Library/PhongShaderProgram.hpp"
#include "Rendering/GL3/Library/ScreenShaderProgram.hpp"
#include "Rendering/GL3/Library/SepiaToneShaderProgram.hpp"

#include "SceneGraph/ParticleSystem.hpp"

#include "Simulation/GLSimulation.hpp"

#include "Simulation/Tasks/LoadSceneTask.hpp"
#include "Simulation/Tasks/UpdateInputStateTask.hpp"

#endif

