//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2017      Sergey Popov <sergobot@protonmail.com>
//

#ifndef OSMCSYMBOL_H
#define OSMCSYMBOL_H

#include <QColor>
#include <QImage>
#include <QString>
#include <QSvgRenderer>

class OsmcSymbol
{
public:
    explicit OsmcSymbol(const QString &tag, int size = 20);
    ~OsmcSymbol();

    QImage icon() const;
    QColor wayColor() const;

private:
    bool parseTag(const QString &tag);
    bool parseBackground(const QString &bg);
    QSvgRenderer* parseForeground(const QString &fg);

    void render();

    QColor  m_wayColor;
    QColor  m_backgroundColor;
    QString m_backgroundType;
    QSvgRenderer *m_foreground;
    QSvgRenderer *m_foreground2;
    QString m_text;
    QColor  m_textColor;

    QImage m_image;

    QStringList m_backgroundTypes;
    QStringList m_foregroundTypes;
    QStringList m_precoloredForegroundTypes;

    int const m_side;
    int m_wayWidth;
};

#endif // OSMCSYMBOL_H
