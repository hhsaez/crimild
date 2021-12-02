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

#include "MaterialComponent.hpp"

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"

using namespace crimild;

MaterialComponent::~MaterialComponent( void )
{
    detachAllMaterials();
}

void MaterialComponent::attachMaterial( SharedPointer< Material > const &material )
{
    _materials.add( material );
}

void MaterialComponent::detachAllMaterials( void )
{
    _materials.clear();
}

void MaterialComponent::forEachMaterial( std::function< void( Material * ) > callback )
{
    _materials.each( [ callback ]( SharedPointer< Material > &m ) {
        callback( crimild::get_ptr( m ) );
    } );
}

SharedPointer< NodeComponent > MaterialComponent::clone( void )
{
    auto other = crimild::alloc< MaterialComponent >();
    forEachMaterial( [ other ]( Material *material ) {
        other->attachMaterial( material );
    } );
    return other;
}

void MaterialComponent::encode( coding::Encoder &encoder )
{
    NodeComponent::encode( encoder );

    Array< SharedPointer< Material > > ms;
    forEachMaterial( [ &ms ]( Material *m ) {
        ms.add( crimild::retain( m ) );
    } );
    encoder.encode( "materials", ms );
}

void MaterialComponent::decode( coding::Decoder &decoder )
{
    NodeComponent::decode( decoder );

    Array< SharedPointer< Material > > ms;
    decoder.decode( "materials", ms );
    ms.each( [ this ]( SharedPointer< Material > &m ) {
        attachMaterial( m );
    } );
}
