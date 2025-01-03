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

#ifndef CRIMILD_RENDERING_SWAPCHAIN_
#define CRIMILD_RENDERING_SWAPCHAIN_

#include "Crimild_Foundation.hpp"
#include "Rendering/Image.hpp"
#include "Rendering/ImageView.hpp"

namespace crimild {

    /**
     * \brief A queue of rendered images waiting to be presented to the surface
     *
     * \todo This should not be a singleton, since we might have multiple swapchains (but only one per surface).
     */
    class Swapchain : public DynamicSingleton< Swapchain > {
    private:
        using ImageArray = Array< SharedPointer< Image > >;
        using ImageViewArray = Array< SharedPointer< ImageView > >;

    public:
        virtual ~Swapchain( void ) noexcept = default;

        [[nodiscard]] inline const Extent2D &getExtent( void ) const noexcept { return m_extent; }
        [[nodiscard]] inline const Format &getFormat( void ) const noexcept { return m_format; }

        inline ImageArray &getImages( void ) noexcept { return m_images; }
        inline ImageViewArray &getImageViews( void ) noexcept { return m_imageViews; };

    protected:
        inline void setExtent( const Extent2D &extent ) noexcept
        {
            m_extent = extent;
            // Scaling mode is always fixed for the swapchain
            m_extent.scalingMode = { ScalingMode::FIXED };
        }

        inline void setFormat( const Format &format ) noexcept
        {
            m_format = format;
        }

    private:
        Extent2D m_extent;
        Format m_format = Format::UNDEFINED;
        ImageArray m_images;
        ImageViewArray m_imageViews;
    };

}

#endif
