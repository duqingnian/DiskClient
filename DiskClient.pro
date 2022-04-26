QT += core gui
QT += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

SOURCES += \
    Auth/authaccount.cpp \
    Auth/authqrcode.cpp \
    Auth/authsetting.cpp \
    Auth/authsms.cpp \
    Common/basecontroller.cpp \
    Component/BaseWindow.cpp \
    Component/FlowLayout.cpp \
    Component/Msg.cpp \
    Component/Toast.cpp \
    Dialog/dialogcreate.cpp \
    DropDown/dropdowncreate.cpp \
    DropDown/dropdownupload.cpp \
    Lib/HttpClient.cpp \
    Component/Label.cpp \
    Component/TitleBar.cpp \
    Lib/qrcodegen.cpp \
    Menu/fdmenu.cpp \
    Menu/filewelcomemenu.cpp \
    Menu/menufileexplorercanvas.cpp \
    Stack/Chat/chat.cpp \
    Stack/Chat/stackempty.cpp \
    Stack/Chat/stackpannel.cpp \
    Stack/File/fileexplorer.cpp \
    Stack/File/filewelcome.cpp \
    Stack/stackchat.cpp \
    Stack/stackfileMain.cpp \
    Stack/stackhelp.cpp \
    auth.cpp \
    main.cpp \
    welcome.cpp

HEADERS += \
    Auth/authaccount.h \
    Auth/authqrcode.h \
    Auth/authsetting.h \
    Auth/authsms.h \
    Common/basecontroller.h \
    Component/BaseWindow.h \
    Component/FlowLayout.h \
    Component/Msg.h \
    Component/Toast.h \
    Data/FD.h \
    Data/Filter.h \
    Data/MenuItem.h \
    Data/Simple.h \
    Data/TabMeta.h \
    Data/UrlMeta.h \
    Dialog/dialogcreate.h \
    DropDown/dropdowncreate.h \
    DropDown/dropdownupload.h \
    Lib/HttpClient.h \
    Component/Label.h \
    Component/TitleBar.h \
    Data/User.h \
    Lib/qrcodegen.hpp \
    Menu/fdmenu.h \
    Menu/filewelcomemenu.h \
    Menu/menufileexplorercanvas.h \
    Stack/Chat/chat.h \
    Stack/Chat/stackempty.h \
    Stack/Chat/stackpannel.h \
    Stack/File/fileexplorer.h \
    Stack/File/filewelcome.h \
    Stack/stackchat.h \
    Stack/stackfileMain.h \
    Stack/stackhelp.h \
    auth.h \
    welcome.h

FORMS += \
    Auth/authaccount.ui \
    Auth/authqrcode.ui \
    Auth/authsetting.ui \
    Auth/authsms.ui \
    Component/Msg.ui \
    Component/Toast.ui \
    Dialog/dialogcreate.ui \
    DropDown/dropdowncreate.ui \
    DropDown/dropdownupload.ui \
    Menu/fdmenu.ui \
    Menu/filewelcomemenu.ui \
    Menu/menufileexplorercanvas.ui \
    Stack/stackhelp.ui \
    auth.ui \
    welcome.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resources.qrc
RC_ICONS = ico.ico
DISTFILES += \
    Resources/16x16/add.png \
    Resources/16x16/attr.png \
    Resources/16x16/cfolder.png \
    Resources/16x16/del.png \
    Resources/16x16/dep_file.png \
    Resources/16x16/ding.png \
    Resources/16x16/down.png \
    Resources/16x16/encode_file.png \
    Resources/16x16/excel.png \
    Resources/16x16/files.png \
    Resources/16x16/folder.png \
    Resources/16x16/left.png \
    Resources/16x16/location.png \
    Resources/16x16/move.png \
    Resources/16x16/office.png \
    Resources/16x16/open.png \
    Resources/16x16/open_dir.png \
    Resources/16x16/pc.png \
    Resources/16x16/personal_file.png \
    Resources/16x16/ppt.png \
    Resources/16x16/pptx.png \
    Resources/16x16/private_file.png \
    Resources/16x16/public_file.png \
    Resources/16x16/refresh.png \
    Resources/16x16/rename.png \
    Resources/16x16/right.png \
    Resources/16x16/send.png \
    Resources/16x16/share.png \
    Resources/16x16/sharein.png \
    Resources/16x16/shareout.png \
    Resources/16x16/txt.png \
    Resources/16x16/up.png \
    Resources/16x16/upload.png \
    Resources/16x16/word.png \
    Resources/16x16/wordx.png \
    Resources/16x16/xlsx.png \
    Resources/fd/ai.png \
    Resources/fd/bmp.png \
    Resources/fd/css.png \
    Resources/fd/dep.ico \
    Resources/fd/doc.png \
    Resources/fd/docx.png \
    Resources/fd/file.png \
    Resources/fd/fl.png \
    Resources/fd/folder.png \
    Resources/fd/gif.png \
    Resources/fd/group.ico \
    Resources/fd/ico.png \
    Resources/fd/iso.png \
    Resources/fd/jpg.png \
    Resources/fd/js.png \
    Resources/fd/mp3.png \
    Resources/fd/mp4.png \
    Resources/fd/myfile.ico \
    Resources/fd/pdf.png \
    Resources/fd/png.png \
    Resources/fd/ppt.png \
    Resources/fd/pptx.png \
    Resources/fd/ps.png \
    Resources/fd/rar.png \
    Resources/fd/raw.png \
    Resources/fd/share_in.ico \
    Resources/fd/share_out.ico \
    Resources/fd/tif.png \
    Resources/fd/txt.png \
    Resources/fd/xls.png \
    Resources/fd/xlsx.png \
    Resources/fd/zip.png
