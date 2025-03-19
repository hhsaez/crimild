#ifndef CRIMILD_NEXT_NODE_
#define CRIMILD_NEXT_NODE_

#include "Next_Object.hpp"

namespace crimild::next {

    class Node : public Object {
        CRIMILD_IMPLEMENT_RTTI( crimild::next::Node )
    public:
        [[nodiscard]] inline std::shared_ptr< Node > getParent( void ) const { return m_parent.lock(); }
        inline void setParent( std::shared_ptr< Node > const &parent ) { m_parent = parent; }

    private:
        std::weak_ptr< Node > m_parent;
    };

    struct Transformable : public Object::Extension {
        CRIMILD_IMPLEMENT_RTTI( crimild::next::Transformable )
    };

    class Node3D : public Node, public std::enable_shared_from_this< Node3D > {
        CRIMILD_IMPLEMENT_RTTI( crimild::next::Node3D )
    public:
        [[nodiscard]] inline std::shared_ptr< Node3D > getParent3D( void ) const { return m_parent3D.lock(); }
        inline void setParent3D( std::shared_ptr< Node3D > const &parent3D ) { m_parent3D = parent3D; }

    private:
        std::weak_ptr< Node3D > m_parent3D;
    };

    class Group3D : public Node3D, public std::enable_shared_from_this< Group3D > {
        CRIMILD_IMPLEMENT_RTTI( crimild::next::Group3D )

    public:
        void attach( std::shared_ptr< Node3D > const &node )
        {
            m_children.push_back( node );
            // node->setParent3D( Group3D::shared_from_this() );
        }

        auto begin( void ) const
        {
            return m_children.begin();
        }

        auto end( void ) const
        {
            return m_children.end();
        }

    private:
        std::vector< std::shared_ptr< Node3D > > m_children;
    };

    class Geometry3D : public Node3D {
        CRIMILD_IMPLEMENT_RTTI( crimild::next::Geometry3D )
    public:
    };

    class Resource : public Object {
        CRIMILD_IMPLEMENT_RTTI( crimild::next::Resource )
    public:
    };

    class Material : public Resource {
        CRIMILD_IMPLEMENT_RTTI( crimild::next::Material )
    public:
    };

    class Behavior : public Node {
        CRIMILD_IMPLEMENT_RTTI( crimild::next::Behavior )
    public:
    };

    class Event : public Node {
        CRIMILD_IMPLEMENT_RTTI( crimild::next::Event )
    public:
    };

}

#endif
