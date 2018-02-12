#include "pvm3.h"
#include <vector>
#include <stdio.h>

int main(){

    int parent = pvm_parent();

    pvm_recv(parent, 1);

    int N;
    pvm_upkint(&N,1,1);
    int sum;
    pvm_upkint(&sum,1,1);

    int result;

    if (sum == 0) result = 1;
    else if (N == 0) result = 0;
    else {        
        std::vector<int> set(N);
        pvm_upkint(set.data(),N,1);

        std::vector<int> children(2);
        int started = 0;
        while (started < 2){            
            started = pvm_spawn((char*)"child", 0, PvmTaskDefault, 0, 2, children.data());
            if (started < 2) {
                printf("out of res");
                if (children[0] != PvmOutOfRes) pvm_kill(children[0]);
                if (children[1] != PvmOutOfRes) pvm_kill(children[1]);
            }
        }

        if (started < 0) {
            printf("sys error");
            pvm_exit();
            return -1;
        }

        int newN = N-1;
        int newSum = sum - set[N-1];

        pvm_initsend(PvmDataDefault);
        pvm_pkint(&newN,1,1);
        pvm_pkint(&sum,1,1);
        if (newN != 0) {
            pvm_pkint(set.data(),newN,1);
        }
        pvm_send(children[0], 1);

        pvm_initsend(PvmDataDefault);
        pvm_pkint(&newN,1,1);
        pvm_pkint(&newSum,1,1);
        if (newN != 0) {
            pvm_pkint(set.data(),newN,1);
        }
        pvm_send(children[1], 1);

        int results[2];
        pvm_recv(children[0], 1);
        pvm_upkint(&results[0],1,1);
        pvm_recv(children[1], 1);
        pvm_upkint(&results[1],1,1);

        result = results[0] + results[1];
    }

    pvm_initsend(PvmDataDefault);
    pvm_pkint(&result,1,1);
    pvm_send(parent, 1);

    pvm_exit();    

    return 0;
}