# appImage.cmake

if(NOT PROJECT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  return()
endif()

if(GENERATE_APPIMAGE)
    if(UNIX AND NOT APPLE)
        message(STATUS "(nsbaci) GENERATE_APPIMAGE is ON")

        #downlaod linuxdeploy

         add_custom_target(download-linuxdeploy ALL
            COMMAND wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage 
            COMMAND chmod +x linuxdeploy-x86_64.AppImage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

        add_custom_target(download-linuxdeploy-qt ALL
            COMMAND wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage
            COMMAND chmod +x linuxdeploy-plugin-qt-x86_64.AppImage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )

        #set paths
        set(APPIMAGE_DIR "${CMAKE_BINARY_DIR}/AppDir")
        set(APP_BIN_DIR "${APPIMAGE_DIR}/usr/bin")

        file(MAKE_DIRECTORY ${APP_BIN_DIR})
        file(MAKE_DIRECTORY ${APPIMAGE_DIR}/usr/share/metainfo)
        file(MAKE_DIRECTORY ${APPIMAGE_DIR}/usr/lib)

#         #utils
#         string(TIMESTAMP BUILD_DATE "%Y-%m-%d")
#         string(REGEX REPLACE "\\.[0-9]+$" "" VERSION_STRING_SHORT "${PROJECT_VERSION_STRING}")

#         #Add metainfo
#         file(WRITE "${APPIMAGE_DIR}/usr/share/metainfo/nsbaci.appdata.xml" "<?xml version=\"1.0\" encoding=\"UTF-8\"?>
# <component type=\"desktop-application\">
# 	<id>nsbaci</id>
# 	<metadata_license>MIT</metadata_license>
# 	<project_license>MIT</project_license>
# 	<name>nsbaci</name>
# 	<summary>Keep your workspace organized</summary>
# 	<description>
# 		<p>nsbaci is a desktop application that lets you save and load your regular workspaces on the go.</p>
# 	</description>
# 	<launchable type=\"desktop-id\">nsbaci.desktop</launchable>
# 	<url type=\"homepage\">https://linktr.ee/NicolasSerrano</url>
# 	<screenshots>
# 		<screenshot type=\"default\">
# 			<image></image>
# 		</screenshot>
# 	</screenshots>
# 	<provides>
# 		<id>nsbaci.desktop</id>
# 	</provides>
# </component>")

        #move nsbaci executable to bin
        add_custom_command(TARGET nsbaci POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:nsbaci> ${APP_BIN_DIR}/nsbaci
        )

        #create and write AppRun
        file(WRITE "${APPIMAGE_DIR}/AppRun" 
"#!/bin/bash
HERE=\"\$(dirname \"\$(readlink -f \"\$0\")\")\"

export LD_LIBRARY_PATH=\"$HERE/usr/lib:\$LD_LIBRARY_PATH\"
export QT_QPA_PLATFORM=xcb
echo \$LD_LIBRARY_PATH

FLAG=\"\$HOME/.local/share/applications/.nsbaci_installed\"

if [ ! -f \"\$FLAG\" ]; then
    \"\$HERE/integrate.sh\"
    touch \"\$FLAG\"
fi

case \"\$1\" in
    --uninstall)
        echo \"Uninstalling nsbaci...\"
        \"\$HERE/uninstall.sh\"
        exit 0
        ;;
esac

exec \"\$HERE/usr/bin/nsbaci\" \"\$@\"
")

        execute_process(COMMAND chmod +x "${APPIMAGE_DIR}/AppRun")

        #create and write .desktop
file(WRITE ${APPIMAGE_DIR}/nsbaci.desktop "[Desktop Entry]
Type=Application
Name=nsbaci
Exec=AppRun
Icon=nsbaci
Categories=Utility;
StartupWMClass=nsbaci
Terminal=false
")

          #create and write uninstall.sh
        file(WRITE ${APPIMAGE_DIR}/uninstall.sh "#!/bin/bash
TARGET_DIR=\"\$HOME/Applications\"
APP_NAME=\"nsbaci\"
LOCAL_SHARE=\"\$HOME/.local/share\"

rm -f \$TARGET_DIR/\$APP_NAME.AppImage

rm -f \$LOCAL_SHARE/icons/hicolor/16x16/apps/\$APP_NAME.png
rm -f \$LOCAL_SHARE/icons/hicolor/32x32/apps/\$APP_NAME.png
rm -f \$LOCAL_SHARE/icons/hicolor/64x64/apps/\$APP_NAME.png
rm -f \$LOCAL_SHARE/icons/hicolor/48x48/apps/\$APP_NAME.png
rm -f \$LOCAL_SHARE/icons/hicolor/96x96/apps/\$APP_NAME.png
rm -f \$LOCAL_SHARE/icons/hicolor/128x128/apps/\$APP_NAME.png
rm -f \$LOCAL_SHARE/icons/hicolor/256x256/apps/\$APP_NAME.png
rm -f \$LOCAL_SHARE/icons/hicolor/512x512/apps/\$APP_NAME.png

rm -f \$LOCAL_SHARE/applications/$APP_NAME.desktop

rm -f \"\$LOCAL_SHARE/applications/.nsbaci_installed\"

update-desktop-database ~/.local/share/applications >/dev/null 2>&1 || true")

        execute_process(COMMAND chmod +x "${APPIMAGE_DIR}/uninstall.sh")

        #move all the icons
        file(COPY ${CMAKE_SOURCE_DIR}/assets/nsbaci-16x16.png DESTINATION ${APPIMAGE_DIR})
        file(COPY ${CMAKE_SOURCE_DIR}/assets/nsbaci-32x32.png DESTINATION ${APPIMAGE_DIR})
        file(COPY ${CMAKE_SOURCE_DIR}/assets/nsbaci-48x48.png DESTINATION ${APPIMAGE_DIR})
        file(COPY ${CMAKE_SOURCE_DIR}/assets/nsbaci-64x64.png DESTINATION ${APPIMAGE_DIR})
        file(COPY ${CMAKE_SOURCE_DIR}/assets/nsbaci-96x96.png DESTINATION ${APPIMAGE_DIR})
        file(COPY ${CMAKE_SOURCE_DIR}/assets/nsbaci-128x128.png DESTINATION ${APPIMAGE_DIR})
        file(COPY ${CMAKE_SOURCE_DIR}/assets/nsbaci-256x256.png DESTINATION ${APPIMAGE_DIR})
        file(COPY ${CMAKE_SOURCE_DIR}/assets/nsbaci-512x512.png DESTINATION ${APPIMAGE_DIR})

        file(COPY ${CMAKE_SOURCE_DIR}/assets/nsbaci.png DESTINATION ${APPIMAGE_DIR})

        execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
            nsbaci.png
            ${APPIMAGE_DIR}/.DirIcon
        )

        #create and write integrate.sh

        file(WRITE "${APPIMAGE_DIR}/integrate.sh" "#!/bin/bash   

APPDIR=\"\$(dirname \"\$(readlink -f \"\$0\")\")\"

APP_NAME=\"nsbaci\"

TARGET_DIR=\"\$HOME/Applications\"

APPIMAGE_PATH=\"\${APPIMAGE:-\$\(readlink -f \"\$0\"\)}\"

LOCAL_SHARE=\"\$HOME/.local/share\"

mkdir -p \"\$TARGET_DIR\"

cp \"\$APPIMAGE_PATH\" \"\$TARGET_DIR/\$APP_NAME.AppImage\"
chmod +x \"\$TARGET_DIR/\$APP_NAME.AppImage\"

mkdir -p \$LOCAL_SHARE/applications
mkdir -p \$LOCAL_SHARE/icons/hicolor/16x16/apps
mkdir -p \$LOCAL_SHARE/icons/hicolor/32x32/apps
mkdir -p \$LOCAL_SHARE/icons/hicolor/64x64/apps
mkdir -p \$LOCAL_SHARE/icons/hicolor/48x48/apps
mkdir -p \$LOCAL_SHARE/icons/hicolor/96x96/apps
mkdir -p \$LOCAL_SHARE/icons/hicolor/128x128/apps
mkdir -p \$LOCAL_SHARE/icons/hicolor/256x256/apps
mkdir -p \$LOCAL_SHARE/icons/hicolor/512x512/apps

cat > \$LOCAL_SHARE/applications/$APP_NAME.desktop <<EOL
[Desktop Entry]
Name=\$APP_NAME
GenericName=Concurrency lab
GenericName[en]=Concurrency lab
GenericName[es]=Laboratorio de concurrencia
Exec=\$TARGET_DIR/\$APP_NAME.AppImage
Icon=\$APP_NAME
Terminal=false
Type=Application
Keywords=Desktop;Concurrency;
Categories=Utility;
Comment[en]=Learn concurrency
Comment[es]=Aprende concurrencia
StartupNotify=true
StartupWMClass=nsbaci

Actions=Uninstall;
[Desktop Action Uninstall]
Name=Uninstall
Name[en]=Uninstall
Name[es]=Desinstalar
Exec=\$TARGET_DIR/\$APP_NAME.AppImage --uninstall
EOL

chmod +x \$LOCAL_SHARE/applications/$APP_NAME.desktop


cp \"\$APPDIR/\$APP_NAME-32x32.png\" \$LOCAL_SHARE/icons/hicolor/16x16/apps/\$APP_NAME.png
cp \"\$APPDIR/\$APP_NAME-32x32.png\" \$LOCAL_SHARE/icons/hicolor/32x32/apps/\$APP_NAME.png
cp \"\$APPDIR/\$APP_NAME-64x64.png\" \$LOCAL_SHARE/icons/hicolor/64x64/apps/\$APP_NAME.png
cp \"\$APPDIR/\$APP_NAME-48x48.png\" \$LOCAL_SHARE/icons/hicolor/48x48/apps/\$APP_NAME.png
cp \"\$APPDIR/\$APP_NAME-96x96.png\" \$LOCAL_SHARE/icons/hicolor/96x96/apps/\$APP_NAME.png
cp \"\$APPDIR/\$APP_NAME-128x128.png\" \$LOCAL_SHARE/icons/hicolor/128x128/apps/\$APP_NAME.png
cp \"\$APPDIR/\$APP_NAME-256x256.png\" \$LOCAL_SHARE/icons/hicolor/256x256/apps/\$APP_NAME.png
cp \"\$APPDIR/\$APP_NAME-512x512.png\" \$LOCAL_SHARE/icons/hicolor/512x512/apps/\$APP_NAME.png

update-desktop-database \$LOCAL_SHARE/applications >/dev/null 2>&1 || true

gtk-update-icon-cache -f -t \"\$LOCAL_SHARE/icons/hicolor\" >/dev/null 2>&1 || true

echo \"\$APP_NAME installed successfully!\"")

        execute_process(COMMAND chmod +x "${APPIMAGE_DIR}/integrate.sh")

        add_custom_target(appimage ALL
            COMMAND ${CMAKE_COMMAND} -E echo "Generating appImage..."
            COMMAND ./linuxdeploy-x86_64.AppImage --appdir AppDir --executable AppDir/usr/bin/nsbaci --plugin qt --exclude-library=libc.so.* --exclude-library=libm.so.* --output appimage
            DEPENDS nsbaci download-linuxdeploy download-linuxdeploy-qt
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )

        add_custom_target(delete-linuxdeploy ALL
            COMMAND rm -f linuxdeploy-x86_64.AppImage
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
    else()
        message(STATUS "(nsbaci) You cannot generate an AppImage on Windows!")
    endif()
else()
    message(STATUS "(nsbaci) GENERATE_APPIMAGE is OFF")
endif()