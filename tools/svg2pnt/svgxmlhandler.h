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
#define SVGXMLHANDLER_H


#include <QXmlDefaultHandler>


class SVGXmlHandler : public QXmlDefaultHandler
{
 public:
    explicit SVGXmlHandler(const QString&);
    // virtual bool startDocument();
    // virtual bool endDocument();
    bool  startElement(const QString&, const QString&, const QString&,
                               const QXmlAttributes&) override;
    // virtual bool endElement();

 private:
    int      m_header;
    int      m_pointnum;
    bool     m_initialized;
    QString  m_targetfile;
};


#endif // SEARCHCOMBOBOX_H
