//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#ifndef MAPTHEME_H
#define MAPTHEME_H


#include <QtCore/QObject>
#include <QtGui/QColor>

#include "marble_export.h"

class QStandardItemModel;


typedef struct
{
    bool     enabled;
    QString  type;
    QString  name;
    QString  dem;
} DgmlLayer;


class QDomElement;
class LegendSection;
class LegendItem;


class MapTheme : public QObject
{
    Q_OBJECT

public:
    MapTheme(QObject *parent = 0);
    int open( const QString& path );

    QString name()          const { return m_name;        }
    QString prefix()        const { return m_prefix;      }
    QString icon()          const { return m_icon;        }
    QColor  labelColor()    const { return m_labelColor;  }
    int     minimumZoom()   const { return m_minimumZoom; }
    int     maximumZoom()   const { return m_maximumZoom; }

    QColor oceanColor()         const { return m_oceanColor;         }
    QColor landColor()          const { return m_landColor;          }
    QColor countryBorderColor() const { return m_countryBorderColor; }
    QColor stateBorderColor()   const { return m_stateBorderColor;   }
    QColor lakeColor()          const { return m_lakeColor;          }
    QColor riverColor()         const { return m_riverColor;         }

    QString tilePrefix()    const { return m_prefix;      }
    QString description()   const { return m_description; }
    QString installMap()    const { return m_installmap;  }

    DgmlLayer bitmaplayer() const { return m_bitmaplayer; }
    DgmlLayer vectorlayer() const { return m_vectorlayer; }

    int maxTileLevel()      const { return m_maxtilelevel;}

    static QStringList findMapThemes( const QString& );
    static QStandardItemModel* mapThemeModel( const QStringList& stringlist );

private:
    bool parseLegend( QDomElement &legendElement );
    bool parseLegendSection( QDomElement   &legendSectionElement,
                             LegendSection *sectionItem );
    bool parseLegendItem( QDomElement &legendItemElement,
                          LegendItem  *legendItem );


private:
    QString    m_name;
    QString    m_prefix;
    QString    m_icon;
    QColor     m_labelColor;
    int        m_minimumZoom;
    int        m_maximumZoom;
    QList<LegendSection*>  m_legend;

    QColor     m_oceanColor;
    QColor     m_landColor;
    QColor     m_countryBorderColor;
    QColor     m_stateBorderColor;
    QColor     m_lakeColor;
    QColor     m_riverColor;

    QString    m_tileprefix;
    QString    m_description;
    QString    m_installmap;
    DgmlLayer  m_vectorlayer;
    DgmlLayer  m_bitmaplayer;
    int        m_maxtilelevel;
};


#endif // MAPTHEME_H
