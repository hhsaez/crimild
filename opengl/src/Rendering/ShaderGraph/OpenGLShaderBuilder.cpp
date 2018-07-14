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

#include "OpenGLShaderBuilder.hpp"

#include "Rendering/ShaderGraph/Node.hpp"
#include "Rendering/ShaderGraph/Outlet.hpp"
#include "Rendering/ShaderGraph/ShaderGraph.hpp"
#include "Rendering/ShaderGraph/Nodes/VertexShaderInput.hpp"
#include "Rendering/ShaderGraph/Nodes/VertexShaderOutput.hpp"
#include "Rendering/ShaderGraph/Nodes/FragmentShaderInput.hpp"
#include "Rendering/ShaderGraph/Nodes/FragmentShaderOutput.hpp"
#include "Rendering/ShaderGraph/Nodes/Vector.hpp"
#include "Rendering/ShaderGraph/Nodes/Multiply.hpp"
#include "Rendering/ShaderGraph/Nodes/Scalar.hpp"
#include "Rendering/ShaderGraph/Nodes/Dot.hpp"
#include "Rendering/ShaderGraph/Nodes/Max.hpp"
#include "Rendering/ShaderGraph/Nodes/Subtract.hpp"
#include "Rendering/ShaderGraph/Nodes/Normalize.hpp"
#include "Rendering/ShaderGraph/Nodes/Negate.hpp"
#include "Rendering/ShaderGraph/Nodes/Pow.hpp"
#include "Rendering/ShaderProgram.hpp"
#include "Rendering/ShaderLocation.hpp"

#include "Foundation/Log.hpp"

#include <sstream>

using namespace crimild;
using namespace crimild::containers;
using namespace crimild::shadergraph;
using namespace crimild::shadergraph::nodes;

