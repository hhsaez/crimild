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

#ifndef CRIMILD_OPENGL_
#define CRIMILD_OPENGL_

#include "Rendering/OpenGLRenderer.hpp"
#include "Rendering/OpenGLUtils.hpp"

#include "Rendering/Catalogs/ShaderProgramCatalog.hpp"
#include "Rendering/Catalogs/VertexBufferObjectCatalog.hpp"
#include "Rendering/Catalogs/IndexBufferObjectCatalog.hpp"
#include "Rendering/Catalogs/FrameBufferObjectCatalog.hpp"
#include "Rendering/Catalogs/TextureCatalog.hpp"

#include "Rendering/Programs/StandardShaderProgram.hpp"
#include "Rendering/Programs/LitTextureShaderProgram.hpp"
#include "Rendering/Programs/UnlitTextureShaderProgram.hpp"
#include "Rendering/Programs/UnlitDiffuseShaderProgram.hpp"
#include "Rendering/Programs/ScreenTextureShaderProgram.hpp"
#include "Rendering/Programs/SignedDistanceFieldShaderProgram.hpp"
#include "Rendering/Programs/TextShaderProgram.hpp"
#include "Rendering/Programs/DepthShaderProgram.hpp"
#include "Rendering/Programs/ColorTintShaderProgram.hpp"

#ifdef CRIMILD_PLATFORM_DESKTOP

#include "Rendering/ImageEffects/DepthOfFieldImageEffect.hpp"
#include "Rendering/ImageEffects/BloomImageEffect.hpp"
#include "Rendering/ImageEffects/SSAOImageEffect.hpp"
#include "Rendering/ImageEffects/VignetteImageEffect.hpp"
#include "Rendering/ImageEffects/BlurImageEffect.hpp"

#endif

#endif

