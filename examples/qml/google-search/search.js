var component
var results = new Array(0)
var count = 0

function prepareResults(number)
{
  for (var i = 0; i < count; i++) {
    if (results[i] != null) {
      results[i].destroy();
    }
  }

  results = new Array(number)
  count = number
}

function createResult(index, name, x, y, parent)
{
  if (component == null)
    component = Qt.createComponent("searchresult.qml");

  if (component.status == Component.Ready) {
    var dynamicObject = component.createObject(parent);
    if (dynamicObject == null) {
      print("error creating search result");
      print(component.errorsString());
      return false;
    }
    dynamicObject.parent = mapcontainer;
    dynamicObject.x = x
    dynamicObject.y = y
    dynamicObject.description = name
    dynamicObject.label = index+1
    dynamicObject.state = "initialized"
    results[index] = dynamicObject
  } else {
    print("error loading search result ");
    print(component.errorsString());
    return false;
  }
  return true;
}

function updateResult(index, name, x, y)
{
  if ( results[index] == null ) {
    return false;
  } else {
    results[index].description = name
    results[index].x = x
    results[index].y = y
    results[index].visible = true
  }
}

function invalidate( index )
{
  if ( results[index] != null ) {
    results[index].visible = false
  }
}
