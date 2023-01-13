//
// Created by sr-71 on 11/01/2023.
//

#ifndef IRIDIUM_REMOTEFRAME_HPP
#define IRIDIUM_REMOTEFRAME_HPP

#include <QFrame>

class RemoteFrame : public QFrame
{
Q_OBJECT
public:
    enum TypeRemote
    {
        Drive
    };
private:
    TypeRemote type {};
    QPixmap image {};
    bool clickRelease {};

public:
    explicit RemoteFrame( const QString & path, TypeRemote type, QWidget * parent = nullptr );

    TypeRemote getType() const;

    void setType( TypeRemote type );

    const QPixmap & getImage() const;

    void setImage( const QPixmap & image );

protected:
    virtual void mousePressEvent( QMouseEvent * event ) override;

    virtual void mouseReleaseEvent( QMouseEvent * event ) override;

    void paintEvent( QPaintEvent * event ) override;

signals:

    void clicked();
};


#endif //IRIDIUM_REMOTEFRAME_HPP
