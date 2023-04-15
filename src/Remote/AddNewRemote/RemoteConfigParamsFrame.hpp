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
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QStateMachine>

#include <Rclone.hpp>
#include <RoundedLineEdit.hpp>

class RemoteConfigParamsFrame : public QFrame
{
Q_OBJECT

protected:
	QPushButton *m_login{}, *m_cancel{};
	QLabel *m_messLabel{};
	QVBoxLayout *m_layout{};
	QFormLayout *m_formLayout{};
	QLineEdit *m_remoteName{};
	RclonePtr m_rclone{};
	std::map<std::string, std::string> m_lstRemote{};
	QStateMachine *m_stateMachine{};

public:
	explicit RemoteConfigParamsFrame(QWidget *parent = nullptr);

	virtual void reset();

protected:
	virtual void addRemote();

	virtual void createUi();

	virtual bool checkFields();

	void clearAllFields();

private:
	void connecLineEdit();


signals:

	void remoteAdded();

};


#endif //IRIDIUM_REMOTECONFIGPARAMSFRAME_HPP
