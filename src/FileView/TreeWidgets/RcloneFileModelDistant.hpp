#pragma once


#include "RcloneFileModel.hpp"
#include <iridium/process.hpp>

#include "Global.hpp"

class RcloneFileModelDistant : public RcloneFileModel
{
Q_OBJECT

    ir::process_pool _process_pool{4};
    std::vector<TreeFileItem *> _items_static_load{};

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

    void stop() override;


protected:
    void init() override;

    void addItem(const RcloneFilePtr &file, TreeFileItem *parent) override;

    void reload(TreeFileItem *item) override;

private:
    void connectProcess(ir::process *process, TreeFileItem *tree_item, bool progress = true);

    void addItemDynamic(const RcloneFilePtr &file, TreeFileItem *parent);

    void addItemStatic(const RcloneFilePtr &file, TreeFileItem *parent, uint8_t depth = Iridium::Global::max_depth);

};
