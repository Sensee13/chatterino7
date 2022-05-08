#pragma once

#include <QString>
#include <pajlada/signals/signal.hpp>
#include "SeventvEventApiMessages.hpp"
#include "SeventvEventApiWebsocket.hpp"

#include <atomic>
#include <vector>

namespace chatterino {
struct Listener {
    QString channel;
};

class SeventvEventApiClient
    : public std::enable_shared_from_this<SeventvEventApiClient>
{
public:
    // The max amount of channels we may join with a single connection
    static constexpr std::vector<QString>::size_type MAX_LISTENS = 100;
    // TODO: document + this is a really terrible name
    static constexpr std::chrono::seconds MAX_PING_SECONDS =
        std::chrono::seconds(60);

    SeventvEventApiClient(eventapi::WebsocketClient &_websocketClient,
                          eventapi::WebsocketHandle _handle);

    void start();
    void stop();

    void close(const std::string &reason,
               websocketpp::close::status::value code =
                   websocketpp::close::status::normal);

    bool join(const QString &channel);
    void part(const QString &channel);

    void handlePing();

    bool isJoinedChannel(const QString &channel);

    std::vector<Listener> getListeners() const;

private:
    void checkPing();
    bool send(const char *payload);

    eventapi::WebsocketClient &websocketClient_;
    eventapi::WebsocketHandle handle_;
    std::vector<Listener> channels;

    std::atomic<std::chrono::time_point<std::chrono::steady_clock>> lastPing_;
    std::atomic<bool> started_{false};
};
}  // namespace chatterino