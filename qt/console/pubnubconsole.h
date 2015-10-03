#ifndef PUBNUBCONSOLE_H
#define PUBNUBCONSOLE_H

#include "../pubnub_qt.h"
#include <QMainWindow>
#include <QLabel>

namespace Ui {
class PubnubConsole;
}

class PubnubConsole : public QMainWindow
{
    Q_OBJECT

public:
    explicit PubnubConsole(QWidget *parent = 0);
    ~PubnubConsole();

private slots:
    void onPublish(pubnub_res result);
    void onSubscribe(pubnub_res result);
    void onPresence(pubnub_res result);
    void onHistory(pubnub_res result);
    void defaultGetHandler(pubnub_res result);
    void defaultGetChannelHandler(pubnub_res result);

    void pushMessage(QString string);
    void pushPresence(QString string);
    void pushHistory(QString string);

    void on_publishButton_clicked();
    void on_subscribeButton_clicked();

    void on_subscribeClearButton_clicked();

    void on_presenceClearButton_clicked();

    void on_historyClearButton_clicked();

    void on_historyLoadButton_clicked();

    void on_listCGButton_clicked();

    void on_addChannel2CGButton_clicked();

    void on_removeChannelFromCGButton_clicked();

    void on_removeCGButton_clicked();

    void on_timeButton_clicked();

    void on_randomUUIDButton_clicked();

    void on_getUUIDButton_clicked();

    void on_setUUIDButton_clicked();

    void on_setOrigin_clicked();

    void on_presenceHereNowCGButton_clicked();

    void on_presenceHereNowGlobalButton_clicked();

    void on_presenceWhereNowButton_clicked();

private:
    Ui::PubnubConsole *ui;
    QLabel *statusLabel;

    char const *currentSlot;
    bool subscribed;
    bool connected;

    QScopedPointer<pubnub_qt> d_pb_publish;
    QScopedPointer<pubnub_qt> d_pb_subscribe;
    QScopedPointer<pubnub_qt> d_pb_presence;

    bool reconnectTo(const char *);

    void doSubscribe();
    void doUnsubscribe();

    QString getSubscribeChannels();
    QString getPresenceChannels();
    QString getTimestamp();

    bool validateInput(QString str);
};

#endif // PUBNUBCONSOLE_H
