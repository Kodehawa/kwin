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

import org.kde.kirigami 2.10 as Kirigami
import org.kde.kquickcontrols 2.0 as KQC
import org.kde.kcms.kwinrules 1.0


Loader {
    id: valueEditor
    focus: true

    property var ruleValue
    property var ruleOptions
    property int controlType

    signal valueEdited(var value)

    sourceComponent: {
        switch (controlType) {
            case RuleItem.Boolean: return booleanEditor
            case RuleItem.String: return stringEditor
            case RuleItem.Integer: return integerEditor
            case RuleItem.Option: return optionEditor
            case RuleItem.FlagsOption: return flagsEditor
            case RuleItem.Percentage: return percentageEditor
            case RuleItem.Coordinate: return coordinateEditor
            case RuleItem.Shortcut: return shortcutEditor
            default: return emptyEditor
        }
    }

    Component {
        id: emptyEditor
        Item {}
    }

    Component {
        id: booleanEditor
        RowLayout {
            Item {
                Layout.fillWidth: true
            }
            QQC2.Switch {
                text: checked ? i18n("Yes") : i18n("No")
                checked: ruleValue
                onToggled: valueEditor.valueEdited(checked)
            }
        }
    }

    Component {
        id: stringEditor
        QQC2.TextField {
            property bool isTextEdited: false
            text: ruleValue
            horizontalAlignment: Text.AlignLeft
            onTextEdited: { isTextEdited = true; }
            onEditingFinished: {
                if (isTextEdited) { valueEditor.valueEdited(text); }
                isTextEdited = false;
            }
        }
    }

    Component {
        id: integerEditor
        QQC2.SpinBox {
            editable: true
            value: ruleValue
            onValueModified: valueEditor.valueEdited(value)
        }
    }

    Component {
        id: optionEditor
        OptionsComboBox {
            flat: true
            model: ruleOptions
            onActivated: (index) => {
                valueEditor.valueEdited(currentValue);
            }
        }
    }

    Component {
        id: flagsEditor
        RowLayout {
            Layout.minimumWidth: 10 * Kirigami.Units.gridUnit
            Layout.alignment: Qt.AlignRight;
            spacing: 0
            Repeater {
                id: flagsRepeater
                model: ruleOptions

                QQC2.ToolButton {
                    property int bit: model.value
                    icon.name: model.iconName
                    checkable: true
                    checked: (ruleValue & (1 << bit)) == (1 << bit)
                    QQC2.ToolTip.text: model.text
                    QQC2.ToolTip.visible: hovered
                    onToggled: {
                        valueEditor.valueEdited((ruleValue & ~(1 << bit)) | (checked << bit));
                    }
                }
            }
        }
    }

    Component {
        id: percentageEditor
        RowLayout {
            QQC2.Slider {
                id: slider
                Layout.fillWidth: true
                from: 0
                to: 100
                value: ruleValue
                onMoved: valueEditor.valueEdited(Math.round(slider.value))
            }
            QQC2.Label {
                text: i18n("%1 %", Math.round(slider.value))
                width: 2 * Kirigami.Units.gridUnit
            }
        }
    }

    Component {
        id: coordinateEditor
        RowLayout {
            id: coordItem
            spacing: Kirigami.Units.smallSpacing

            property var coords: ruleValue ? ruleValue.split(',') : [0, 0]
            property int coordWidth: (coordItem.width - coordSeparator.width) / 2 - coordItem.spacing

            QQC2.SpinBox {
                id: coordX
                editable: true
                implicitWidth: coordWidth
                from: 0
                to: 4098
                value: coords[0]
                onValueModified: valueEditor.valueEdited(coordX.value + "," + coordY.value)
            }
            QQC2.Label {
                id: coordSeparator
                text: i18nc("(x, y) coordinates separator in size/position","x")
                horizontalAlignment: Text.AlignHCenter
            }
            QQC2.SpinBox {
                id: coordY
                editable: true
                from: 0
                to: 4098
                implicitWidth: coordWidth
                value: coords[1]
                onValueModified: valueEditor.valueEdited(coordX.value + "," + coordY.value)
            }
        }
    }

    Component {
        id: shortcutEditor
        RowLayout {
            Item {
                Layout.fillWidth: true
            }
            KQC.KeySequenceItem {
                keySequence: ruleValue
                onCaptureFinished: valueEditor.valueEdited(keySequence)
            }
        }
    }
}