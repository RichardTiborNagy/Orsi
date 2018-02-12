#include <fstream>
#include "pvm3.h"
#include <vector>
#include <string>
//#include <chrono>
#include <stdlib.h>

int main(int argc, char* argv[]){

    int sum = atoi(argv[1]);
    std::ifstream input(argv[2]);
    std::ofstream output(argv[3]);

    int N;
    input >> N;
    std::vector<int> set(N);
    for(auto& element : set)
        input >> element;
    input.close();

    //auto start = std::chrono::system_clock::now();

    int child;
    pvm_spawn((char*)"child", 0, PvmTaskDefault, 0, 1, &child);

    pvm_initsend(PvmDataDefault);
    pvm_pkint(&N,1,1);
    pvm_pkint(&sum,1,1);
    pvm_pkint(set.data(),N,1);
    pvm_send(child, 1);

    int result;
    pvm_recv(child, 1);
    pvm_upkint(&result,1,1);

    //auto end = std::chrono::system_clock::now();
    //std::chrono::duration<double> elapsed = end-start;

    output << (result ? "May the subset be with You!" : "I find your lack of subset disturbing!"); //output << elapsed.count(); 
    output.close();

    pvm_exit();

    return 0;
}