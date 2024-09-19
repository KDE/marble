// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//

#ifndef SVGXMLHANDLER_H
#define SVGXMLHANDLER_H

#include <QXmlDefaultHandler>

class SVGXmlHandler : public QXmlDefaultHandler
{
public:
    explicit SVGXmlHandler(const QString &);
    // virtual bool startDocument();
    // virtual bool endDocument();
    bool startElement(const QString &, const QString &, const QString &, const QXmlAttributes &) override;
    // virtual bool endElement();

private:
    int m_header;
    int m_pointnum;
    bool m_initialized;
    QString m_targetfile;
};

#endif // SEARCHCOMBOBOX_H
