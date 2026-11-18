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

#ifndef CRIMILD_CORE_FOUNDATION_POLICIES_CACHE_
#define CRIMILD_CORE_FOUNDATION_POLICIES_CACHE_

namespace crimild {

    /**
     	\brief Define how objects should be cached and keep alive.
     */
    enum class CachePolicy {
        /**
             \brief No caching policy

             Resource will not be cached
         */
        NONE,

        /**
            \brief Temporal caching policy

             The resource will be cached by a short amount of time. It will be
             removed from cache at some point in the near future.
             For example, when loading a new scene, we may want to keep some
             resources cached (i.e. images, materials, meshes, etc) so they can
             be reused by different objects. Once the scene is loaded, there's no
             point in keeping them in the cache.
         */
        TRANSIENT,

        /**
             \brief Scene caching policy

             Resources will be kept in cache as long as the scene does not changes.
             This is useful when we know we'll need some resources later when
             executing the scene. For example, we may spawn new objects reusing
             some of the existing resources.
         */
        SCENE,

        /**
             \brief Persistent caching policy

             Resources will be kept alive for the duration of the program. Useful for
             shared resources like shaders or materials that will be used several times
             by different scenes.
         */
        PERSISTENT,
    };

}

#endif


