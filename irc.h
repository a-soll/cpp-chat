#ifndef IRC_H
#define IRC_H

#include <cppstr.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <parse.h>
#include <sys/socket.h>
#include <vector>

namespace Twitch {

typedef struct Header {
    cppstr string;
    cppstr color;
    cppstr user;
} Header;

class Chat {
  public:
    Chat();
    ~Chat();
    int send(const char *to_send);
    void begin();
    void join(const char *channel);
    bool finished;
    cppstr nick;
    cppstr oauth;
    cppstr message;
    struct user {
        cppstr name;
        cppstr color;
    } user;
    Header header;
  private:
    int size;
    cppstr buf;
    char hdr[500];
    char uname[500];
    struct irc {
        int status;
        int sockfd;
        socklen_t addr_size;
        struct sockaddr_storage their_addr;
        struct addrinfo hints;
        struct addrinfo *res, *p;
        char ipstr[INET6_ADDRSTRLEN];
        int con;
        char port[5];
    } irc;
    Iterator iterator;

    void parseHeaderLine();
    void parseMessageLine();
    void parseToMessageStart();
    void recieve();
    cppstr parseHeaderValue(std::string value);
};

} // namespace Twitch
#endif /* IRC_H */
