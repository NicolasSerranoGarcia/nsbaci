#include "semaphore.h"

namespace nsbaci::services::runtime {
 bool Semaphore::wait(nsbaci::types::ThreadID currentThread){
    count--;

    if(count < 0){
        blocked.push(currentThread);
        return false;
    }
        
    return true;
 }

   nsbaci::types::ThreadID Semaphore::signal(){
    count++;
    
    if(count <= 0 && !blocked.empty()){
        auto t = blocked.front();
        blocked.pop();
        return t;
    }
    //no thread to wake
    return 0;
   }
}