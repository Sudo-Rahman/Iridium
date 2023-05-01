//
// Created by sr-71 on 02/02/2023.
//

#ifndef IRIDIUM_RCLONEFILEMODEL_HPP
#define IRIDIUM_RCLONEFILEMODEL_HPP

#include <QStandardItemModel>
#include "TreeFileItem.hpp"
#include <QTreeView>
#include <Rclone.hpp>


class RcloneFileModel : public QStandardItemModel
{
Q_OBJECT

protected:
	RemoteInfoPtr m_remoteInfo{};
	QModelIndex m_root_index{};

	QTreeView *m_view{};

	bool m_expandOrDoubleClick{};

    bool m_check_is_valid = false;

	virtual void init() = 0;

    explicit RcloneFileModel();

	explicit RcloneFileModel(const RemoteInfoPtr &remoteInfo, QTreeView *View);

	void addProgressBar(const QModelIndex &index);

public:
	virtual void addItem(const RcloneFilePtr &file, TreeFileItem *parent);

	[[nodiscard]] const QModelIndex &getRootIndex() const;

	static QList<QStandardItem *> getItemList(TreeFileItem *item);

	void setExpandOrDoubleClick(bool expandOrDoubleClick){
		m_expandOrDoubleClick = expandOrDoubleClick;
	}

protected:
public:
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

signals:

    void beforeDataChanged(const QVariant &value, int role);
};


#endif //IRIDIUM_RCLONEFILEMODEL_HPP
