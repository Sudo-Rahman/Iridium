//
// Created by Rahman on 18/01/2023.
//

#ifndef IRIDIUM_TREEFILEITEM_HPP
#define IRIDIUM_TREEFILEITEM_HPP

#include <QStandardItem>
#include <QJsonObject>
#include <RcloneFile.hpp>

#define SORT_ROLE (Qt::UserRole + 1)

class TreeFileItem : public QStandardItem
{
public:
    enum State
    {
        NotLoaded,
        Loaded,
        Loading
    };
protected:
    RcloneFilePtr m_file{};
    TreeFileItem *m_parent{};
    QList<TreeFileItem *> m_children{};
    State m_state{NotLoaded};

public:
    explicit TreeFileItem(TreeFileItem * = nullptr);

    explicit TreeFileItem(const QString &path, const RemoteInfoPtr &remoteInfo, TreeFileItem * = nullptr);

    explicit TreeFileItem(const int &column, const std::shared_ptr<RcloneFile> &file, TreeFileItem * = nullptr);

    [[nodiscard]] const std::shared_ptr<RcloneFile> &getFile() const;

    [[nodiscard]] TreeFileItem *getParent() const;

    [[nodiscard]] const QList<TreeFileItem *> &getChildren() const { return m_children; }

    void addChild(TreeFileItem *child) { m_children << child; }

    [[nodiscard]] State state() const { return m_state; }

    void setState(const State &state) { TreeFileItem::m_state = state; }

protected:

    void initIcon();

};


#endif //IRIDIUM_TREEFILEITEM_HPP
