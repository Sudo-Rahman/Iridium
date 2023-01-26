//
// Created by sr-71 on 18/01/2023.
//

#ifndef IRIDIUM_TREEFILEWIDGET_HPP
#define IRIDIUM_TREEFILEWIDGET_HPP

#include <QTreeWidget>
#include "../../Rclone/Rclone.hpp"
#include "TreeFileItem.hpp"

class TreeFileWidget : public QTreeWidget
{
Q_OBJECT
    QString remoteName{};
    QList<Rclone *> list{};
    int c{};

public:
    explicit TreeFileWidget(QString remoteName, QWidget *parent = nullptr);

    const QString &getRemoteName() const;

private:
    void addItem(const QString &path, TreeFileItem *parent);

protected:
    void resizeEvent(QResizeEvent *event) override;
};


#endif //IRIDIUM_TREEFILEWIDGET_HPP
