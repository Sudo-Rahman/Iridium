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

    QScrollArea *_left_scrollArea{}, *_right_scrollArea{};
    QWidget *_left_scrollAreaWidget{};
    QVBoxLayout *_left_scrollAreaLayout{};

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

};


#endif //IRIDIUM_SETTINGSDIALOG_HPP
