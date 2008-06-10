//
// C++ Implementation: jsonparser
//
// Description:
//
//
// Author:  <Shashank Singh>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "jsonparser.h"

JsonParser::JsonParser()
{
    
}


JsonParser::~JsonParser()
{
 
}

/*!
    \fn JsonParser::parse(const QString &content)
 */
QList <panoramioDataStructure> JsonParser::parse(const QString &content)
{

    QString temp="var myJSONObject = " + content;

//     myEngine.evaluate(temp);
//     dataStorage.count = myEngine.evaluate("return myJSONObject.count;").toInteger();
//     dataStorage.photo_url = myEngine.evaluate("return myJSONObject.photos[0].photo_url;").toString();
//     dataStorage.height = myEngine.evaluate("return myJSONObject.photos[0].height;").toString();
//     dataStorage.latitude = myEngine.evaluate("return myJSONObject.photos[0].latitude;").toString();
//     dataStorage.longitude = myEngine.evaluate("return myJSONObject.photos[0].longitude;").toString();
//     dataStorage.owner_id = myEngine.evaluate("return myJSONObject.photos[0].owner_id;").toString();
//     dataStorage.owner_name = myEngine.evaluate("return myJSONObject.photos[0].owner_name;").toString();
//     dataStorage.photo_id = myEngine.evaluate("return myJSONObject.photos[0].photo_id;").toString();
//     dataStorage.photo_title = myEngine.evaluate("return myJSONObject.photos[0].photo_title;").toString();
//     dataStorage.upload_date = myEngine.evaluate("return myJSONObject.photos[0].upload_date;").toString();
//     dataStorage.width  = myEngine.evaluate("return myJSONObject.photos[0].width;").toString();
//     dataStorage.photo_url=QString("http://mw2.google.com/mw-panoramio/photos/medium/1578881.jpg");          
    parsedJsonOutput.append(dataStorage);
    return parsedJsonOutput;
}

