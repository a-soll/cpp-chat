#ifndef PARSE_H
#define PARSE_H

#include <cppstr.h>

namespace Twitch {

class Iterator {
  public:
    size_t process_len;
    int hind; // header index
    int bind; // buffer index
    int mind; // message index
    cppstr header_str;
    cppstr message_str;
    bool processing_header;
    cppstr to_process;
    cppstr body;
    int proc_ind;
    bool done_initial;
    bool processing_msg;
    bool msg_progress; // are we in the middle of a message?

    Iterator();
    ~Iterator();
};

#endif /* PARSE_H */

} // namespace Twitch
