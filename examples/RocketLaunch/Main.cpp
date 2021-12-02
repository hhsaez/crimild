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

#include <Crimild.hpp>
#include <Crimild_SDL.hpp>
#include <Crimild_Scripting.hpp>

using namespace crimild;
using namespace crimild::sdl;

namespace crimild {

	namespace demos {

		class RocketController : public NodeComponent {
			CRIMILD_IMPLEMENT_RTTI( crimild::demos::RocketController )
			
		public:
			RocketController( void ) { }
			
			RocketController( scripting::ScriptEvaluator &eval )
			{
				eval.getPropValue( "countdown", _countdown );
				eval.getPropValue( "ignition", _ignition );
				eval.getPropValue( "launch", _launch );
				eval.getPropValue( "speed", _speed );
			}

			virtual ~RocketController( void )
			{

			}

			virtual void update( const Clock &c ) override
			{
				auto group = getNode< Group >();

				_timer += c;
				
				group->getNodeAt( 1 )->setEnabled( false );
				group->getNodeAt( 2 )->setEnabled( false );
				group->getNodeAt( 3 )->setEnabled( false );
				group->getNodeAt( 4 )->setEnabled( false );
				
				if ( _timer.getAccumTime() >= _countdown ) {
					group->getNodeAt( 1 )->setEnabled( true );
				}

				if ( _timer.getAccumTime() >= _ignition ) {
					group->getNodeAt( 2 )->setEnabled( true );
				}

				if ( _timer.getAccumTime() >= _launch ) {
					group->local().translate() += _speed * c.getDeltaTime() * Vector3f::UNIT_Y;

					group->getNodeAt( 3 )->setEnabled( true );
					group->getNodeAt( 4 )->setEnabled( true );
				}
			}

			virtual void decode( crimild::coding::Decoder &decoder ) override
			{
				NodeComponent::decode( decoder );

				decoder.decode( "countdown", _countdown );
				decoder.decode( "ignition", _ignition );
				decoder.decode( "launch", _launch );
				decoder.decode( "speed", _speed );
			}

		private:
			crimild::Real32 _countdown = 10.0f;
			crimild::Real32 _ignition = 15.0;
			crimild::Real32 _launch = 20.0f;
			crimild::Real32 _speed = 2.0f;
			Clock _timer;
		};

		class CaptionsController : public NodeComponent {
			CRIMILD_IMPLEMENT_RTTI( crimild::demos::CaptionsController )
			
		public:
			CaptionsController( void )
			{
				_lines.add( Line { 1.0, 1.75, "10" } );
				_lines.add( Line { 2.0, 2.75, "9" } );
				_lines.add( Line { 3.0, 3.75, "8" } );
				_lines.add( Line { 4.0, 4.75, "7" } );
				_lines.add( Line { 5.0, 5.75, "6" } );
				_lines.add( Line { 6.0, 6.75, "5" } );
				_lines.add( Line { 7.0, 7.75, "4" } );
				_lines.add( Line { 8.0, 8.75, "3" } );
				_lines.add( Line { 9.0, 9.75, "2" } );
				_lines.add( Line { 10.0, 10.75, "1" } );
				_lines.add( Line { 11.0, 11.75, "LAUNCH" } );
			}
			
			CaptionsController( scripting::ScriptEvaluator &eval )
			{
				eval.foreach( "lines", [ this ]( scripting::ScriptEvaluator &lEval, int ) {
					crimild::Real32 from;
					lEval.getPropValue( "from", from );
					
					crimild::Real32 to;
					lEval.getPropValue( "to", to );
					
					std::string text;
					lEval.getPropValue( "text", text );

					_lines.add( Line { from, to, text } );
				});
			}

			virtual ~CaptionsController( void )
			{

			}

			virtual void update( const Clock &c ) override
			{
				_timer += c;

				std::string text;

				_lines.foreach( [ this, &text ]( Line &l, int ) {
					if ( l.from <= _timer.getAccumTime() && l.to >= _timer.getAccumTime() ) {
						text = l.text;
					}
				});

				setText( text );
			}

		private:
			struct Line {
				crimild::Real32 from;
				crimild::Real32 to;
				std::string text;
			};
			
			virtual void setText( std::string textStr )
			{
				auto text = getNode< Text >();

				if ( textStr == text->getText() ) {
					return;
				}

				text->setText( textStr );
			}

		private:
			Clock _timer;
			Array< Line > _lines;
		};

		class TextProfilerOutputHandler : public NodeComponent {
			CRIMILD_IMPLEMENT_RTTI( crimild::demos::TextProfilerOutputHandler )

