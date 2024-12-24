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

#include "SkinnedMesh.hpp"

#include "Crimild_Coding.hpp"
#include "Crimild_Foundation.hpp"

using namespace crimild;

#if 0

SkinnedMeshJoint::SkinnedMeshJoint( void )
{

}

SkinnedMeshJoint::SkinnedMeshJoint( unsigned int id, const Transformation &offset, std::string name )
	: NamedObject( name ),
	  _id( id ),
	  _offset( offset )
{

}

SkinnedMeshJoint::~SkinnedMeshJoint( void )
{

}

void SkinnedMeshJoint::encode( coding::Encoder &encoder )
{
	Codable::encode( encoder );

	encoder.encode( "name", getName() );
	encoder.encode( "id", _id );
	encoder.encode( "offset", _offset );
}

void SkinnedMeshJoint::decode( coding::Decoder &decoder )
{
	Codable::decode( decoder );

	std::string name;
	decoder.decode( "name", name );
	setName( name );

	decoder.decode( "id", _id );
	decoder.decode( "offset", _offset );
}

bool SkinnedMeshJoint::registerInStream( Stream &s )
{
	if ( !StreamObject::registerInStream( s ) ) {
		return false;
	}

	return true;
}

void SkinnedMeshJoint::save( Stream &s )
{
	StreamObject::save( s );

	s.write( getName() );
	s.write( _id );
	s.write( _offset );
}

void SkinnedMeshJoint::load( Stream &s )
{
	StreamObject::load( s );

	std::string name;
	s.read( name );
	setName( name );

	s.read( _id );
	s.read( _offset );
}

SkinnedMeshJointCatalog::SkinnedMeshJointCatalog( void )
{

}

SkinnedMeshJointCatalog::~SkinnedMeshJointCatalog( void )
{

}

SkinnedMeshJoint *SkinnedMeshJointCatalog::find( std::string name )
{
    /*
	if ( !_joints.find( name ) ) {
		return nullptr;
        }*/

	return crimild::get_ptr( _joints[ name ] );
}

SkinnedMeshJoint *SkinnedMeshJointCatalog::updateOrCreateJoint( std::string name, const Transformation &offset )
{
    /*
	if ( _joints.find( name ) ) {
		auto joint = _joints[ name ];
		joint->setOffset( offset );
		return crimild::get_ptr( joint );
	}
    */
	unsigned int jointId = _joints.size();
	auto joint = crimild::alloc< SkinnedMeshJoint >( jointId, offset, name );
	_joints[ name ] = joint;

	return crimild::get_ptr( joint );
}

void SkinnedMeshJointCatalog::encode( coding::Encoder &encoder )
{
	Codable::encode( encoder );

	Array< SharedPointer< SkinnedMeshJoint >> js;
    /*
	_joints.foreach( [ &js ]( std::string const &, SharedPointer< SkinnedMeshJoint > &j, unsigned int ) {
		if ( j != nullptr ) {
			js.add( j );
		}
	});
    */
	encoder.encode( "joints", js );
}

void SkinnedMeshJointCatalog::decode( coding::Decoder &decoder )
{
	Codable::decode( decoder );

	Array< SharedPointer< SkinnedMeshJoint >> js;
	decoder.decode( "joints", js );
	js.each( [ this ]( SharedPointer< SkinnedMeshJoint > &j, crimild::Size ) {
		_joints[ j->getName() ] = j;
	});
}

bool SkinnedMeshJointCatalog::registerInStream( Stream &s )
{
	if ( !StreamObject::registerInStream( s ) ) {
		return false;
	}

    /*
	_joints.foreach( [&s]( std::string const &, SharedPointer< SkinnedMeshJoint > &j, unsigned int ) {
		if ( j != nullptr ) {
			j->registerInStream( s );
		}
	});
    */

	return true;
}

void SkinnedMeshJointCatalog::save( Stream &s )
{
	StreamObject::save( s );

	std::vector< SharedPointer< SkinnedMeshJoint >> os;
    /*
	_joints.foreach( [&os]( std::string const &, SharedPointer< SkinnedMeshJoint > &j, unsigned int ) {
		if ( j != nullptr ) {
			os.push_back( j );
		}
	});
    */
	s.write( os );
}

void SkinnedMeshJointCatalog::load( Stream &s )
{
	StreamObject::load( s );

	std::vector< SharedPointer< SkinnedMeshJoint >> js;
	s.read( js );
	for ( auto &j : js ) {
		_joints[ j->getName() ] = j;
	}
}

