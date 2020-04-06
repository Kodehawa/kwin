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

import QtQuick 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls 2.14 as QQC2
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

    // Manage KCM pages
    Connections {
        target: kcm
        onEditIndexChanged: {
            if (kcm.editIndex < 0) {
                // If no rule is being edited, hide RulesEdidor page
                kcm.pop();
            } else if (kcm.depth < 2) {
                // Add the RulesEditor page if it wasn't already
                kcm.push("RulesEditor.qml");
            }
        }
    }

    view: ListView {
        id: ruleBookView
        clip: true

        model: kcm.ruleBookModel
        delegate: Kirigami.DelegateRecycler {
            width: ruleBookView.width
            sourceComponent: ruleBookDelegate
        }
        currentIndex: kcm.editIndex

        Rectangle {
            id: dropIndicator
            x: 0
            z: 100
            width: parent.width
            height: Kirigami.Units.smallSpacing
            color: Kirigami.Theme.highlightColor
            visible: (dropIndex >= 0) && (dropIndex != dragIndex)

            function reposition() {
                if (dropIndex >= 0) {
                    var dropItem = ruleBookView.itemAtIndex(dropIndex);
                    dropIndicator.y = ruleBookView.contentItem.y
                                        + ((dropIndex < dragIndex) ? dropItem.y : dropItem.y + dropItem.height);
                }
            }
            Connections {
                target: rulesListKCM
                onDropIndexChanged: dropIndicator.reposition();
            }
            Connections {
                target: ruleBookView.contentItem
                onYChanged: dropIndicator.reposition();
            }
        }
    }

    footer: Kirigami.ActionToolBar {
        Layout.fillWidth: true
        alignment: Qt.AlignRight
        flat: false

        actions: [
            Kirigami.Action {
                text: i18n("Import")
                iconName: "document-import"
                onTriggered: {
                    importDialog.active = true;
                }
            }
            ,
            Kirigami.Action {
                text: i18n("New")
                iconName: "list-add-symbolic"
                onTriggered: {
                    kcm.createRule();
                }
            }
        ]
    }

    Component {
        id: ruleBookDelegate
        Kirigami.AbstractListItem {
            id: ruleBookItem

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
                        var sourceIndex = dragIndex;
                        var destIndex = dropIndex;
                        dragIndex = -1;
                        dropIndex = -1;
                        // Execute the movement after resetting the global indices
                        if (sourceIndex >= 0 && sourceIndex != destIndex) {
                            kcm.moveRule(sourceIndex, destIndex);
                        }
                    }
                }

                QQC2.TextField {
                    id: descriptionField
                    Layout.minimumWidth: Kirigami.Units.gridUnit * 2
                    Layout.fillWidth: true
                    background: Item {}
                    horizontalAlignment: Text.AlignLeft
                    text: display
                    onEditingFinished: {
                        kcm.setRuleDescription(index, text);
                    }
                    Keys.onPressed: {
                        switch (event.key) {
                        case Qt.Key_Escape:
                            // On <Esc> key reset to model data before losing focus
                            text = display;
                        case Qt.Key_Enter:
                        case Qt.Key_Return:
                            ruleBookItem.focus = true;
                            event.accepted = true;
                            break;
                        }
                    }
                }

                Kirigami.ActionToolBar {
                    Layout.preferredWidth: maximumContentWidth + Kirigami.Units.smallSpacing
                    alignment: Qt.AlignRight
                    display: QQC2.Button.IconOnly
                    opacity: ruleBookItem.hovered ? 1 : 0
                    focus: false

                    actions: [
                        Kirigami.Action {
                            text: i18n("Edit")
                            iconName: "edit-entry"
                            onTriggered: {
                                kcm.editRule(index);
                            }
                        }
                        ,
                        Kirigami.Action {
                            text: i18n("Export")
                            iconName: "document-export"
                            onTriggered: {
                                exportDialog.index = index;
                                exportDialog.active = true;
                            }
                        }
                        ,
                        Kirigami.Action {
                            text: i18n("Delete")
                            iconName: "entry-delete"
                            onTriggered: {
                                kcm.removeRule(index);
                            }
                        }
                    ]
                }
            }
        }
    }

    FileDialogLoader {
        id: importDialog
        title: i18n("Import Rules")
        isSaveDialog: false
        onLastFolderChanged: {
            exportDialog.lastFolder = lastFolder;
        }
        onFileSelected: {
            kcm.importFromFile(path);
        }
    }

    FileDialogLoader {
        id: exportDialog
        property int index : 0
        title: i18n("Export Rules")
        isSaveDialog: true
        onLastFolderChanged: {
            importDialog.lastFolder = lastFolder;
        }
        onFileSelected: {
            kcm.exportToFile(path, index);
        }
    }
}
