/*
 * Copyright (c) 2002-present, H. Hernan Saez
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

#include "OpenGLShaderGraph.hpp"

#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/ShaderGraphVariable.hpp"
#include "Rendering/ShaderGraph/Nodes/VertexShaderInputs.hpp"
#include "Rendering/ShaderGraph/Nodes/VertexOutput.hpp"
#include "Rendering/ShaderGraph/Nodes/VertexShaderOutputs.hpp"
#include "Rendering/ShaderGraph/Nodes/FragmentInput.hpp"
#include "Rendering/ShaderGraph/Nodes/FragmentColorOutput.hpp"
#include "Rendering/ShaderGraph/Nodes/Vector.hpp"
#include "Rendering/ShaderGraph/Nodes/Multiply.hpp"
#include "Rendering/ShaderGraph/Nodes/Scalar.hpp"
#include "Rendering/ShaderGraph/Nodes/Dot.hpp"
#include "Rendering/ShaderGraph/Nodes/Max.hpp"
#include "Rendering/ShaderGraph/Nodes/Subtract.hpp"
#include "Rendering/ShaderGraph/Nodes/Normalize.hpp"
#include "Rendering/ShaderGraph/Nodes/Negate.hpp"
#include "Rendering/ShaderGraph/Nodes/Pow.hpp"
#include "Rendering/ShaderGraph/Nodes/Copy.hpp"
#include "Rendering/ShaderGraph/Nodes/Convert.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/ShaderLocation.hpp"

#include "Foundation/Log.hpp"

#include <sstream>

using namespace crimild;
using namespace crimild::containers;
using namespace crimild::shadergraph;

OpenGLShaderGraph::OpenGLShaderGraph( void )
{
	_translators[ ShaderGraphVariable::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto var = static_cast< ShaderGraphVariable * >( node );

		std::string storageQualifier;
		CodeSection *section = &_variablesSection;
		switch ( var->getStorage() ) {
			case ShaderGraphVariable::Storage::INPUT:
				storageQualifier = "in ";
				section = &_inputsSection;
				break;

			case ShaderGraphVariable::Storage::OUTPUT:
				storageQualifier = "out ";
				section = &_outputsSection;
				break;

			case ShaderGraphVariable::Storage::UNIFORM:
				storageQualifier = "uniform ";
				section = &_uniformsSection;
				break;

			default:
				break;
		}

		std::stringstream ss;
		ss << storageQualifier << getVariableTypeString( var ) << " " << var->getUniqueName() << ";";
		section->add( ss.str() );
	};
	
	_translators[ VertexShaderInputs::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto inputs = static_cast< VertexShaderInputs * >( node );
	};

	_translators[ Multiply::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto multiply = static_cast< Multiply * >( node );

		auto a = multiply->getA();
		auto b = multiply->getB();
		auto ret = multiply->getResult();

		std::stringstream ss;
		ss << ret->getUniqueName()
		<< " = " << a->getUniqueName()
		<< " * " << b->getUniqueName()
		<< ";";

		_mainSection.add( ss.str() );
	};

	_translators[ Dot::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto dot = static_cast< Dot * >( node );

		auto a = dot->getA()->getUniqueName();
		auto b = dot->getB()->getUniqueName();
		auto ret = dot->getResult()->getUniqueName();

		std::stringstream ss;
		ss << ret << " = dot( " << a << ", " << b << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ Max::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto max = static_cast< Max * >( node );

		auto a = max->getA()->getUniqueName();
		auto b = max->getB()->getUniqueName();
		auto ret = max->getResult()->getUniqueName();

		std::stringstream ss;
		ss << ret << " = max( " << a << ", " << b << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ Subtract::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto subtract = static_cast< Subtract * >( node );

		auto a = subtract->getA()->getUniqueName();
		auto b = subtract->getB()->getUniqueName();
		auto ret = subtract->getResult()->getUniqueName();

		std::stringstream ss;
		ss << ret << " = " << a << " - " << b << ";";
		_mainSection.add( ss.str() );
	};

	_translators[ Pow::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto pow = static_cast< Pow * >( node );

		auto a = pow->getBase()->getUniqueName();
		auto b = pow->getExponent()->getUniqueName();
		auto ret = pow->getResult()->getUniqueName();

		std::stringstream ss;
		ss << ret << " = pow( " << a << ", " << b << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ ScalarConstant::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto scalar = static_cast< ScalarConstant * >( node );

		auto v = scalar->getVariable();

		std::stringstream ss;
		ss << v->getUniqueName() << " = " << scalar->getValue() << ";";
		_mainSection.add( ss.str() );
	};

	_translators[ VectorToScalars::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto vector = static_cast< VectorToScalars * >( node );

		auto v = vector->getVector();
		auto x = vector->getX();
		auto y = vector->getY();
		auto z = vector->getZ();
		auto w = vector->getW();

		switch ( v->getType() ) {
			case ShaderGraphVariable::Type::VECTOR_2:
				if ( isConnected( x ) ) _mainSection.add( x->getUniqueName() + " = " + v->getUniqueName() + ".x;" );
				if ( isConnected( y ) ) _mainSection.add( y->getUniqueName() + " = " + v->getUniqueName() + ".y;" );
				break;

			case ShaderGraphVariable::Type::VECTOR_3:
				if ( isConnected( x ) ) _mainSection.add( x->getUniqueName() + " = " + v->getUniqueName() + ".x;" );
				if ( isConnected( y ) ) _mainSection.add( y->getUniqueName() + " = " + v->getUniqueName() + ".y;" );
				if ( isConnected( z ) ) _mainSection.add( z->getUniqueName() + " = " + v->getUniqueName() + ".z;" );
				break;

			case ShaderGraphVariable::Type::VECTOR_4:
				if ( isConnected( x ) ) _mainSection.add( x->getUniqueName() + " = " + v->getUniqueName() + ".x;" );
				if ( isConnected( y ) ) _mainSection.add( y->getUniqueName() + " = " + v->getUniqueName() + ".y;" );
				if ( isConnected( z ) ) _mainSection.add( z->getUniqueName() + " = " + v->getUniqueName() + ".z;" );
				if ( isConnected( w ) ) _mainSection.add( w->getUniqueName() + " = " + v->getUniqueName() + ".w;" );
				break;
			default:
				break;
		}
	};

	_translators[ ScalarsToVector::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto scalars = static_cast< ScalarsToVector * >( node );

		auto x = scalars->getX();
		auto y = scalars->getY();
		auto z = scalars->getZ();
		auto w = scalars->getW();
		auto v = scalars->getVector();

		std::stringstream ss;
		ss << v->getUniqueName() << " = ";
		switch ( v->getType() ) {
			case ShaderGraphVariable::Type::VECTOR_2:
				ss << "vec2( "
				   << x->getUniqueName()
				   << ", "
				   << y->getUniqueName()
				   << " );";
				break;

			case ShaderGraphVariable::Type::VECTOR_3:
				ss << "vec3( "
				   << x->getUniqueName()
				   << ", "
				   << y->getUniqueName()
				   << ", "
				   << z->getUniqueName()
				   << " );";
				break;

			case ShaderGraphVariable::Type::VECTOR_4:
				ss << "vec4( "
				   << x->getUniqueName()
				   << ", "
				   << y->getUniqueName()
				   << ", "
				   << z->getUniqueName()
				   << ", "
				   << w->getUniqueName()
				   << " );";
				break;
			default:
				break;
		}
		_mainSection.add( ss.str() );
	};

	_translators[ VectorConstant::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto vector = static_cast< VectorConstant * >( node );

		auto v = vector->getVector();
		auto k = vector->getValue();

		std::stringstream ss;
		ss << v->getUniqueName() << " = ";
		switch ( v->getType() ) {
			case ShaderGraphVariable::Type::VECTOR_2:
				ss << "vec2( " << k.x() << ", " << k.y() << " );";
				break;

			case ShaderGraphVariable::Type::VECTOR_3:
				ss << "vec3( " << k.x() << ", " << k.y() << ", " << k.z() << " );";
				break;

			case ShaderGraphVariable::Type::VECTOR_4:
				ss << "vec4( " << k.x() << ", " << k.y() << ", " << k.z() << ", " << k.w() << " );";
				break;
				
			default:
				break;
		}
		_mainSection.add( ss.str() );
	};

	_translators[ VertexShaderOutputs::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto outputs = static_cast< VertexShaderOutputs * >( node );

		if ( auto var = outputs->getVertexPosition() ) {
			std::stringstream ss;
			ss << "gl_Position = " << var->getUniqueName() << ";";
			_mainSection.add( ss.str() );
		}
	};

	_translators[ Negate::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto negate = static_cast< Negate * >( node );

		std::stringstream ss;
		ss << negate->getResult()->getUniqueName() << " = -" << negate->getInput()->getUniqueName() << ";";
		_mainSection.add( ss.str() );
	};

	_translators[ Normalize::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto normalize = static_cast< Normalize * >( node );

		std::stringstream ss;
		ss << normalize->getResult()->getUniqueName() << " = normalize( " << normalize->getInput()->getUniqueName() << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ Copy::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto copy = static_cast< Copy * >( node );

		std::stringstream ss;
		ss << copy->getOutput()->getUniqueName() << " = " << copy->getInput()->getUniqueName() << ";";
		_mainSection.add( ss.str() );
	};

	_translators[ Convert::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto convert = static_cast< Convert * >( node );

		std::stringstream ss;
		ss << convert->getResult()->getUniqueName()
		<< " = " << getVariableTypeString( convert->getResult() ) << "( "
		<< convert->getInput()->getUniqueName() << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ VertexOutput::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto out = static_cast< VertexOutput * >( node );

		std::stringstream ss;
		ss << out->getOutput()->getUniqueName() << " = " << out->getInput()->getUniqueName() << ";";
		_mainSection.add( ss.str() );
	};

	_translators[ FragmentColorOutput::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto output = static_cast< FragmentColorOutput * >( node );

		std::stringstream ss;
		ss << "vFragColor = " << output->getFragmentColor()->getUniqueName() << ";";
		_mainSection.add( ss.str() );

		_outputsSection.add( "out vec4 vFragColor;" );
	};
}

OpenGLShaderGraph::~OpenGLShaderGraph( void )
{

}

std::string OpenGLShaderGraph::generateShaderSource( containers::Array< ShaderGraphNode * > const &sortedNodes )
{
	Array< ShaderGraphNode * > variables;
	Array< ShaderGraphNode * > operations;
	
	_inputsSection.clear();
	_uniformsSection.clear();
	_outputsSection.clear();
	_globalsSection.clear();
	_mainSection.clear();
	
	sortedNodes.each( [ &variables, &operations ]( ShaderGraphNode *node ) {
		if ( node->getNodeType() == ShaderGraphNode::NodeType::VARIABLE ) {
			variables.add( node );
		}
		else {
			operations.add( node );
		}
	});

	variables.each( [ this ]( ShaderGraphNode *node ) {
		if ( _translators.contains( node->getClassName() ) ) {
			_translators[ node->getClassName() ]( node );
		}
		else {
			CRIMILD_LOG_WARNING( "No valid translator for type ", node->getClassName() );
		}
	});
	
	operations.each( [ this ]( ShaderGraphNode *node ) {
		if ( _translators.contains( node->getClassName() ) ) {
			_translators[ node->getClassName() ]( node );
		}
		else {
			CRIMILD_LOG_WARNING( "No valid translator for type ", node->getClassName() );
		}
	});
	
	std::stringstream ss;

	ss << "#version 400\n";
	
	ss << "\n// Inputs";
	_inputsSection.each( [ &ss ]( std::string &line ) {
		ss << "\n" << line;
	});
	ss << "\n";
	
	ss << "\n// Uniforms";
	_uniformsSection.each( [ &ss ]( std::string &line ) {
		ss << "\n" << line;
	});
	ss << "\n";
	
	ss << "\n// Outputs";
	_outputsSection.each( [ &ss ]( std::string &line ) {
		ss << "\n" << line;
	});
	ss << "\n";
	
	ss << "\nvoid main()\n{";
	_variablesSection.each( [ &ss ]( std::string &line ) {
		ss << "\n\t" << line;
	});
	ss << "\n";
	_mainSection.each( [ &ss ]( std::string &line ) {
		ss << "\n\t" << line;
	});
	ss << "\n}\n";
	
	return ss.str();
}

std::string OpenGLShaderGraph::getVariableTypeString( ShaderGraphVariable *var )
{
	auto type = var->getType();
	
	std::string typeStr;
	switch ( type ) {
		case ShaderGraphVariable::Type::SCALAR:
			typeStr = "float";
			break;
			
		case ShaderGraphVariable::Type::VECTOR_2:
			typeStr = "vec2";
			break;
			
		case ShaderGraphVariable::Type::VECTOR_3:
			typeStr = "vec3";
			break;
			
		case ShaderGraphVariable::Type::VECTOR_4:
			typeStr = "vec4";
			break;
			
		case ShaderGraphVariable::Type::MATRIX_3:
			typeStr = "mat3";
			break;
			
		case ShaderGraphVariable::Type::MATRIX_4:
			typeStr = "mat4";
			break;
			
		default:
			typeStr = "unknown";
			break;
	}

	return typeStr;
}