SkinnedMeshAnimationChannel::SkinnedMeshAnimationChannel( void )
{

}

SkinnedMeshAnimationChannel::~SkinnedMeshAnimationChannel( void )
{

}

bool SkinnedMeshAnimationChannel::computePosition( float animationTime, Vector3f &result )
{
	if ( getPositionKeys().size() == 1 ) {
		result = getPositionKeys()[ 0 ].value;
		return true;
	}

	unsigned int positionIndex = 0;
	for ( int i = 0; i < getPositionKeys().size() - 1; i++ ) {
		if ( animationTime < getPositionKeys()[ i + 1 ].time ) {
			positionIndex = i;
			break;
		}
	}

	auto const &p0 = getPositionKeys()[ positionIndex ];
	auto const &p1 = getPositionKeys()[ positionIndex + 1 ];

	float dt = p1.time - p0.time;
	float factor = ( animationTime - p0.time ) / dt;
	Interpolation::linear( p0.value, p1.value, factor, result );

	return true;
}

bool SkinnedMeshAnimationChannel::computeRotation( float animationTime, Quaternion4f &result )
{
	if ( getRotationKeys().size() == 1 ) {
		result = getRotationKeys()[ 0 ].value;
		return true;
	}

	unsigned int rotationIndex = 0;
	for ( int i = 0; i < getRotationKeys().size() - 1; i++ ) {
		if ( animationTime < getRotationKeys()[ i + 1 ].time ) {
			rotationIndex = i;
			break;
		}
	}

	auto const &r0 = getRotationKeys()[ rotationIndex ];
	auto const &r1 = getRotationKeys()[ rotationIndex + 1 ];

	float dt = r1.time - r0.time;
	float factor = ( animationTime - r0.time ) / dt;
	Interpolation::slerp( r0.value, r1.value, factor, result );

	return true;
}

bool SkinnedMeshAnimationChannel::computeScale( float animationTime, float &result )
{
	if ( getScaleKeys().size() == 1 ) {
		result = getScaleKeys()[ 0 ].value;
		return true;
	}

	unsigned int scaleIndex = 0;
	for ( int i = 0; i < getScaleKeys().size() - 1; i++ ) {
		if ( animationTime < getScaleKeys()[ i + 1 ].time ) {
			scaleIndex = i;
			break;
		}
	}

	auto const &s0 = getScaleKeys()[ scaleIndex ];
	auto const &s1 = getScaleKeys()[ scaleIndex + 1 ];

	float dt = s1.time - s0.time;
	float factor = ( animationTime - s0.time ) / dt;
	Interpolation::linear( s0.value, s1.value, factor, result );

	return true;
}

void SkinnedMeshAnimationChannel::encode( coding::Encoder &encoder )
{
	Codable::encode( encoder );

    encoder.encode( "name", getName() );

	Array< crimild::Real32 > pTimes;
	Array< Vector3f > pValues;
    /*
	_positionKeys.foreach( [ &pTimes, &pValues ]( PositionKey const &k, unsigned int ) {
		pTimes.add( k.time );
		pValues.add( k.value );
	});
    */
	encoder.encode( "position_times", pTimes );
	encoder.encode( "position_values", pValues );

	Array< crimild::Real32 > rTimes;
	Array< Quaternion4f > rValues;
    /*
	_rotationKeys.foreach( [ &rTimes, &rValues ]( RotationKey const &k, unsigned int ) {
		rTimes.add( k.time );
		rValues.add( k.value );
	});
    */
	encoder.encode( "rotation_times", rTimes );
	encoder.encode( "rotation_values", rValues );

	Array< crimild::Real32 > sTimes;
	Array< crimild::Real32 > sValues;
    /*
	_scaleKeys.foreach( [ &sTimes, &sValues ]( ScaleKey const &k, unsigned int ) {
		sTimes.add( k.time );
		sValues.add( k.value );
	});
    */
	encoder.encode( "scale_times", sTimes );
	encoder.encode( "scale_values", sValues );
}

