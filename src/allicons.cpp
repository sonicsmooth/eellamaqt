#include "allicons.h"

#include <QApplication>
#include <QGuiApplication>
#include <QStringList>
#include <QGridLayout>
#include <QPushButton>
#include <QIcon>
#include <QStyle>

AllIcons::AllIcons(QWidget *parent) : QWidget(parent)
{
    QStringList icons = {
        "SP_TitleBarMenuButton",
        "SP_TitleBarMinButton",
        "SP_TitleBarMaxButton",
        "SP_TitleBarCloseButton",
        "SP_TitleBarNormalButton",
        "SP_TitleBarShadeButton",
        "SP_TitleBarUnshadeButton",
        "SP_TitleBarContextHelpButton",
        "SP_MessageBoxInformation",
        "SP_MessageBoxWarning",
        "SP_MessageBoxCritical",
        "SP_MessageBoxQuestion",
        "SP_DesktopIcon",
        "SP_TrashIcon",
        "SP_ComputerIcon",
        "SP_DriveFDIcon",
        "SP_DriveHDIcon",
        "SP_DriveCDIcon",
        "SP_DriveDVDIcon",
        "SP_DriveNetIcon",
        "SP_DirHomeIcon",
        "SP_DirOpenIcon",
        "SP_DirClosedIcon",
        "SP_DirIcon",
        "SP_DirLinkIcon",
        "SP_FileIcon",
        "SP_FileLinkIcon",
        "SP_FileDialogStart",
        "SP_FileDialogEnd",
        "SP_FileDialogToParent",
        "SP_FileDialogNewFolder",
        "SP_FileDialogDetailedView",
        "SP_FileDialogInfoView",
        "SP_FileDialogContentsView",
        "SP_FileDialogListView",
        "SP_FileDialogBack",
        "SP_DockWidgetCloseButton",
        "SP_ToolBarHorizontalExtensionButton",
        "SP_ToolBarVerticalExtensionButton",
        "SP_DialogOkButton",
        "SP_DialogCancelButton",
        "SP_DialogHelpButton",
        "SP_DialogOpenButton",
        "SP_DialogSaveButton",
        "SP_DialogCloseButton",
        "SP_DialogApplyButton",
        "SP_DialogResetButton",
        "SP_DialogDiscardButton",
        "SP_DialogYesButton",
        "SP_DialogNoButton",
        "SP_ArrowUp"
        "SP_ArrowDown",
        "SP_ArrowLeft",
        "SP_ArrowRight",
        "SP_ArrowBack",
        "SP_ArrowForward",
        "SP_CommandLink",
        "SP_VistaShield",
        "SP_BrowserReload",
        "SP_BrowserStop",
        "SP_MediaPlay",
        "SP_MediaStop",
        "SP_MediaPause",
        "SP_MediaSkipForwar",
        "SP_MediaSkipBackward",
        "SP_MediaSeekForwar",
        "SP_MediaSeekBackward",
        "SP_MediaVolume",
        "SP_MediaVolumeMuted",
        "SP_CustomBase"};

    auto layout = new QGridLayout;
    int columns = 6;
    int count = 0;
    auto style = dynamic_cast<QApplication *>(QApplication::instance())->style();
    for (auto const & iname : icons) {
        auto button = new QPushButton(iname);
        button->setIcon(style->standardIcon(QStyle::StandardPixmap(count)));
        layout->addWidget(button, count / columns, count % columns);
        count++;
    }
    setLayout(layout);
    
}
