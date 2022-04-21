#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include <functional>
#include <QMap>
#include <QVariant>
#include <QStringList>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

class HttpClientPrivate;

class HttpClient {
public:
    HttpClient(const QString &url);
    ~HttpClient();
    void stop2();

    HttpClient& manager(QNetworkAccessManager *manager);
    HttpClient& debug(bool debug);
    HttpClient& param(const QString &name, const QVariant &value);
    HttpClient& params(const QMap<QString, QVariant> &ps);
    HttpClient& json(const QString &json);
    HttpClient& header(const QString &name, const QString &value);
    HttpClient& headers(const QMap<QString, QString> nameValues);
    HttpClient& success(std::function<void(const QString &)> successHandler);
    HttpClient& fail(std::function<void(const QString &, int)> failHandler);
    HttpClient& complete(std::function<void()> completeHandler);
    HttpClient& charset(const QString &cs);
    void get();
    void post();
    void put();
    void remove();
    void download(const QString &savePath);

    /**
    * @brief 上传单个文件
    *        使用 POST 上传，服务器端获取文件的参数名为 file
    *
    * @param path 要上传的文件的路径
    */
    void upload(const QString &path);

    /**
    * @brief 上传文件，文件的内容已经读取到 data 中
    *        使用 POST 上传，服务器端获取文件的参数名为 file
    *
    * @param path 要上传的文件的路径
    */
    void upload(const QByteArray &data);

    /**
    * @brief 上传多个文件
    *        使用 POST 上传，服务器端获取文件的参数名为 files
    *
    * @param paths 要上传的文件的路径
    */
    void upload(const QStringList &paths);

private:
    HttpClientPrivate *d;
};

#endif // HTTPCLIENT_H