void SkinnedMeshAnimationChannel::decode( coding::Decoder &decoder )
{
	Codable::decode( decoder );

    std::string name;
    decoder.decode( "name", name );
    setName( name );

	Array< crimild::Real32 > pTimes;
	Array< Vector3f > pValues;
	decoder.decode( "position_times", pTimes );
	decoder.decode( "position_values", pValues );
	const auto pCount = pTimes.size();
    _positionKeys.resize( pCount );
	for ( crimild::Size i = 0; i < pCount; i++ ) {
		_positionKeys[ i ] = PositionKey { pTimes[ i ], pValues[ i ] };
	}

	Array< crimild::Real32 > rTimes;
	Array< Quaternion4f > rValues;
	decoder.decode( "rotation_times", rTimes );
	decoder.decode( "rotation_values", rValues );
	const auto rCount = rTimes.size();
    _rotationKeys.resize( rCount );
	for ( crimild::Size i = 0; i < rCount; i++ ) {
		_rotationKeys[ i ] = RotationKey { rTimes[ i ], rValues[ i ] };
	}

	Array< crimild::Real32 > sTimes;
	Array< crimild::Real32 > sValues;
	decoder.decode( "scale_times", sTimes );
	decoder.decode( "scale_values", sValues );
	const auto sCount = sTimes.size();
    _scaleKeys.resize( sCount );
	for ( crimild::Size i = 0; i < sCount; i++ ) {
		_scaleKeys[ i ] = ScaleKey { sTimes[ i ], sValues[ i ] };
	}
}

bool SkinnedMeshAnimationChannel::registerInStream( Stream &s )
{
	if ( !StreamObject::registerInStream( s ) ) {
		return false;
	}

	return true;
}

void SkinnedMeshAnimationChannel::save( Stream &s )
{
	StreamObject::save( s );

	s.write( getName() );

	unsigned int positionKeyCount = _positionKeys.size();
	s.write( positionKeyCount );
    /*
	_positionKeys.foreach( [&s]( PositionKey const &p, unsigned int ) {
		s.write( p.time );
		s.write( p.value );
	});
    */

	unsigned int rotationKeyCount = _rotationKeys.size();
	s.write( rotationKeyCount );
    /*
	_rotationKeys.foreach( [&s]( RotationKey const &p, unsigned int ) {
		s.write( p.time );
		s.write( p.value );
	});
    */

	unsigned int scaleKeyCount = _scaleKeys.size();
	s.write( scaleKeyCount );
    /*
	_scaleKeys.foreach( [&s]( ScaleKey const &p, unsigned int ) {
		s.write( p.time );
		s.write( p.value );
	});
    */
}

void SkinnedMeshAnimationChannel::load( Stream &s )
{
	StreamObject::load( s );

	std::string name;
	s.read( name );
	setName( name );

	unsigned int positionKeyCount;
	s.read( positionKeyCount );
	_positionKeys.resize( positionKeyCount );
	for ( unsigned int i = 0; i < positionKeyCount; i++ ) {
		PositionKey p;
		s.read( p.time );
		s.read( p.value );
		_positionKeys[ i ] = p;
	}

	unsigned int rotationKeyCount;
	s.read( rotationKeyCount );
	_rotationKeys.resize( rotationKeyCount );
	for ( unsigned int i = 0; i < rotationKeyCount; i++ ) {
		RotationKey k;
		s.read( k.time );
		s.read( k.value );
		_rotationKeys[ i ] = k;
	}

	unsigned int scaleKeyCount;
	s.read( scaleKeyCount );
	_scaleKeys.resize( scaleKeyCount );
	for ( unsigned int i = 0; i < scaleKeyCount; i++ ) {
		ScaleKey k;
		s.read( k.time );
		s.read( k.value );
		_scaleKeys[ i ] = k;
	}
}

SkinnedMeshAnimationClip::SkinnedMeshAnimationClip( void )
{

}

SkinnedMeshAnimationClip::~SkinnedMeshAnimationClip( void )
{

}

void SkinnedMeshAnimationClip::encode( coding::Encoder &encoder )
{
	Codable::encode( encoder );

	encoder.encode( "duration", _duration );
	encoder.encode( "frame_rate", _frameRate );

	Array< SharedPointer< SkinnedMeshAnimationChannel >> cs;
	_channels.each( [ &cs ]( std::string const &, SharedPointer< SkinnedMeshAnimationChannel > const &c ) {
		cs.add( c );
	});
	encoder.encode( "channels", cs );
}

void SkinnedMeshAnimationClip::decode( coding::Decoder &decoder )
{
	Codable::decode( decoder );

	decoder.decode( "duration", _duration );
	decoder.decode( "frame_rate", _frameRate );

	Array< SharedPointer< SkinnedMeshAnimationChannel >> cs;
	decoder.decode( "channels", cs );
	cs.each( [ this ]( SharedPointer< SkinnedMeshAnimationChannel > &c ) {
		//getChannels().add( c->getName(), c);
	});
}

