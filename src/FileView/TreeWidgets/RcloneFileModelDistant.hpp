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
        Dynamic, Static
    };
private:
    static Load _load;

    static uint8_t _max_depth;

    std::vector<RclonePtr> _locked_rclone{};

public:
    explicit RcloneFileModelDistant(const RemoteInfoPtr &remoteInfo, QTreeView *parent);

    static void setLoadType(Load load) { RcloneFileModelDistant::_load = load; }

    static void setMaxDepth(uint8_t maxDepth) { RcloneFileModelDistant::_max_depth = maxDepth; }

    static Load loadType() { return RcloneFileModelDistant::_load; }

    static uint8_t maxDepth() { return RcloneFileModelDistant::_max_depth; }

    ~RcloneFileModelDistant() override
    {
        for (auto &rclone: _locked_rclone)
            rclone->kill();
        _locked_rclone.clear();
    }


protected:
    void init() override;

    void addItem(const RcloneFilePtr &file, TreeFileItem *parent) override;

private:
    void addItemDynamic(const QString &path, TreeFileItem *parent);


    void addItemStatic(const QString &path, TreeFileItem *parent, uint8_t depth = _max_depth);

};


#endif //IRIDIUM_RCLONEFILEMODELDISTANT_HPP
