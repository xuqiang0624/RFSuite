/**
 * @file app/MainWindow.cpp
 * @brief 主窗口实现：界面装配入口。
 */

#include "app/MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    buildUi();
}

MainWindow::~MainWindow() = default;

void MainWindow::buildUi()
{
    // 标题与初始尺寸属于装配层配置；工具栏、停靠面板与各显示控件
    // 随功能迭代在此扩展，保持本函数只做布局与接线。
    setWindowTitle(QStringLiteral("RFSuite · 射频测量上位机"));
    resize(960, 600);
}
