#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

class QDataStream;

class Serializable
{
 public:
    virtual ~Serializable() {}

    virtual void pack( QDataStream& stream ) const = 0;
    virtual void unpack( QDataStream& stream ) = 0;
};

#endif
