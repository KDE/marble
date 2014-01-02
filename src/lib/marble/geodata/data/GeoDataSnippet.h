//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013 Levente Kurusa <levex@linux.com>
//

#ifndef MARBLE_GEODATASNIPPET_H
#define MARBLE_GEODATASNIPPET_H

#include <QString>

namespace Marble
{

class GeoDataSnippet
{
public:
    /**
     * Create a new snippet with the given @text text and @maxLines maximum of lines.
     */
    explicit GeoDataSnippet( const QString &text = QString() , int maxLines = 0 );

    /**
     * Return the number of lines that should be displayed at maximum. The value
     * 0 (default) means "all"
     */
    int maxLines() const;

    /**
     * Set the number of lines displayed at maximum.
     */
    void setMaxLines( int lines );

    /**
     * Returns the text that is associated with this snippet.
     */
    QString text() const;

    /**
     * Set the text that the snippet will display.
     */
    void setText( const QString &text );

private:
    QString   m_text;     // Text of the snippet
    int       m_maxLines; // max of lines that are displayed
};

}

#endif
