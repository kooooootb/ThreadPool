#include <vector>
#include <thread>
#include <iostream>
#include <sstream>
#include <cassert>

#include "Common.h"
#include "Generator.h"

void makeGenerator(std::ostream &ostream, arg_t a, arg_t b, func_t func){
    Generator *generator;
    try{
        generator = new Generator;
    } catch(std::runtime_error &re){
        std::stringstream report;
        report << re.what() << '\n';
        ostream << report.str();
        return;
    }

    generator->sendRequest(a, b, func);
    Request result = generator->receiveRequest();

    delete generator;

    ostream << result.fullReport();
}

arg_t makeArg(){
    return static_cast<double>(rand()) / static_cast<double>(RAND_MAX / 10e10);
}

func_t makeFunc(){
    int choice = rand() % 4;
    switch(choice){
        case 0: return '+';
        case 1: return '-';
        case 2: return '*';
        case 3: return '/';
        default: assert(false && "reached default case in makeFunc");
    }
}

int main(){
    srand(time(nullptr));

    std::vector<std::thread> threads;
    for(int i = 0;i < 100;++i){
        threads.emplace_back(makeGenerator, std::ref(std::cout), makeArg(), makeArg(), makeFunc());
    }

    for(auto &thread : threads){
        thread.join();
    }

    return 0;
}
