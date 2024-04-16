//
// Created by Rahman on 03/04/2023.
//

#ifndef IRIDIUM_TREEFILEVIEWCONTAINER_HPP
#define IRIDIUM_TREEFILEVIEWCONTAINER_HPP

#include "TreeFileView.hpp"
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QDialog>

#include <RoundedButton.hpp>
#include <RoundedLineEdit.hpp>


class TreeFileViewContainer : public QWidget
{
Q_OBJECT

private:
    TreeFileView *_treeFileView{};
    QVBoxLayout *_layout{};
    QPushButton *_back_button{}, *_front_button{}, *_refresh_button{};
    QLabel *_path_label{};
    QLineEdit *_search_line_edit{};

    void initUI();

public:
    explicit TreeFileViewContainer(const RemoteInfoPtr &remoteInfo, QWidget *parent = nullptr);

    explicit TreeFileViewContainer(QWidget *parent = nullptr);

    [[nodiscard]] TreeFileView *treeFileView() const { return _treeFileView; }
};


#endif //IRIDIUM_TREEFILEVIEWCONTAINER_HPP
