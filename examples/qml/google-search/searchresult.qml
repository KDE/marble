import Qt 4.7

Image {
  property string label : "A"
  property string description: "Description..."

  id: searchresult
  source: "marker.svg"
  scale: 0.2
  visible: false

  Text {
    x: 9 - width / 2
    y: 2
    text: label
    font.pixelSize: 12
  }

  State {
    name: "initialized"
    StateChangeScript{ script: searchresult.visible = true }
  }

  MouseArea {
    anchors.fill: parent

    onClicked: {
      itemdetails.visible = !itemdetails.visible
      itemdetails.x = searchresult.x + searchresult.width
      itemdetails.y = searchresult.y + searchresult.height
      itemdetailtext.text = description
    }
  }

  transitions: [
  Transition {
    SequentialAnimation {
      // Having results appear at slightly different times looks so much cooler
      PauseAnimation { duration: Math.floor(Math.random()*1500) }
      PropertyAction { target: searchresult; property: "visible"; value: true }
      NumberAnimation {
        target: searchresult
        property: "scale"
        from: 0.2; to: 1
        duration: 200
      }
    }
  }
  ]
}
