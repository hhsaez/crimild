/*
 * Copyright (c) 2013-2018, Hernan Saez
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

#ifndef CRIMILD_RENDERING_SHADER_GRAPH_NODE_
#define CRIMILD_RENDERING_SHADER_GRAPH_NODE_

#include "Foundation/NamedObject.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Foundation/Containers/Array.hpp"
#include "Coding/Codable.hpp"

#include "Outlet.hpp"

namespace crimild {

	namespace shadergraph {

		class Node : public coding::Codable {
		private:
			using OutletMap = containers::Map< std::string, SharedPointer< Outlet >>;
			using OutletArray = containers::Array< SharedPointer< Outlet >>;
			using OutletArrayCallback = std::function< void( Outlet * ) >;
			
		protected:
			Node( void );

		public:
			virtual ~Node( void );

		public:
			Outlet *addInputOutlet( std::string name, Outlet::Type type );
			void setInputOutlet( Outlet *outlet ) { setInputOutlet( crimild::retain( outlet ) ); }
			void setInputOutlet( SharedPointer< Outlet > const &outlet );
			Outlet *getInputOutlet( std::string name );
			void eachInputOutlet( OutletArrayCallback const &callback );

			Outlet *addOutputOutlet( std::string name, Outlet::Type type );
			void setOutputOutlet( Outlet *outlet ) { setOutputOutlet( crimild::retain( outlet ) ); }
			void setOutputOutlet( SharedPointer< Outlet > const &outlet );			
			Outlet *getOutputOutlet( std::string name );
			void eachOutputOutlet( OutletArrayCallback const &callback );

		private:
			OutletMap _inputs;
			OutletMap _outputs;
		};

	}

}

#endif

