//
// Created by Rahman on 11/01/2023.
//

#ifndef IRIDIUM_REMOTEWIDGETPARAM_HPP
#define IRIDIUM_REMOTEWIDGETPARAM_HPP

#include <QGroupBox>
#include <Remote.h>
#include "RemoteConfigParamsFrame.hpp"
#include <Rclone.hpp>

class RemoteWidgetParam : public QGroupBox
{
Q_OBJECT

private:
    RemoteType _type;
    QIcon _icon{};
    bool _hover{}, _click{}, _selected{};
    RemoteConfigParamsFrame *_paramsFrame{};

    void initParamsFrame();

    void addBlur();

public:
    explicit RemoteWidgetParam(RemoteType type, QWidget *parent = nullptr);

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
