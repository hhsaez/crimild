/*
 * Copyright (c) 2002 - present, Hernan Saez
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

#include "Foundation/SharedObject.hpp"
#include "Foundation/Types.hpp"

#include <list>
#include <functional>
#include <memory>

namespace crimild {

	class ShaderProgram;
	class ShaderLocation;

	/*
	  \brief Usage hint
	  \todo Move to policy?
	*/
	enum class ResourceUsage {
		Static, //< Data is modified once and never changed
		Dynamic //< Data can be modified multiple times
	};

	template< class RESOURCE_TYPE >
	class Catalog : public SharedObject {
	public:

		/**
			RESOURCE_TYPE must be derived from SharedObject

            \deprecated
		*/
		class Resource : public NonCopyable {
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
				if ( _catalog != nullptr ) {
                    _catalog->unload( static_cast< RESOURCE_TYPE * >( this ) );
                    _catalog = nullptr;
				}
            }

            virtual void onBind( void ) { }

            virtual void onUnbind( void ) { }

		private:
            Catalog< RESOURCE_TYPE > *_catalog = nullptr;
			int _catalogId;

			/**
			   \name Usage
			*/
			//@{

		public:
			inline void setUsage( ResourceUsage value ) { m_usage = value; }
			inline ResourceUsage getUsage( void ) const { return m_usage; }

		private:
			ResourceUsage m_usage = ResourceUsage::Static;

			//@}
		};

	public:
		Catalog( void )
		{

		}

		virtual ~Catalog( void )
		{
			unloadAll();
            cleanup();
		}

		crimild::Bool hasResources( void ) const
		{
			return _resources.size() > 0;
		}

		inline crimild::Size getResourceCount( void ) const
		{
			return _resources.size();
		}

		inline crimild::Size getActiveResourceCount( void ) const
		{
			return _activeResourceCount;
		}

		virtual int getNextResourceId( RESOURCE_TYPE *resource )
		{
			return _resources.size();
		}

		virtual int getDefaultIdValue( void )
		{
			return -1;
		}

        virtual void bind( RESOURCE_TYPE *resource )
		{
            bindResource( resource );
		}

        virtual void bind( ShaderProgram *program, RESOURCE_TYPE *resource )
		{
			bindResource( resource );
		}

        virtual void bind( ShaderLocation *location, RESOURCE_TYPE *resource )
		{
			bindResource( resource );
		}

        virtual void unbind( RESOURCE_TYPE *resource )
		{
            unbindResource( resource );
		}

        virtual void unbind( ShaderProgram *program, RESOURCE_TYPE *resource )
		{
			unbindResource( resource );
		}

        virtual void unbind( ShaderLocation *location, RESOURCE_TYPE *resource )
		{
			unbindResource( resource );
		}

		virtual void update( RESOURCE_TYPE *resource )
		{

		}

        virtual void load( RESOURCE_TYPE *resource )
		{
            /*
			resource->setCatalogInfo( this, getNextResourceId( resource ) );
			_resources.push_back( resource );
            */
		}

        virtual void unload( RESOURCE_TYPE *resource )
		{
            /*
			resource->setCatalogInfo( nullptr, getDefaultIdValue() );
            _resources.remove( resource );
            */
		}

		virtual void unloadAll( void )
		{
            /*
            auto rs = _resources;
			for ( auto resource : rs ) {
                unload( resource );
			}

            cleanup();

            _resources.clear();
            */
		}

        virtual void cleanup( void )
        {
            // no-op
        }

        virtual void configure( void )
        {
            // no-op
        }

    private:
        void bindResource( RESOURCE_TYPE *resource )
        {
            /*
            if ( resource->getCatalog() == nullptr ) {
                load( resource );
            }

            resource->onBind();

            _activeResourceCount++;
            */
        }

        void unbindResource( RESOURCE_TYPE *resource )
        {
            /*
            if ( resource != nullptr ) {
                resource->onUnbind();
                if ( _activeResourceCount > 0 ) {
                    _activeResourceCount--;
                }
            }
            */
        }

	private:
        std::list< RESOURCE_TYPE * > _resources;
        std::list< int > _cleanupList;
        crimild::Size _activeResourceCount = 0;
	};

}

#endif
