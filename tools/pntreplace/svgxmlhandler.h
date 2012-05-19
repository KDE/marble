//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//


#ifndef SVGXMLHANDLER_H
#define SVGXMLHHANDLER_H


#include <QtCore/QString>
#include <QtCore/QDataStream>
#include <QtXml/QXmlDefaultHandler>


class SVGXmlHandler : public QXmlDefaultHandler
{
 public:
    SVGXmlHandler( QDataStream * out, const QString & path, int header );
    // virtual bool startDocument();
    // virtual bool endDocument();
    virtual bool  startElement(const QString&, const QString&, const QString&,
                               const QXmlAttributes&);
    // virtual bool endElement();

 private:
    QDataStream * m_stream;
    int      m_header;
    QString  m_path;
};


#endif // SVGXMLHANDLER_H
