#ifndef IRC_STRUCT_H
#define IRC_STRUCT_H

#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>

#define MYPORT "6667"
#define BACKLOG 10
#define BUFF_SIZE 500
#define COLOR_LEN 11
#define NONCE_LEN 51
#define NAME_LEN 26
#define EMOTES_LEN 650
#define BADGE_LEN 250
#define BADGE_INFO_LEN 55
#define FLAG_LEN 150
#define ID_LEN 50
#define ROOM_ID_LEN 15
#define TMI_SENT_LEN 15
#define USER_ID_LEN 11
#define USER_TYPE_LEN 15
#define KEY_LEN 30

#define MESSAGE_LEN 550
#define USER_LEN 26

#define HEADER_FRAG_LEN 1700
#define BODY_LEN 1500

class TwitchChat {
    int status;
    int sockfd;
    socklen_t addr_size;
    struct sockaddr_storage their_addr;
    struct addrinfo hints;
    struct addrinfo *res, *p;
    char ipstr[INET6_ADDRSTRLEN];
    int con;
    std::string pw;
    std::string nick;

    TwitchChat::TwitchChat();
    TwitchChat::~TwitchChat();
    int TwitchChat::send(std::string *msg);
    int TwitchChat::recieve(std::string *buf);
    void TwitchChat::parse_irc();
};

typedef struct Message {
    char user[USER_LEN];
    char message[MESSAGE_LEN];
    size_t size;
} Message;

typedef struct Header {
    char badge_info[BADGE_INFO_LEN];
    char badges[BADGE_LEN];
    char client_nonce[NONCE_LEN];
    char color[COLOR_LEN];
    char display_name[NAME_LEN];
    char emotes[EMOTES_LEN];
    char bits[150];
    bool first_msg;
    char flags[FLAG_LEN];
    char id[ID_LEN];
    bool mod;
    char room_id[ROOM_ID_LEN];
    bool subscriber;
    char tmi_sent_ts[TMI_SENT_LEN];
    bool turbo;
    char user_id[USER_ID_LEN];
    char user_type[USER_TYPE_LEN];
    int ind;
    char user_name[USER_LEN];
    bool emote_only;
    char reply_parent_display_name[USER_LEN];
    char reply_parent_msg_body[MESSAGE_LEN];
    char reply_parent_msg_id[NONCE_LEN];
    char reply_parent_user_id[USER_ID_LEN];
    char reply_parent_user_login[USER_LEN];
    char msg_id[ID_LEN];
} Header;

typedef struct Iterator {
    size_t process_len;
    int hind;
    int bind;
    char header_str[HEADER_FRAG_LEN];
    char message_string[MESSAGE_LEN];
    bool processing_header;
    char *to_process;
    bool processing_msg;
    char body[BODY_LEN];
    int proc_ind;
    bool done_initial;
} Iterator;

typedef struct Irc {
    Message message;
    Header header;
    char buf[BUFF_SIZE];
    size_t size;
    Iterator iterator;
    bool finished;
} Irc;

void chat_init(TwitchChat *chat);
int chat_send(TwitchChat *chat, const char *msg);
void reset_message(Message *message);
void init_irc(Irc *irc);
void parse_irc(TwitchChat *chat, Irc *irc);
void chat_deinit(TwitchChat *chat);

#endif /* IRC_STRUCT_H */
