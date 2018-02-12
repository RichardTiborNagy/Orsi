#include "pvm3.h"
#include <vector>
#include <stdio.h>
#include <future>

std::vector<int> rowLabel(std::vector<std::vector<int> > pixels, int rowNum){
    int size = pixels.size();
    std::vector<int> result;

    int count = 1;
    for (int j = 1; j < size; j++){
        if (pixels[rowNum][j] == pixels[rowNum][j-1]){  
            count++;
        } else {
            result.push_back(count);   
            count = 1;     
        }
    }
    result.push_back(count);   
    
    return result;
}

std::vector<int> colLabel(std::vector<std::vector<int> > pixels, int colNum){
    int size = pixels.size();
    std::vector<int> result;

    int count = 1;
    for (int j = 1; j < size; j++){
        if (pixels[j][colNum] == pixels[j-1][colNum]){         
            count++;
        } else {
            result.push_back(count);   
            count = 1;     
        }
    }
    result.push_back(count);   
    
    return result;
}

int main(){

    int pictureNumber;

    int master = pvm_parent();

    int second;
    pvm_recv(master, 1);
    pvm_upkint(&second,1,1);
    pvm_upkint(&pictureNumber,1,1);

    for (int i = 0; i < pictureNumber; i++){
        int pictureSize;
        
        pvm_recv(second, 1);
        pvm_upkint(&pictureSize,1,1);

        std::vector<std::vector<int> > pixels (pictureSize, std::vector<int>(pictureSize));
        std::vector<std::future<std::vector<int> > >  rows;
        std::vector<std::future<std::vector<int> > > columns;

        for(int j = 0; j < pictureSize; j++){
            pvm_upkint(pixels[j].data(),pictureSize,1);
        }

        for (int j = 0; j < pictureSize-1; j++){
            rows.push_back(std::async(std::launch::async, rowLabel, pixels, j));
            columns.push_back(std::async(std::launch::async, colLabel, pixels, j));
        }

        std::vector<int> lastRow;
        std::vector<int> lastColumn;
        lastRow = rowLabel(pixels, pictureSize-1); 
        lastColumn = colLabel(pixels, pictureSize-1);

        pvm_initsend(PvmDataDefault);
        pvm_pkint(&pictureSize,1,1);
        for (int j = 0; j < pictureSize; j++){
            pvm_pkint(pixels[j].data(),pictureSize,1);
        }
        for (int j = 0; j < pictureSize-1; j++){
            std::vector<int> temp;
            temp = rows[j].get();
            int size = temp.size();
            pvm_pkint(&size,1,1);
            pvm_pkint(temp.data(),size,1);
        }
        int tempsize = lastRow.size();
        pvm_pkint(&tempsize,1,1);
        pvm_pkint(lastRow.data(),tempsize,1);

        for (int j = 0; j < pictureSize-1; j++){
            std::vector<int> temp;
            temp = columns[j].get();
            int size = temp.size();
            pvm_pkint(&size,1,1);
            pvm_pkint(temp.data(),size,1);
        }
        tempsize = lastColumn.size();
        pvm_pkint(&tempsize,1,1);
        pvm_pkint(lastColumn.data(),tempsize,1);

        pvm_send(master, 1);
    }

    pvm_exit();    

    return 0;   

}