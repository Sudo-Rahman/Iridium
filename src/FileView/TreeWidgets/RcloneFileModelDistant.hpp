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

private:

    std::vector<RclonePtr> _rclones_static{}, _rclones_dynamic{};

    static boost::signals2::signal<void()> _load_change;

    bool _stop{false};

public:
    explicit RcloneFileModelDistant(const RemoteInfoPtr &remoteInfo, QTreeView *parent);

    static void setLoadType(Iridium::Load load)
    {
        Iridium::Global::load_type = load;
        _load_change();
    }

    static void setMaxDepth(uint8_t maxDepth) { Iridium::Global::max_depth = maxDepth; }

    static Iridium::Load loadType() { return Iridium::Global::load_type; }

    static uint8_t maxDepth() { return Iridium::Global::max_depth; }

    ~RcloneFileModelDistant() override
    {
        RcloneFileModelDistant::stop();
    }

    void stop() override
    {
        _stop = true;
        for (auto &rclone: _rclones_static)rclone->kill();
        for (auto &rclone: _rclones_dynamic) rclone->kill();
        _rclones_static.clear();
        _rclones_dynamic.clear();
    }


protected:
    void init() override;

    void addItem(const RcloneFilePtr &file, TreeFileItem *parent) override;

private:
    void addItemDynamic(const QString &path, TreeFileItem *parent);


    void addItemStatic(const QString &path, TreeFileItem *parent, uint8_t depth = Iridium::Global::max_depth);

};


#endif //IRIDIUM_RCLONEFILEMODELDISTANT_HPP
