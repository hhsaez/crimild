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

#include "Rendering/Renderer.hpp"
#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/Variable.hpp"
#include "Rendering/ShaderGraph/Nodes/VertexShaderInputs.hpp"
#include "Rendering/ShaderGraph/Nodes/VertexOutput.hpp"
#include "Rendering/ShaderGraph/Nodes/VertexShaderOutputs.hpp"
#include "Rendering/ShaderGraph/Nodes/FragmentInput.hpp"
#include "Rendering/ShaderGraph/Nodes/FragmentColorOutput.hpp"
#include "Rendering/ShaderGraph/Nodes/Vector.hpp"
#include "Rendering/ShaderGraph/Nodes/Multiply.hpp"
#include "Rendering/ShaderGraph/Nodes/Divide.hpp"
#include "Rendering/ShaderGraph/Nodes/Scalar.hpp"
#include "Rendering/ShaderGraph/Nodes/Dot.hpp"
#include "Rendering/ShaderGraph/Nodes/Max.hpp"
#include "Rendering/ShaderGraph/Nodes/Subtract.hpp"
#include "Rendering/ShaderGraph/Nodes/Add.hpp"
#include "Rendering/ShaderGraph/Nodes/Normalize.hpp"
#include "Rendering/ShaderGraph/Nodes/Negate.hpp"
#include "Rendering/ShaderGraph/Nodes/Inverse.hpp"
#include "Rendering/ShaderGraph/Nodes/Clamp.hpp"
#include "Rendering/ShaderGraph/Nodes/Fract.hpp"
#include "Rendering/ShaderGraph/Nodes/Length.hpp"
#include "Rendering/ShaderGraph/Nodes/Pow.hpp"
#include "Rendering/ShaderGraph/Nodes/Copy.hpp"
#include "Rendering/ShaderGraph/Nodes/Convert.hpp"
#include "Rendering/ShaderGraph/Nodes/TextureColor.hpp"
#include "Rendering/ShaderGraph/Nodes/Reflect.hpp"
#include "Rendering/ShaderGraph/Nodes/Refract.hpp"
#include "Rendering/ShaderGraph/Nodes/AlphaClip.hpp"
#include "Rendering/ShaderGraph/Nodes/PointSize.hpp"
#include "Rendering/ShaderGraph/Nodes/FragmentCoordInput.hpp"
#include "Rendering/ShaderGraph/Nodes/PointCoordInput.hpp"
#include "Rendering/ShaderGraph/Nodes/MeshVertexMaster.hpp"
#include "Rendering/ShaderGraph/Nodes/PhongFragmentMaster.hpp"
#include "Rendering/ShaderGraph/Nodes/UnlitFragmentMaster.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/ShaderLocation.hpp"

#include "Foundation/Log.hpp"

#include <sstream>
#include <iomanip>

using namespace crimild;
using namespace crimild::containers;
using namespace crimild::shadergraph;

