#include "pvm3.h"
#include <vector>
#include <stdio.h>

int main(){

    int destination;
    int pictureNumber;
    int proportion;

    int master = pvm_parent();

    pvm_recv(master, 1);
    pvm_upkint(&destination,1,1);
    pvm_upkint(&pictureNumber,1,1);
    pvm_upkint(&proportion,1,1);

    for (int i = 0; i < pictureNumber; i++){
        int pictureSize;

        pvm_recv(master, 1);
        pvm_upkint(&pictureSize,1,1);

        std::vector<std::vector<int> > red (pictureSize, std::vector<int>(pictureSize));
        std::vector<std::vector<int> > green (pictureSize, std::vector<int>(pictureSize));
        std::vector<std::vector<int> > blue (pictureSize, std::vector<int>(pictureSize));

        for(int j = 0; j < pictureSize; j++){
            pvm_upkint(red[j].data(),pictureSize,1);
            pvm_upkint(green[j].data(),pictureSize,1);
            pvm_upkint(blue[j].data(),pictureSize,1);
        }

        if (pictureSize > 32){
            std::vector<int> children(4);

            int started = 0;
            while (started < 4){            
                started = pvm_spawn((char*)"first", 0, PvmTaskDefault, 0, 4, children.data());
                if (started < 4) {
                    printf("Atlasz failed to start a process.");
                    for (int j = 0; j < 4; j++){
                        if (children[j] != PvmOutOfRes) pvm_kill(children[j]);
                    }
                }
            }

            int myTid = pvm_mytid();
            int childPictures = 1;
            int halvedSize = pictureSize/2;

            for (int j = 0; j < 4; j++){
                pvm_initsend(PvmDataDefault);
                pvm_pkint(&myTid,1,1);
                pvm_pkint(&childPictures,1,1);
                pvm_pkint(&proportion,1,1);
                pvm_send(children[j], 1);

                int offsetX = (j == 1 || j == 3) ? halvedSize : 0;
                int offsetY = (j == 2 || j == 3) ? halvedSize : 0;

                pvm_initsend(PvmDataDefault);
                pvm_pkint(&halvedSize,1,1);
                for (int k = 0; k < halvedSize; k++){
                    pvm_pkint(red[k+offsetY].data()+offsetX,halvedSize,1);
                    pvm_pkint(green[k+offsetY].data()+offsetX,halvedSize,1);
                    pvm_pkint(blue[k+offsetY].data()+offsetX,halvedSize,1);
                }
                pvm_send(children[j], 1);
            }

            int compressedSize = pictureSize * proportion / 100;

            std::vector<std::vector<int> > compressedRed (compressedSize, std::vector<int>(compressedSize));
            std::vector<std::vector<int> > compressedGreen (compressedSize, std::vector<int>(compressedSize));
            std::vector<std::vector<int> > compressedBlue (compressedSize, std::vector<int>(compressedSize));

            for (int j = 0; j < 4; j++){
                pvm_recv(children[j], 1);
                int temp;
                pvm_upkint(&temp,1,1);
                int offsetX = (j == 1 || j == 3) ? compressedSize/2 : 0;
                int offsetY = (j == 2 || j == 3) ? compressedSize/2 : 0;       
                for (int k = 0; k < compressedSize/2; k++){
                    pvm_upkint(compressedRed[k+offsetY].data()+offsetX,compressedSize/2,1);
                    pvm_upkint(compressedGreen[k+offsetY].data()+offsetX,compressedSize/2,1);
                    pvm_upkint(compressedBlue[k+offsetY].data()+offsetX,compressedSize/2,1);
                }
            }

            pvm_initsend(PvmDataDefault);
            pvm_pkint(&compressedSize,1,1);
            for (int j = 0; j < compressedSize; j++){
                pvm_pkint(compressedRed[j].data(),compressedSize,1);
                pvm_pkint(compressedGreen[j].data(),compressedSize,1);
                pvm_pkint(compressedBlue[j].data(),compressedSize,1);
            }
            pvm_send(destination, 1);

        } else {

            int compressedSize = pictureSize * proportion / 100;
            int p = pictureSize / compressedSize;

            std::vector<std::vector<int> > compressedRed (compressedSize, std::vector<int>(compressedSize));
            std::vector<std::vector<int> > compressedGreen (compressedSize, std::vector<int>(compressedSize));
            std::vector<std::vector<int> > compressedBlue (compressedSize, std::vector<int>(compressedSize));

            for (int k = 0; k < compressedSize; k++){
                for (int l = 0; l < compressedSize; l++){
                    int redSum = 0;
                    int greenSum = 0;
                    int blueSum = 0;
                    for (int m = 0; m < p; m++){
                        for (int n = 0; n < p; n++){
                            redSum += red[k*p + m][l*p + n];
                            greenSum += green[k*p + m][l*p + n];
                            blueSum += blue[k*p + m][l*p + n];
                        }
                    }
                    compressedRed[k][l] = redSum / (p*p);
                    compressedGreen[k][l] = greenSum / (p*p);
                    compressedBlue[k][l] = blueSum / (p*p);
                }
            }

            pvm_initsend(PvmDataDefault);
            pvm_pkint(&compressedSize,1,1);
            for (int j = 0; j < compressedSize; j++){
                pvm_pkint(compressedRed[j].data(),compressedSize,1);
                pvm_pkint(compressedGreen[j].data(),compressedSize,1);
                pvm_pkint(compressedBlue[j].data(),compressedSize,1);
            }
            pvm_send(destination, 1);
        }
    }

    pvm_exit();    

    return 0;

}