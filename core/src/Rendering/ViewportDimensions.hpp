/*
* Copyright (c) 2002 - present, H. Hernan Saez
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the copyright holder nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CRIMILD_RENDERING_VIEWPORT_DIMENSIONS_
#define CRIMILD_RENDERING_VIEWPORT_DIMENSIONS_

#include "Mathematics/Rect.hpp"
#include "Rendering/ScalingMode.hpp"
#include "Foundation/Containers/Array.hpp"

namespace crimild {

    struct ViewportDimensions {

        ScalingMode scalingMode = ScalingMode::SWAPCHAIN_RELATIVE;
        Rectf dimensions = Rectf( 0, 0, 1, 1 );
        Vector2f depthRange = Vector2f( 0.0f, 1.0f );

        static ViewportDimensions viewportFrom( const ViewportDimensions &parent, const ViewportDimensions &child ) noexcept
        {
        	const auto &pd = parent.dimensions;
         	const auto &cd = child.dimensions;
          	return ViewportDimensions {
           		.scalingMode = ScalingMode::RELATIVE,
	         	.dimensions = Rectf(
                    pd.getX() + cd.getX() * pd.getWidth(),
                    pd.getY() + cd.getY() * pd.getHeight(),
                    pd.getWidth() * cd.getWidth(),
                    pd.getHeight() * cd.getHeight()
                ),
            };
        }

        static Array< ViewportDimensions > cubeViewportsFrom( const ViewportDimensions &parent ) noexcept
        {
            return Array< ViewportDimensions > {
                viewportFrom(
                    parent,
                    {
                        .dimensions = Rectf( 0.0f, 0.5f, 0.25f, 0.25f ),
                    }
            	),
                viewportFrom(
                    parent,
                    {
                        .dimensions = Rectf( 0.5f, 0.5f, 0.25f, 0.25f ),
                    }
                ),
                viewportFrom(
                    parent,
                    {
                        .dimensions = Rectf( 0.5f, 0.25f, 0.25f, 0.25f ),
                    }
                ),
                viewportFrom(
                    parent,
                    {
                        .dimensions = Rectf( 0.5f, 0.75f, 0.25f, 0.25f ),
                    }
            	),
                viewportFrom(
                    parent,
                    {
                        .dimensions = Rectf( 0.25f, 0.5f, 0.25f, 0.25f ),
                    }
                ),
                viewportFrom(
                    parent,
                    {
                        .dimensions = Rectf( 0.75f, 0.5f, 0.25f, 0.25f ),
                    }
                ),
            };
        }
    };

}

#endif



