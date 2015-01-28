import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Slider {
    id: slider
    minimumValue: 0
    maximumValue: 50
    stepSize: 1
    onValueChanged: {
        print( value )
    }
}