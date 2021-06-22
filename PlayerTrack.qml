import QtQuick 2.0

Item {
  id: root

  property string icon: ""
  property string title: ""
  property string artists: ""
  property string extra: ""
  property string idInt: ""
  property bool liked: false

  signal toggleLiked(bool liked)

  PlayerTrackIcon {
    id: _icon
    anchors.verticalCenter: root.verticalCenter

    src: icon
  }

  PlayerTrackInfo {
    id: _info
    anchors.left: _icon.right
    anchors.leftMargin: 11
    width: root.width - anchors.leftMargin - _icon.width
    height: root.height

    title: root.title
    artists: root.artists
    extra: root.extra
    idInt: root.idInt
    liked: root.liked

    onToggleLiked: root.toggleLiked(liked)
  }
}
