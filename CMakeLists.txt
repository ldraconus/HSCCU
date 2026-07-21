cmake_minimum_required(VERSION 3.19)
project(HSCCU LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

# C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include(GNUInstallDirs)

find_package(Qt6 REQUIRED COMPONENTS Gui PrintSupport)
find_package(Qt6 OPTIONAL_COMPONENTS Widgets)

set(source_files
    main.cpp
    character.cpp
    characteristic.cpp
    complication.cpp
    complicationsdialog.cpp
    Equipment.cpp
    fraction.cpp
    main.cpp
    modifier.cpp
    modifiersdialog.cpp
    option.cpp
    optiondialog.cpp
    powerdialog.cpp
    powers.cpp
    printwindow.cpp
    sheet.cpp
    skilldialog.cpp
    skilltalentorperk.cpp
   )

set(header_files
    AccidentalChange.h
    AdjustmentPowers.h
    AgilitySkills.h
    AttackPowers.h
    AutomatonPowers.h
    BackgroundSkills.h
    BodyAffectingPowers.h
    character.h
    characteristic.h
    CombatSkills.h
    complication.h
    complicationsdialog.h
    CustomSkill.h
    Debug.h
    DefensePowers.h
    Dependence.h
    Dependent.h
    DistinctiveFeature.h
    Equipment.h
    Enraged.h
    fraction.h
    FrameworkPowers.h
    Hunted.h
    IntellectSkills.h
    InteractionSkills.h
    MentalPowers.h
    MiscSkill.h
    modifier.h
    modifiersdialog.h
    Money.h
    MovementPowers.h
    NegativeReputation.h
    option.h
    optiondialog.h
    Perk.h
    PhysicalComplication.h
    powerdialog.h
    powers.h
    printwindow.h
    PsychologicalComplication.h
    Rivalry.h
    SenseAffectingPowers.h
    SensoryPowers.h
    shared.h
    sheet.h
    sheet_ui.h
    skilldialog.h
    SkillEnhancers.h
    skilltalentorperk.h
    SocialComplication.h
    SpecialPowers.h
    StandardPowers.h
    Susceptibility.h
    Talent.h
    Unluck.h
    Vulnerability.h
   )

set(form_files
    complicationsdialog.ui
    modifiersdialog.ui
    optiondialog.ui
    powerdialog.ui
    printwindow.ui
    sheet.ui
    skilldialog.ui
   )

if(EMSCRIPTEN)
    qt_add_executable(HSCCU WIN32 MACOSX_BUNDLE
            ${source_files}
            ${header_files}
            ${form_files}
            resizenotifier.h resizenotifier.cpp
            dialog.h dialog.cpp
            packages/com.vendor.product/meta/installScripts.qs
            packages/com.vendor.product/meta/installScripts.qs.linux
            packages/com.vendor.product/meta/license.txt
            packages/com.vendor.product/meta/package.xml
            installer.sh
            config/Background.png
            config/config.xml
            config/controller.qs
            config/style.css
            HSCCU.html
            Background.png Installer.ico
            data.qrc
    )
else()
    qt_add_executable(HSCCU WIN32 MACOSX_BUNDLE
        ${source_files}
        ${header_files}
        ${form_files}
        resizenotifier.h resizenotifier.cpp
        dialog.h dialog.cpp
        packages/com.vendor.product/meta/installScripts.qs
        packages/com.vendor.product/meta/installScripts.qs.linux
        packages/com.vendor.product/meta/license.txt
        packages/com.vendor.product/meta/package.xml
        installer.sh
        config/Background.png
        config/config.xml
        config/controller.qs
        config/style.css
        HSCCU.html
        Background.png Installer.ico
        appicon.rc
        data.qrc
)
endif()

# Resources:
set(res_resource_files
    "Champions-Banner.png"
    "Copy.ico"
    "Cut.ico"
    "DarkChampions-Banner.png"
    "Delete.ico"
    "Edit.ico"
    "FantasyHero-Banner.png"
    "HeroSystem-Banner.png"
    "HeroSystem.ico"
    "MoveDown.ico"
    "MoveUp.ico"
    "New.ico"
    "Page1.png"
    "Page2.png"
    "Page3.png"
    "Paste.ico"
    "PulpHero-Banner.png"
    "Sheet.png"
    "StarHero-Banner.png"
)

qt_add_resources(HSCCU "res"
    PREFIX
        "/gfx"
    FILES
        ${res_resource_files}
)
set(res1_resource_files
    "SegoeUIHS.ttf"
)

qt_add_resources(HSCCU "res1"
    PREFIX
        "/font"
    FILES
        ${res1_resource_files}
)
set(res2_resource_files
    "equipment.json"
)

qt_add_resources(HSCCU "res2"
    PREFIX
        "/data"
    FILES
        ${res2_resource_files}
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(HSCCU PRIVATE
        Qt::Widgets
    )
endif()

if(WIN32)
    target_compile_options(HSCCU PRIVATE /bigobj)
    target_include_directories(HSCCU PRIVATE
        E:/QtSource/include
    )
endif()

if(EMSCRIPTEN)
    target_compile_options(HSCCU PRIVATE -Wno-deprecated-literal-operator)
    target_sources(HSCCU PUBLIC
        powerMenuDialog.cpp
        powerMenuDialog.h
        powerMenuDialog.ui
        complicationsmenudialog.cpp
        complicationsmenudialog.h
        complicationsmenudialog.ui
        editmenudialog.cpp
        editmenudialog.h
        editmenudialog.ui
        filemenudialog.cpp
        filemenudialog.h
        filemenudialog.ui
        imgmenudialog.cpp
        imgmenudialog.h
        imgmenudialog.ui
        skillmenudialog.cpp
        skillmenudialog.h
        skillmenudialog.ui
        wasm.h
        wasm.ui
    )

    target_include_directories(HSCCU PRIVATE
        E:/QtSource/include
    )
elseif()
    target_compile_options(HSCCU PRIVATE -Wno-deprecated-literal-operator)
    target_include_directories(HSCCU PRIVATE
        /home/chris/src/include
    )
endif()

target_link_libraries(HSCCU
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::PrintSupport
)

install(TARGETS HSCCU
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET HSCCU
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

find_program(BASH_EXECUTABLE bash)
if (EMSCRIPTEN)
    set(BINARY_CREATOR "echo")
    set(WINDEPLOY "echo")
elseif(UNIX)
    find_program(BINARY_CREATOR
                 NAMES binarycreator
                 PATHS "/home/chris//Qt/Tools/QtInstallerFramework/4.11/bin"
    )
    set(WINDEPLOY "echo")
else()
    find_program(BASH_EXECUTABLE bash)
    find_program(BINARY_CREATOR
                 NAMES binarycreator
                 PATHS "D:/ProgramFiles/Qt/Tools/QtInstallerFramework/4.11/bin"
    )
    set(WINDEPLOY "D:/ProgramFiles/Qt/6.12.0/msvc2022_64/bin/windeployqt6.exe")
endif()

# 2. Call bash, passing the script as an argument
add_custom_target(InstallerCreator
                  COMMAND "${BASH_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/installer.sh"
                          "${CMAKE_CURRENT_BINARY_DIR}"
                          "${BINARY_CREATOR}"
                          "B-D"
                          "${WINDEPLOY}"
                          "HSCCU"
                          "HSCCU"
                  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                  DEPENDS HSCCU
)
