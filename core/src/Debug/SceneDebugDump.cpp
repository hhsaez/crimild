#include "SceneDebugDump.hpp"

#include "SceneGraph/Node.hpp"
#include "SceneGraph/Group.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Text.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Light.hpp"

#include "Primitives/Primitive.hpp"

#include "Simulation/FileSystem.hpp"

#include <sstream>

using namespace crimild;

SceneDebugDump::SceneDebugDump( std::string filename )
    : _output( FileSystem::getInstance().pathForResource( filename ), std::ios::out )
{

}

SceneDebugDump::~SceneDebugDump( void )
{

}

void SceneDebugDump::traverse( Node *node )
{
    _output << "\n*****************"
            << "\n* Traversing... *"
            << "\n*****************";

    NodeVisitor::traverse( node );

    _output << "\n********"
            << "\n* DONE *"
            << "\n********";
}

void SceneDebugDump::visitNode( Node *node )
{
    NodeVisitor::visitNode( node );

    dumpNode( node, "Node" );
}

void SceneDebugDump::visitGroup( Group *group )
{
    dumpNode( group, "Group" );

    ++_parentLevel;
    NodeVisitor::visitGroup( group );
    --_parentLevel;
}

void SceneDebugDump::visitGeometry( Geometry *geometry )
{
    dumpNode( geometry, "Geometry" );


    std::stringstream indent;
    for ( int i = 0; i < _parentLevel + 1; i++ ) {
        indent << " ";
    }

	geometry->forEachPrimitive( [ this, &indent ]( Primitive *primitive ) {
		_output << "\n";
		_output << indent.str()
				<< "(Primitive)" << " [" << &primitive << "]"
				<< " Vertices: " << primitive->getVertexData()[ 0 ]->getVertexCount()
				<< " Indices: " << primitive->getIndices()->getIndexCount();
	});
}

void SceneDebugDump::visitText( Text *text )
{
    dumpNode( text, "Text" );
}

void SceneDebugDump::visitCamera( Camera *camera )
{
    dumpNode( camera, "Camera" );

    ++_parentLevel;
    NodeVisitor::visitGroup( camera );
    --_parentLevel;
}

void SceneDebugDump::visitLight( Light *light )
{
    dumpNode( light, "Light" );
}

void SceneDebugDump::dumpNode( Node *node, std::string type )
{
    _output << "\n";
    std::stringstream indent;
    for ( int i = 0; i < _parentLevel; i++ ) {
        indent << " ";
    }

    _output << indent.str()
            << "(" + type + ") " << node->getName() << " [" << &node << "]"
            << "\t" << node->getWorld().getTranslate();
}
