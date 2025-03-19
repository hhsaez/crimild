#ifndef CRIMILD_NEXT_NODE_
#define CRIMILD_NEXT_NODE_

#include "Object.hpp"

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
