#!/usr/bin/env bash
BUILD_DIR="$1"
BINARY_CREATOR="$2"
REPOGEN="$3"
WINDEPLOY="$4"
FROM_PROG=$5
TO_PROG=$6

if [[ "$BINARY_CREATOR" == "echo" ]]; then
    echo "Preparing WASM directory"
    rm -rf ${TO_PROG}
    mkdir -p ${TO_PROG}
else
    mkdir -p install
    echo "Gathering build files"
    cd install
    rm -rf  config packages
    cp -ruf ../packages packages
    cp -ruf ../config config
    cp -uf  ../Installer.ico config/Installer.ico
    cp -uf  ../Background.png config/Background.png
fi

# Update versions
#sed -e '/<!-- VERSION -->/r ../TSVersion' -e '/<!-- VERSION -->/d' config/config.xml > config/config.xml2
#awk '/<Version>/{getline ver; getline; print "<Version>" ver "</Version>"; next} 1' config/config.xml2 > config/config.xml
#rm config/config.xml2
#sed -e '/<!-- VERSION -->/r ../TSVersion' -e '/<!-- VERSION -->/d' packages/com.vendor.product/meta/package.xml > packages/com.vendor.product/meta/package.xml2
#awk '/<Version>/{getline ver; getline; print "<Version>" ver "</Version>"; next} 1' packages/com.vendor.product/meta/package.xml2 > packages/com.vendor.product/meta/package.xml
#rm packages/com.vendor.product/meta/package.xml2

# Handle platform differences
if [[ "$BINARY_CREATOR" == "echo" ]]; then
    EXE=""
    INSTALLER_EXT=""
else
    if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "cygwin" || "$OSTYPE" == "win32" ]]; then
        EXE=".exe"
        INSTALLER_EXT=".exe"
    else
        EXE=""
        INSTALLER_EXT=".run"
    fi
fi


# windeployqt is Windows-only; Linux handles Qt libs via other means
if [[ -n "$EXE" && -n "$WINDEPLOY" ]]; then
    mkdir -p packages/com.vendor.product/data
    cp -uf  $BUILD_DIR/${FROM_PROG}${EXE} packages/com.vendor.product/data/${TO_PROG}${EXE}

    cd packages/com.vendor.product/data
    $WINDEPLOY --no-translations ${TO_PROG}${EXE}
    cd ../../..
else
    if [[ "$BINARY_CREATOR" == "echo" ]]; then
        echo "Configuring WebAssmebly Install"
    else
        mkdir -p packages/com.vendor.product/data
        cp -uf  $BUILD_DIR/${FROM_PROG}${EXE} packages/com.vendor.product/data/${TO_PROG}${EXE}

        cd packages/com.vendor.product/data
        echo "Configuring linux post install"
        cp ../../../../HeroSystem.png .
        cp ../meta/installScripts.qs.linux ../meta/installScripts.qs
        cd ../../..
    fi
fi

if [[ "$BINARY_CREATOR" == "echo" ]]; then
    echo "Gathering files"
    cp -uf $BUILD_DIR/${TO_PROG}.js ${TO_PROG}
    cp -uf $BUILD_DIR/${TO_PROG}.wasm ${TO_PROG}
    cp -uf $BUILD_DIR/qtloader.js ${TO_PROG}
    cp -uf HeroSystem.png ${TO_PROG}
    cp -uf ${TO_PROG}.html ${TO_PROG}
else
    echo "Building installer"
    $BINARY_CREATOR -c config/config.xml -p packages --offline-only ${TO_PROG}Installer${INSTALLER_EXT}
fi
