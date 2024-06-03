#pragma once


#include "RcloneFileModel.hpp"
#include <iridium/process.hpp>
#include <boost/signals2.hpp>
#include <mutex>

#include "Global.hpp"

class RcloneFileModelDistant : public RcloneFileModel
{
Q_OBJECT

    ir::process_pool _process_pool{4};
    std::vector<TreeFileItem *> _items_static_load{};

    static boost::signals2::signal<void()> _load_change;
    static boost::signals2::signal<void(uint8_t)> _simultaneous_processes_change;

    bool _stop{false};

    std::mutex _mutex;

public:
    explicit RcloneFileModelDistant(const RemoteInfoPtr &remoteInfo, QTreeView *parent);

    static void setLoadType(Iridium::Load load)
    {
        Iridium::Global::load_type = load;
        _load_change();
    }

    static void setMaxDepth(uint8_t maxDepth) { Iridium::Global::max_depth = maxDepth; }

    static void setSimultaneousProcesses(uint8_t maxProcess) { _simultaneous_processes_change(maxProcess); }

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
    void connectProcess(ir::process *process, TreeFileItem *tree_item);

    void addItemDynamic(const RcloneFilePtr &file, TreeFileItem *parent);

    void addItemStatic(const RcloneFilePtr &file, TreeFileItem *parent, uint8_t depth = Iridium::Global::max_depth);

};
