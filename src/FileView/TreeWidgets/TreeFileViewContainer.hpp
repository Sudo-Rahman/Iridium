//
// Created by sr-71 on 03/04/2023.
//

#ifndef IRIDIUM_TREEFILEVIEWCONTAINER_HPP
#define IRIDIUM_TREEFILEVIEWCONTAINER_HPP

#include "TreeFileView.hpp"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>


class TreeFileViewContainer : public QWidget
{
Q_OBJECT

private:
	TreeFileView *m_treeFileView{};
	QVBoxLayout *m_layout{};
	QPushButton *m_backButton{};
	QPushButton *m_frontButton{};
	QLabel *m_pathLabel{};

public:
	explicit TreeFileViewContainer(const RemoteInfoPtr &remoteInfo,QWidget *parent = nullptr);

	void changeRemote(const RemoteInfoPtr &remoteInfo);

};


#endif //IRIDIUM_TREEFILEVIEWCONTAINER_HPP
