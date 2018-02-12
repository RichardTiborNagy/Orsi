#include <fstream>
#include "pvm3.h"
#include <vector>
//#include <chrono>
#include <stdlib.h>
#include <stdio.h>
#include <string>

std::string pixToStr(int code){
    switch(code){
        case 1:
            return "(0,0,0) ";
        case 2:
            return "(255,0,0) ";
        case 3:
            return "(0,255,0) ";
        case 4:
            return "(0,0,255) ";
        case 5:
            return "(0,255,255) ";
        case 6:
            return "(255,0,255) ";
        case 7:
            return "(255,255,0) ";
        case 8:
            return "(255,255,255) ";
        default:
            return "err";
    }
}

int main(int argc, char* argv[]){

    int proportion = atoi(argv[1]);
    std::ifstream input(argv[2]);
    std::ofstream output(argv[3]);

    int first;
    pvm_spawn((char*)"first", 0, PvmTaskDefault, 0, 1, &first);
    int second;
    pvm_spawn((char*)"second", 0, PvmTaskDefault, 0, 1, &second);
    int third;
    pvm_spawn((char*)"third", 0, PvmTaskDefault, 0, 1, &third);

    int pictureNumber;
    input >> pictureNumber;

    pvm_initsend(PvmDataDefault);
    pvm_pkint(&second,1,1);
    pvm_pkint(&pictureNumber,1,1);
    pvm_pkint(&proportion,1,1);
    pvm_send(first, 1);

    pvm_initsend(PvmDataDefault);
    pvm_pkint(&first,1,1);
    pvm_pkint(&third,1,1);
    pvm_pkint(&pictureNumber,1,1);
    pvm_send(second, 1);

    pvm_initsend(PvmDataDefault);
    pvm_pkint(&second,1,1);
    pvm_pkint(&pictureNumber,1,1);
    pvm_send(third, 1);

    //auto start = std::chrono::system_clock::now();

    for (int i = 0; i < pictureNumber; i++){
        int pictureSize;
        input >> pictureSize;
        std::vector<std::vector<int> > red (pictureSize, std::vector<int>(pictureSize));
        std::vector<std::vector<int> > green (pictureSize, std::vector<int>(pictureSize));
        std::vector<std::vector<int> > blue (pictureSize, std::vector<int>(pictureSize));
        
        for (int j = 0; j < pictureSize; j++){
            for (int k = 0; k < pictureSize; k++){
                input >> red[j][k] >> green[j][k] >> blue[j][k];
            }
        }

        pvm_initsend(PvmDataDefault);
        pvm_pkint(&pictureSize,1,1);
        for (int j = 0; j < pictureSize; j++){
            pvm_pkint(red[j].data(),pictureSize,1);
            pvm_pkint(green[j].data(),pictureSize,1);
            pvm_pkint(blue[j].data(),pictureSize,1);
        }
        pvm_send(first, 1);
    }

    for (int i = 0; i < pictureNumber; i++){
        pvm_recv(third, 1);
        int pictureSize;
        pvm_upkint(&pictureSize,1,1);

        std::vector<std::vector<int> > pixels (pictureSize, std::vector<int>(pictureSize));
        for (int j = 0; j < pictureSize; j++){
            pvm_upkint(pixels[j].data(),pictureSize,1);
            for (int k = 0; k < pictureSize; k++){
                output << pixToStr(pixels[j][k]);
            }
            output << std::endl;
        }

        for (int j = 0; j < pictureSize; j++){
            int rowSize;
            pvm_upkint(&rowSize,1,1);
            std::vector<int> row(rowSize);
            pvm_upkint(row.data(),rowSize,1);
            for (int k = 0; k < rowSize; k++){
                output << row[k] << " ";
            }
            output << std::endl;
        }

        for (int j = 0; j < pictureSize; j++){
            int colSize;
            pvm_upkint(&colSize,1,1);
            std::vector<int> column(colSize);
            pvm_upkint(column.data(),colSize,1);
            for (int k = 0; k < colSize; k++){
                output << column[k] << " ";
            }
            output << std::endl;
        }
    }

    //auto end = std::chrono::system_clock::now();
    //std::chrono::duration<double> elapsed = end-start;
    //output << std::endl << elapsed.count(); 

    output.close();

    pvm_exit();

    return 0;
}