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

#ifndef CRIMILD_CORE_COMPONENTS_CATALOG_
#define CRIMILD_CORE_COMPONENTS_CATALOG_

#include <list>

namespace crimild {

	template< class NODE_COMPONENT_TYPE >
	class NodeComponentCatalog {
	private:
		NodeComponentCatalog( void ) { };
		
		virtual ~NodeComponentCatalog( void ) { };

	public:
		static NodeComponentCatalog &getInstance( void )
		{
			static NodeComponentCatalog instance;
			return instance;
		}

	public:
        void registerComponent( NODE_COMPONENT_TYPE *component )
		{
			_components.push_back( component );
		}

		void unregisterComponent( NODE_COMPONENT_TYPE *component )
		{
            _components.remove( component );
		}

		bool isEmpty( void ) const { return _components.size() == 0; }

		NODE_COMPONENT_TYPE *first( void )
        {
            return !isEmpty() ? _components.front() : nullptr;
        }

		void forEach( std::function< void( NODE_COMPONENT_TYPE * ) > callback )
		{
            auto cs = _components;
			for ( auto component : cs ) {
				callback( component );
			}
		}

	private:
        std::list< NODE_COMPONENT_TYPE * > _components;
	};

}

#endif