bool SkinnedMeshAnimationClip::registerInStream( Stream &s )
{
	if ( !StreamObject::registerInStream( s ) ) {
		return false;
	}

    /*
	_channels.foreach( [&s]( std::string const &, SharedPointer< SkinnedMeshAnimationChannel > const &c, unsigned int ) {
		c->registerInStream( s );
	});
    */

	return true;
}

void SkinnedMeshAnimationClip::save( Stream &s )
{
	StreamObject::save( s );

	s.write( _duration );
	s.write( _frameRate );

	std::vector< SharedPointer< StreamObject >> cs;
    /*
	_channels.foreach( [&cs]( std::string const &, SharedPointer< SkinnedMeshAnimationChannel > const &c, unsigned int ) {
		cs.push_back( c );
	});
    */
	s.write( cs );
}

void SkinnedMeshAnimationClip::load( Stream &s )
{
	StreamObject::load( s );

	s.read( _duration );
	s.read( _frameRate );

	std::vector< SharedPointer< SkinnedMeshAnimationChannel >> cs;
	s.read( cs );
    /*
	for ( auto &c : cs ) {
		getChannels().add( c->getName(), c );
	}
    */
}

SkinnedMeshSkeleton::SkinnedMeshSkeleton( void )
{
	_joints = crimild::alloc< SkinnedMeshJointCatalog >();
}

SkinnedMeshSkeleton::~SkinnedMeshSkeleton( void )
{

}

void SkinnedMeshSkeleton::encode( coding::Encoder &encoder )
{
	Codable::encode( encoder );

	Array< SharedPointer< SkinnedMeshAnimationClip >> clips;
    /*
	_clips.foreach( [ &clips ]( SharedPointer< SkinnedMeshAnimationClip > const &c, unsigned int ) {
		clips.add( c );
	});
    */
	encoder.encode( "clips", clips );

	encoder.encode( "joints", _joints );
	encoder.encode( "global_inverse_transform", _globalInverseTransform );
}

void SkinnedMeshSkeleton::decode( coding::Decoder &decoder )
{
	Codable::decode( decoder );

	Array< SharedPointer< SkinnedMeshAnimationClip >> clips;
	decoder.decode( "clips", clips );
	clips.each( [ this ]( SharedPointer< SkinnedMeshAnimationClip > &c, crimild::Size ) {
		_clips.add( c );
	});

	decoder.decode( "joints", _joints );
	decoder.decode( "global_inverse_transform", _globalInverseTransform );
}

bool SkinnedMeshSkeleton::registerInStream( Stream &s )
{
	if ( !StreamObject::registerInStream( s ) ) {
		return false;
	}

    /*
	_clips.foreach( [&s]( SharedPointer< SkinnedMeshAnimationClip > const &c, unsigned int ) {
		c->registerInStream( s );
	});
    */

	if ( _joints != nullptr ) {
		_joints->registerInStream( s );
	}

	return true;
}

void SkinnedMeshSkeleton::save( Stream &s )
{
	StreamObject::save( s );

	std::vector< SharedPointer< SkinnedMeshAnimationClip >> cs;
	_clips.foreach( [&cs]( SharedPointer< SkinnedMeshAnimationClip > const &c, unsigned int ) {
		cs.push_back( c );
	});
	s.write( cs );

	s.write( _joints );
	s.write( _globalInverseTransform );
}

void SkinnedMeshSkeleton::load( Stream &s )
{
	StreamObject::load( s );

	std::vector< SharedPointer< SkinnedMeshAnimationClip >> cs;
	s.read( cs );
	for ( auto &c : cs ) {
		_clips.add( c );
	}

	s.read( _joints );
	s.read( _globalInverseTransform );
}

SkinnedMeshAnimationState::SkinnedMeshAnimationState( void )
{

}

SkinnedMeshAnimationState::~SkinnedMeshAnimationState( void )
{

}

void SkinnedMeshAnimationState::encode( coding::Encoder &encoder )
{
	Codable::encode( encoder );

	Array< Matrix4f > poses;
	_jointPoses.foreach( [ &poses ]( Matrix4f &m, unsigned int ) {
		poses.add( m );
	});
	encoder.encode( "poses", poses );
}

