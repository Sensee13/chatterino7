#pragma once

#include "messages/MessageElement.hpp"

#include <QRegularExpression>
#include <boost/optional.hpp>
#include <ctime>
#include <utility>

namespace chatterino {
struct BanAction;
struct UnbanAction;
struct AutomodAction;
struct AutomodUserAction;
struct AutomodInfoAction;
struct Message;
using MessagePtr = std::shared_ptr<const Message>;

struct SystemMessageTag {
};
struct TimeoutMessageTag {
};
struct SevenTvEventApiUpdateEmoteMessageTag {
};
struct SevenTvEventApiRemoveEmoteMessageTag {
};
struct SevenTvEventApiAddEmoteMessageTag {
};
const SystemMessageTag systemMessage{};
const TimeoutMessageTag timeoutMessage{};
const SevenTvEventApiUpdateEmoteMessageTag seventvUpdateEmoteMessage{};
const SevenTvEventApiRemoveEmoteMessageTag seventvRemoveEmoteMessage{};
const SevenTvEventApiAddEmoteMessageTag seventvAddEmoteMessage{};

MessagePtr makeSystemMessage(const QString &text);
MessagePtr makeSystemMessage(const QString &text, const QTime &time);
std::pair<MessagePtr, MessagePtr> makeAutomodMessage(
    const AutomodAction &action);
MessagePtr makeAutomodInfoMessage(const AutomodInfoAction &action);

struct MessageParseArgs {
    bool disablePingSounds = false;
    bool isReceivedWhisper = false;
    bool isSentWhisper = false;
    bool trimSubscriberUsername = false;
    bool isStaffOrBroadcaster = false;
    bool isSubscriptionMessage = false;
    QString channelPointRewardId = "";
};

class MessageBuilder
{
public:
    MessageBuilder();
    MessageBuilder(SystemMessageTag, const QString &text,
                   const QTime &time = QTime::currentTime());
    MessageBuilder(TimeoutMessageTag, const QString &timeoutUser,
                   const QString &sourceUser, const QString &systemMessageText,
                   int times, const QTime &time = QTime::currentTime());
    MessageBuilder(TimeoutMessageTag, const QString &username,
                   const QString &durationInSeconds, bool multipleTimes,
                   const QTime &time = QTime::currentTime());
    MessageBuilder(SevenTvEventApiAddEmoteMessageTag, const QString &actor,
                   std::vector<QString> emoteNames);
    MessageBuilder(SevenTvEventApiRemoveEmoteMessageTag, const QString &actor,
                   std::vector<QString> emoteNames);
    MessageBuilder(SevenTvEventApiUpdateEmoteMessageTag, const QString &actor,
                   const QString &emoteName, const QString &oldEmoteName);
    MessageBuilder(const BanAction &action, uint32_t count = 1);
    MessageBuilder(const UnbanAction &action);
    MessageBuilder(const AutomodUserAction &action);

    virtual ~MessageBuilder() = default;

    Message *operator->();
    Message &message();
    MessagePtr release();
    std::weak_ptr<Message> weakOf();

    void append(std::unique_ptr<MessageElement> element);
    QString matchLink(const QString &string);
    void addLink(const QString &origLink, const QString &matchedLink);

    /**
     * Adds the text, applies irc colors, adds links,
     * and updates the message's messageText.
     * See https://modern.ircdocs.horse/formatting.html
     */
    void addIrcMessageText(const QString &text);

    template <typename T, typename... Args>
    // clang-format off
    // clang-format can be enabled once clang-format v11+ has been installed in CI
    T *emplace(Args &&...args)
    // clang-format on
    {
        static_assert(std::is_base_of<MessageElement, T>::value,
                      "T must extend MessageElement");

        auto unique = std::make_unique<T>(std::forward<Args>(args)...);
        auto pointer = unique.get();
        this->append(std::move(unique));
        return pointer;
    }

protected:
    virtual void addTextOrEmoji(EmotePtr emote);
    virtual void addTextOrEmoji(const QString &value);

    MessageColor textColor_ = MessageColor::Text;

private:
    // Helper method that emplaces some text stylized as system text
    // and then appends that text to the QString parameter "toUpdate".
    // Returns the TextElement that was emplaced.
    TextElement *emplaceSystemTextAndUpdate(const QString &text,
                                            QString &toUpdate);

    /**
     * This will add the text and replace any emojis
     * with an emoji emote-element.
     *
     * @param text Text to add
     * @param color Color of the text
     * @param addSpace true if a trailing space should be added after emojis
     */
    void addIrcWord(const QString &text, const QColor &color,
                    bool addSpace = true);

    std::shared_ptr<Message> message_;
};

}  // namespace chatterino
