#ifndef KMLOBJECT_H
#define KMLOBJECT_H

class KMLObject
{
 public:
    int id() const;
    void setId( int value );

    int targetId() const;
    void setTargetId( int value );

 protected:
    KMLObject();

 private:
    int m_id;
    int m_targetId;
};

#endif // KMLOBJECT_H
