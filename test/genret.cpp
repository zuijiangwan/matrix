// 能够生成若干返回帧，写入genret.txt文件
#include <iostream>
#include <fstream>
#include <vector>
int check(int packLen, int status, std::vector<int> info);
int main(){
    std::ofstream fout("genret.txt");
    for(int i = 1; i <= 100; i++){
        // 随机生成包号状态码，额外信息长度和额外信息
        int packageNum = rand() % 16;
        int status = rand() % 1;
        int infoLen = rand() % 16;
        std::vector<int> info(infoLen);
        for(int j = 0; j < infoLen; j++){
            info[j] = rand() % 256;
        }
        // 包总长：8字节包头+1字节总长+1字节包号+1字节状态码+若干字节额外信息+1字节校验字
        int packLen = 8 + 1 + 1 + 1 + infoLen + 1;
        int checkCode = check(packLen, status, info) & 0xff;

        fout << std::dec << "Return Package " << i << ":"<< std::endl;
        fout << "5a5a5a5a5a5a5a5a" << " "; // 包头
        if(packLen < 16)
            fout << "0";
        fout << std::hex << packLen << " "; // 包总长
        if(packageNum < 16)
            fout << "0";
        fout << std::hex << packageNum << " "; // 包号
        if(status < 16)
            fout << "0";
        fout << std::hex << status << " "; // 状态码
        for(auto j : info){
            if(j < 16)
                fout << "0";
            fout << std::hex << j << " "; // 额外信息
        }
        if(checkCode < 16)
            fout << "0";
        fout << std::hex << checkCode << '\n' << std::endl; // 校验字
    }
    fout.close();
    return 0;
}

int check(int packLen, int status, std::vector<int> info){ // 校验字算法
    // 算法内容现在是乱写的简单版checksum
    int check = 0x5a * 8 + packLen + status;
    for(auto i : info){
        check += i;
    }
    return check;
}