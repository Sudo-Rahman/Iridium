//
// Created by sr-71 on 03/04/2023.
//

#ifndef IRIDIUM_TREEFILEVIEWCONTAINER_HPP
#define IRIDIUM_TREEFILEVIEWCONTAINER_HPP

#include "TreeFileView.hpp"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

#include <RoundedButton.hpp>


class TreeFileViewContainer : public QWidget
{
Q_OBJECT

private:
	TreeFileView *m_treeFileView{};
	QVBoxLayout *m_layout{};
	RoundedButton *m_backButton{}, *m_frontButton{}, *m_refreshButton{};
	QLabel *m_pathLabel{};

	void initUI();

public:
	explicit TreeFileViewContainer(const RemoteInfoPtr &remoteInfo, QWidget *parent = nullptr);

	explicit TreeFileViewContainer(QWidget *parent = nullptr);

	[[nodiscard]] TreeFileView *treeFileView() const
	{ return m_treeFileView; }
};


#endif //IRIDIUM_TREEFILEVIEWCONTAINER_HPP
