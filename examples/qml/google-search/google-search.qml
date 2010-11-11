import Qt 4.7
import org.kde.edu.marble 0.11

import "search.js" as Search

Rectangle {
  width: 700
  height: 700

  SystemPalette { id: activePalette }

  Image {
    x: 113
    y: 0
    source: "marble.png"
    anchors.horizontalCenter: parent.horizontalCenter
  }

  Rectangle {
    id: searchinput
    x: 38
    y: 117
    width: 565
    height: 49
    border.color: "black"
    border.width: 1
    anchors.horizontalCenter: parent.horizontalCenter

    TextInput {
      anchors.fill: parent
      anchors.margins: 10
      id: searchterm
      text: ""
      font.pointSize: 18
      focus: true
    }
  }

  Item {
    id: buttonlayouter
    anchors.top: searchinput.bottom
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.margins: 10
    height: button1.height

    Button {
      id: button1
      anchors.right: buttonlayouter.horizontalCenter
      anchors.margins: 10
      width: 126
      height: 25
      label: "Marble Search"

      onClicked: { search.search( searchterm.text ) }
    }

    Button {
      id: button2
      anchors.left: buttonlayouter.horizontalCenter
      anchors.margins: 10
      width: 140
      height: 25
      label: "I'm Feeling Lucky"
    }
  }

  Item {
    id: mapcontainer
    width: 600
    height: 400
    anchors.horizontalCenter: buttonlayouter.horizontalCenter
    anchors.top: buttonlayouter.bottom
    anchors.margins: 30
    clip: true

    MarbleWidget {
      id: map
      width: 600
      height: 400
      activeFloatItems: [ "scalebar", "navigation" ]

      onVisibleLatLonAltBoxChanged: {
        itemdetails.visible = false
        var result = search.searchResult()
        for( var i=0; i<result.length; ++i ) {
          var x = map.screenX( result[i].longitude, result[i].latitude ) - 10
          var y = map.screenY( result[i].longitude, result[i].latitude ) - 30
          if ( x > 0 && y > 0 ) {
            Search.updateResult( i, result[i].name, x, y )
          } else {
            Search.invalidate( i )
          }
        }
      }
    }

    MarbleRunnerManager {
      id: search

      onSearchFinished: {
        var result = search.searchResult()
        Search.prepareResults(result.length)
        for( var i=0; i<result.length; ++i ) {
          var x = map.screenX( result[i].longitude, result[i].latitude ) - 10
          var y = map.screenY( result[i].longitude, result[i].latitude ) - 30
          if ( x > 0 && y > 0 ) {
            Search.createResult( i, result[i].name, x, y, mapcontainer )
          }
        }
    }
  }

    Rectangle
    {
      id: itemdetails
      scale: 0.75
      width: 140
      height: 60
      color: "yellow"
      radius: 10
      border.width: 1
      border.color: "gray"
      z: 42

      visible: false

      Text {
        id: itemdetailtext
        x: 10
        y: 5
        width: parent.width - 20
        height: parent.height - 10
        text: "Details here..."
        wrapMode: "WrapAtWordBoundaryOrAnywhere"
        clip: true
      }

      states: State {
        name: "back"
        PropertyChanges { target: itemdetails; scale: 1 }
        when: itemdetailtext.visible
      }

      transitions: Transition {
        NumberAnimation { properties: "scale"; duration: 100 }
      }
    }
  }
}
