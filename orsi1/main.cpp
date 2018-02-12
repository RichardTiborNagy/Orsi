#include <fstream>
#include <vector>
#include <future>
#include <sstream>
#include <string>
#include <math.h> 
#include <numeric>
#include <functional>

bool isPrime(uint64_t number){
    if (number == 2) return true;
    if (number % 2 == 0) return false;
    double squareRoot = sqrt(number);
    for(int i=3; i<=squareRoot; i+=2){
        if(number % i == 0 ){
            return false;
        }
    }
    return true;
}

uint64_t charHash(char ch){
    uint64_t code = 0x666;
    code = (ch % 2 == 1) ? code << 11 : code << 6;
    code ^= ch & 0xFF; 
    code = isPrime(code) ? code | 0x12345678 : code & 0x12345678;
    return code;
}

uint64_t wordHash(std::string word){
    return std::accumulate(word.begin(), word.end(), 0, [](uint64_t sum, const char& ch){return sum + charHash(ch);});
}

std::string lineHash(std::string line){
    std::istringstream lineStream(line);
    std::ostringstream result;
    for (std::string word; lineStream >> word;){
        result << wordHash(word) << " " ;
    }
    return result.str();
}

int main(){

    // // benchmarking
    // std::ofstream benchmark("benchmark.txt");
    // for (int z = 0; z < 100; ++z){

    //open the input file, read in the number of lines
    std::ifstream input("input.txt");
    uint64_t N;
    input >> N;
    input.ignore(1,'\n'); //ignore the rest of the first line, so getline doesn't read an empty one

    //read in the lines of the file, then close it
	std::vector<std::string> inputLines(N);
	for (auto& line : inputLines){
        getline(input, line);
    }
    input.close();

    // // benchmarking
    // clock_t startTime = clock();

    //calculate the hash code of each line
    std::vector<std::future<std::string>> outputLines;
	for (auto& line : inputLines){
		outputLines.push_back(std::async(std::launch::async, lineHash, line));
    }
    
    // // benchmarking
    // clock_t endTime = clock();
    // clock_t clockTicksTaken = endTime - startTime;
    // long double timeInSeconds = clockTicksTaken / (long double) CLOCKS_PER_SEC;
    // benchmark << timeInSeconds << std::endl;

    //write out the results to the output file, then close it
    std::ofstream output("output.txt");
	for (auto& line : outputLines){
		output << line.get() << std::endl;
	}
    output.close();
    
    // }
    // benchmark.close();

    return 0;
}