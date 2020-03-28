# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmkwinrules\")
add_definitions(-DKCMRULES)

########### next target ###############

include_directories(../../)
set(kwinrules_MOC_HDRS ../../cursor.h ../../plugins/platforms/x11/standalone/x11cursor.h)
qt5_wrap_cpp(kwinrules_MOC_SRCS ${kwinrules_MOC_HDRS})

set(kwinrules_SRCS
    ../../rulebooksettings.cpp
    optionsmodel.cpp
    ruleitem.cpp
    rulesmodel.cpp
    rulesdialog.cpp
    main.cpp
    kwinsrc.cpp
    ${kwinrules_MOC_SRCS}
)

kconfig_add_kcfg_files(kwinrules_SRCS ../../rulesettings.kcfgc)
kconfig_add_kcfg_files(kwinrules_SRCS ../../rulebooksettingsbase.kcfgc)

add_executable(kwin_rules_dialog ${kwinrules_SRCS})

set(kwin_kcm_rules_XCB_LIBS
    XCB::CURSOR
    XCB::XCB
    XCB::XFIXES
)

set(kcm_libs
    Qt5::QuickWidgets

    KF5::I18n
    KF5::WindowSystem
    KF5::XmlGui
)

if (KWIN_BUILD_ACTIVITIES)
    set(kcm_libs ${kcm_libs} KF5::Activities)
endif()

target_link_libraries(kwin_rules_dialog ${kcm_libs} ${kwin_kcm_rules_XCB_LIBS})

install(TARGETS kwin_rules_dialog DESTINATION ${LIBEXEC_INSTALL_DIR})

########### next target ###############

set(kcmkwinrules_SRCS
    kcmrules.cpp
    ${kwinrules_SRCS}
)

add_library(kcm_kwinrules MODULE ${kcmkwinrules_SRCS})

target_link_libraries(kcm_kwinrules
    Qt5::Quick

    KF5::I18n
    KF5::QuickAddons

    ${kcm_libs}
    ${kwin_kcm_rules_XCB_LIBS}
)

kcoreaddons_desktop_to_json(kcm_kwinrules "kcm_kwinrules.desktop" SERVICE_TYPES kcmodule.desktop)

########### install files ###############

# This desktop file is installed only for retrocompatibility with sycoca
install(TARGETS kcm_kwinrules DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms)
install(FILES kcm_kwinrules.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})

kpackage_install_package(package kcm_kwinrules kcms)

