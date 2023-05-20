//
// Created by Rahman on 19/04/2023.
//

#ifndef IRIDIUM_SETTINGSBUTTON_HPP
#define IRIDIUM_SETTINGSBUTTON_HPP

#include <QLabel>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QFrame>
#include <QMouseEvent>
#include <RcloneFrame.hpp>
#include <GeneralFrame.hpp>
#include <AppearanceFrame.hpp>

class SettingsButton : public QGroupBox
{
Q_OBJECT

    QHBoxLayout *_layout{};
    QLabel *_label{};
    QFrame *_frame{};

public:

    [[nodiscard]] QString getName() const { return _label->text(); }

    [[nodiscard]] QFrame *getFrame() const { return _frame; }

    enum class Type
    {
        General,
        Rclone,
        Appearance,
    };

    explicit SettingsButton(const Type &type, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;


signals:

    void clicked();

};


#endif //IRIDIUM_SETTINGSBUTTON_HPP
