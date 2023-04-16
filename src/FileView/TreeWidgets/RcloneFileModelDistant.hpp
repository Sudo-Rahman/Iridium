//
// Created by rahman on 05/02/23.
//

#ifndef IRIDIUM_RCLONEFILEMODELDISTANT_HPP
#define IRIDIUM_RCLONEFILEMODELDISTANT_HPP

#include "RcloneFileModel.hpp"
#include <QTimer>

class RcloneFileModelDistant : public RcloneFileModel
{
Q_OBJECT

public:
	enum Load
	{
		Static, Dynamic
	};
private:
	static Load m_load;

	static uint8_t m_maxDepth;

	std::vector<RclonePtr> m_rcloneList{};

	bool m_newIndex{};

public:
	explicit RcloneFileModelDistant(const RemoteInfoPtr &remoteInfo, QTreeView *parent);

protected:
	void init() override;

	void addItem(const RcloneFilePtr &file, TreeFileItem *parent) override;

private:
	void addItemDynamic(const QString &path, TreeFileItem *parent);


	void addItemStatic(const QString &path, TreeFileItem *parent, uint8_t depth = m_maxDepth);

};


#endif //IRIDIUM_RCLONEFILEMODELDISTANT_HPP
