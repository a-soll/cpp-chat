#include "main_irc.h"

typedef struct header_keys {
    char *name;
    int (*parse_token)(char *field, char *header_str, int cur_ind);
    int (*parse_bool)(bool *field, char *header_str, int cur_ind);
    int offset;
} header_keys;

header_keys *in_word_set(const char *str, unsigned int len);
int parse_token(char *field, char header_str[], int cur_ind);
int parse_bool(bool *field, char *header_str, int cur_ind);
void parse_message(Message *message, char *msg_str);
int chat_recv(TwitchChat *chat, char buf[BUFF_SIZE]);
void parse_header(Header *header, char *header_str, int len);
char *lookahead(char *str, int position, int end);
unsigned int hash(const char *str, unsigned int len);
void init_header(Header *header);
int parse_header_line(Irc *irc, TwitchChat *chat, int i);
void init_iterator(Iterator *iterator);
int parse_msg_line(Irc *irc, int i);
void pong_check(Irc *irc, TwitchChat *chat, int i);
