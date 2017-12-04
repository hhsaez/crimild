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

#include "LuaDecoratorUtils.hpp"

#include "SceneGraph/LuaSceneBuilder.hpp"

using namespace crimild;
using namespace crimild::behaviors;
using namespace crimild::behaviors::decorators;
using namespace crimild::scripting;

void LuaDecoratorUtils::setBehavior( SharedPointer< Decorator > const &parent, ScriptEvaluator &eval )
{
	ScriptEvaluator behaviorEval = eval.getChildEvaluator( "behavior" );
	std::string type;
	if ( behaviorEval.getPropValue( "type", type ) ) {
		auto behaviorBuilder = LuaObjectBuilderRegistry::getInstance()->getBuilder( type );
		if ( behaviorBuilder != nullptr ) {
			auto behavior = crimild::cast_ptr< Behavior >( behaviorBuilder( behaviorEval ) );
			if ( behavior != nullptr ) {
				parent->setBehavior( behavior );
			}
			else {
				Log::error( CRIMILD_CURRENT_CLASS_NAME, "Failed to build behavior wit type ", type );
			}
		}
		else {
			Log::error( CRIMILD_CURRENT_CLASS_NAME, "No builder found for behavior with type ", type );
		}
	}
	else {
		Log::error( CRIMILD_CURRENT_CLASS_NAME, "No 'type' field found for root behavior" );
	}
}

