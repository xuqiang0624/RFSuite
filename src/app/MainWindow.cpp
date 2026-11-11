/**
 * @file app/MainWindow.cpp
 * @brief 主窗口实现
 */

#include "app/MainWindow.h"

#include <QLabel>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("RFSuite · 射频测量上位机"));
    resize(1200, 760);

    // 1. 中央占位，待显示模块接入后替换
    auto *tip = new QLabel(QStringLiteral("显示模块尚未接入"), this);
    tip->setAlignment(Qt::AlignCenter);
    setCentralWidget(tip);

    // 2. 状态栏用于显示运行状态
    m_statusLabel = new QLabel(QStringLiteral("就绪"), this);
    statusBar()->addWidget(m_statusLabel);
}
