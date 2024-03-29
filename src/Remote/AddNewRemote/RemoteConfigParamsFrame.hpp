//
// Created by rahman on 13/01/23.
//

#ifndef IRIDIUM_REMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_REMOTECONFIGPARAMSFRAME_HPP

#include <QFrame>
#include <QPushButton>
#include <QLayout>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>

#include <Rclone.hpp>
#include <RoundedLineEdit.hpp>

class RemoteConfigParamsFrame : public QFrame
{
Q_OBJECT

protected:
    QPushButton *_login{}, *_cancel{};
    QLabel *_mess_label{};
    QVBoxLayout *_layout{};
    QFormLayout *_form_layout{};
    RoundedLineEdit *_remote_name{};
    RcloneUniquePtr _rclone{};
    std::vector<RemoteInfoPtr> _remotes{};

public:
    explicit RemoteConfigParamsFrame(QWidget *parent = nullptr);

    virtual void reset();

    void focusLineEdit() { if (_remote_name)_remote_name->setFocus(); }

protected:
    virtual void addRemote();

    virtual void createUi();

    virtual bool checkFields();

    void clearAllFields();


signals:

    void remoteAdded();

};


#endif //IRIDIUM_REMOTECONFIGPARAMSFRAME_HPP
