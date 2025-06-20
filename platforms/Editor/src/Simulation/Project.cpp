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

#include "Simulation/Project.hpp"

using namespace crimild;
using namespace crimild::editor;

Project::Project( void ) noexcept
   : Named( "Crimild" ),
     Versionable( Version( 1, 0, 0 ) )
{
   // no-op
}

Project::Project( std::string name, const Version &version ) noexcept
   : Named( name ),
     Versionable( version )
{
   // no-op
}

bool Project::isRelativePath( std::filesystem::path path ) const
{
   return path.string().starts_with( "assets://" );
}

std::filesystem::path Project::toRelativePath( std::filesystem::path absolutePath ) const
{
   auto root = getRootDirectory();
   auto relative = std::filesystem::relative( absolutePath, root );
   return std::filesystem::path( "assets://" ) / relative;
}

bool Project::isAbsolutePath( std::filesystem::path path ) const
{
   return !isRelativePath( path );
}

std::filesystem::path Project::toAbsolutePath( std::filesystem::path relativePath ) const
{
   const std::string assetsPrefix = "assets://";
   if ( !relativePath.string().starts_with( assetsPrefix ) ) {
      // Path is not relative. Return it as is.
      return relativePath;
   }
   return getRootDirectory() / relativePath.string().substr( assetsPrefix.length() );
}

std::shared_ptr< coding::Codable > Project::load( std::filesystem::path relativePath ) const
{
   auto path = toAbsolutePath( relativePath );

   if ( !std::filesystem::exists( path ) ) {
      CRIMILD_LOG_WARNING( "Path does not exists: ", path );
      return nullptr;
   }

   coding::FileDecoder decoder;
   if ( !decoder.read( path ) ) {
      CRIMILD_LOG_WARNING( "Cannot decode file: ", path );
   }

   if ( decoder.getObjectCount() == 0 ) {
      CRIMILD_LOG_WARNING( "File is empty: ", path );
      return nullptr;
   }

   return decoder.getObjectAt< coding::Codable >( 0 );
}

void Project::save( std::filesystem::path relativePath, std::shared_ptr< coding::Codable > const &codable ) const
{
   auto path = toAbsolutePath( relativePath );

   coding::FileEncoder encoder;
   if ( !encoder.encode( codable ) ) {
      CRIMILD_LOG_WARNING( "Cannot encode object to file: ", path );
      return;
   }

   if ( !encoder.write( path ) ) {
      CRIMILD_LOG_ERROR( "Failed to write to file: ", path );
      return;
   }
}

void Project::encode( coding::Encoder &encoder )
{
   Codable::encode( encoder );

   encoder.encode( "name", getNameRefForCoding() );
   encoder.encode( "version", getVersionRefForCoding() );
   encoder.encode( "currentSceneName", m_currentSceneName );
}

void Project::decode( coding::Decoder &decoder )
{
   Codable::decode( decoder );

   decoder.decode( "name", getNameRefForCoding() );
   decoder.decode( "version", getVersionRefForCoding() );
   decoder.decode( "currentSceneName", m_currentSceneName );
}
