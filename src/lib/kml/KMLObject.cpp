#include "KMLObject.h"

KMLObject::KMLObject()
  : m_id(0),
    m_targetId(0)
{
}

int KMLObject::id() const
{
    return m_id;
}

void KMLObject::setId( int value )
{
    m_id = value;
}

int KMLObject::targetId() const
{
    return m_targetId;
}

void KMLObject::setTargetId( int value )
{
    m_targetId = value;
}
