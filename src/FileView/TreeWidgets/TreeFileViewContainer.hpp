//
// Created by Rahman on 03/04/2023.
//

#ifndef IRIDIUM_TREEFILEVIEWCONTAINER_HPP
#define IRIDIUM_TREEFILEVIEWCONTAINER_HPP

#include "TreeFileView.hpp"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

#include <RoundedButton.hpp>


class TreeFileViewContainer : public QWidget
{
Q_OBJECT

private:
    TreeFileView *m_treeFileView{};
    QVBoxLayout *m_layout{};
    RoundedButton *m_back_button{}, *m_front_button{}, *m_refresh_button{};
    QLabel *m_path_label{};

    void initUI();

public:
    explicit TreeFileViewContainer(const RemoteInfoPtr &remoteInfo, QWidget *parent = nullptr);

    explicit TreeFileViewContainer(QWidget *parent = nullptr);

    [[nodiscard]] TreeFileView *treeFileView() const { return m_treeFileView; }
};


#endif //IRIDIUM_TREEFILEVIEWCONTAINER_HPP
