import QtQuick 2.12
import QtQuick.Controls 2.12
import QtWebView 1.15

Item {
    width: parent.width; height: parent.height
    clip: true

    Item {
        id: web
        anchors.fill: parent

        property string accessToken: auth.isAuthenticated && Boolean(auth.accessToken) ? auth.accessToken : ""
        property string studioId: virtualstudio.currentStudio >= 0 ? serverModel[virtualstudio.currentStudio].id : ""

        WebView {
            id: webEngineView
            anchors.fill: parent
            httpUserAgent: `JackTrip/${virtualstudio.versionString}`
            url: `https://${virtualstudio.apiHost}/studios/${studioId}/live?accessToken=${accessToken}`
        }
    }
}
