#include "jsonparser.h"

jsonParser::jsonParser()
{}


jsonParser::~jsonParser()
{}

panoramioDataStructure jsonParser::parseObjectOnPosition ( const QString &content , int requiredObjectPosition )
{
    QString temp="var myJSONObject =" + content;
    myEngine.evaluate ( temp );

    dataStorage.count = myEngine.evaluate ( "return myJSONObject.count;" ).toInteger();
    
    dataStorage.height = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                         +QString::number ( requiredObjectPosition )
                         +QString ( "].height;" ) ) .toInteger();
    dataStorage.latitude = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                           +QString::number ( requiredObjectPosition )
                           +QString ( "].latitude;" ) ).toInteger();
    dataStorage.longitude = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                            +QString::number ( requiredObjectPosition )
                            +QString ( "].longitude;" ) ).toInteger();
    dataStorage.owner_id = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                           +QString::number ( requiredObjectPosition )
                           +QString ( "].owner_id;" ) ).toInteger();
    dataStorage.owner_name = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                             +QString::number ( requiredObjectPosition )
                             +QString ( "].owner_name;" ) ).toString();
    dataStorage.owner_url = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                            +QString::number ( requiredObjectPosition )
                            +QString ( "].owner_url;" ) ).toString();
    dataStorage.photo_id = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                           +QString::number ( requiredObjectPosition )
                           +QString ( "].photo_id;" ) ).toInteger();
    dataStorage.photo_title = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                              +QString::number ( requiredObjectPosition )
                              +QString ( "].photo_title;" ) ).toString();
    dataStorage.photo_url = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                            +QString::number ( requiredObjectPosition )
                            +QString ( "].photo_url;" ) ).toString();
    dataStorage.upload_date = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                              +QString::number ( requiredObjectPosition )
                              +QString ( "].upload_date;" ) ).toString();
    dataStorage.width  = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                         +QString::number ( requiredObjectPosition )
                         +QString ( "].width;" ) ).toInteger();
                               
return dataStorage;
}

QList <panoramioDataStructure> jsonParser::parseAllObjects ( const QString &content ,int numberOfObjects )
{
    QString temp="var myJSONObject =" + content;
    int iterator = 0;//the count starts fom one

    myEngine.evaluate ( temp );
    while ( ( iterator ) < numberOfObjects )
    {

        dataStorage.count = myEngine.evaluate ( "return myJSONObject.count;" ).toInteger();
        dataStorage.height = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                             +QString::number ( iterator )
                             +QString ( "].height;" ) ) .toInteger();
        dataStorage.latitude = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                               +QString::number ( iterator )
                               +QString ( "].latitude;" ) ).toInteger();
        dataStorage.longitude = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                                +QString::number ( iterator )
                                +QString ( "].longitude;" ) ).toInteger();
        dataStorage.owner_id = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                               +QString::number ( iterator )
                               +QString ( "].owner_id;" ) ).toInteger();
        dataStorage.owner_name = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                                 +QString::number ( iterator )
                                 +QString ( "].owner_name;" ) ).toString();
        dataStorage.owner_url = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                                +QString::number ( iterator )
                                +QString ( "].owner_url;" ) ).toString();
        dataStorage.photo_id = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                               +QString::number ( iterator )
                               +QString ( "].photo_id;" ) ).toInteger();
        dataStorage.photo_title = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                                  +QString::number ( iterator )
                                  +QString ( "].photo_title;" ) ).toString();
        dataStorage.photo_url = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                                +QString::number ( iterator )
                                +QString ( "].photo_url;" ) ).toString();
        dataStorage.upload_date = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                                  +QString::number ( iterator )
                                  +QString ( "].upload_date;" ) ).toString();
        dataStorage.width  = myEngine.evaluate ( QString ( "return myJSONObject.photos[" )
                             +QString::number ( iterator )
                             +QString ( "].width;" ) ).toInteger();
        parsedJsonOutput.insert ( iterator , dataStorage );
        iterator++;
    }
    return parsedJsonOutput;
}