OpenGLShaderGraph::OpenGLShaderGraph( void )
{
	_translators[ Variable::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto var = static_cast< Variable * >( node );

		std::string storageQualifier;
		CodeSection *section = &_variablesSection;
		switch ( var->getStorage() ) {
			case Variable::Storage::INPUT:
				storageQualifier = "in ";
				section = &_inputsSection;
				break;

			case Variable::Storage::OUTPUT:
				storageQualifier = "out ";
				section = &_outputsSection;
				break;

			case Variable::Storage::UNIFORM:
				storageQualifier = "uniform ";
				section = &_uniformsSection;
				break;

			case Variable::Storage::CONSTANT:
				return;

			default:
				break;
		}

		std::stringstream ss;
		auto location = var->getLayoutLocation();
		if ( location >= 0 ) {
			ss << "layout ( location = "
			   << location
			   << " ) ";
		}
		ss << storageQualifier
		   << getVariableTypeString( var )
		   << " " << var->getName()
		   << ";";
		section->add( ss.str() );
	};
	
	_translators[ VertexShaderInputs::__CLASS_NAME ] = []( ShaderGraphNode *node ) {
		// no-op
	};

	_translators[ Dot::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto dot = static_cast< Dot * >( node );

		auto a = dot->getA()->getName();
		auto b = dot->getB()->getName();
		auto ret = dot->getResult()->getName();

		std::stringstream ss;
		ss << ret << " = dot( " << a << ", " << b << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ Max::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto max = static_cast< Max * >( node );

		auto a = max->getA()->getName();
		auto b = max->getB()->getName();
		auto ret = max->getResult()->getName();

		std::stringstream ss;
		ss << ret << " = max( " << a << ", " << b << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ Add::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto add = static_cast< Add * >( node );

		auto line = writeOp( add->getResult(), add->getInputs(), " + " );
		_mainSection.add( line );
	};

	_translators[ Subtract::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto subtract = static_cast< Subtract * >( node );

		auto a = subtract->getA()->getName();
		auto b = subtract->getB()->getName();
		auto ret = subtract->getResult()->getName();

		std::stringstream ss;
		ss << ret << " = " << a << " - " << b << ";";
		_mainSection.add( ss.str() );
	};

	_translators[ Multiply::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto op = static_cast< Multiply * >( node );
		auto line = writeOp( op->getResult(), op->getInputs(), " * " );
		_mainSection.add( line );
	};

	_translators[ Divide::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto divide = static_cast< Divide * >( node );

		auto a = divide->getA()->getName();
		auto b = divide->getB()->getName();
		auto ret = divide->getResult()->getName();

		std::stringstream ss;
		ss << ret << " = " << a << " / " << b << ";";
		_mainSection.add( ss.str() );
	};

	_translators[ Pow::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto pow = static_cast< Pow * >( node );

		auto a = pow->getBase()->getName();
		auto b = pow->getExponent()->getName();
		auto ret = pow->getResult()->getName();

		std::stringstream ss;
		ss << ret << " = pow( " << a << ", " << b << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ ScalarConstant::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto scalar = static_cast< ScalarConstant * >( node );

		auto v = scalar->getVariable();

		std::stringstream ss;
		ss << v->getName() << " = "
		   << std::fixed
		   << scalar->getValue() << ";";
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
			case Variable::Type::VECTOR_2:
				if ( isConnected( x ) ) _mainSection.add( x->getName() + " = " + v->getName() + ".x;" );
				if ( isConnected( y ) ) _mainSection.add( y->getName() + " = " + v->getName() + ".y;" );
				break;

			case Variable::Type::VECTOR_3:
				if ( isConnected( x ) ) _mainSection.add( x->getName() + " = " + v->getName() + ".x;" );
				if ( isConnected( y ) ) _mainSection.add( y->getName() + " = " + v->getName() + ".y;" );
				if ( isConnected( z ) ) _mainSection.add( z->getName() + " = " + v->getName() + ".z;" );
				break;

			case Variable::Type::VECTOR_4:
				if ( isConnected( x ) ) _mainSection.add( x->getName() + " = " + v->getName() + ".x;" );
				if ( isConnected( y ) ) _mainSection.add( y->getName() + " = " + v->getName() + ".y;" );
				if ( isConnected( z ) ) _mainSection.add( z->getName() + " = " + v->getName() + ".z;" );
				if ( isConnected( w ) ) _mainSection.add( w->getName() + " = " + v->getName() + ".w;" );
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
		ss << v->getName() << " = ";
		switch ( v->getType() ) {
			case Variable::Type::VECTOR_2:
				ss << "vec2( "
				   << x->getName()
				   << ", "
				   << y->getName()
				   << " );";
				break;

			case Variable::Type::VECTOR_3:
				ss << "vec3( "
				   << x->getName()
				   << ", "
				   << y->getName()
				   << ", "
				   << z->getName()
				   << " );";
				break;

			case Variable::Type::VECTOR_4:
				ss << "vec4( "
				   << x->getName()
				   << ", "
				   << y->getName()
				   << ", "
				   << z->getName()
				   << ", "
				   << w->getName()
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
		ss << v->getName() << " = ";
		switch ( v->getType() ) {
			case Variable::Type::VECTOR_2:
				ss << "vec2( " << k.x() << ", " << k.y() << " );";
				break;

			case Variable::Type::VECTOR_3:
				ss << "vec3( " << k.x() << ", " << k.y() << ", " << k.z() << " );";
				break;

			case Variable::Type::VECTOR_4:
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
			ss << "gl_Position = " << var->getName() << ";";
			_mainSection.add( ss.str() );
		}
	};

	_translators[ VertexPointSize::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto pointSize = static_cast< VertexPointSize * >( node );

		if ( auto var = pointSize->getPointSize() ) {
			std::stringstream ss;
			ss << "gl_PointSize = " << var->getName() << ";";
			_mainSection.add( ss.str() );
		}
	};

	_translators[ Negate::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto negate = static_cast< Negate * >( node );

		std::stringstream ss;
		ss << negate->getResult()->getName() << " = -" << negate->getInput()->getName() << ";";
		_mainSection.add( ss.str() );
	};

	_translators[ Inverse::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto inverse = static_cast< Inverse * >( node );

		std::stringstream ss;
		ss << inverse->getResult()->getName() << " = inverse( " << inverse->getInput()->getName() << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ Clamp::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto clamp = static_cast< Clamp * >( node );

		std::stringstream ss;
		ss << clamp->getResult()->getName()
		   << " = clamp( " << clamp->getValue()->getName()
		   << ", " << clamp->getMin()->getName()
		   << ", " << clamp->getMax()->getName()
		   << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ Fract::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto fract = static_cast< Fract * >( node );

		std::stringstream ss;
		ss << fract->getResult()->getName() << " = fract( " << fract->getInput()->getName() << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ Length::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto length = static_cast< Length * >( node );

		std::stringstream ss;
		ss << length->getResult()->getName() << " = length( " << length->getInput()->getName() << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ Normalize::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto normalize = static_cast< Normalize * >( node );

		std::stringstream ss;
		ss << normalize->getResult()->getName() << " = normalize( " << normalize->getInput()->getName() << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ Reflect::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto reflect = static_cast< Reflect * >( node );

		std::stringstream ss;
		ss << reflect->getResult()->getName() << " = reflect( "
		   << reflect->getIncident()->getName() << ", "
		   << reflect->getNormal()->getName()
		   << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ Refract::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto refract = static_cast< Refract * >( node );

		std::stringstream ss;
		ss << refract->getResult()->getName() << " = refract( "
		   << refract->getIncident()->getName() << ", "
		   << refract->getNormal()->getName() << ", "
		   << refract->getRatio()->getName()
		   << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ Copy::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto copy = static_cast< Copy * >( node );

		std::stringstream ss;
		ss << copy->getOutput()->getName() << " = " << copy->getInput()->getName() << ";";
		_mainSection.add( ss.str() );
	};

	_translators[ Convert::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto convert = static_cast< Convert * >( node );

		std::stringstream ss;
		ss << convert->getResult()->getName()
		<< " = " << getVariableTypeString( convert->getResult() ) << "( "
		<< convert->getInput()->getName() << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ VertexOutput::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto out = static_cast< VertexOutput * >( node );

		std::stringstream ss;
		ss << out->getOutput()->getName() << " = " << out->getInput()->getName() << ";";
		_mainSection.add( ss.str() );
	};

	_translators[ FragmentColorOutput::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto output = static_cast< FragmentColorOutput * >( node );

		std::stringstream ss;
		ss << "vFragColor = " << output->getFragmentColor()->getName() << ";";
		_mainSection.add( ss.str() );

		_outputsSection.add( "out vec4 vFragColor;" );
	};

	_translators[ FragmentCoordInput::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto fragCoord = static_cast< FragmentCoordInput * >( node );

		std::stringstream ss;
		ss << fragCoord->getInput()->getName() << " = gl_FragCoord;";
		_mainSection.add( ss.str() );

#ifndef CRIMILD_PLATFORM_EMSCRIPTEN
		_inputsSection.add( "in vec4 gl_FragCoord;" );
#endif
	};

	_translators[ PointCoordInput::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto pointCoord = static_cast< PointCoordInput * >( node );

		std::stringstream ss;
		ss << pointCoord->getInput()->getName() << " = gl_PointCoord;";
		_mainSection.add( ss.str() );
	};

	_translators[ TextureColor::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto op = static_cast< TextureColor * >( node );

		auto t = op->getTexture();
		auto uv = op->getTextureCoords();
		auto ret = op->getColor();

		std::stringstream ss;
		ss << ret->getName() << " = " << "texture( " << t->getName() << ", " << uv->getName() << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ AlphaClip::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto alphaClip = static_cast< AlphaClip * >( node );

		auto alpha = alphaClip->getAlphaInput()->getName();
		auto threshold = alphaClip->getThresholdInput()->getName();

		std::stringstream ss;
		ss << "if ( " << alpha << " < " << threshold << " ) discard;";
		_mainSection.add( ss.str() );
	};

	_translators[ MeshVertexMaster::__CLASS_NAME ] = []( ShaderGraphNode *node ) {
		// no-op (avoid warnings)
	};
		
	_translators[ UnlitFragmentMaster::__CLASS_NAME ] = []( ShaderGraphNode *node ) {
		// no-op (avoid warnings)
	};
		
	_translators[ PhongFragmentMaster::__CLASS_NAME ] = [ this ]( ShaderGraphNode *node ) {
		auto master = static_cast< PhongFragmentMaster * >( node );

		auto P = master->getWorldPosition()->getName();
		auto N = master->getWorldNormal()->getName();
		auto E = master->getWorldEye()->getName();
		auto ambient = master->getAmbient()->getName();
		auto diffuse = master->getDiffuse()->getName();
		auto specular = master->getSpecular()->getName();
		auto shininess = master->getShininess()->getName();
		auto alpha = master->getAlpha()->getName();
		auto alphaClip = master->getAlphaClipThreshold()->getName();

		std::stringstream ss;

		_outputsSection.add( "out vec4 vFragColor;" );

		ss.str( "" );
        ss << "#define MAX_LIGHTS " << Renderer::getInstance()->getMaxLights( Light::Type::DIRECTIONAL );
		_globalsSection.add( ss.str() );
		
		_globalsSection.add(
			#include "calcPhongLighting.glsl"
		);

		ss.str( "" );
		ss << "if ( " << alpha << " < " << alphaClip << " ) discard;";
		_mainSection.add( ss.str() );

		ss.str( "" );
		ss << "vFragColor = vec4( calcPhongLighting( "
		<< P << ", "
		<< N << ", "
		<< E << ", "
		<< ambient << ", "
		<< diffuse << ", "
		<< specular << ", "
		<< shininess << " ), "
		<< alpha << " );";
		_mainSection.add( ss.str() );
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

#ifdef CRIMILD_PLATFORM_EMSCRIPTEN
	ss << "#version 300 es\n"
	   << "precision highp int;\n"
	   << "precision mediump float;\n";
#else
	ss << "#version 330 core\n"
       << "precision highp int;\n"
       << "precision highp float;\n";
#endif

	ss << "\n// Macros";
#ifdef CRIMILD_PLATFORM_EMSCRIPTEN
	ss << "\n#define CRIMILD_PACK_FLOAT_TO_RGBA 1";
#endif
	ss << "\n";
	
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
	
	ss << "\n// Globals";
	_globalsSection.each( [ &ss ]( std::string &line ) {
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

std::string OpenGLShaderGraph::getVariableTypeString( Variable *var )
{
	auto type = var->getType();
	
	std::string typeStr;
	switch ( type ) {
		case Variable::Type::SCALAR:
			typeStr = "float";
			break;
			
		case Variable::Type::VECTOR_2:
			typeStr = "vec2";
			break;
			
		case Variable::Type::VECTOR_3:
			typeStr = "vec3";
			break;
			
		case Variable::Type::VECTOR_4:
			typeStr = "vec4";
			break;
			
		case Variable::Type::MATRIX_3:
			typeStr = "mat3";
			break;
			
		case Variable::Type::MATRIX_4:
			typeStr = "mat4";
			break;

		case Variable::Type::SAMPLER_2D:
			typeStr = "sampler2D";
			break;
			
		case Variable::Type::SAMPLER_CUBE_MAP:
			typeStr = "samplerCube";
			break;
			
		default:
			typeStr = "unknown";
			break;
	}

	return typeStr;
}

std::string OpenGLShaderGraph::writeOp( Variable *result, containers::Array< Variable * > const &inputs, std::string separator )
{
	std::stringstream ss;
	ss << result->getName() << " = ";
	inputs.each( [ &ss, separator ]( Variable *in, crimild::Size idx ) {
		if ( idx > 0 ) {
			ss << separator;
		}
		ss << in->getName();
	});
	ss << ";";
	return ss.str();
}

