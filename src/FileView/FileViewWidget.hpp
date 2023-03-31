//
// Created by sr-71 on 30/03/2023.
//

#ifndef IRIDIUM_FILEVIEWWIDGET_HPP
#define IRIDIUM_FILEVIEWWIDGET_HPP

#include "TreeWidgets/TreeFileView.hpp"
#include <QLayout>

class FileViewWidget : public QWidget
{
Q_OBJECT

private:
	TreeFileView *m_treeFileView1{};
	TreeFileView *m_treeFileView2{};
	QHBoxLayout *m_layout{};

	RemoteInfo m_remoteInfo1{};
	RemoteInfo m_remoteInfo2{};

public:
	explicit FileViewWidget(RemoteInfo remoteInfo,QWidget *parent = nullptr);

	void changeRemote1(RemoteInfo info);

	void changeRemote2(RemoteInfo info);

};


#endif //IRIDIUM_FILEVIEWWIDGET_HPP
