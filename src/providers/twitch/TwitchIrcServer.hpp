#pragma once

#include "common/Atomic.hpp"
#include "common/Channel.hpp"
#include "common/Singleton.hpp"
#include "pajlada/signals/signalholder.hpp"
#include "providers/bttv/BttvEmotes.hpp"
#include "providers/ffz/FfzEmotes.hpp"
#include "providers/irc/AbstractIrcServer.hpp"
#include "providers/seventv/SeventvEmotes.hpp"
#include "providers/seventv/SeventvEventApiClient.hpp"

#include <chrono>
#include <memory>
#include <queue>

namespace chatterino {

class Settings;
class Paths;
class PubSub;
class TwitchChannel;
class SeventvEventApi;

class TwitchIrcServer final : public AbstractIrcServer, public Singleton
{
public:
    TwitchIrcServer();
    virtual ~TwitchIrcServer() override = default;

    virtual void initialize(Settings &settings, Paths &paths) override;

    void forEachChannelAndSpecialChannels(std::function<void(ChannelPtr)> func);

    std::shared_ptr<Channel> getChannelOrEmptyByID(const QString &channelID);

    void bulkRefreshLiveStatus();

    Atomic<QString> lastUserThatWhisperedMe;

    const ChannelPtr whispersChannel;
    const ChannelPtr mentionsChannel;
    const ChannelPtr liveChannel;
    IndirectChannel watchingChannel;

    PubSub *pubsub;
    SeventvEventApi *eventApi;

    const SeventvEmotes &getSeventvEmotes() const;
    const BttvEmotes &getBttvEmotes() const;
    const FfzEmotes &getFfzEmotes() const;

protected:
    virtual void initializeConnection(IrcConnection *connection,
                                      ConnectionType type) override;
    virtual std::shared_ptr<Channel> createChannel(
        const QString &channelName) override;

    virtual void privateMessageReceived(
        Communi::IrcPrivateMessage *message) override;
    virtual void readConnectionMessageReceived(
        Communi::IrcMessage *message) override;
    virtual void writeConnectionMessageReceived(
        Communi::IrcMessage *message) override;

    virtual std::shared_ptr<Channel> getCustomChannel(
        const QString &channelname) override;

    virtual QString cleanChannelName(const QString &dirtyChannelName) override;
    virtual bool hasSeparateWriteConnection() const override;

private:
    void onMessageSendRequested(TwitchChannel *channel, const QString &message,
                                bool &sent);

    std::mutex lastMessageMutex_;
    std::queue<std::chrono::steady_clock::time_point> lastMessagePleb_;
    std::queue<std::chrono::steady_clock::time_point> lastMessageMod_;
    std::chrono::steady_clock::time_point lastErrorTimeSpeed_;
    std::chrono::steady_clock::time_point lastErrorTimeAmount_;

    SeventvEmotes seventv;
    BttvEmotes bttv;
    FfzEmotes ffz;
    QTimer bulkLiveStatusTimer_;

    pajlada::Signals::SignalHolder signalHolder_;
};

}  // namespace chatterino
