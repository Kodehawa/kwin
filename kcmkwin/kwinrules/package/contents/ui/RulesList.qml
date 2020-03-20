/*
 * Copyright (c) 2020 Ismael Asensio <isma.af@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12 as QQC2
import org.kde.kcm 1.2
import org.kde.kirigami 2.5 as Kirigami

ScrollViewKCM {
    id: rulesListKCM

    property int dragIndex: -1
    property int dropIndex: -1

    // FIXME: ScrollViewKCM.qml:73:13: QML Control: Binding loop detected for property "implicitHeight"
    implicitWidth: Kirigami.Units.gridUnit * 35
    implicitHeight: Kirigami.Units.gridUnit * 25

    ConfigModule.columnWidth: Kirigami.Units.gridUnit * 22
    ConfigModule.buttons: ConfigModule.Apply

    view: ListView {
        id: ruleBookView
        clip: true

        model: kcm.ruleBookModel
        delegate: Kirigami.DelegateRecycler {
            width: ruleBookView.width
            sourceComponent: ruleBookDelegate
        }
        currentIndex: kcm.editingIndex

        Rectangle {
            id: dropIndicator
            x: 0
            z: 100
            width: parent.width
            height: Kirigami.Units.smallSpacing
            color: Kirigami.Theme.highlightColor
            visible: (dropIndex >= 0) && (dropIndex != dragIndex)

            Connections {
                target: rulesListKCM
                onDropIndexChanged: {
                    if (dropIndex >= 0) {
                        // TODO: After Qt 5.13 we can use ListView.itemAtIndex(index)
                        var dropItem = ruleBookView.contentItem.children[dropIndex];
                        dropIndicator.y = (dropIndex < dragIndex) ? dropItem.y : dropItem.y + dropItem.height;
                    }
                }
            }
        }
    }

    footer: Kirigami.ActionToolBar {
        Layout.fillWidth: true
        alignment: Qt.AlignRight

        actions: [
            Kirigami.Action {
                text: i18n("Import")
                iconName: "document-import"
                onTriggered: kcm.importRules();
            }
            ,
            Kirigami.Action {
                text: i18n("New")
                iconName: "list-add-symbolic"
                onTriggered: kcm.createRule();
            }
        ]
    }

    Component {
        id: ruleBookDelegate
        Kirigami.AbstractListItem {
            id: ruleBookItem

            highlighted: kcm.editIndex == index
            onClicked: {
                kcm.editRule(index);
            }

            RowLayout {
                //FIXME: If not used within DelegateRecycler, item goes on top of the first item when clicked
                //FIXME: Improve visuals and behavior when dragging on the list.
                Kirigami.ListItemDragHandle {
                    listItem: ruleBookItem
                    listView: ruleBookView
                    onMoveRequested: {
                        dragIndex = oldIndex;
                        dropIndex = newIndex;
                    }
                    onDropped: {
                        if (dropIndex >= 0 && dropIndex != dragIndex) {
                            kcm.moveRule(dragIndex, dropIndex);
                        }
                        dragIndex = -1;
                        dropIndex = -1;
                    }
                }

                QQC2.Label {
                    text: modelData
                }

                Kirigami.ActionToolBar {
                    Layout.fillWidth: true
                    alignment: Qt.AlignRight
                    display: QQC2.Button.IconOnly
                    opacity: ruleBookItem.hovered ? 1 : 0
                    focus: false

                    actions: [
                        Kirigami.Action {
                            text: i18n("Export")
                            iconName: "document-export"
                            onTriggered: kcm.exportRule(index);
                        }
                        ,
                        Kirigami.Action {
                            text: i18n("Delete")
                            iconName: "entry-delete"
                            onTriggered: kcm.removeRule(index);
                        }
                    ]
                }
            }
        }
    }
}
