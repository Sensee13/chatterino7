#pragma once

#include "common/FlagsEnum.hpp"
#include "providers/twitch/TwitchBadge.hpp"
#include "util/QStringHash.hpp"
#include "widgets/helper/ScrollbarHighlight.hpp"

#include <QTime>
#include <boost/noncopyable.hpp>
#include <cinttypes>
#include <memory>
#include <vector>

namespace chatterino {
class MessageElement;
class MessageThread;

enum class MessageFlag : uint64_t {
    None = 0,
    System = (1 << 0),
    Timeout = (1 << 1),
    Highlighted = (1 << 2),
    DoNotTriggerNotification = (1 << 3),  // disable notification sound
    Centered = (1 << 4),
    Disabled = (1 << 5),
    DisableCompactEmotes = (1 << 6),
    Collapsed = (1 << 7),
    ConnectedMessage = (1 << 8),
    DisconnectedMessage = (1 << 9),
    Untimeout = (1 << 10),
    PubSub = (1 << 11),
    Subscription = (1 << 12),
    DoNotLog = (1 << 13),
    AutoMod = (1 << 14),
    RecentMessage = (1 << 15),
    Whisper = (1 << 16),
    HighlightedWhisper = (1 << 17),
    Debug = (1 << 18),
    Similar = (1 << 19),
    RedeemedHighlight = (1 << 20),
    RedeemedChannelPointReward = (1 << 21),
    ShowInMentions = (1 << 22),
    FirstMessage = (1 << 23),
    ReplyMessage = (1 << 24),
    ElevatedMessage = (1 << 25),

    // SevenTV Flags
    // For some reason C++ compiler is unable to infer that these are all 64bit values so when you do 1 << 60 it thinks the 1 and the 60 are 32bit values and it overflows.
    // So we have to explicitly cast them to 64bit values. (1ULL << 60)
    SevenTvEventApiAddEmoteMessage = (11ULL << 60),
    SevenTvEventApiRemoveEmoteMessage = (11ULL << 61),
    SevenTvEventApiUpdateEmoteMessage = (11ULL << 62),
};
using MessageFlags = FlagsEnum<MessageFlag>;

struct Message : boost::noncopyable {
    Message();
    ~Message();

    // Making this a mutable means that we can update a messages flags,
    // while still keeping Message constant. This means that a message's flag
    // can be updated without the renderer being made aware, which might be bad.
    // This is a temporary effort until we can figure out what the right
    // const-correct way to deal with this is.
    // This might bring race conditions with it
    mutable MessageFlags flags;
    QTime parseTime;
    QString id;
    QString searchText;
    QString messageText;
    QString loginName;
    QString displayName;
    QString localizedName;
    QString timeoutUser;
    QString channelName;
    QColor usernameColor;
    QDateTime serverReceivedTime;
    std::vector<Badge> badges;
    std::unordered_map<QString, QString> badgeInfos;
    std::shared_ptr<QColor> highlightColor;
    // Each reply holds a reference to the thread. When every reply is dropped,
    // the reply thread will be cleaned up by the TwitchChannel.
    // The root of the thread does not have replyThread set.
    std::shared_ptr<MessageThread> replyThread;
    uint32_t count = 1;
    std::vector<std::unique_ptr<MessageElement>> elements;
    std::vector<QString> seventvEventTargetEmotes;

    ScrollbarHighlight getScrollBarHighlight() const;
};

using MessagePtr = std::shared_ptr<const Message>;

}  // namespace chatterino
