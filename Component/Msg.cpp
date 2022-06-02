#pragma execution_character_set("utf-8")
#include "Msg.h"
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOption>
#include <QPainterPath>
#include "ui_Msg.h"

MSGBOX::MSGBOX(QWidget *parent) :BaseWindow(parent),ui(new Ui::MSGBOX)
{
    ui->setupUi(this);
    initWindow();
}

void MSGBOX::initWindow()
{
    initTitleBar();

    connect(ui->pButtonOk, SIGNAL(clicked()), this, SLOT(onOkClicked()));
    connect(ui->pButtonCancel, SIGNAL(clicked()), this, SLOT(onCancelClicked()));
}

//初始化标题栏
void MSGBOX::initTitleBar()
{
    m_TitleBar->set_width(this->width()-10);
    m_TitleBar->move(10, 10);
    m_TitleBar->setTitle("MessageBox");
    m_TitleBar->setButtonType(ONLY_CLOSE_BUTTON);
    loadStyleSheet("MessageBox/message_box");
    Qt::WindowFlags flags = this->windowFlags();
    this->setWindowFlags(flags | Qt::Window);

    ui->inputContent->setVisible(false);
}

void MSGBOX::setWindowTitle(QString title)
{
    m_TitleBar->setTitleContent(title);
}

void MSGBOX::setContentText(QString contentText)
{
    ui->MessageContent->setText(contentText);
}

void MSGBOX::setMessageType(MessageType messageType)
{
    switch (messageType)
    {
    case MESSAGE_INFORMATION:
        ui->MessageIcon->setPixmap(QPixmap(":/Resources/MessageBox/information.png"));
        break;
    case MESSAGE_WARNNING:
        ui->MessageIcon->setPixmap(QPixmap(":/Resources/MessageBox/warnning.png"));
        break;
    case MESSAGE_QUESTION:
        ui->MessageIcon->setPixmap(QPixmap(":/Resources/MessageBox/question.png"));
        break;
    case MESSAGE_ERROR:
        ui->MessageIcon->setPixmap(QPixmap(":/Resources/MessageBox/error.png"));
        break;
    case MESSAGE_INPUT:
        ui->MessageIcon->setVisible(false);
        ui->inputContent->setVisible(true);
    default:
        break;
    }
}

int MSGBOX::alert(QWidget* parent, const QString &title, const QString &contentText, MessageType messageType, MessageButtonType messageButtonType)
{
    MSGBOX * myMessageBox = new MSGBOX(parent);
    myMessageBox->setWindowTitle(title);
    myMessageBox->setContentText(contentText);
    myMessageBox->setMessageType(messageType);
    myMessageBox->setButtonType(messageButtonType);

    bool isModelWindow = true;
    if (isModelWindow)
    {
        // 设置为模态窗口时，参数parent必须设置父窗口指针，否则模态设置无效;
        // 因为 Qt::WindowModal 参数只对父窗口有效，如果想要模态对全局窗口都有效可以设置 Qt::ApplicationModal
        return myMessageBox->exec();
    }
    else
    {
        myMessageBox->show();
    }

    return -1;
}

int MSGBOX::alert(QWidget *parent, const QString &text)
{
    MSGBOX * myMessageBox = new MSGBOX(parent);
    myMessageBox->setWindowTitle("提示");
    myMessageBox->setContentText(text);
    myMessageBox->setMessageType(MESSAGE_INFORMATION);
    myMessageBox->setButtonType(BUTTON_OK);

    return myMessageBox->exec();
}

int MSGBOX::question(QWidget *parent, const QString &text)
{
    MSGBOX * myMessageBox = new MSGBOX(parent);
    myMessageBox->setWindowTitle("Confirm");
    myMessageBox->setContentText(text);
    myMessageBox->setMessageType(MESSAGE_QUESTION);
    myMessageBox->setButtonType(BUTTON_OK);

    return myMessageBox->exec();
}

int MSGBOX::error(QWidget *parent, const QString text)
{
    MSGBOX * myMessageBox = new MSGBOX(parent);
    myMessageBox->setWindowTitle("错误");
    myMessageBox->setContentText(text);
    myMessageBox->setMessageType(MESSAGE_ERROR);
    myMessageBox->setButtonType(BUTTON_OK);

    return myMessageBox->exec();
}

int MSGBOX::exec()
{
    // 因为QWidget没有exec()方法，所以需要自己定义来完成exec()方法;
    // 而exec()方法就是直接设置窗口显示为模态，并且窗口关闭结束后返回用户选择结果(按了确定还是取消按钮);
    // 而show()方法只是显示窗口，并不会设置窗口的模态或者非模态，需要自己调用setWindowModality()方法进行设置;
    // 而且show()方法并不会返回用户选择结果;
    // 这里也可以继承QDialog类，QDialog有自己的exec()方法，根据返回 Accepted, Rejected来决定是否按了确定按钮;

    // 设置为窗口级模态，也可设置为应用程序及模态 Qt::ApplicationModal;
    this->setWindowModality(Qt::WindowModal);
    show();
    // 使用事件循环QEventLoop ,不让exec()方法结束，在用户选择确定或者取消后，关闭窗口结束事件循环，并返回最后用户选择的结果;
    // 根据返回结果得到用户按下了确定还是取消，采取相应的操作。从而模拟出QDialog类的exec()方法;
    m_eventLoop = new QEventLoop(this);
    m_eventLoop->exec();

    return m_chooseResult;
}

void MSGBOX::onOkClicked()
{
    m_chooseResult = ID_OK;
    close();
}

void MSGBOX::onCancelClicked()
{
    m_chooseResult = ID_CANCEL;
    close();
}

void MSGBOX::closeEvent(QCloseEvent *event)
{
    // 关闭窗口时结束事件循环，在exec()方法中返回选择结果;
    if (m_eventLoop != NULL)
    {
        m_eventLoop->exit();
    }
    event->accept();
}

void MSGBOX::setButtonType(MessageButtonType buttonType)
{
    switch (buttonType)
    {
    case BUTTON_OK:
    {
        ui->pButtonOk->setText("确定");
        ui->pButtonCancel->setVisible(false);
    }
        break;
    case BUTTON_OK_AND_CANCEL:
    {
        ui->pButtonOk->setText("确定");
        ui->pButtonCancel->setText("取消");
    }
        break;
    default:
        break;
    }
}

void MSGBOX::paintEvent(QPaintEvent *event)
{
    // 绘制窗口白色背景色;
    QPainter painter(this);

    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

    // 绘制按钮部分灰色背景;
    QPainterPath pathButtonBack;
    pathButtonBack.setFillRule(Qt::WindingFill);
    pathButtonBack.addRect(QRect(10, this->height()-58, this->width()-20, 48));
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.fillPath(pathButtonBack, QBrush(QColor(247, 247, 247)));

    return QWidget::paintEvent(event);
}

MSGBOX::~MSGBOX()
{
    delete ui;
}
