import Qt 4.7

Rectangle {
    id: container

    signal clicked
    property string image: "zoom-in.svg"

    color: activePalette.button; smooth: true
    width: buttonImage.width; height: buttonImage.height
    border.width: 1; border.color: "black"; radius: 8;
    anchors.margins: 10;

    gradient: Gradient {
        GradientStop {
            position: 0.0
            color: if (mouseArea.pressed) { activePalette.light } else { "transparent" }
        }
        GradientStop {
            position: 1.0;
            color: if (mouseArea.pressed) { activePalette.light } else { activePalette.light }
        }
    }

    MouseArea { id: mouseArea; anchors.fill: parent; onClicked: container.clicked() }

    Image {
        id: buttonImage
        source: container.image
    }
}
