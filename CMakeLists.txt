cmake_minimum_required(VERSION 3.19)
project(HSCCU LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

set(CMAKE_AUTOMOC ON)
include(GNUInstallDirs)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Gui PrintSupport)
find_package(Qt6 OPTIONAL_COMPONENTS Widgets)

set(source_files
    main.cpp
    character.cpp
    characteristic.cpp
    complication.cpp
    complicationsdialog.cpp
    complicationsmenudialog.cpp
    editmenudialog.cpp
    Equipment.cpp
    filemenudialog.cpp
    fraction.cpp
    imgmenudialog.cpp
    main.cpp
    modifier.cpp
    modifiersdialog.cpp
    option.cpp
    optiondialog.cpp
    powerdialog.cpp
    PowerMenuDialog.cpp
    powers.cpp
    printwindow.cpp
    sheet.cpp
    skilldialog.cpp
    skillmenudialog.cpp
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
    complicationsmenudialog.h
    CustomSkill.h
    Debug.h
    DefensePowers.h
    Dependence.h
    Dependent.h
    DistinctiveFeature.h
    editmenudialog.h
    Equipment.h
    Enraged.h
    filemenudialog.h
    fraction.h
    FrameworkPowers.h
    Hunted.h
    imgmenudialog.h
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
    PowerMenuDialog.h
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
    skillmenudialog.h
    skilltalentorperk.h
    SocialComplication.h
    SpecialPowers.h
    StandardPowers.h
    Susceptibility.h
    Talent.h
    Unluck.h
    Vulnerability.h
    wasm.h
   )

set(form_files
    complicationsdialog.ui
    complicationsmenudialog.ui
    editmenudialog.ui
    filemenudialog.ui
    imgmenudialog.ui
    modifiersdialog.ui
    optiondialog.ui
    powerdialog.ui
    PowerMenuDialog.ui
    printwindow.ui
    sheet.ui
    skilldialog.ui
    skillmenudialog.ui
    wasm.ui
   )

qt_add_executable(HSCCU WIN32 MACOSX_BUNDLE
    ${source_Files}
    ${header_files}
    ${form_files}
    res.qrc
)

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
    target_include_directories(HSCCU PRIVATE
        C:/Users/chris/src/include
    )
endif()

if(WASM)
    target_sources(HSCCU PUBLIC
        PowerMenuDialog.cpp PowerMenuDialog.h PowerMenuDialog.ui
        complicationsmenudialog.cpp complicationsmenudialog.h complicationsmenudialog.ui
        editmenudialog.cpp editmenudialog.h editmenudialog.ui
        filemenudialog.cpp filemenudialog.h filemenudialog.ui
        imgmenudialog.cpp imgmenudialog.h imgmenudialog.ui
        skillmenudialog.cpp skillmenudialog.h skillmenudialog.ui
        wasm.h wasm.ui
    )

    target_include_directories(HSCCU PRIVATE
        C:/users/chris/src/include
    )
endif()

if(UNIX)
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