		private:
			class HandlerImpl : public ProfilerOutputHandler {
			public:
				HandlerImpl( Text *node ) : _node( node ) { }
				virtual ~HandlerImpl( void ) { }

				virtual void beginOutput( crimild::Size fps, crimild::Real64 avgFrameTime, crimild::Real64 minFrameTime, crimild::Real64 maxFrameTime ) override
				{
					_line.str( "" );

					_line << std::setiosflags( std::ios::fixed )
						  << std::setprecision( 3 )
						  << "FPS: " << std::setw( 10 ) << fps
						  << "MIN: " << std::setw( 10 ) << minFrameTime
						  << "AVG: " << std::setw( 10 ) << avgFrameTime
						  << "MAX: " << std::setw( 10 ) << maxFrameTime
						  << "\n--------------------------------------------------------------------------------------------"
						  << "\n";
					
					_line << std::setiosflags( std::ios::fixed )
						  << std::setprecision( 3 )
						  << std::setw( 15 ) << std::right << "MIN | "
						  << std::setw( 15 ) << std::right << "AVG | "
						  << std::setw( 15 ) << std::right << "MAX | "
						  << std::setw( 15 ) << std::right << "TIME | "
						  << std::setw( 15 ) << std::right << "COUNT | "
						  << std::left << "NAME"
						  << "\n--------------------------------------------------------------------------------------------"
						  << "\n";
				}
				
				virtual void sample( float minPc, float avgPc, float maxPc, unsigned int totalTime, unsigned int callCount, std::string name, unsigned int parentCount ) override
				{
					std::stringstream spaces;
					for ( int i = 0; i < parentCount; i++ ) {
						spaces << " ";
					}
					
					_line << std::setiosflags( std::ios::fixed | std::ios::showpoint )
						  << std::setprecision( 3 )
						  << std::setw( 12 ) << std::right << minPc << " | "
						  << std::setw( 12 ) << std::right << avgPc << " | "
						  << std::setw( 12 ) << std::right << maxPc << " | "
						  << std::setw( 12 ) << std::right << totalTime << " | "
						  << std::setw( 12 ) << std::right << callCount << " | "
						  << std::left << spaces.str() << name
						  << "\n";
				}
				
				virtual void endOutput( void ) override
				{
					_line << "\n";
					
					_node->setText( _line.str() );
				}
				
			private:
				Text *_node = nullptr;
				std::stringstream _line;
			};

			
		public:
			TextProfilerOutputHandler( void )
			{
				
			}
			
			TextProfilerOutputHandler( scripting::ScriptEvaluator &eval )
			{
				
			}
			
			virtual ~TextProfilerOutputHandler( void )
			{
				
			}

			virtual void onAttach( void ) override
			{
				Profiler::getInstance()->setOutputHandler( crimild::alloc< HandlerImpl >( getNode< Text >() ) );
			}

			virtual void onDetach( void ) override
			{
				//Profiler::getInstance()->setOutputHandler( nullptr );
			}

		};
		
	}

}

SharedPointer< Node > room( void )
{
	OBJLoader loader( FileSystem::getInstance().pathForResource( "assets/models/room.obj" ) );
	auto model = loader.load();
	return model;
}

SharedPointer< Group > loadModel( std::string filename )
{
	SharedPointer< Group > model;
	auto modelPath = FileSystem::getInstance().pathForResource( filename );
	FileStream is( modelPath, FileStream::OpenMode::READ );
	is.load();
	if ( is.getObjectCount() > 0 ) {
		model = is.getObjectAt< Group >( 0 );
	}
	
	return model;
}

int main( int argc, char **argv )
{
	crimild::init();

	CRIMILD_REGISTER_OBJECT_BUILDER( crimild::demos::RocketController )
	CRIMILD_REGISTER_OBJECT_BUILDER( crimild::demos::CaptionsController )
	CRIMILD_REGISTER_OBJECT_BUILDER( crimild::demos::TextProfilerOutputHandler )
	
    auto sim = crimild::alloc< SDLSimulation >(
		"Rocket Launch",
		crimild::alloc< Settings >( argc, argv ) );

	//Profiler::getInstance()->setOutputHandler( crimild::alloc< ProfilerConsoleOutputHandler >() );

	sim->getRenderer()->getScreenBuffer()->setClearColor( RGBAColorf( 0.5f, 0.55f, 1.0f, 1.0f ) );

	sim->loadScene( FileSystem::getInstance().pathForResource( "assets/scenes/main.lua" ) );
	
	return sim->run();
}

