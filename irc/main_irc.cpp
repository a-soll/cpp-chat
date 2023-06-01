#include "parse.h"

TwitchChat::TwitchChat() {
    memset(&this->hints, 0, sizeof(this->hints));
    this->hints.ai_family = AF_UNSPEC;
    this->hints.ai_socktype = SOCK_STREAM;
    this->hints.ai_flags = AI_PASSIVE;

    if ((this->status = getaddrinfo("irc.chat.twitch.tv", MYPORT, &this->hints, &this->res)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(this->status));
        exit(1);
    }
    this->sockfd = socket(this->res->ai_family, this->res->ai_socktype, this->res->ai_protocol);
    this->con = connect(this->sockfd, this->res->ai_addr, this->res->ai_addrlen);
}

TwitchChat::~TwitchChat() {
    freeaddrinfo(this->res);
}

int TwitchChat::send(std::string *msg) {
    int len = send(this->sockfd, &msg->begin(), msg->length(), 0);
    return len;
}

int TwitchChat::recieve(std::string *buf) {
    int rec = recv(this->sockfd, &buf->begin(), buf->length(), 0);
    if (rec != -1) {
        buf[rec] = '\0';
    }
    return rec;
}

TwitchChat::~TwitchChat() {
    freeaddrinfo(this->res);
}

void reset_message(Message *message) {
    message->message[0] = '\0';
    message->user[0] = '\0';
    message->size = 0;
}

void init_irc(Irc *irc) {
    irc->finished = false;
    irc->message.message[0] = '\0';
    irc->message.user[0] = '\0';
    irc->message.size = 0;
    irc->size = 0;
    init_iterator(&irc->iterator);
    init_header(&irc->header);
}

void parse_irc(TwitchChat *chat, Irc *irc) {
    if (irc->finished) {
        irc->finished = false;
        reset_message(&irc->message);
    }
    Iterator *iter = &irc->iterator;

    // request the next buffer chunk once finished with the current
    if (iter->proc_ind == 0) {
        irc->size = chat_recv(chat, irc->buf);
        if (irc->size <= 0) {
            return;
        }
        irc->buf[irc->size] = '\0';
    }

    while (irc->buf[iter->proc_ind] != '\0') {
        if (iter->processing_header) {
            iter->proc_ind += parse_header_line(irc, chat, iter->proc_ind);
        }
        if (iter->processing_msg) {
            iter->proc_ind += parse_msg_line(irc, iter->proc_ind);
            if (!iter->processing_msg) {
                irc->finished = true;
            }
            return;
        }
        if (iter->proc_ind < irc->size) {
            iter->proc_ind++;
        }
    }
    iter->proc_ind = 0;
}

void join_chat(TwitchChat *chat, const char *user, const char *token, const char *channel) {
    char msg[150];
    snprintf(msg, 150, "pass oauth:%s\r\n", token);
    printf("%s\n", msg);
}

void pong_check(Irc *irc, TwitchChat *chat, int i) {
    Iterator *iter = &irc->iterator;
    if (strcmp(iter->header_str, "PING") == 0) {
        if (irc->buf[i + 1] == ':') {
            chat_send(chat, "PONG :tmi.twitch.tv\r\n");
        }
    }
}
