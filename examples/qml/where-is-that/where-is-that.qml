import Qt 4.7
import org.kde.edu.marble 0.11

Rectangle {
  id: screen
  width: 800;
  height: 400

  Rectangle
  {
    id: banner
    x: 0
    y: 0
    width: screen.width
    height: 30
    color: "yellow"

    //    XmlListModel {
    //         id: questionModel
    //         source: "questions.xml"
    //         query: "/items/item"
    //         XmlRole { name: "question"; query: "question/string()" }
    //         XmlRole { name: "lon"; query: "lon/string()" }
    //         XmlRole { name: "lat"; query: "lat/string()" }
    //    }

    // @todo: XmlListModel would be nicer, but does not yet allow data access for non-views
    ListModel {
      id: questionModel
      ListElement {
        question: "Tom Sawyer paints aunt Polly's fence. Where was that?"
        lon: -91.3775
        lat: 39.704167
      }
      ListElement {
        question: "Jonathan Harker meets Count Dracula. Where?"
        lon: 25.370894
        lat: 45.517444
      }
      ListElement {
        question: "Henry Jekyll becomes Edward Hyde in...?"
        lon: -0.11832
        lat: 51.50939
      }
      ListElement {
        question: "Where did Quasimodo ring the bells?"
        lon: 2.35
        lat: 48.852778
      }
    }

    Component {
      id: questionDelegate
      Row {
        Text {
          id: questionItem
          text: question
          font { pointSize: 12; bold: true }
          color: "#606060"
        }
      }
    }

    ListView {
      id: questionView

      x: 10
      y: 5
      width: banner.width - button.width - 30
      height: 20

      visible: false
      clip: true

      model: questionModel
      delegate: questionDelegate
    }

    FloatButton
    {
      id: button
      y: 2
      anchors.left: questionView.right
      font { pointSize: 12; bold: true }
      color: "#606060"
      label: "Start"

      onClicked:
      {
        if ( screen.state == "moving" ) {
          screen.state = "solving"
        } else if ( screen.state == "solving" ) {
          if ( questionView.currentIndex < questionView.count - 1 ) {
            questionView.visible = true
            questionView.currentIndex = questionView.currentIndex + 1
            screen.state = "selecting"
            messages.text = "Select the target in the map"
          } else {
            screen.state = "finished"
            messages.text = ""
          }
        } else if ( screen.state == "finished" ) {
          questionView.currentIndex = 0
          screen.state = "selecting"
        }
        else {
          screen.state = "selecting"
        }
      }
    }
  }

  MouseArea
  {
    id: area
    anchors.top: banner.bottom
    width: screen.width
    height: screen.height - banner.height

    onClicked:
    {
      if ( screen.state == "selecting" || screen.state == "moving" ) {
        screen.state = "selecting"

        var opx = pointer.x
        var opy = pointer.y
        pointer.x = area.mouseX
        pointer.y = area.mouseY
        selection.x = pointer.x;
        selection.y = pointer.y - selection.height
        screen.state = "moving"
        messages.text = "Done? Click 'Solve'"

        var diff = (pointer.x-opx) * (pointer.x-opx);
        diff += (pointer.y-opy) * (pointer.y-opy);
        if ( diff > 2000 ) {
          animation.duration = 500
          animation.start()
        } else {
          animation.duration = 100
        }
      }
    }

    MarbleWidget {
      id: map
      width: area.width
      height: area.height

      projection: "Mercator"
      mapThemeId: "earth/plain/plain.dgml"
    }

    Image
    {
      id: selection
      x: 200
      y: 200
      source: "fixing-pin.svg"
      visible: false
    }

    Rectangle
    {
      id: pointer
      x: 200
      y: 200
      width: 1
      height: 1
      visible: false
    }

    PropertyAnimation {
      id: solutionanimation
      target: solution
      property: "scale"
      from: .01; to: 1
      duration: 1500
    }

    Image
    {
      id: solution
      x: 200
      y: 200
      visible: false
      source: "target.svg"
    }

    Rectangle
    {
      x: 5
      y: parent.height - 40
      width: 300
      height: 30
      radius: 5
      color: "yellow"

      Text
      {
        id: messages
        x: 10
        y: 5
        width: 280
        text: "Where is that?"
        font { pointSize: 12; bold: true }
        color: "#606060"
        visible: true
      }
    }
  }

  PropertyAnimation {
    id: animation
    target: selection
    property: "scale"
    from: .1; to: 1
    duration: 500
  }

  function calculateSolution()
  {
    var lon = questionModel.get(questionView.currentIndex).lon
    var lat = questionModel.get(questionView.currentIndex).lat
    solution.x = map.screenX( lon, lat )
    solution.y = map.screenY( lon, lat ) - solution.height
  }

  function solve()
  {
    var flon = questionModel.get(questionView.currentIndex).lon
    var flat = questionModel.get(questionView.currentIndex).lat
    var coordinate = map.coordinate( pointer.x, pointer.y )
    var dist = coordinate.distance( flon, flat ) / 1000
    messages.text = "Target distance: " + dist.toFixed(1) + " km"
  }

  states: [
  State {
    name: "selecting"
    PropertyChanges { target: questionView; visible: true }
    PropertyChanges { target: button; label: "Solve" }
    PropertyChanges { target: button; visible: false }
    PropertyChanges { target: solution; visible: false }
    PropertyChanges { target: map; inputEnabled: false }
    PropertyChanges { target: selection; visible: false }
    StateChangeScript{ script: calculateSolution(); }
  },
  State {
    name: "moving"
    PropertyChanges { target: questionView; visible: true }
    PropertyChanges { target: button; label: "Solve" }
    PropertyChanges { target: button; visible: true }
    PropertyChanges { target: solution; visible: false }
    PropertyChanges { target: map; inputEnabled: false }
    PropertyChanges { target: selection; visible: true; }
  },
  State {
    name: "solving"
    PropertyChanges { target: questionView; visible: true }
    PropertyChanges { target: button; label: "Next" }
    PropertyChanges { target: button; visible: true }
    PropertyChanges { target: solution; visible: true }
    PropertyChanges { target: map; inputEnabled: false }
    PropertyChanges { target: selection; visible: true }
    StateChangeScript{ script: solutionanimation.start(); }
    StateChangeScript{ script: solve(); }
  },
  State {
    name: "finished"
    PropertyChanges { target: questionView; visible: true }
    PropertyChanges { target: button; label: "Try Again" }
    PropertyChanges { target: button; visible: true }
    PropertyChanges { target: solution; visible: false }
    PropertyChanges { target: messages; text: "" }
    PropertyChanges { target: map; inputEnabled: true }
    PropertyChanges { target: selection; visible: false }
  }
  ]

  transitions: [
  Transition {
    id: movetransition
    from: "selecting"
    to: "moving"
    reversible: true
    NumberAnimation {
      target: selection
      properties: "x,y"
      duration: animation.duration
    }
  }
  ]

}
