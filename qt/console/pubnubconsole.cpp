#include "pubnubconsole.h"
#include "ui_pubnubconsole.h"

extern "C" {
#include "pubnub_helper.h"
}

PubnubConsole::PubnubConsole(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PubnubConsole)
{
    ui->setupUi(this);

    statusLabel = new QLabel(this);
    statusLabel->setText("Initializing...");
    ui->statusbar->addPermanentWidget(statusLabel, 1);

    doSubscribe();
}

PubnubConsole::~PubnubConsole()
{
    delete ui;
}

bool PubnubConsole::reconnectTo(const char *to) {
    disconnect(d_pb_publish.data(), SIGNAL(outcome(pubnub_res)), this, currentSlot);
    currentSlot = to;
    return connect(d_pb_publish.data(), SIGNAL(outcome(pubnub_res)), this, to);
}

void PubnubConsole::onPublish(pubnub_res result) {
    QString report =  QString("Publish result: '") + pubnub_res_2_string(result) + "', response: " + d_pb_publish->last_publish_result();

    pushMessage(report);
}

void PubnubConsole::onHistory(pubnub_res result) {
    if (PNR_OK != result) {
        pushMessage(QString("History failed, result: ") + pubnub_res_2_string(result));
    } else {
        QList<QString> msg = d_pb_publish->get_all();

        for (int i = 0; i < msg.size(); ++i) {
            pushHistory(msg[i]);
        }
    }
}

void PubnubConsole::onSubscribe(pubnub_res result) {
    if (PNR_OK != result) {
        pushMessage(QString("Subscribe failed, result: ") + pubnub_res_2_string(result));
    } else {
        QList<QString> msg = d_pb_subscribe->get_all();

        if (msg.size() == 0 && connected == false) {
            statusLabel->setText("CONNECTED - Press 'Publish' button");
            connected = true;
            pushMessage(getSubscribeChannels());
        }

        for (int i = 0; i < msg.size(); ++i) {
            pushMessage(msg[i]);
        }
    }

    result = d_pb_subscribe->subscribe(getSubscribeChannels());
    if (result != PNR_STARTED) {
        pushMessage(QString("Subscribe failed, result: '") + pubnub_res_2_string(result) + "'");
    }
}

void PubnubConsole::onPresence(pubnub_res result) {
    if (PNR_OK != result) {
        pushMessage(QString("Presence failed, result: ") + pubnub_res_2_string(result));
    } else {
        QList<QString> msg = d_pb_presence->get_all();

        for (int i = 0; i < msg.size(); ++i) {
            pushPresence(msg[i]);
        }
    }

    result = d_pb_presence->subscribe(getPresenceChannels());
    if (result != PNR_STARTED) {
        pushMessage(QString("Presence failed, result: '") + pubnub_res_2_string(result) + "'");
    }
}

void PubnubConsole::doSubscribe() {
    QString pub_key = ui->publishKeyField->text().trimmed();
    QString sub_key = ui->subscribeKeyField->text().trimmed();
    QString auth_key = ui->authKeyField->text().trimmed();
    QString uuid = ui->uuidField->text().trimmed();

    d_pb_publish.reset(new pubnub_qt(pub_key, sub_key));

    d_pb_subscribe.reset(new pubnub_qt(pub_key, sub_key));
    connect(d_pb_subscribe.data(), SIGNAL(outcome(pubnub_res)), this, SLOT(onSubscribe(pubnub_res)));

    d_pb_presence.reset(new pubnub_qt(pub_key, sub_key));
    connect(d_pb_presence.data(), SIGNAL(outcome(pubnub_res)), this, SLOT(onPresence(pubnub_res)));

    if (!uuid.isEmpty()) {
        d_pb_publish.data()->set_uuid(uuid);
        d_pb_subscribe.data()->set_uuid(uuid);
        d_pb_presence.data()->set_uuid(uuid);
    }

    if (!auth_key.isEmpty()) {
        d_pb_publish.data()->set_auth(auth_key);
        d_pb_subscribe.data()->set_auth(auth_key);
        d_pb_presence.data()->set_auth(auth_key);
    }

    d_pb_subscribe->subscribe(getSubscribeChannels());
    d_pb_presence->subscribe(getPresenceChannels());

    ui->subscribeButton->setText("Unsubscribe");
    statusLabel->setText("CONNECTING...");
    subscribed = true;
    connected = false;
}

void PubnubConsole::doUnsubscribe() {
    d_pb_subscribe->leave(ui->channelField->text());

    disconnect(d_pb_subscribe.data(), SIGNAL(outcome(pubnub_res)), this, SLOT(onSubscribe(pubnub_res)));
    d_pb_subscribe.reset();

    disconnect(d_pb_presence.data(), SIGNAL(outcome(pubnub_res)), this, SLOT(onPresence(pubnub_res)));
    d_pb_subscribe.reset();

    ui->subscribeButton->setText("Subscribe");
    statusLabel->setText("NOT CONNECTED - Press 'Subscribe' button");
    subscribed = false;
    connected = false;
}

void PubnubConsole::pushMessage(QString string)
{
    ui->messagesOutput->moveCursor(QTextCursor::Start);
    ui->messagesOutput->insertPlainText(getTimestamp() + ":  " + string + "\n");
}

void PubnubConsole::pushPresence(QString string)
{
    ui->presenceOutput->moveCursor(QTextCursor::Start);
    ui->presenceOutput->insertPlainText(string + "\n");
}

void PubnubConsole::pushHistory(QString string)
{
    ui->historyOutput->moveCursor(QTextCursor::Start);
    ui->historyOutput->insertPlainText(string + "\n");
}

QString PubnubConsole::getSubscribeChannels()
{
    return ui->channelField->text();
}

QString PubnubConsole::getPresenceChannels()
{
    QString channels = ui->channelField->text();

    // TODO: slit channels by ',' and add -pnpres for each

    return QString(channels + "-pnpres");
}

QString PubnubConsole::getTimestamp()
{
    return QString::number(QDateTime::currentDateTime().toTime_t());
}

void PubnubConsole::on_subscribeClearButton_clicked()
{
    ui->messagesOutput->clear();
}

void PubnubConsole::on_presenceClearButton_clicked()
{
    ui->presenceOutput->clear();
}

void PubnubConsole::on_historyClearButton_clicked()
{
    ui->historyOutput->clear();
}

void PubnubConsole::on_publishButton_clicked()
{
    reconnectTo(SLOT(onPublish(pubnub_res)));

    pubnub_res result = d_pb_publish->publish(ui->channelField->text(), ui->publishTextEdit->toPlainText());

    if (result != PNR_STARTED) {
        pushMessage(pubnub_res_2_string(result));
    }
}

void PubnubConsole::on_subscribeButton_clicked()
{
    subscribed ? doUnsubscribe() : doSubscribe();
}

void PubnubConsole::on_historyLoadButton_clicked()
{
    reconnectTo(SLOT(onHistory(pubnub_res)));

    pubnub_res result = d_pb_publish->history(ui->channelField->text());

    if (result != PNR_STARTED) {
        pushMessage(pubnub_res_2_string(result));
    }
}
