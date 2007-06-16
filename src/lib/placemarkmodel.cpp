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

#include "placemarkmodel.h"

#include <cmath>

#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QModelIndex>
#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtGui/QIcon>
#include <QtGui/QMessageBox>

#include "katlasdirs.h"


namespace GeoString
{

    static const QChar similar_a[] = {
        // a, à, á, â, ã, ä, å, æ
        0x0061, 0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, // Table 0
        0x0101, 0x0103, 0x0105, 0x01CE, 0x01FB, 0x01FD                  // Table 1
    };

    static const QChar similar_A[] = {
        // A, À, Á, Â, Ã, Ä, Å, Æ
        0x0041, 0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, // Table 0
        0x0100, 0x0102, 0x0104, 0x01CD, 0x01FA, 0x01FC                  // Table 1
    };

    static const QChar similar_c[] = {
        // c, ç
        0x0063, 0x00E7,                                                 // Table 0 
        0x0107, 0x0109, 0x010B, 0x010D                                  // Table 1
    };

    static const QChar similar_C[] = {
        // C, Ç
        0x0043, 0x00C7,                                                // Table 0
        0x0106, 0x0108, 0x010A, 0x010C                                 // Table 1
    };

    static const QChar similar_e[] = {
        // e, è, é, ê, ë
        0x0065, 0x00E8, 0x00E9, 0x00EA, 0x00EB,                        // Table 0 
        0x0113, 0x0115, 0x0117, 0x0119, 0x011B                         // Table 1
    };

    static const QChar similar_E[] = {
        // E, È, É, Ê, Ë
        0x0045, 0x00C8, 0x00C9, 0x00CA, 0x00CB,                        // Table 0
        0x0112, 0x0114, 0x0116, 0x0118, 0x011A                         // Table 1
    };

    static const QChar similar_i[] = {
        // i, ì, í, î, ï
        0x0069, 0x00EC, 0x00ED, 0x00EE, 0x00EF,                        // Table 0 
        0x0129, 0x012B, 0x012D, 0x012F, 0x0131, 0x01D0                 // Table 1
    };

    static const QChar similar_I[] = {
        // I, Ì, Í, Î, Ï
        0x0049, 0x00CC, 0x00CD, 0x00CE, 0x00CF,                        // Table 0
        0x0128, 0x012A, 0x012C, 0x012E, 0x0130, 0x01CF                 // Table 1
    };

    static const QChar similar_n[] = {
        // n, ñ
        0x006E, 0x00F1,                                                // Table 0 
        0x0144, 0x0146, 0x0148, 0x0149, 0x014B                         // Table 1
    };

    static const QChar similar_N[] = {
        // N, Ñ
        0x004E, 0x00D1,                                                // Table 0
        0x0143, 0x0145, 0x0147, 0x014A                                 // Table 1
    };

    static const QChar similar_o[] = {
        // o, ò, ó, ô, õ, ö, ø
        0x006F, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F8,        // Table 0 
        0x014D, 0x014F, 0x0151, 0x0153, 0x01A1, 0x01D2, 0x01FF         // Table 1 
    };

    static const QChar similar_O[] = {
        // O, Ò, Ó, Ô, Õ, Ö, Ø
        0x004F, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D8,        // Table 0
        0x014C, 0x014E, 0x0150, 0x0152, 0x01A0, 0x01D1, 0x01FE         // Table 1 
    };

    static const QChar similar_s[] = {
        0x0073, 0x015A, 0x015C, 0x015E, 0x0160                         // Table 1 
    };

    static const QChar similar_S[] = {
        0x0053, 0x015B, 0x015D, 0x015F, 0x0161                         // Table 1 
    };

    static const QChar similar_t[] = {
        0x0074, 0x0163, 0x0165, 0x0167                                 // Table 1 
    };

    static const QChar similar_T[] = {
        0x0054, 0x0162, 0x0164, 0x0166                                 // Table 1 
    };

    static const QChar similar_u[] = {
        // u, ù, ú, û, ü
        0x0075, 0x00F9, 0x00FA, 0x00FB, 0x00FC,                        // Table 0 
        0x0169, 0x016B, 0x016D, 0x016F, 0x0171, 0x0173, 0x01B0, 
        0x01D4, 0x01D6, 0x01D8, 0x01DA, 0x01DC                         // Table 1
    };

    static const QChar similar_U[] = {
        // U, Ù, Ú, Û, Ü
        0x0055, 0x00D9, 0x00DA, 0x00DB, 0x00DC,                        // Table 0
        0x0168, 0x016A, 0x016C, 0x016E, 0x0170, 0x0172, 0x01AF, 
        0x01D3, 0x01D5, 0x01D7, 0x01D9, 0x01DB                         // Table 1
    };

    static const QChar similar_y[] = {
        // y, ý, ÿ
        0x0079, 0x00FD, 0x00FF,                                        // Table 0 
        0x0177                                                         // Table 1
    };

    static const QChar similar_Y[] = {
        // Y, Ý
        0x0059, 0x00DD,                                                // Table 0
        0x0176, 0x0178                                                 // Table 1
    };

