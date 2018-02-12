#include "pvm3.h"
#include <vector>
#include <stdio.h>
#include <future>

// 1 BLACK  (0,0,0)
// 2 RED    (255,0,0)
// 3 GREEN  (0,255,0)
// 4 BLUE   (0,0,255)
// 5 CYAN   (0,255,255)
// 6 MAGENTA(255,0,255)
// 7 YELLOW (255,255,0)
// 8 WHITE  (255,255,255)

std::vector<int> colorCode(std::vector<int> red, std::vector<int> green, std::vector<int> blue){
    int size = red.size();
    std::vector<int> result (size);

    for (int i = 0; i < size; i++){
        if (red[i] < 128){
            if (green[i] < 128){
                if (blue[i] < 128){
                    result[i] = 1;
                } else {
                    result[i] = 4;
                }
            } else {
                if (blue[i] < 128){
                    result[i] = 3;
                } else {
                    result[i] = 5;
                }
            }
        } else {
            if (green[i] < 128){
                if (blue[i] < 128){
                    result[i] = 2;
                } else {
                    result[i] = 6;
                }
            } else {
                if (blue[i] < 128){
                    result[i] = 7;
                } else {
                    result[i] = 8;
                }
            }
        }
    }

    return result;
}

int main(){

    int pictureNumber;

    int master = pvm_parent();

    int first;
    int third;
    pvm_recv(master, 1);
    pvm_upkint(&first,1,1);
    pvm_upkint(&third,1,1);
    pvm_upkint(&pictureNumber,1,1);

    for (int i = 0; i < pictureNumber; i++){
        int pictureSize;

        pvm_recv(first, 1);
        pvm_upkint(&pictureSize,1,1);

        std::vector<std::vector<int> > red (pictureSize, std::vector<int>(pictureSize));
        std::vector<std::vector<int> > green (pictureSize, std::vector<int>(pictureSize));
        std::vector<std::vector<int> > blue (pictureSize, std::vector<int>(pictureSize));

        for(int j = 0; j < pictureSize; j++){
            pvm_upkint(red[j].data(),pictureSize,1);
            pvm_upkint(green[j].data(),pictureSize,1);
            pvm_upkint(blue[j].data(),pictureSize,1);
        }

        std::vector<std::future<std::vector<int>> > results;

        for (int j = 0; j < pictureSize-1; j++){
            results.push_back(std::async(std::launch::async, colorCode, red[j], green[j], blue[j]));
        }

        std::vector<int> mainThreadresult;
        mainThreadresult = colorCode(red[pictureSize-1], green[pictureSize-1], blue[pictureSize-1]);

        pvm_initsend(PvmDataDefault);
        pvm_pkint(&pictureSize,1,1);
        for (int j = 0; j < pictureSize-1; j++){
            pvm_pkint(results[j].get().data(),pictureSize,1);
        }
        pvm_pkint(mainThreadresult.data(),pictureSize,1);
        pvm_send(third, 1);
    }

    pvm_exit();    

    return 0;

}