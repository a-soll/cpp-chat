#include <parse.h>

namespace Twitch {

Iterator::Iterator() {
    this->done_initial = false;
    this->processing_header = true;
    this->processing_msg = false;
    this->done_initial = false;
    this->proc_ind = 0;
};

Iterator::~Iterator(){};

} // namespace Twitch
