#include "connviews.h"


void ConnViews::log(ILogger *lgr) {
    lgr->log("");
    lgr->log("%-19s  %-13s  %-10s  %-10s  %-10s  %-10s",
        "Fullpath","ViewType","Model","View","SubWidget","Mainwindow");
    for (auto cv : m_cvl) {
        std::string vts(cv.viewType == ViewType::INVALID          ? "INVALID"          :
                        cv.viewType == ViewType::LIBSYMBOLVIEW    ? "LibSymbolView"    :
                        cv.viewType == ViewType::LIBTREEVIEW      ? "LibTreeView"      :
                        cv.viewType == ViewType::LIBTABLEVIEW     ? "LibTableView"     :
                        cv.viewType == ViewType::LIBFOOTPRINTVIEW ? "LibFootprintView" : "really invalid");
    std::string title(std::filesystem::path(cv.fullpath).filename().string());
    lgr->log("%-19s  %-13s  0x%08x  0x%08x  0x%08x  0x%08x",
    title.c_str(), vts.c_str(), cv.model, cv.view, cv.subWidget, cv.mainWindow);

    }
}
