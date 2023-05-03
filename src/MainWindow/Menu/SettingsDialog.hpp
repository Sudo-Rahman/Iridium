//
// Created by Rahman on 16/04/2023.
//

#ifndef IRIDIUM_SETTINGSDIALOG_HPP
#define IRIDIUM_SETTINGSDIALOG_HPP

#include <QDialog>
#include <QScrollArea>
#include <QLayout>

class SettingsDialog : public QDialog
{
Q_OBJECT

    QScrollArea *m_left_scrollArea;
    QWidget *m_left_scrollAreaWidget;
    QVBoxLayout *m_left_scrollAreaLayout;

    QScrollArea *m_right_scrollArea;
    QWidget *m_right_scrollAreaWidget;
    QVBoxLayout *m_right_scrollAreaLayout;


public:
    explicit SettingsDialog(QWidget *parent = nullptr);

};


#endif //IRIDIUM_SETTINGSDIALOG_HPP
