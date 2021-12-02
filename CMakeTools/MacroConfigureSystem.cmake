#
# Copyright (c) 2002 - present, H. Hernan Saez
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the copyright holder nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

IF ( ${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten" )
  	SET( CRIMILD_ENABLE_EMSCRIPTEN ON )
	ADD_DEFINITIONS( -DCRIMILD_PLATFORM_EMSCRIPTEN=1 )
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -static -U__STRICT_ANSI__" )
	RETURN()
ENDIF ()

SET( CRIMILD_ENABLE_EMSCRIPTEN OFF )

SET( CMAKE_CXX_STANDARD 20 )
SET( CMAKE_CXX_STANDARD_REQUIRED ON )
SET( CMAKE_CXX_EXTENSIONS OFF )

IF ( MSVC )
	ADD_COMPILE_OPTIONS( /W4 )
ELSE() 
	ADD_COMPILE_OPTIONS( 
		-Wall 
		-Wextra 
		-Wpedantic 
	)
	
	# Ignore specific warnings
	ADD_COMPILE_OPTIONS( 
		-Wno-missing-braces 
		-Wno-c++98-compat 
		-Wno-c++98-compat-pedantic 
	)
ENDIF()
