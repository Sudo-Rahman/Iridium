//
// Created by rahman on 13/01/23.
//

#ifndef IRIDIUM_REMOTECONFIGPARAMSFRAME_HPP
#define IRIDIUM_REMOTECONFIGPARAMSFRAME_HPP

#include <QFrame>
#include <QPushButton>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>

#include "../../Rclone/Rclone.hpp"
#include "../../Other/RoundedLineEdit.hpp"

class RemoteConfigParamsFrame : public QFrame
{
Q_OBJECT
protected:
	QPushButton *logInBtn{}, *cancelBtn{};
	QLabel *messageLabel{};
	QVBoxLayout *m_layout{};
	QLineEdit *m_remoteName{};
	RclonePtr m_rclone{};
	std::map<std::string, std::string> listRemotes{};
public:
	explicit RemoteConfigParamsFrame(QWidget *parent = nullptr);

protected:
	virtual void addRemote();

	virtual void createUi();

	virtual bool checkFields();

	void connecLineEdit(QLineEdit *lineEdit);

signals:

	void remoteAdded();

};


#endif //IRIDIUM_REMOTECONFIGPARAMSFRAME_HPP
