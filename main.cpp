#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
int countBytes = 0;
std::vector<int> HammingEncoder(char byte);

std::vector<int> generateRandomError(std::vector<int> HammingCode);

char HammingDecoder(std::vector<int> HammingCode);


int ConvertBinaryVectorToInt(std::vector<int> HammingCode);

std::vector<int> HammingCodeCorrector(int value);
int count;
int main() {
    // Open the file in binary mode

    std::ifstream file(R"(C:\Users\Hp\Desktop\temp.txt)");
    std::ofstream ErrorMessageFile(R"(C:\Users\Hp\Desktop\Error Message.txt)", std::ios::trunc);
    std::fstream CorrectHammingMessageFile(R"(C:\Users\Hp\Desktop\Hamming Code Message.txt)");
    std::fstream ErrorHammingMessageFile(R"(C:\Users\Hp\Desktop\Hamming Code Message Error.txt)");
    std::fstream CorrectedHammingFile(R"(C:\Users\Hp\Desktop\Corrected Hamming Code.txt)");
    std::ofstream CorrectedMessageFile(R"(C:\Users\Hp\Desktop\Corrected Message.txt)", std::ios::trunc);

    // Check if the file is opened successfully
    if (!file.is_open() || !ErrorMessageFile.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        return 1;
    }
    if (!CorrectHammingMessageFile.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        return 1;
    }
    if (!CorrectedHammingFile.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        return 1;
    }

    if (!ErrorHammingMessageFile.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        return 1;
    }

    // Read the file byte-wise until the end
    char byte;
    while (file.read(&byte, 1)) {
        countBytes++;
        std::vector<int> HammingCodedByte;
        HammingCodedByte.reserve(20);
        HammingCodedByte = HammingEncoder(byte);
        //write encoded hamming code to file as integers
        int HammingCodeInteger = ConvertBinaryVectorToInt(HammingCodedByte);
        CorrectHammingMessageFile << HammingCodeInteger << "\n";
        std::vector<int> HammingCodeWithError;
        HammingCodeWithError.reserve(20);

        HammingCodeWithError = generateRandomError(HammingCodedByte);
        //print hamming code with error to error file
        HammingCodeInteger = ConvertBinaryVectorToInt(HammingCodeWithError);
        ErrorHammingMessageFile << HammingCodeInteger << "\n";
        char errorByte = HammingDecoder(HammingCodeWithError);
        ErrorMessageFile.put(errorByte);

    }

    // Close the file explicitly
    file.close();
    CorrectHammingMessageFile.close();
    ErrorMessageFile.close();
    ErrorHammingMessageFile.seekg(0, std::ios::beg);// the message that the receiver receives
    int choice;
    std::cout << "to read hamming code from a file and correct it and write it to another file press 1\n";
    std::cin >> choice;
    int HammingValue;
    if(choice == 1){
        while(ErrorHammingMessageFile >> HammingValue){
            std::vector<int> CorrectedHammingCode;
            CorrectedHammingCode.reserve(20);
            CorrectedHammingCode = HammingCodeCorrector(HammingValue);
            HammingValue = ConvertBinaryVectorToInt(CorrectedHammingCode);
            CorrectedHammingFile << HammingValue << "\n";
            char CorrectedChar = HammingDecoder(CorrectedHammingCode);
            CorrectedMessageFile.put(CorrectedChar);
        }
    }
    ErrorHammingMessageFile.close();
    CorrectedMessageFile.close();
    std::cout << "the percentage of error is : " << ((double)count/(countBytes*12))*100 << "%" << std::endl;
    return 0;
}
bool isNotPowerOf2(int a){

    return (a & (a - 1)) != 0;
}

std::vector<int> HammingCodeCorrector(int value) {
    std::vector<int> CorrectedHammingCode;
    CorrectedHammingCode.reserve(20);
    int shmt = -1;
    for (int i = 0; i <= 12; ++i) {

        shmt++;
        //int shift = value >> shmt;
        int bit = (value >> shmt) & 1;
        CorrectedHammingCode.push_back(bit);
    }

    int XORValue = 0;
    int i = 0;
    shmt = 0;
    int index = 0;
    while((1 << i) <= CorrectedHammingCode.size()){//loop until index is larger than the size of the vector
        for (int j = 0; j < CorrectedHammingCode.size(); ++j) {

            if((j >> shmt) & 1){//if the index is not a power of 2 and has a 1 at place n:
                XORValue = XORValue ^ CorrectedHammingCode[j];

            }

        }
        index += XORValue * (1 << i);
        XORValue = 0;
        shmt++;
        i++;

    }
    if(index != 0){
        CorrectedHammingCode[index] = ! CorrectedHammingCode[index];
    }
    return CorrectedHammingCode;
}


int ConvertBinaryVectorToInt(std::vector<int> HammingCode) {
    int HammingInteger = 0;
    for (int j = 0; j < HammingCode.size(); ++j) {
        HammingInteger += HammingCode[j] * (1 << j);
    }

    return HammingInteger;
}



char HammingDecoder(std::vector<int> HammingCode) {
    int RecoveredByte;
    //the bits holding the info are those that are not a multiple of powers of 2
    int ParityIndex = 1;
    int i = 0;
    HammingCode[0] = 9;
    while (ParityIndex <= HammingCode.size()){
        HammingCode[ParityIndex] = 9;
        ParityIndex = (1 << (++i));
    }
    HammingCode.erase(std::remove(HammingCode.begin(), HammingCode.end(), 9), HammingCode.end());
    for (int j = 0; j < HammingCode.size(); ++j) {
        RecoveredByte += HammingCode[j] * (1 << j);
    }


    return (char)RecoveredByte;
}

std::vector<int> generateRandomError(std::vector<int> HammingCode) {
    count++;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(1, 12);

    int randomValue = distr(gen);
    HammingCode[randomValue] = !HammingCode[randomValue];
    return HammingCode;
}

std::vector<int> HammingEncoder(char byte) {

    std::vector<int>HammingCode;
    HammingCode.reserve(20);
    int shmt = -1;
    for (int i = 0; i <= 12; ++i) {
        if(isNotPowerOf2(i)){
            shmt++;
            int bit = (byte >> shmt) & 1;
            HammingCode.push_back(bit);
        }
        else{
            HammingCode.push_back(0);
        }
    }
    //now to fill the bits in the 2^n indexes
    //the steps are:
    //1. shift the vector indexes by a shmt := 0
    //  a. test for ___1
    //  b. test for __1_
    //  c. test for _1__
    //  d. test for 1___
    //  where shmt(shift amount) is changed each iteration for the 4 cases
    //2. mask by 1
    //3. xor it with the previous bit
    //4. update the parity indexes
    int i = 0;
    shmt = 0;
    int XORValue = 0;
    while((1 << i) <= HammingCode.size()){//loop until index is larger than the size of the vector
        int index = 1 << i;// to fill the indexes with multiple of 2
        for (int j = 0; j < HammingCode.size(); ++j) {
            if(isNotPowerOf2(j)){
                if((j >> shmt) & 1){//if the index is not a power of 2 and has a 1 at place n:
                    XORValue = XORValue ^ HammingCode[j];

                }
            }
        }
        HammingCode[index] = XORValue;
        XORValue = 0;
        shmt++;
        i++;

    }

    return HammingCode;
}


