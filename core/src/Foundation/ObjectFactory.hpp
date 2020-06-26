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

#ifndef CRIMILD_FOUNDATION_OBJECT_FACTORY_
#define CRIMILD_FOUNDATION_OBJECT_FACTORY_

#include "RTTI.hpp"
#include "Singleton.hpp"
#include "Containers/Map.hpp"
#include "Memory.hpp"
#include "SharedObject.hpp"

namespace crimild {

	class ObjectFactory : public StaticSingleton< ObjectFactory > {
	public:
		using Builder = std::function< SharedPointer< SharedObject >() >;

	public:
		ObjectFactory( void );
		~ObjectFactory( void );

		template< typename T >
		void registerBuilder( std::string type )
		{
			_builders[ type ] = []() -> SharedPointer< SharedObject > {
				return crimild::alloc< T >();
			};
		}

		void registerCustomBuilder( std::string type, Builder builder )
		{
			_builders[ type ] = builder;
		}

		inline Builder getBuilder( std::string type )
		{
			if ( !_builders.contains( type ) ) {
				return nullptr;
			}
			
			return _builders[ type ];
		}
        
        SharedPointer< SharedObject > build( std::string className );
        
        template< typename T >
        SharedPointer< T > build( std::string className )
        {
            return crimild::cast_ptr< T >( build( className ) );
        }

	private:
        Map< std::string, Builder > _builders;
	};

}

#define CRIMILD_REGISTER_OBJECT_BUILDER( X ) \
		crimild::ObjectFactory::getInstance()->registerBuilder< X >( #X );

#define CRIMILD_REGISTER_CUSTOM_OBJECT_BUILDER( X, BUILDER_FUNC ) \
		crimild::ObjectFactory::getInstance()->registerCustomBuilder( #X, BUILDER_FUNC );
        
#endif

