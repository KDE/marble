//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Shashan Singh <shashank.personal@gmail.com>
//

#ifndef PANORAMIOPARSER_H
#define PANORAMIOPARSER_H

#include <QtScript>
#include <QList>

/**
This is a generic class built up for parsing Json that is JavaScript Object Notification
FIXME: the class presently has no sanity checking mechanism, it just can't check whether the input given to it is only JSON or Javascript ; a point of potential breach for the software.
sanity checking would include :
(1)regex matching for following characters "" {} , [a-zA-Z] everything else should be discarded (but some unicode names could pose problem)
(2)checking for javascript constructs and eliminating them.
(3)some other plan that i have yet not thought upon :)
    @author Shashank Singh
*/
struct panoramioDataStructure
{
    long int count;// Total number of photographs will be stored in this int
    long int photo_id ; // Id of each photograph
    QString photo_title; // Title of each photograph
    QString photo_url; // Url of each photograph
    QString photo_file_url;
    qreal longitude;
    qreal latitude;
    int width;
    int height;
    QDate upload_date;
    long int owner_id;
    QString owner_name;
    QString owner_url; // Url of the User Uplaoded
};

class PanoramioParser
{
public:
    PanoramioParser();

    ~PanoramioParser();

    panoramioDataStructure parseObjectOnPosition(const QString &content, int requiredObjectPosition);   //for parsing single object

    QList<panoramioDataStructure> parseAllObjects(const QString &content, int number);   //for parsing a list objects

private:
    QList <panoramioDataStructure> parsedJsonOutput;

    panoramioDataStructure dataStorage;

    QScriptEngine myEngine;
};


#endif
