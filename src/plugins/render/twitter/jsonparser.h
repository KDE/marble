// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Shashank Singh <shashank.personal@gmail.com>
//

#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <QDebug>
#include <QList>
#include <QScriptEngine>
#include <QScriptValue>
#include <QtScript>
/**
This is a generic class built up for parsing Json that is JavaScript Object Notification
FIXME: the class presently has no sanity checking mechanism, it just can't check whether the input given to it is only JSON or Javascript ; a point of potential
breach for the software. sanity checking would include : (1)regex matching for following characters "" {} , [a-zA-Z] everything else should be discarded (but
some unicode names could pose problem) (2)checking for javascript constructs and eliminating them. (3)some other plan that i have yet not thought upon :)
    @author Shashank Singh
*/
struct twitterDataStructure {
    // defining the only parts that are required ,, all oher fields are hence left
    QString user;
    QString location;
    QString text;
};

struct googleMapDataStructure {
    qreal lat;
    qreal lon;
};

class jsonParser
{
public:
    jsonParser();

    ~jsonParser();

    twitterDataStructure parseObjectOnPosition(const QString &content, int requiredObjectPosition); // for parsing single object

    QList<twitterDataStructure> parseAllObjects(const QString &content, int numberOfObjects); // for parsing a list objects

    googleMapDataStructure geoCodingAPIparseObject(QString content); // google geocoding api parser

private:
    QList<twitterDataStructure> parsedJsonOutput;

    twitterDataStructure dataStorage;

    QScriptEngine myEngine;
};

#endif
