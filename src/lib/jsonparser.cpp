//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.

#include "jsonparser.h"

jsonParser::jsonParser()
{
}

jsonParser::~jsonParser()
{
}

wikipediaLinkStructure jsonParser::parseObjectOnPosition(const QString &content , int requiredObjectPosition)
{
    QString temp = "var myJSONObject =" + content;
    myEngine.evaluate(temp);
    dataStorage.wikipediaUrl = myEngine.evaluate(QString("return myJSONObject.geonames[")
                               + QString::number(requiredObjectPosition)
                               + QString("].wikipediaUrl;")) .toString();
    dataStorage.title = myEngine.evaluate(QString("return myJSONObject.geonames[")
                                          + QString::number(requiredObjectPosition)
                                          + QString("].title;")) .toString();
    dataStorage.summary = myEngine.evaluate(QString("return myJSONObject.geonames[")
                                            + QString::number(requiredObjectPosition)
                                            + QString("].summary;")) .toString();

    return dataStorage;
}

QList <wikipediaLinkStructure> jsonParser::parseAllObjects(const QString &content)
{
    QString temp = "var myJSONObject = " + content;
    int iterator = 0;//the count starts fom one
    myEngine.evaluate(temp);
    int numberOfObjects = myEngine.evaluate("return myJSONObject.geonames.length").toInteger();
    while ((iterator) < numberOfObjects) {
        dataStorage.numberOfObjects = numberOfObjects;
        dataStorage.wikipediaUrl = myEngine.evaluate(QString("return myJSONObject.geonames[")
                                   + QString::number(iterator)
                                   + QString("].wikipediaUrl;")) .toString();
        dataStorage.title = myEngine.evaluate(QString("return myJSONObject.geonames[")
                                              + QString::number(iterator)
                                              + QString("].title;")) .toString();
        dataStorage.summary = myEngine.evaluate(QString("return myJSONObject.geonames[")
                                                + QString::number(iterator)
                                                + QString("].summary;")) .toString();
        dataStorage.feature = myEngine.evaluate(QString("return myJSONObject.geonames[")
                                                + QString::number(iterator)
                                                + QString("].feature;")) .toString();
        dataStorage.distance = myEngine.evaluate(QString("return myJSONObject.geonames[")
                                                + QString::number(iterator)
                                                + QString("].distance;")) .toNumber();          

        parsedJsonOutput.insert(iterator , dataStorage);
        ++iterator;
    }

    return parsedJsonOutput;
}