OpenGLShaderBuilder::OpenGLShaderBuilder( void )
{
	_translators[ VertexShaderInput::__CLASS_NAME ] = [ this ]( Node *node, ShaderGraph *graph, ShaderProgram *program ) {
		Array< Outlet * > attributes;
		Array< Outlet * > uniforms;

		auto vsInput = static_cast< VertexShaderInput * >( node );

		if ( graph->isConnected( vsInput->getPosition() ) ) {
			program->registerStandardLocation(
				ShaderLocation::Type::ATTRIBUTE,
				ShaderProgram::StandardLocation::POSITION_ATTRIBUTE,
				vsInput->getPosition()->getName() );
			attributes.add( vsInput->getPosition() );
		}

		if ( graph->isConnected( vsInput->getNormal() ) ) {
			program->registerStandardLocation(
				ShaderLocation::Type::ATTRIBUTE,
				ShaderProgram::StandardLocation::NORMAL_ATTRIBUTE,
				vsInput->getNormal()->getName() );
			attributes.add( vsInput->getNormal() );
		}
		
		if ( graph->isConnected( vsInput->getUV() ) ) {
			program->registerStandardLocation(
				ShaderLocation::Type::ATTRIBUTE,
				ShaderProgram::StandardLocation::TEXTURE_COORD_ATTRIBUTE,
				vsInput->getUV()->getName() );
			attributes.add( vsInput->getUV() );
		}
		
		if ( graph->isConnected( vsInput->getColor() ) ) {
			program->registerStandardLocation(
				ShaderLocation::Type::ATTRIBUTE,
				ShaderProgram::StandardLocation::COLOR_ATTRIBUTE,
				vsInput->getColor()->getName() );
			attributes.add( vsInput->getColor() );
		}

		if ( graph->isConnected( vsInput->getMMatrix() ) ) {
			program->registerStandardLocation(
				ShaderLocation::Type::UNIFORM,
				ShaderProgram::StandardLocation::MODEL_MATRIX_UNIFORM,
				vsInput->getMMatrix()->getName() );
			uniforms.add( vsInput->getMMatrix() );
		}
		
		if ( graph->isConnected( vsInput->getVMatrix() ) ) {
			program->registerStandardLocation(
				ShaderLocation::Type::UNIFORM,
				ShaderProgram::StandardLocation::VIEW_MATRIX_UNIFORM,
				vsInput->getVMatrix()->getName() );
			uniforms.add( vsInput->getVMatrix() );
		}

		if ( graph->isConnected( vsInput->getPMatrix() ) ) {
			program->registerStandardLocation(
				ShaderLocation::Type::UNIFORM,
				ShaderProgram::StandardLocation::PROJECTION_MATRIX_UNIFORM,
				vsInput->getPMatrix()->getName() );
			uniforms.add( vsInput->getPMatrix() );
		}
		
		if ( graph->isConnected( vsInput->getMVMatrix() ) ) {
			uniforms.add( vsInput->getMVMatrix() );
		}
		
		if ( graph->isConnected( vsInput->getMVPMatrix() ) ) uniforms.add( vsInput->getMVPMatrix() );

		attributes.each( [ this, graph ]( Outlet *outlet ) {
			auto typeStr = getOutletTypeStr( outlet );
			_inputsSection.add( "in " + typeStr + " " + outlet->getName() + ";" );
			_mainSection.add( typeStr + " " + outlet->getUniqueName() + " = " + outlet->getName() + ";" );
		});
		
		uniforms.each( [ this, graph ]( Outlet *outlet ) {
			auto typeStr = getOutletTypeStr( outlet );
			_uniformsSection.add( "uniform " + typeStr + " " + outlet->getName() + ";" );
			_mainSection.add( typeStr + " " + outlet->getUniqueName() + " = " + outlet->getName() + ";" );
		});
    };
		
	_translators[ VertexShaderOutput::__CLASS_NAME ] = [ this ]( Node *node, ShaderGraph *graph, ShaderProgram *program ) {
		auto vsOutput = static_cast< VertexShaderOutput * >( node );

		node->eachInputOutlet( [ this, vsOutput, graph ]( Outlet *outlet ) {
			if ( graph->isConnected( outlet ) ) {
				auto typeStr = getOutletTypeStr( outlet );
				if ( outlet == vsOutput->getScreenPosition() ) {
					_mainSection.add( "gl_Position = " + graph->anyConnection( outlet )->getUniqueName() + ";" );
				}
				else {
					_outputsSection.add( "out " + typeStr + " " + outlet->getName() + ";" );
					graph->eachConnection( outlet, [ this, outlet ]( Outlet *source ) {
						_mainSection.add( outlet->getName() + " = " + source->getUniqueName() + ";" );
					});
				}
			}
		});
	};

	_translators[ FragmentShaderInput::__CLASS_NAME ] = [ this ]( Node *node, ShaderGraph *graph, ShaderProgram *program ) {
		node->eachOutputOutlet( [ this, graph ]( Outlet *outlet ) {
			if ( graph->isConnected( outlet ) ) {
				auto typeStr = getOutletTypeStr( outlet );
				_inputsSection.add( "in " + typeStr + " " + outlet->getName() + ";" );
				_mainSection.add( typeStr + " " + outlet->getUniqueName() + " = " + outlet->getName() + ";" );
			}
		});
	};

	_translators[ FragmentShaderOutput::__CLASS_NAME ] = [ this ]( Node *node, ShaderGraph *graph, ShaderProgram *program ) {
		auto fsOutput = static_cast< FragmentShaderOutput * >( node );

		node->eachInputOutlet( [ this, graph, fsOutput ]( Outlet *outlet ) {
			if ( graph->isConnected( outlet ) ) {
				auto typeStr = getOutletTypeStr( outlet );
				_outputsSection.add( "out " + typeStr + " " + outlet->getName() + ";" );
				graph->eachConnection( outlet, [ this, outlet ]( Outlet *source ) {
					_mainSection.add( outlet->getName() + " = " + source->getUniqueName() + ";" );
				});
			}
		});
	};

	_translators[ Multiply::__CLASS_NAME ] = [ this ]( Node *node, ShaderGraph *graph, ShaderProgram *program ) {
		auto multiply = static_cast< Multiply * >( node );

		auto a = graph->anyConnection( multiply->getA() );
		auto b = graph->anyConnection( multiply->getB() );
		auto ret = multiply->getOutput();
		auto retType = getOutletTypeStr( ret );

		auto line = retType + " " + ret->getUniqueName() + " = " + a->getUniqueName() + " * " + b->getUniqueName() + ";";

		_mainSection.add( line );
	};

	_translators[ Dot::__CLASS_NAME ] = [ this ]( Node *node, ShaderGraph *graph, ShaderProgram *program ) {
		auto dot = static_cast< Dot * >( node );

		auto a = graph->anyConnection( dot->getA() );
		auto b = graph->anyConnection( dot->getB() );
		auto value = dot->getValue();
		auto valueType = getOutletTypeStr( value );

		auto line = valueType + " " + value->getUniqueName() + " = dot( " + a->getUniqueName() + ", " + b->getUniqueName() + " );";

		_mainSection.add( line );
	};

	_translators[ Max::__CLASS_NAME ] = [ this ]( Node *node, ShaderGraph *graph, ShaderProgram *program ) {
		auto max = static_cast< Max * >( node );

		auto a = graph->anyConnection( max->getA() );
		auto b = graph->anyConnection( max->getB() );
		auto value = max->getValue();
		auto valueType = getOutletTypeStr( value );

		auto line = valueType + " " + value->getUniqueName() + " = max( " + a->getUniqueName() + ", " + b->getUniqueName() + " );";

		_mainSection.add( line );
	};

	_translators[ Subtract::__CLASS_NAME ] = [ this ]( Node *node, ShaderGraph *graph, ShaderProgram *program ) {
		auto sub = static_cast< Subtract * >( node );

		auto a = graph->anyConnection( sub->getA() );
		auto b = graph->anyConnection( sub->getB() );
		auto value = sub->getValue();
		auto valueType = getOutletTypeStr( value );

		auto line = valueType + " " + value->getUniqueName() + " = " + a->getUniqueName() + " - " + b->getUniqueName() + ";";

		_mainSection.add( line );
	};

	_translators[ Normalize::__CLASS_NAME ] = [ this ]( Node *node, ShaderGraph *graph, ShaderProgram *program ) {
		auto n = static_cast< Normalize * >( node );

		auto x = graph->anyConnection( n->getInputValue() );
		auto ret = n->getNormalized();
		auto retType = getOutletTypeStr( ret );

		std::stringstream ss;
		ss << retType << " " << ret->getUniqueName()
		   << " = "
		   << "normalize( " << x->getUniqueName() << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ Negate::__CLASS_NAME ] = [ this ]( Node *node, ShaderGraph *graph, ShaderProgram *program ) {
		auto n = static_cast< Negate * >( node );

		auto x = graph->anyConnection( n->getInputValue() );
		auto ret = n->getNegated();
		auto retType = getOutletTypeStr( ret );

		std::stringstream ss;
		ss << retType << " " << ret->getUniqueName()
		   << " = "
		   << "-" << x->getUniqueName() << ";";
		_mainSection.add( ss.str() );
	};

	_translators[ Pow::__CLASS_NAME ] = [ this ]( Node *node, ShaderGraph *graph, ShaderProgram *program ) {
		auto op = static_cast< Pow * >( node );

		auto base = graph->anyConnection( op->getBase() );
		auto exp = graph->anyConnection( op->getExponent() );
		auto ret = op->getValue();
		auto retType = getOutletTypeStr( ret );

		std::stringstream ss;
		ss << retType << " " << ret->getUniqueName()
		   << " = "
		   << "pow( " << base->getUniqueName() << ", " << exp->getUniqueName() << " );";
		_mainSection.add( ss.str() );
	};

	_translators[ nodes::Vector::__CLASS_NAME ] = [ this ]( Node *node, ShaderGraph *graph, ShaderProgram *program ) {
		auto vector = static_cast< nodes::Vector * >( node );

		if ( graph->isConnected( vector->getOutputXYZW() ) ) {
			std::stringstream ss;
			ss << "vec4 " << vector->getOutputXYZW()->getUniqueName()
			   << " = ";

			if ( graph->isConnected( vector->getInputXYZW() ) ) {
				auto inXYZW = graph->anyConnection( vector->getInputXYZW() );
				ss << inXYZW->getUniqueName();
			}
			else if ( graph->isConnected( vector->getInputXYZ() ) ) {
				auto inXYZ = graph->anyConnection( vector->getInputXYZ() );
				std::string w = "1.0";
				if ( auto inW = graph->anyConnection( vector->getInputW() ) ) {
					w = inW->getUniqueName();
				}

				ss << "vec4( " << inXYZ->getUniqueName() << ", " << w << " )";
			}
			else {
				auto v = vector->getConstant();
				ss << "vec4( " << v.x() << ", " << v.y() << ", " << v.z() << ", " << v.w() << " )";
			}
			
			ss << ";";
			_mainSection.add( ss.str() );
		}
		else if ( graph->isConnected( vector->getOutputXYZ() ) ) {
			std::stringstream ss;
			ss << "vec3 " << vector->getOutputXYZ()->getUniqueName()
			   << " = ";
			if ( graph->isConnected( vector->getInputXYZW() ) ) {
				auto inXYZW = graph->anyConnection( vector->getInputXYZW() );
				ss << inXYZW->getUniqueName() << ".xyz";
			}
			else if ( graph->isConnected( vector->getInputXYZ() ) ) {
				auto inXYZ = graph->anyConnection( vector->getInputXYZ() );
				ss << inXYZ->getUniqueName();
			}
			else {
				auto v = vector->getConstant();
				ss << "vec3( " << v.x() << ", " << v.y() << ", " << v.z() << " )";
			}
			ss << ";";
			_mainSection.add( ss.str() );
		}
	};

	_translators[ Scalar::__CLASS_NAME ] = [ this ]( Node *node, ShaderGraph *graph, ShaderProgram *program ) {
		auto scalar = static_cast< Scalar * >( node );

		if ( graph->isConnected( scalar->getOutputValue() ) ) {
			std::stringstream ss;
			ss << getOutletTypeStr( scalar->getOutputValue() ) << " " << scalar->getOutputValue()->getUniqueName()
			   << " = ";
			if ( graph->isConnected( scalar->getInputValue() ) ) {
				auto inValue = graph->anyConnection( scalar->getInputValue() );
				ss << inValue->getUniqueName();
			}
			else {
				ss << scalar->getConstant();
			}
				
			ss << ";";
			_mainSection.add( ss.str() );
		}
	};
}

