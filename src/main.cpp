/**
 * @file main.cpp
 * @brief 应用程序入口：创建 QApplication 并显示主窗口。
 *
 * 入口只做装配，不承载业务逻辑；模块初始化与对象间连接统一由
 * MainWindow 负责，后续新增能力不需要修改本文件。
 */

#include "app/MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("RFSuite"));
    app.setApplicationVersion(QStringLiteral("0.1.0"));
    app.setOrganizationName(QStringLiteral("RFSuite"));

    MainWindow window;
    window.show();
    return app.exec();
}
