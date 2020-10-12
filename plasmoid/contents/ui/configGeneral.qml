import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.12
import org.kde.kirigami 2.4 as Kirigami

Kirigami.FormLayout {
    id: page
    width: childrenRect.width
    height: childrenRect.height

    property alias cfg_showWorkspaceNames: showWorkspaceNames.checked
    property alias cfg_filterByCurrentScreen: filterByCurrentScreen.checked

    CheckBox {
        id: showWorkspaceNames
        text: i18n("Show workspace names")
        checked: plasmoid.configuration.showWorkspaceNames
        onCheckedChanged: plasmoid.configuration.showWorkspaceNames = checked
    }

    CheckBox {
        id: filterByCurrentScreen
        text: i18n("Filter workspaces by current screen")
        checked: plasmoid.configuration.filterByCurrentScreen
        onCheckedChanged: plasmoid.configuration.filterByCurrentScreen = checked
    }
}