OpenGLShaderBuilder::~OpenGLShaderBuilder( void )
{

}

std::string OpenGLShaderBuilder::generateShaderSource(
	containers::Array< Node * > const &nodes,
	ShaderGraph *graph,
	ShaderProgram *program )
{
	_inputsSection.clear();
	_uniformsSection.clear();
	_outputsSection.clear();
	_globalsSection.clear();
	_mainSection.clear();
	
	nodes.each( [ this, graph, program ]( Node *node ) {
		if ( auto translator = _translators[ node->getClassName() ] ) {
			translator( node, graph, program );
		}
		else {
			Log::warning( CRIMILD_CURRENT_CLASS_NAME, "No valid translator for type ", node->getClassName() );
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
	_mainSection.each( [ &ss ]( std::string &line ) {
		ss << "\n\t" << line;
	});
	ss << "\n}\n";
	
	return ss.str();
}

std::string OpenGLShaderBuilder::getOutletTypeStr( Outlet *outlet )
{
	auto type = outlet->getType();
	
	std::string typeStr;
	switch ( type ) {
		case Outlet::Type::SCALAR:
			typeStr = "float";
			break;
			
		case Outlet::Type::VECTOR_2:
			typeStr = "vec2";
			break;
			
		case Outlet::Type::VECTOR_3:
			typeStr = "vec3";
			break;
			
		case Outlet::Type::VECTOR_4:
			typeStr = "vec4";
			break;
			
		case Outlet::Type::MATRIX_3:
			typeStr = "mat3";
			break;
			
		case Outlet::Type::MATRIX_4:
			typeStr = "mat4";
			break;
			
		default:
			typeStr = "unknown";
			break;
	}
	
	return typeStr;
}

