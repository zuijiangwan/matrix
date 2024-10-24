// 能够生成若干返回帧，写入genret.txt文件
#include <iostream>
#include <fstream>
#include <vector>

#define PACKAGENUM 10
#define HEADSIZE 4
#define PACKLENSIZE 2

using namespace std;

unsigned char check(vector<unsigned char> &package);
int main(){
    std::ofstream fout("genret.txt");
    for(int i = 0; i < PACKAGENUM; i++){
        vector<unsigned char> package;
        int infosize = rand() % 16; // 额外信息长度
        int packlen = infosize + HEADSIZE + PACKLENSIZE + 3; // 包总长
        // 包头
        for(int j = 0; j < HEADSIZE; j++){
            package.push_back(0x5A);
        }
        // 包长
        for(int j = 0; j < PACKLENSIZE; j++){
            package.push_back(packlen >> (PACKLENSIZE - j - 1) * 8);
        }
        // 包号
        package.push_back(rand()%16);
        // 状态码
        package.push_back(rand()%2);
        // 额外信息
        for(int j = 0; j < infosize; j++){
            package.push_back(rand()%256);
        }
        // 校验字
        package.push_back(check(package));

        // 写入文件
        for(auto j : package){
            if(j < 16)
                fout << "0";
            fout << hex << (unsigned int)j << " ";
        }
        fout << endl;
    }

    fout.close();
    return 0;
}

unsigned char check(vector<unsigned char> &package){ // 校验字算法
    unsigned char check = 0;
    for(unsigned char i : package){
        check += i;
    }
    return check;
}