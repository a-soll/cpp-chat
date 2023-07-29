#include <fmt/core.h>
#include <irc.h>

namespace Twitch {

Chat::Chat() {
    this->finished      = false;
    this->size          = 0;
    this->iterator.mind = 0;

    memset(&this->irc.hints, 0, sizeof(this->irc.hints));
    this->irc.hints.ai_family   = AF_UNSPEC;
    this->irc.hints.ai_socktype = SOCK_STREAM;
    this->irc.hints.ai_flags    = AI_PASSIVE;
    strcpy(this->irc.port, "6667");

    if ((this->irc.status = getaddrinfo("irc.chat.twitch.tv", this->irc.port, &this->irc.hints, &this->irc.res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(this->irc.status));
        exit(1);
    }
    this->irc.sockfd = socket(this->irc.res->ai_family, this->irc.res->ai_socktype, this->irc.res->ai_protocol);
    this->irc.con    = connect(this->irc.sockfd, this->irc.res->ai_addr, this->irc.res->ai_addrlen);
}

Chat::~Chat() {
    freeaddrinfo(this->irc.res);
}

int Chat::send(const char *to_send) {
    int len = ::send(this->irc.sockfd, to_send, strlen(to_send), 0);
    return len;
}

void Chat::recieve() {
    std::vector<char> buffer(500);
    this->size = ::recv(this->irc.sockfd, &buffer[0], buffer.size(), 0);
    if (this->size != -1) {
        this->buf.toString().erase();
        this->buf.toString().append(buffer.cbegin(), buffer.cend());
    }
}

cppstr Chat::parseHeaderValue(std::string value) {
    cppstr ret;
    int start = this->header.string.toString().find(value);
    start += value.length() + 1;
    while (this->header.string[start] != '\0' && this->header.string[start] != ';') {
        ret += this->header.string[start];
        start++;
    }
    return ret;
}

void Chat::parseToMessageStart() {
    int &i = this->iterator.proc_ind;
    i++; // we left off on ':' and need to skip it
    while (this->buf[i] != '\0' && this->buf[i] != ':') {
        i++;
    }
}

void Chat::parseMessageLine() {
    int &i      = this->iterator.proc_ind; // processing index
    int &mind   = this->iterator.mind;     // message index
    bool add_to = false;
    char msg_buf[500];

    this->parseToMessageStart();
    this->user.color = this->parseHeaderValue("color");
    this->user.name  = this->parseHeaderValue("display-name");

    while (this->buf[i] != '\0') {
        if (this->buf[i] == '\r') {
            msg_buf[mind] = '\0';
            mind          = 0;
            this->message.append(msg_buf);
            i++;
            this->finished                   = true;
            this->iterator.processing_msg    = false;
            this->iterator.processing_header = true;
            break;
        }
        if (this->buf[i] == ':' && !add_to) {
            i++;
            add_to = true;
        }
        if (add_to) {
            msg_buf[mind] = this->buf[i];
            mind++;
        }
        i++;
    }
}

void Chat::parseHeaderLine() {
    int &i    = this->iterator.proc_ind; // processing index
    int &hind = this->iterator.hind;     // header index
    Header &h = this->header;

    while (this->buf[i] != '\0') {
        if (this->buf[i] != '\r' && this->buf[i] != '\n') { // weed out welcome text
            if (this->buf[i] == ' ') {                      // header ends on space
                this->hdr[hind] = '\0';
                if (this->hdr[0] == '@') { // if header starts with @ we know it's valid
                    header.string.append(this->hdr);
                    this->iterator.processing_header = false;
                    this->iterator.processing_msg    = true;
                }
                hind = 0; // reset hind since done building header
                i++;
                break;
            }
            this->hdr[hind] = this->buf[i];
            hind++;
        } else { // if we run into \r or \n we know current header is invalid
            if (hind > 0) {
                this->hdr[hind] = '\0';
            }
            hind = 0;
        }
        i++;
    }
    return;
}

void Chat::begin() {
    if (this->finished) {
        if (this->iterator.proc_ind >= this->buf.length()) {
            this->iterator.proc_ind = 0;
        }
        this->finished = false;
        this->message.toString().clear();
        this->header.string.toString().clear();
        this->user.name.toString().clear();
        this->user.color.toString().clear();
    }
    if (!this->iterator.processing_header) {
        this->iterator.processing_header = true;
    }

    // request the next buffer chunk once finished with current
    if (this->iterator.proc_ind == 0) {
        this->recieve();
        if (this->size <= 0) {
            return;
        }
        this->buf[this->size] = '\0';
    }

    while (this->buf[this->iterator.proc_ind] != '\0') {
        if (this->iterator.processing_header) {
            this->parseHeaderLine();
        }
        if (this->iterator.processing_msg) {
            this->parseMessageLine();
        }
    }
    if (this->buf[this->iterator.proc_ind] == '\0') {
        this->iterator.proc_ind = 0;
    }
}

void Chat::join(const char *channel) {
    cppstr str;
    str = fmt::format("pass oauth:{0}\n", this->oauth.toString());
    str += fmt::format("nick {}\n", this->nick.toString());
    str += fmt::format(":{}!{}@{}.tmi.twitch.tv JOIN #{}\r\n", this->nick.toString(), this->nick.toString(),
                       this->nick.toString(), channel);
    this->send(str.toString().c_str());
    this->send("CAP REQ :twitch.tv/tags\n");
}

} // namespace Twitch