void SkinnedMeshAnimationState::decode( coding::Decoder &decoder )
{
	Codable::decode( decoder );

	Array< Matrix4f > poses;
	decoder.decode( "poses", poses );
	poses.each( [ this ]( Matrix4f &m, crimild::Size ) {
		_jointPoses.add( m );
	});
}

bool SkinnedMeshAnimationState::registerInStream( Stream &s )
{
	if ( !StreamObject::registerInStream( s ) ) {
		return false;
	}

	return true;
}

void SkinnedMeshAnimationState::save( Stream &s )
{
	StreamObject::save( s );

	unsigned int posesCount = _jointPoses.size();
	s.write( posesCount );
	_jointPoses.foreach( [&s]( Matrix4f &m, unsigned int ) {
		s.write( m );
	});
}

void SkinnedMeshAnimationState::load( Stream &s )
{
	StreamObject::load( s );

	unsigned int posesCount;
	s.read( posesCount );
	for ( int i = 0; i < posesCount; i++ ) {
		Matrix4f m;
		s.read( m );
		_jointPoses.add( m );
	}
}

SkinnedMesh::SkinnedMesh( void )
	: _animationState( crimild::alloc< SkinnedMeshAnimationState >() )
{

}

SkinnedMesh::~SkinnedMesh( void )
{

}

SharedPointer< SkinnedMesh > SkinnedMesh::clone( void )
{
	auto result = crimild::alloc< SkinnedMesh >();
	result->setSkeleton( crimild::retain( getSkeleton() ) );
	return result;
}

void SkinnedMesh::encode( coding::Encoder &encoder )
{
	Codable::encode( encoder );

	encoder.encode( "skeleton", _skeleton );
	encoder.encode( "animation_state", _animationState );
}

void SkinnedMesh::decode( coding::Decoder &decoder )
{
	Codable::decode( decoder );

	decoder.decode( "skeleton", _skeleton );
	decoder.decode( "animation_state", _animationState );
}

bool SkinnedMesh::registerInStream( Stream &s )
{
	if ( !StreamObject::registerInStream( s ) ) {
		return false;
	}

	if ( _skeleton != nullptr ) {
		_skeleton->registerInStream( s );
	}

	if ( _animationState != nullptr ) {
		_animationState->registerInStream( s );
	}

	return true;
}

void SkinnedMesh::save( Stream &s )
{
	StreamObject::save( s );

	s.write( _skeleton );
	s.write( _animationState );
}

void SkinnedMesh::load( Stream &s )
{
	StreamObject::load( s );

	s.read( _skeleton );
	s.read( _animationState );
}

void SkinnedMesh::debugDump( void )
{
	if ( getSkeleton() == nullptr ) {
        Log::debug( CRIMILD_CURRENT_CLASS_NAME, "No skeleton attached to skinned mesh" );
		return;
	}

	std::stringstream ss;

	ss << "Skinned Mesh: ";
	getSkeleton()->getClips().foreach( [&ss]( SharedPointer< SkinnedMeshAnimationClip > &clip, unsigned int clipIdx ) {
		ss << "\n  Animation: #" << clipIdx
		   << "\n    Duration: " << clip->getDuration()
		   << "\n    Frame Rate: " << clip->getFrameRate()
		   << "\n    Channels: " << clip->getChannels().size()
		   << "\n";

		clip->getChannels().foreach( [&ss]( std::string name, SharedPointer< SkinnedMeshAnimationChannel > &channel, unsigned int channelIdx ) {
			ss << "      *"
			   << " Channel: " << std::string( channel->getName() )
			   << " P=" << channel->getPositionKeys().size()
			   << "(" << channel->getPositionKeys()[ 0 ].time
			   << "-" << channel->getPositionKeys()[ channel->getPositionKeys().size() - 1 ].time << ")"
			   << " R=" << channel->getRotationKeys().size()
			   << "(" << channel->getRotationKeys()[ 0 ].time
			   << "-" << channel->getRotationKeys()[ channel->getRotationKeys().size() - 1 ].time << ")"
			   << " S=" << channel->getScaleKeys().size()
			   << "(" << channel->getScaleKeys()[ 0 ].time
			   << "-" << channel->getScaleKeys()[ channel->getScaleKeys().size() - 1 ].time << ")"
			   << "\n";
		});
	});

	Log::debug( CRIMILD_CURRENT_CLASS_NAME, ss.str() );
}

#endif
