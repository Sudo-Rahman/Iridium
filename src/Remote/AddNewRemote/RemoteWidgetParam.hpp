//
// Created by Rahman on 11/01/2023.
//

#ifndef IRIDIUM_REMOTEWIDGETPARAM_HPP
#define IRIDIUM_REMOTEWIDGETPARAM_HPP

#include <QGroupBox>
#include <iridium/entities/remote.hpp>
#include "RemoteConfigParamsFrame.hpp"
#include <Rclone.hpp>

class RemoteWidgetParam : public QGroupBox
{
Q_OBJECT

private:
    iridium::rclone::entity::remote::remote_type _type;
    QIcon _icon{};
    bool _hover{}, _click{}, _selected{};
    RemoteConfigParamsFrame *_paramsFrame{};

    void initParamsFrame();

    void addBlur();

public:
    explicit RemoteWidgetParam(iridium::rclone::entity::remote::remote_type type, QWidget *parent = nullptr);

    [[nodiscard]] RemoteConfigParamsFrame *getParamsFrame() const;

    void select()
    {
        _selected = true;
        repaint();
    }

    void unselect()
    {
        _selected = false;
        repaint();
    }


protected:

    virtual bool event(QEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

signals:

    void clicked(RemoteConfigParamsFrame *);

    void newRemoteAdded();
};


#endif //IRIDIUM_REMOTEWIDGETPARAM_HPP
