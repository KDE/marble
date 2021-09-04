// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//


#ifndef SVGXMLHANDLER_H
#define SVGXMLHANDLER_H


#include <QString>
#include <QDataStream>
#include <QXmlDefaultHandler>


class SVGXmlHandler : public QXmlDefaultHandler
{
 public:
    SVGXmlHandler( QDataStream * out, const QString & path, int header );
    // virtual bool startDocument();
    // virtual bool endDocument();
    bool  startElement(const QString&, const QString&, const QString&,
                               const QXmlAttributes&) override;
    // virtual bool endElement();

 private:
    QDataStream * m_stream;
    int      m_header;
    QString  m_path;
};


#endif // SVGXMLHANDLER_H
