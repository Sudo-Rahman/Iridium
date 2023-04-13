//
// Created by sr-71 on 30/03/2023.
//

#ifndef IRIDIUM_FILEVIEWWIDGET_HPP
#define IRIDIUM_FILEVIEWWIDGET_HPP

#include <TreeFileViewContainer.hpp>
#include <QLayout>
#include <QProgressBar>
#include <ListRemoteWidget.hpp>
#include <QSplitter>

class FileViewWidget : public QWidget
{
Q_OBJECT

private:
	TreeFileViewContainer *m_treeFileView1{};
	TreeFileViewContainer *m_treeFileView2{};
	QHBoxLayout *m_layout{};

	QSplitter *m_splitter{};

	QList<TreeFileItem *> m_currentFileList{};

public:
	explicit FileViewWidget(QWidget *parent = nullptr);

	void changeRemote(const std::shared_ptr<remotes_selected> &);


signals:

	void
	taskAdded(const QString &src, const QString &dst, const RclonePtr &rclone, const std::function<void()> &callable,
			  const Rclone::TaskType &type = Rclone::Unknown);
};


#endif //IRIDIUM_FILEVIEWWIDGET_HPP
