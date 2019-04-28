#ifndef __cotton_h__
#define __cotton_h__
#endif
#include<functional>
namespace cotton{

void init_runtime();
void async(std::function<void()> &&lambda); //accepts a C++11 lambda function
void start_finish();
void end_finish();
void finalize_runtime();
}
