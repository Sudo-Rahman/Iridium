//
// Created by rahman on 05/02/23.
//

#ifndef IRIDIUM_RCLONEFILEMODELDISTANT_HPP
#define IRIDIUM_RCLONEFILEMODELDISTANT_HPP

#include "RcloneFileModel.hpp"
#include <QTimer>
#include <Settings.hpp>

class RcloneFileModelDistant : public RcloneFileModel
{
Q_OBJECT

public:
	enum Load
	{
		 Dynamic,Static
	};
private:
	static Load m_load;

	static uint8_t m_maxDepth;

	std::deque<RcloneLocked> m_lockedRclone{};

public:
	explicit RcloneFileModelDistant(const RemoteInfoPtr &remoteInfo, QTreeView *parent);

	static void setLoadType(Load load)
	{ RcloneFileModelDistant::m_load = load;}

	static void setMaxDepth(uint8_t maxDepth)
	{ RcloneFileModelDistant::m_maxDepth = maxDepth;}

	static Load loadType()
	{ return RcloneFileModelDistant::m_load; }

	static uint8_t maxDepth()
	{ return RcloneFileModelDistant::m_maxDepth; }

	~RcloneFileModelDistant() override{
		RcloneManager::stop(m_lockedRclone);
	};


protected:
	void init() override;

	void addItem(const RcloneFilePtr &file, TreeFileItem *parent) override;

private:
	void addItemDynamic(const QString &path, TreeFileItem *parent);


	void addItemStatic(const QString &path, TreeFileItem *parent, uint8_t depth = m_maxDepth);

};


#endif //IRIDIUM_RCLONEFILEMODELDISTANT_HPP
