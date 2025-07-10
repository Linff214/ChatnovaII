#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmgr.h"
RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    ui->pass_edit->setEchoMode(QLineEdit::Password);//设置密码非明文
    ui->confirm_edit->setEchoMode(QLineEdit::Password);
    ui->error_tip->setProperty("state","normal");
    repolish(ui->error_tip);
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish,this,&RegisterDialog::slot_reg_mod_finish);
    //创建注册窗口 RegisterDialog 的时候，就会自动调用构造函数，而构造函数里就调用了 initHttpHandlers()
    initHttpHandlers();
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}
//响应按钮点击事件;
void RegisterDialog::on_get_code_clicked()
{
    //从注册窗口界面上的邮箱输入框中获取用户输入的邮箱地址
    auto email = ui->email_edit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
    // 如果邮箱格式合法，则执行发送验证码的逻辑。
    if(match) {
        //发送http验证码
        //构建一个 QJsonObject 对象 json_obj，并设置字段 "email" 的值为用户输入的邮箱。
        QJsonObject json_obj;
        json_obj["email"] = email;
        //调用 HTTP 管理器的 PostHttpReq() 方法，发送一个 POST 请求到本地服务器。
        HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/get_varifycode"),
                                            json_obj, ReqId::ID_GET_VARIFY_CODE,Modules::REGISTERMOD);
    }
    else{
        showTip(tr("邮箱地址不正确"),false);
    }
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err)
{
    qDebug() << "响应内容原始数据：" << res;

    if(err != ErrorCodes::SUCCESS) {
        //jsondocument可以把字节流的字符串转成json文档
        showTip(tr("网络请求错误"), false);
        return;
    }
    //解析json字符串，res转换为QByteArray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if(jsonDoc.isNull()){
        showTip(tr("Json解析错误"),false);
        return;
    }
    if(!jsonDoc.isObject()) {
        showTip(tr("Json解析失败"),false);
        return;
    }
    //转换为json对象,然后传给回调函数处理
    jsonDoc.object();
    //从 下面的_handlers 中取出并调用对应回调
    _handlers[id](jsonDoc.object());
    return;

}

void RegisterDialog::initHttpHandlers()
{
    //注册获取验证码回包逻辑
    //初始化 _handlers 哈希表，即将具体请求类型的回调逻辑注册进去，比如
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE,[this](const QJsonObject& jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"), false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已经发送到邮箱，请注意查收"),true);
        qDebug() << "email is" << email;
    });

    //注册注册用户回包逻辑
    _handlers.insert(ReqId::ID_REG_USER, [this](QJsonObject jsonObj){
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS){
            showTip(tr("参数错误"),false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("用户注册成功"), true);
        qDebug()<< "email is " << email ;
    });

}

void RegisterDialog::showTip(QString str, bool b_ok)
{
    if(b_ok) {
        ui->error_tip->setProperty("state","normal");
    }
    else {
        ui->error_tip->setProperty("state","err");
    }
    ui->error_tip->setText(str);

    repolish(ui->error_tip);
}


void RegisterDialog::on_sure_btn_clicked()
{
    if(ui->user_edit->text() == ""){
        showTip(tr("用户名不能为空"), false);
        return;
    }

    if(ui->email_edit->text() == ""){
        showTip(tr("邮箱不能为空"), false);
        return;
    }

    if(ui->pass_edit->text() == ""){
        showTip(tr("密码不能为空"), false);
        return;
    }

    if(ui->confirm_edit->text() == ""){
        showTip(tr("确认密码不能为空"), false);
        return;
    }

    if(ui->confirm_edit->text() != ui->pass_edit->text()){
        showTip(tr("密码和确认密码不匹配"), false);
        return;
    }

    if(ui->varify_edit->text() == ""){
        showTip(tr("验证码不能为空"), false);
        return;
    }

    //day11 发送http请求注册用户
    QJsonObject json_obj;
    json_obj["user"] = ui->user_edit->text();
    json_obj["email"] = ui->email_edit->text();
    json_obj["passwd"] = ui->pass_edit->text();
    json_obj["confirm"] = ui->confirm_edit->text();
    json_obj["varifycode"] = ui->varify_edit->text();
    HttpMgr::GetInstance()->PostHttpReq(QUrl(gate_url_prefix+"/user_register"),
                                        json_obj, ReqId::ID_REG_USER,Modules::REGISTERMOD);
}

