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

#include "Settings.hpp"
#include "FileSystem.hpp"

using namespace crimild;

const char *Settings::SETTINGS_APP_NAME = "application.name";
const char *Settings::SETTINGS_APP_VERSION_MAJOR = "application.version.major";
const char *Settings::SETTINGS_APP_VERSION_MINOR = "application.version.minor";
const char *Settings::SETTINGS_APP_VERSION_PATCH = "application.version.patch";
const char *Settings::SETTINGS_RENDERING_SHADOWS_ENABLED = "crimild.rendering.shadows.enabled";
const char *Settings::SETTINGS_RENDERING_SHADOWS_RESOLUTION_WIDTH = "crimild.rendering.shadows.resolution.width";
const char *Settings::SETTINGS_RENDERING_SHADOWS_RESOLUTION_HEIGHT = "crimild.rendering.shadows.resolution.height";

Settings::Settings( void )
{

}

Settings::Settings( int argc, char **argv )
{
    parseCommandLine( argc, argv );
}

Settings::~Settings( void )
{

}

void Settings::parseCommandLine(int argc, char **argv)
{
	if (argc > 0 && argv != nullptr) {
		FileSystem::getInstance().init(argc, argv);
	}

	if (argc > 0) {
		_settings["__base_directory"] = FileSystem::getInstance().getBaseDirectory();
	}

	for (int i = 1; i < argc; i++) {
		std::string option = argv[i];
		int separatorPos = option.find_first_of("=");
		if (separatorPos > 0) {
			std::string key = option.substr(0, separatorPos);
			std::string value = option.substr(separatorPos + 1);
			set(key, value);
		}
	}

	for (auto it : _settings) {
		Log::debug(CRIMILD_CURRENT_CLASS_NAME, it.first, " -> ", it.second);
	}
}

void Settings::each(std::function< void(std::string, Settings *) > callback)
{
	for (auto it : _settings) {
		callback(it.first, this);
	}
}

