#include "socket.h"

namespace kiedis
{

    void Socket::resume_read(){
        if(!read_co_handle.done()){
            read_co_handle.resume();
        }
    }

    void Socket::resume_write(){

    }

} // namespace kiedis
