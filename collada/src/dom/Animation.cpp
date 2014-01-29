#include "Animation.hpp"

using namespace crimild;
using namespace crimild::collada;

Animation::Animation( void )
{

}

Animation::~Animation( void )
{

}

bool Animation::parseXML( xmlNode *animation )
{
	Log::Debug << "Parsing animation" << Log::End;

	_sources.parseXML( animation );
	_samplers.parseXML( animation );
	_channels.parseXML( animation );

	_channels.foreach( [&]( Channel *channel ) {
		Sampler *sampler = _samplers.get( channel->getSource() );
		if ( sampler ) {
			Log::Debug << "Linking sampler ' " << sampler->getID() << "' to channel with target '" << channel->getTarget() << "'" << Log::End;
			channel->setSamplerRef( sampler );

			sampler->getInputs()->foreach( [&]( Input *input ) {
				Source *source = _sources.get( input->getSourceID() );
				if ( source ) {
					Log::Debug << "Linking source with id ' " << source->getID() << "' to input with semantic '" << input->getSemantic() << "'" << Log::End;
					input->setSourceRef( source );
				}
				else {
					Log::Error << "Cannot find source matching with '" << channel->getSource() << "'" << Log::End;
				}
			});
		}
		else {
			Log::Error << "Cannot find sampler matching with '" << channel->getSource() << "'" << Log::End;
		}
	});

	return true;
}

