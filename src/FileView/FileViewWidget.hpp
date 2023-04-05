//
// Created by sr-71 on 30/03/2023.
//

#ifndef IRIDIUM_FILEVIEWWIDGET_HPP
#define IRIDIUM_FILEVIEWWIDGET_HPP

#include "TreeWidgets/TreeFileViewContainer.hpp"
#include <QLayout>

class FileViewWidget : public QWidget
{
Q_OBJECT

private:
	TreeFileViewContainer *m_treeFileView1{};
	TreeFileViewContainer *m_treeFileView2{};
	QHBoxLayout *m_layout{};

	uint8_t m_currentView{};

public:
	explicit FileViewWidget(const RemoteInfo& remoteInfo,QWidget *parent = nullptr);

	uint8_t changeRemote(const RemoteInfoPtr &);

};


#endif //IRIDIUM_FILEVIEWWIDGET_HPP
