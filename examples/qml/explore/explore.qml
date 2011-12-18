import Qt 4.7
import org.kde.edu.marble 0.11

Rectangle {
  id: screen
  width: 600; height: 450

  SystemPalette { id: activePalette }

  Flipable {
    id: flipable
    width: screen.width
    height: screen.height
    scale: 1
    z: 0
    transformOrigin: "Center"
    rotation: 0

    property int angle: 0
    property bool flipped: false

    MapThemeManager {
      id: themes
    }

    MarbleSettings {
        id: settings
    }

    front:

    MarbleWidget {
      id: map
      width: flipable.width
      height: flipable.height

      mapThemeId: settings.mapTheme
      activeFloatItems: [ "compass", "scalebar", "progress" ]

      FloatButton {
        id: configure
        image: "configure.svg"
        x: flipable.width - width - 10
        y: flipable.height - height - 10;

        onClicked: flipable.flipped = !flipable.flipped
      }

      FloatButton {
        id: zoom_in
        anchors.bottom: configure.top
        image: "zoom-in.svg"
        x: flipable.width - width - 10

        onClicked: map.zoomIn()
      }

      FloatButton {
        id: zoom_out
        anchors.bottom: zoom_in.top

        image: "zoom-out.svg"
        x: flipable.width - width - 10

        onClicked: map.zoomOut()
      }

      Component.onCompleted: {
          map.center.longitude = settings.quitLongitude
          map.center.latitude = settings.quitLatitude
          map.radius = settings.quitRadius
      }
    }

    back:

    Rectangle {
      x: 0; y:0
      width: screen.width;
      height: screen.height;
      color: "black"

      Component {
        id: delegate
        Item {
          id: wrapper
          width: 128+10; height: 128+25
          Column {
            x: 5; y: 10
            Image {
              id: mapimaged
              width: 128; height: 128;
              source: "image://maptheme/" + model.modelData.id
              }
            Text {
              width: parent.width
              anchors.left: model.modelData.name.right
              text: model.modelData.name;
              font.pointSize: 8
              horizontalAlignment: "AlignHCenter"
              color: "white"
            }
          }
        }
      }
      // Define a highlight component.  Just one of these will be instantiated
      // by each ListView and placed behind the current item.
      Component {
        id: highlight
        Rectangle {
          color: "lightsteelblue"
          radius: 5
        }
      }
      // The actual list
      GridView {
        id: mapListView
        width: parent.width; height: parent.height - flipback.height - 30
        cellWidth: 130; cellHeight: 150
        model: themes.mapThemes()
        delegate: delegate
        highlight: highlight
        focus: true
        clip: true
        //orientation: "Horizontal"

        MouseArea {
          id: maplistarea
          anchors.fill: parent

          onClicked: {
            var x = maplistarea.mouseX + mapListView.contentX
            var y = maplistarea.mouseY + mapListView.contentY
            mapListView.currentIndex = mapListView.indexAt( x, y )
          }
        }

        Rectangle {
            opacity: 0.5;
            anchors.top: mapListView.bottom;
            height: 6
            x: mapListView.visibleArea.xPosition * mapListView.width
            width: mapListView.visibleArea.widthRatio * mapListView.width
            color: "black"
        }
      }

      FloatButton {
        id: flipback
        image: "flipback.svg"
        x: 10
        y: flipable.height - height - 10;

        onClicked: {
          // First go back, then apply changes
          flipable.flipped = !flipable.flipped

          map.mapThemeId = themes.mapThemes()[mapListView.currentIndex].id
        }
      }
    }

    transform: Rotation {
      origin.x: flipable.width/2
      origin.y: flipable.height/2
      axis.x: 1; axis.y:0; axis.z: 0     // rotate around y-axis
      angle: flipable.angle
    }

    states: State {
      name: "back"
      PropertyChanges { target: flipable; angle: 180 }
      when: flipable.flipped
    }

    transitions: Transition {
      NumberAnimation { properties: "angle"; duration: 400 }
    }
  }
}
