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

#include "WavAudioClip.hpp"

#include <vector>

using namespace crimild;
using namespace crimild::al;

/*
 * Struct that holds the RIFF data of the Wave file.
 * The RIFF data is the meta data information that holds,
 * the ID, size and format of the wave file
 */
struct RIFF_Header {
	char chunkID[ 4 ];
  	int chunkSize; //size not including chunkSize or chunkID
  	char format[ 4 ];
};

/*
 * Struct to hold fmt subchunk data for WAVE files.
 */
struct WAVE_Format {
  	char subChunkID[ 4 ];
  	int subChunkSize;
  	short audioFormat;
  	short numChannels;
  	int sampleRate;
  	int byteRate;
  	short blockAlign;
  	short bitsPerSample;
};

/*
* Struct to hold the data of the wave file
*/
struct WAVE_Data {
  	char subChunkID[ 4 ]; //should contain the word data
  	int subChunk2Size; //Stores the size of the data block
};

WavAudioClip::WavAudioClip( std::string filename )
{
  	WAVE_Format wave_format;
  	RIFF_Header riff_header;
  	WAVE_Data wave_data;
 
	FILE *soundFile = fopen( filename.c_str(), "rb" );
	if ( !soundFile ) {
        Log::error( "Cannot open file ", filename );
		return;
	}

	// Read in the first chunk into the struct
	fread( & riff_header, sizeof( RIFF_Header ), 1, soundFile );

	//check for RIFF and WAVE tag in memeory
	if ( riff_header.chunkID[ 0 ] != 'R' || riff_header.chunkID[ 1 ] != 'I' || riff_header.chunkID[ 2 ] != 'F' || riff_header.chunkID[ 3 ] != 'F') {
        Log::error( "Invalid RIFF header: ", riff_header.chunkID );
		return;
	}

	if ( riff_header.format[ 0 ] != 'W' || riff_header.format[ 1 ] != 'A' || riff_header.format[ 2 ] != 'V' || riff_header.format[ 3 ] != 'E' ) {
        Log::error( "Invalid WAVE header: ", riff_header.format );
		return;
	}

	//Read in the 2nd chunk for the wave info
	fread( &wave_format, sizeof( WAVE_Format ), 1, soundFile );

	//check for fmt tag in memory
	if ( wave_format.subChunkID[ 0 ] != 'f' || wave_format.subChunkID[ 1 ] != 'm' || wave_format.subChunkID[ 2 ] != 't' || wave_format.subChunkID[ 3 ] != ' ') {
        Log::error( "Invalid Wave format: ", wave_format.subChunkID );
		return;
	}

	//check for extra parameters;
	if ( wave_format.subChunkSize > 16 ) {
    	fseek( soundFile, sizeof( short ), SEEK_CUR );
    }

	//Read in the the last byte of data before the sound file
	fread( &wave_data, sizeof( WAVE_Data ), 1, soundFile );
	//check for data tag in memory
	if ( wave_data.subChunkID[ 0 ] != 'd' || wave_data.subChunkID[ 1 ] != 'a' || wave_data.subChunkID[ 2 ] != 't' || wave_data.subChunkID[ 3 ] != 'a') {
        Log::error( "Invalid data header: ", wave_data.subChunkID );
		return;
	}

	//Allocate memory for data
	std::vector< unsigned char > data( wave_data.subChunk2Size );

	// Read in the sound data into the soundData variable
	if ( !fread( &data[ 0 ], data.size() * sizeof( unsigned char ), 1, soundFile ) ) {
        Log::error( "Error loading WAVE data into struct" );
		return;
	}

	fclose( soundFile );

	load( wave_format.numChannels, wave_format.bitsPerSample, wave_format.sampleRate, data.size(), &data[ 0 ] );

}

WavAudioClip::~WavAudioClip( void )
{

}

