#include "pubnubconsole.h"
#include "ui_pubnubconsole.h"

extern "C" {
#include "pubnub_helper.h"
}

#include <QJsonObject>
#include <QJsonDocument>

PubnubConsole::PubnubConsole(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PubnubConsole)
{
    ui->setupUi(this);

    isNumberRe = new QRegExp("\\d*");
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

void PubnubConsole::defaultGetHandler(pubnub_res result) {
    if (PNR_OK == result) {
        pushMessage(d_pb_publish->get());
    } else {
        pushMessage(pubnub_res_2_string(result));
    }
}

void PubnubConsole::defaultGetChannelHandler(pubnub_res result) {
    if (PNR_OK == result) {
        pushMessage(d_pb_publish->get_channel());
    } else {
        pushMessage(pubnub_res_2_string(result));
    }
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
    QString origin = ui->originField->text().trimmed();

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

    if (!origin.isEmpty()) {
        d_pb_publish.data()->set_origin(origin);
        d_pb_subscribe.data()->set_origin(origin);
        d_pb_presence.data()->set_origin(origin);
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

bool PubnubConsole::validateInput(QString str)
{
    bool ok = true;

    str = str.trimmed();

    if (str.length() == 0) {
        ok = false;
    }

    return ok;
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

void PubnubConsole::on_listCGButton_clicked()
{
    reconnectTo(SLOT(defaultGetChannelHandler(pubnub_res)));

    QString group = ui->channelGroupField->text();

    if (!validateInput(group)) {
        pushMessage("Channel Group is not valid");
        return;
    }

    pubnub_res result = d_pb_publish->list_channel_group(group);

    if (result != PNR_STARTED) {
        pushMessage(pubnub_res_2_string(result));
    }
}

void PubnubConsole::on_addChannel2CGButton_clicked()
{
    reconnectTo(SLOT(defaultGetChannelHandler(pubnub_res)));

    QString group = ui->channelGroupField->text();
    QString channel = ui->cgChannelField->text();

    if (!validateInput(group)) {
        pushMessage("Channel Group is not valid");
        return;
    }

    if (!validateInput(channel)) {
        pushMessage("Channel is not valid");
        return;
    }

    pubnub_res result = d_pb_publish->add_channel_to_group(channel, group);

    if (result != PNR_STARTED) {
        pushMessage(pubnub_res_2_string(result));
    }
}

void PubnubConsole::on_removeChannelFromCGButton_clicked()
{
    reconnectTo(SLOT(defaultGetChannelHandler(pubnub_res)));

    QString group = ui->channelGroupField->text();
    QString channel = ui->cgChannelField->text();

    if (!validateInput(group)) {
        pushMessage("Channel Group is not valid");
        return;
    }

    if (!validateInput(channel)) {
        pushMessage("Channel is not valid");
        return;
    }

    pubnub_res result = d_pb_publish->remove_channel_from_group(channel, group);

    if (result != PNR_STARTED) {
        pushMessage(pubnub_res_2_string(result));
    }
}

void PubnubConsole::on_removeCGButton_clicked()
{
    reconnectTo(SLOT(defaultGetChannelHandler(pubnub_res)));

    QString group = ui->channelGroupField->text();

    if (!validateInput(group)) {
        pushMessage("Channel Group is not valid");
        return;
    }

    pubnub_res result = d_pb_publish->remove_channel_group(group);

    if (result != PNR_STARTED) {
        pushMessage(pubnub_res_2_string(result));
    }
}

void PubnubConsole::on_timeButton_clicked()
{
    reconnectTo(SLOT(defaultGetHandler(pubnub_res)));

    pubnub_res result = d_pb_publish->time();

    if (result != PNR_STARTED) {
        pushMessage(pubnub_res_2_string(result));
    }
}

void PubnubConsole::on_randomUUIDButton_clicked()
{
    d_pb_publish->set_uuid_v4_random();
    ui->uuidField->setText(d_pb_publish->uuid());
    pushMessage("Random UUID is set");
}

void PubnubConsole::on_getUUIDButton_clicked()
{
    pushMessage(d_pb_publish->uuid());
}

void PubnubConsole::on_setUUIDButton_clicked()
{
    QString uuid = ui->uuidField->text().trimmed();

    if (!validateInput(uuid)) {
        pushMessage("UUID is not valid");
        return;
    }

    d_pb_publish->set_uuid(uuid);

    pushMessage("New UUID is set");
}

void PubnubConsole::on_setOrigin_clicked()
{
    QString origin = ui->originField->text().trimmed();

    if (!validateInput(origin)) {
        pushMessage("Origin is not valid");
        return;
    }

    d_pb_publish->set_origin(origin);

    pushMessage("New Origin is set");
}

void PubnubConsole::on_presenceHereNowCGButton_clicked()
{
    reconnectTo(SLOT(defaultGetHandler(pubnub_res)));

    QString channel = ui->presenceHereNowChannelField->text().trimmed();
    QString group = ui->presenceHereNowGroupField->text().trimmed();

    if (!validateInput(channel) && !validateInput(group)) {
        pushMessage("Channel or Group is not valid");
        return;
    }

    d_pb_publish->here_now(channel, group);
}

void PubnubConsole::on_presenceHereNowGlobalButton_clicked()
{
    reconnectTo(SLOT(defaultGetHandler(pubnub_res)));
    d_pb_publish->global_here_now();
}

void PubnubConsole::on_presenceWhereNowButton_clicked()
{
    reconnectTo(SLOT(defaultGetHandler(pubnub_res)));

    QString uuid = ui->presenceWhereNowUUIDField->text().trimmed();

    if (!validateInput(uuid)) {
        pushMessage("UUID is not valid");
        return;
    }

    d_pb_publish->where_now(uuid);
}

void PubnubConsole::on_getStateButton_clicked()
{
    reconnectTo(SLOT(defaultGetHandler(pubnub_res)));

    QString channel = ui->stateChannelField->text().trimmed();
    QString uuid = ui->stateUuidField->text().trimmed();

    if (!validateInput(channel)) {
        pushMessage("Channels not valid");
        return;
    }

    if (!validateInput(uuid)) {
        pushMessage("UUID is not valid");
        return;
    }

    d_pb_publish->state_get(channel, "", uuid);
}

void PubnubConsole::on_setStateButton_clicked()
{
    int i;

    QJsonObject state;
    QString key;
    QString value;
    QTableWidgetItem *keyItem;
    QTableWidgetItem *valueItem;

    reconnectTo(SLOT(defaultGetHandler(pubnub_res)));

    QString channel = ui->stateChannelField->text().trimmed();
    QString uuid = ui->stateUuidField->text().trimmed();

    if (!validateInput(channel)) {
        pushMessage("Channels not valid");
        return;
    }

    if (!validateInput(uuid)) {
        pushMessage("UUID is not valid");
        return;
    }

    for (i = 0; i < ui->stateTable->rowCount(); i++) {
        keyItem = ui->stateTable->item(i, 0);
        valueItem = ui->stateTable->item(i, 1);

        if (keyItem && valueItem) {
            key = keyItem->text().trimmed();
            value = valueItem->text().trimmed();

            if (key.length() > 0 && value.length() > 0) {
                if (value.toLower() == "true") {
                    state.insert(key, true);
                } else if (value.toLower() == "false") {
                    state.insert(key, false);
                } else if (value.toLower() == "null") {
                    state.insert(key, QJsonValue::Null);
                } else if (value.toLower() == "undefined") {
                    state.insert(key, QJsonValue::Undefined);
                } else if (isNumberRe->exactMatch(value)) {
                    state.insert(key, value.toInt());
                } else {
                    state.insert(key, value);
                }
            }
        }
    }

    QJsonDocument doc(state);
    QString strJson(doc.toJson(QJsonDocument::Compact));

    d_pb_publish->set_state(channel, "", uuid, strJson);
}
