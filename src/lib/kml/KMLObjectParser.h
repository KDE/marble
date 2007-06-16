//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#ifndef KMLOBJECTPARSER_H
#define KMLOBJECTPARSER_H

#include <QtXml/QXmlDefaultHandler>

class KMLObject;

class KMLObjectParser : public QXmlDefaultHandler
{
 public:
    virtual ~KMLObjectParser();

 protected:
    KMLObjectParser( KMLObject& m_object );

    KMLObject&  m_object;
    bool        m_parsed;
};

#endif // KMLOBJECTPARSER_H
