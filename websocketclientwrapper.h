#pragma once

#include <QObject>

class WebSocketTransport;
class QWebSocketServer;
class QWebChannelAbstractTransport;

class WebSocketClientWrapper : public QObject
{
    Q_OBJECT

public:
    WebSocketClientWrapper(QWebSocketServer *server, QObject *parent = nullptr);

signals:
    void clientConnected(QWebChannelAbstractTransport *client);

private slots:
    void handleNewConnection();

private:
    QWebSocketServer *m_server;
};
