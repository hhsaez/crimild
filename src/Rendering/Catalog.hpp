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

#ifndef CRIMILD_RENDERING_CATALOG_
#define CRIMILD_RENDERING_CATALOG_

#include <memory>
#include <list>
#include <functional>

namespace Crimild {

	class ShaderProgram;

	template< class RESOURCE_TYPE >
	class Catalog {
	public:
		class Resource {
		public:
			Resource( void )
				: _catalog( nullptr ),
				  _catalogId( -1 )
			{
			}

			virtual ~Resource( void )
			{
                unload();
			}

			Catalog< RESOURCE_TYPE > *getCatalog( void ) { return _catalog; }

			int getCatalogId( void ) const { return _catalogId; }

			void setCatalogInfo( Catalog< RESOURCE_TYPE > *catalog, int id )
			{
				_catalog = catalog;
				_catalogId = id;
			}
            
            void unload( void )
            {
				if ( _catalog ) {
					_catalog->unload( static_cast< RESOURCE_TYPE * >( this ) );
					_catalog = nullptr;
				}
            }

		private:
			Catalog< RESOURCE_TYPE > *_catalog;
			int _catalogId;
		};

	public:
		Catalog( void ) 
		{

		}

		virtual ~Catalog( void ) 
		{
			unloadAll();
		}

		bool hasResources( void ) const
		{ 
			return _resources.size() > 0;
		}

		virtual int getNextResourceId( void )
		{
			return _resources.size();
		}

		virtual int getDefaultIdValue( void ) 
		{
			return -1;
		}

		virtual void bind( RESOURCE_TYPE *resource )
		{
			if ( resource->getCatalog() == nullptr ) {
				load( resource );
			}
		}

		virtual void bind( ShaderProgram *program, RESOURCE_TYPE *resource )
		{
			bind( resource );
		}

		virtual void unbind( RESOURCE_TYPE *resource )
		{
		}

		virtual void unbind( ShaderProgram *program, RESOURCE_TYPE *resource )
		{
			unbind( resource );
		}

		virtual void load( RESOURCE_TYPE *resource )
		{
			_resources.push_back( resource );
			resource->setCatalogInfo( this, getNextResourceId() );
		}

		virtual void unload( RESOURCE_TYPE *resource )
		{
			_resources.remove( resource );
			resource->setCatalogInfo( nullptr, getDefaultIdValue() );
		}

		virtual void unloadAll( void )
		{
			for ( auto resource : _resources ) {
				resource->setCatalogInfo( nullptr, getDefaultIdValue() );
			}
			_resources.clear();
		}

	private:
		std::list< RESOURCE_TYPE * > _resources;
	};

}

#endif

