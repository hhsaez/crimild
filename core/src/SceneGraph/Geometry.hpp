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
 *     * Neither the name of the copyright holders nor the
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

#ifndef CRIMILD_SCENE_GRAPH_GEOMETRY_
#define CRIMILD_SCENE_GRAPH_GEOMETRY_

#include "Crimild_Foundation.hpp"
#include "Node.hpp"

#include <functional>

namespace crimild {

    class DescriptorSet;
    class Primitive;

    class Geometry : public Node {
        CRIMILD_IMPLEMENT_RTTI( crimild::Geometry )

    public:
        explicit Geometry( std::string name = "" );
        virtual ~Geometry( void );

        bool hasPrimitives( void ) const { return _primitives.size(); }

        void attachPrimitive( Primitive *primitive );
        void attachPrimitive( SharedPointer< Primitive > const &primitive );

        void detachPrimitive( Primitive *primitive );
        void detachPrimitive( SharedPointer< Primitive > const &primitive );

        void detachAllPrimitives( void );

        Primitive *anyPrimitive( void ) noexcept { return crimild::get_ptr( _primitives.first() ); }

        void forEachPrimitive( std::function< void( Primitive * ) > callback );

        void updateModelBounds( void );

    private:
        Array< SharedPointer< Primitive > > _primitives;

    public:
        virtual void accept( NodeVisitor &visitor ) override;

        /**
                   \name Instancing
                 */
        //@{

    public:
        void setInstancingEnabled( crimild::Bool enabled ) { _instancingEnabled = enabled; }
        crimild::Bool isInstancingEnabled( void ) const { return _instancingEnabled; }

    private:
        crimild::Bool _instancingEnabled = false;

        //@}

        /**
                   \name Descriptors
                */
        //@{

    public:
        DescriptorSet *getDescriptors( void ) noexcept;

    private:
        SharedPointer< DescriptorSet > m_descriptors;

        //@}

        /**
            \name Coding
         */
        //@{

    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;

        //@}
    };

    using GeometryPtr = SharedPointer< Geometry >;

}

#endif