    QString deaccent( const QString& accentString )
    {
        QString    result = accentString;
        const int  csize  = sizeof(QChar);

        // TODO: instead of doing an infinite amount of replacing, 
        //       check whether each's letter is below 0x007b and
        //       only if it's  

        for ( uint i = 1; i < sizeof(similar_a)/csize; ++i )
            result.replace( similar_a[i], similar_a[0] );

        for ( uint i = 1; i < sizeof(similar_A)/csize; ++i )
            result.replace( similar_A[i], similar_A[0] );

        for ( uint i = 1; i < sizeof(similar_c)/csize; ++i )
            result.replace( similar_c[i], similar_c[0] );

        for ( uint i = 1; i < sizeof(similar_C)/csize; ++i )
            result.replace( similar_C[i], similar_C[0] );

        for ( uint i = 1; i < sizeof(similar_e)/csize; ++i )
            result.replace( similar_e[i], similar_e[0] );

        for ( uint i = 1; i < sizeof(similar_E)/csize; ++i )
            result.replace( similar_E[i], similar_E[0] );

        for ( uint i = 1; i < sizeof(similar_i)/csize; ++i )
            result.replace( similar_i[i], similar_i[0] );

        for ( uint i = 1; i < sizeof(similar_I)/csize; ++i )
            result.replace( similar_I[i], similar_I[0] );
#if 0
        for ( uint i = 1; i < sizeof(similar_n)/csize; ++i )
            result.replace( similar_n[i], similar_n[0] );

        for ( uint i = 1; i < sizeof(similar_N)/csize; ++i )
            result.replace( similar_N[i], similar_N[0] );
#endif
        for ( uint i = 1; i < sizeof(similar_o)/csize; ++i )
            result.replace( similar_o[i], similar_o[0] );

        for ( uint i = 1; i < sizeof(similar_O)/csize; ++i )
            result.replace( similar_O[i], similar_O[0] );
#if 0
        for ( uint i = 1; i < sizeof(similar_s)/csize; ++i )
            result.replace( similar_s[i], similar_s[0] );

        for ( uint i = 1; i < sizeof(similar_S)/csize; ++i )
            result.replace( similar_S[i], similar_S[0] );

        for ( uint i = 1; i < sizeof(similar_t)/csize; ++i )
            result.replace( similar_t[i], similar_t[0] );

        for ( uint i = 1; i < sizeof(similar_T)/csize; ++i )
            result.replace( similar_T[i], similar_T[0] );
#endif
        for ( uint i = 1; i < sizeof(similar_u)/csize; ++i )
            result.replace( similar_u[i], similar_u[0] );

        for ( uint i = 1; i < sizeof(similar_U)/csize; ++i )
            result.replace( similar_U[i], similar_U[0] );
#if 0
        for ( uint i = 1; i < sizeof(similar_y)/csize; ++i )
            result.replace( similar_y[i], similar_y[0] );

        for ( uint i = 1; i < sizeof(similar_Y)/csize; ++i )
            result.replace( similar_Y[i], similar_Y[0] );
#endif
        return result;
    }
}


PlaceMarkModel::PlaceMarkModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

PlaceMarkModel::~PlaceMarkModel(){
}


int PlaceMarkModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED( parent );

    return m_placemarkindex.size();
}

int PlaceMarkModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED( parent );

    return 1;
}

PlaceMark* PlaceMarkModel::placeMark(const QModelIndex &index) const
{
    if ( !index.isValid() )
        return 0;

    if ( index.row() >= m_placemarkindex.size() )
        return 0;

    return m_placemarkindex.at( index.row() );
}

QVariant PlaceMarkModel::data(const QModelIndex &index, int role) const
{
    if ( !index.isValid() )
	return QVariant();

    if ( index.row() >= m_placemarkindex.size() )
        return QVariant();

    if ( role == Qt::DisplayRole ) {
        return m_placemarkindex.at( index.row() )->name();
    }
    if ( role == Qt::DecorationRole ) {
        return m_placemarkindex.at( index.row() )->symbolPixmap();
    }
    else
        return QVariant();
}

QVariant PlaceMarkModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const
{
    if ( role != Qt::DisplayRole )
        return QVariant();

    if ( orientation == Qt::Horizontal )
        return QString( "Column %1" ).arg( section );
    else
        return QString( "Row %1" ).arg( section );
}

void PlaceMarkModel::setContainer(PlaceMarkContainer* container)
{
    PlaceMarkContainer::const_iterator  it;

    for ( it=container->constBegin(); it != container->constEnd(); it++ ) {
        m_placemarkindex << *it;
    }
    qStableSort( m_placemarkindex.begin(), m_placemarkindex.end(), nameSort );
}

QModelIndexList PlaceMarkModel::match( const QModelIndex & start, int role, 
                                       const QVariant & value, int hits,
                                       Qt::MatchFlags flags ) const
{
    QList<QModelIndex> results;

    int      count = 0;
    QString  listName;
    QString  queryString;
    QString  simplifiedListName;
    int      row = start.row();

    while ( row < rowCount() && count != hits ) {
        if ( flags & Qt::MatchStartsWith ) {
            listName    = data(index( row, 0 ), role).toString();
            queryString = value.toString();
            simplifiedListName = GeoString::deaccent(listName);

            if ( listName.startsWith( queryString ) 
                 || listName.remove( QChar('\''), Qt::CaseSensitive ).startsWith( queryString )
                 || listName.replace( QChar('-'), QChar(' ') ).startsWith( queryString )
                 || GeoString::deaccent( simplifiedListName ).startsWith( queryString )
                 )
            {
                results << index( row, 0 );

                ++count;
            }
        }

        ++row;
    }

    return results;
}


#include "placemarkmodel.moc"
