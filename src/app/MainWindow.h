/**
 * @file app/MainWindow.h
 * @brief 主窗口：界面装配与模块间接线入口
 *
 * 职责：提供主窗口骨架与状态栏；后续功能以独立模块接入，
 * 对象间连接集中在本类完成。
 *
 * 设计边界：主窗口不实现协议解析、绘图算法与 SQL 语句。
 */

#pragma once

#include <QMainWindow>

class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private:
    QLabel *m_statusLabel = nullptr;    ///< 状态栏左侧提示
};
