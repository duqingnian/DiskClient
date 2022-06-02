#ifndef MSG_H
#define MSG_H

#include <QWidget>
#include <QPaintEvent>
#include <QEventLoop>
#include "BaseWindow.h"

namespace Ui {
class MSGBOX;
}

enum ChosseResult
{
    ID_OK = 0,						// 确定;
    ID_CANCEL						// 取消;
};

enum MessageType
{
    MESSAGE_INFORMATION = 0,		// 提示信息;
    MESSAGE_WARNNING,				// 提示警告;
    MESSAGE_QUESTION,				// 提示询问;
    MESSAGE_INPUT,					// 提示输入框;
    MESSAGE_ERROR,                  //错误提示
};

enum MessageButtonType
{
    BUTTON_OK = 0,					// 只有确定按钮;
    BUTTON_OK_AND_CANCEL,			// 确定、取消按钮;
    BUTTON_CLOSE					// 关闭按钮;
};

class MSGBOX : public BaseWindow
{
    Q_OBJECT

public:
    explicit MSGBOX(QWidget *parent = nullptr);
    ~MSGBOX();
    void initWindow();
    void initTitleBar();
    void paintEvent(QPaintEvent *event);
    void setWindowTitle(QString title);
    void setContentText(QString contentText);
    void setMessageType(MessageType messageType);
    void setButtonType(MessageButtonType buttonType);
    int exec();
    int static alert(QWidget* parent, const QString &title, const QString &contentText, MessageType messageType, MessageButtonType messageButtonType);
    int static alert(QWidget* parent, const QString &title);
    int static question(QWidget* parent, const QString &title);
    int static error(QWidget* parent, const QString);
    void closeEvent(QCloseEvent *event);
private:
    Ui::MSGBOX *ui;
    QEventLoop* m_eventLoop;
    ChosseResult m_chooseResult;
private slots:
    void onOkClicked();
    void onCancelClicked();
};


#endif // MSG_H
