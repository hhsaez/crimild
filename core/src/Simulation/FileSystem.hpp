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

#ifndef CRIMILD_SIMULATION_FILE_SYSTEM_
#define CRIMILD_SIMULATION_FILE_SYSTEM_

#include <string>

namespace crimild {

	class FileSystem {
	public:
		static FileSystem &getInstance( void );

	private:
		FileSystem( void );
		~FileSystem( void );

	public:
		void init( int argc, char **argv );

        /**
            \brief Sets the base directory
         
            As a side effect, this also sets the documents directory to
            the same path. It can be changed later
         */
		void setBaseDirectory( std::string baseDirectory );
		std::string getBaseDirectory( void ) const { return _baseDirectory; }

		void setDocumentsDirectory( std::string documentsDirectory ) { _documentsDirectory = documentsDirectory; }
		std::string getDocumentsDirectory( void ) const { return _documentsDirectory; }

		std::string extractDirectory( std::string input );

		/**
			\brief Gets the full path for a resources

			Resources are usually bundled with the application and
			are supposed to be stored in a read-only directory.
		*/
		std::string pathForResource( std::string relativePath );

		/**
			\brief Gets the full path for a document

			Documents are stored in writable directories. 
		*/
		std::string pathForDocument( std::string relativePath );
        
        std::string getRelativePath( std::string absolutePath );

        std::string getFileName( std::string path, bool includeExtension = true );

	private:
		std::string _baseDirectory;
        std::string _documentsDirectory;

	};

}

#endif

