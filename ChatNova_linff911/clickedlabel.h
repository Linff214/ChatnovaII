#ifndef CLICKEDLABEL_H
#define CLICKEDLABEL_H
#include <QLabel>
#include "global.h"

class ClickedLabel:public QLabel
{
    Q_OBJECT
public:
    ClickedLabel(QWidget* parent);
    virtual void mousePressEvent(QMouseEvent *ev) override;
    virtual void mouseReleaseEvent(QMouseEvent *ev) override;
    virtual void enterEvent(QEnterEvent* event) override;
    virtual void leaveEvent(QEvent* event) override;
    void SetState(QString normal="", QString hover="", QString press="",
                  QString select="", QString select_hover="", QString select_press="");//闭眼睁眼状态

    ClickLbState GetCurState();//获取当前是睁眼还是闭眼状态
    bool SetCurState(ClickLbState state);
    void ResetNormalState();
protected:

private:
    //闭眼状态
    QString _normal;
    QString _normal_hover;
    QString _normal_press;
    //睁眼状态
    QString _selected;
    QString _selected_hover;
    QString _selected_press;

    ClickLbState _curstate;
signals:
    void clicked(QString, ClickLbState);

};

#endif // CLICKEDLABEL_H
