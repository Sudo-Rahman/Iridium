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
    RcloneFilePtr _file{};
    QList<TreeFileItem *> _children{};
    State _state{NotLoaded};

public:
    explicit TreeFileItem();

    explicit TreeFileItem(const QString &path, const RemoteInfoPtr &remoteInfo);

    explicit TreeFileItem(const int &column, const std::shared_ptr<RcloneFile> &file);

    [[nodiscard]] const std::shared_ptr<RcloneFile> &getFile() const;

    [[nodiscard]] TreeFileItem *siblingAtFirstColumn() const;


    static QList<QStandardItem *> decorateList(){
        QList<QStandardItem *> decoratedItems;
        for (int i = 0; i < 4; ++i) {
            auto *item = new QStandardItem;
            decoratedItems << item;
        }
        return decoratedItems;
    }

    [[nodiscard]] const QList<TreeFileItem *> &getChildren() const { return _children; }

    void addChild(TreeFileItem *child) { _children << child; }

    [[nodiscard]] State state() const { return _state; }

    void setState(const State &state) { TreeFileItem::_state = state; }

protected:

    void initIcon();

};


#endif //IRIDIUM_TREEFILEITEM_HPP
