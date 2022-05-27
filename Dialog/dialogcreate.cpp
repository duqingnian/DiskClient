#pragma execution_character_set("utf-8")
#include "dialogcreate.h"
#include "ui_dialogcreate.h"
#include <QDebug>
#include "Lib/HttpClient.h"
#include "Component/Toast.h"
#include "Component/Msg.h"

DialogCreate::DialogCreate(QWidget *parent) : BaseWindow(parent), ui(new Ui::DialogCreate)
{
    ui->setupUi(this);

    m_TitleBar->set_width(this->width()-10);
    m_TitleBar->setButtonType(ONLY_CLOSE_BUTTON);

    meta = new UrlMeta();
    init();
}

DialogCreate::~DialogCreate()
{
    delete ui;
}

void DialogCreate::setType(QString _type)
{
    type_map["folder"] = "文件夹";
    type_map["txt"] = "文本文件";

    type = _type;
    typeName = type_map[type];

    title->setText(typeName);
    edit->setPlaceholderText("输入"+typeName+"名称");
    edit->setText("");
}

void DialogCreate::setMeta(UrlMeta* _meta)
{
    meta = _meta;
}

void DialogCreate::init()
{
    title = new QLabel(this);
    title->resize(260,35);
    title->setStyleSheet("color: #323130;font-size: 20px;font-weight: 600;");
    title->move(35,35);

    edit = new QLineEdit(this);
    edit->setObjectName("edit_name");
    edit->resize(320,35);
    edit->setStyleSheet("#edit_name{padding: 8px;border: 1px solid #605e5c;color: #605e5c;font-weight: 400;font-size: 14px;}#edit_name:focus{border-color:#0078d4;}");
    edit->move(35,85);
    edit->setText("");
    edit->setFocus();

    btn_submit = new QPushButton(this);
    btn_submit->setCursor(QCursor(Qt::PointingHandCursor));
    btn_submit->setText("创建");
    btn_submit->setStyleSheet("background-color: #0078d4;border-color: #0078d4;padding: 4px 20px 6px;color: #ffffff;border:0px;");
    btn_submit->resize(80,32);
    btn_submit->move(275,145);

    connect(btn_submit,&QPushButton::clicked,this,[=](){
        QString uid = get_reg("uid");

        bool can_ajax = true;
        if(can_ajax && "" == edit->text())
        {
            can_ajax = false;
            Toast::err("请输入名称");
        }
        QVector<QString> illegal_chars;
        illegal_chars.append("~");
        illegal_chars.append("!");
        illegal_chars.append("@");
        illegal_chars.append("#");
        illegal_chars.append("$");
        illegal_chars.append("%");
        illegal_chars.append("^");
        illegal_chars.append("&");
        illegal_chars.append("*");
        illegal_chars.append("(");
        illegal_chars.append(")");
        illegal_chars.append("_");
        illegal_chars.append("-");
        illegal_chars.append("=");
        illegal_chars.append("+");
        illegal_chars.append(" ");
        illegal_chars.append("|");

        for (int i=0; i<illegal_chars.length(); i++) {
            if(can_ajax && (edit->text().trimmed().contains(illegal_chars[i]))){
                can_ajax = false;
                Toast::err("名称不能包含特殊字符:"+illegal_chars[i]);
            }
        }
        if(can_ajax){
            QString _T = "socket";
            if("socket" == _T)
            {
                emit intent("CREATE",type.toUpper(),edit->text());
            }
            else if("web" == _T)
            {
//                qDebug() << "create api = " << path("client/file/create");
//                HttpClient(path("client/file/create")).success([=](const QString &response) {
//                    qDebug() << "response=" << response;
//                    SIMPLE _simple = SimpleParse(response);
//                    if("0" == _simple.id){
//                        Toast::succ(_simple.title);
//                        emit create_succ();
//                    }else{
//                        MSGBOX::error(this,_simple.title);
//                    }
//                }).fail([=](const QString &response,int httpcode) {
//                    MSGBOX::error(this,"请求失败了,httpcode" + QString::number(httpcode));
//                })
//                        .header("content-type", "application/x-www-form-urlencoded")
//                        .param("uid", uid)
//                        .param("type", type.toUpper())
//                        .param("value", edit->text())
//                        .param("meta", meta->key)
//                        .param("meta_id", meta->id)
//                        .param("fd_id", fd->id)
//                        .post();
            }
            else{}
        }
    });

}



