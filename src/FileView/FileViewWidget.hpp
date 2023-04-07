//
// Created by sr-71 on 30/03/2023.
//

#ifndef IRIDIUM_FILEVIEWWIDGET_HPP
#define IRIDIUM_FILEVIEWWIDGET_HPP

#include "TreeWidgets/TreeFileViewContainer.hpp"
#include <QLayout>
#include <QProgressBar>

class FileViewWidget : public QWidget
{
Q_OBJECT

private:
	TreeFileViewContainer *m_treeFileView1{};
	TreeFileViewContainer *m_treeFileView2{};
	QHBoxLayout *m_layout{};

	RclonesManager m_rclonesManager{};

	uint8_t m_currentView{};

	QList<TreeFileItem *> m_currentFileList{};

public:
	explicit FileViewWidget(const RemoteInfo &remoteInfo, QWidget *parent = nullptr);

	void changeRemote(const RemoteInfoPtr &);

private:
	void copyTo(const QPair<QList<RcloneFilePtr>, RcloneFilePtr> &lst, TreeFileViewContainer *view);


signals:

	void progressBarCreated(QProgressBar *progressBar);

};


#endif //IRIDIUM_FILEVIEWWIDGET_HPP
