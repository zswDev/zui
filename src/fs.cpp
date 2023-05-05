#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#include "fs.h"


size_t fs::file_size(std::ifstream& fin){

    fin.seekg(0, std::ios_base::end);
    size_t file_size = fin.tellg();
    fin.seekg(0, std::ios_base::beg);
    return file_size;
}

std::string fs::read(std::string path) {

    std::ifstream fin;
    fin.open(path);
    if (!fin.is_open()) {
        std::cout<<"err_open"<<std::endl;
        return nullptr;
    }
    std::stringstream buf;
    buf<<fin.rdbuf();
    std::string ret(buf.str());
    return ret;

    // size_t flen = fs::file_size(fin);
    // char* buf = (char*)malloc(sizeof(char)*flen);
    // memset(buf, 0, flen);
    // std::cout<<buf<<std::endl;
    // fin.read(buf, flen);
    // fin.close();
    // std::cout<<buf<<":"<<flen<<std::endl;
    // return buf;
}

void fs::write(std::string path, char* buf) {

    std::ofstream fout;
    fout.open(path);
    if (!fout.is_open()){
        std::cout<<"err_open"<<std::endl;
        return;
    }
    fout<<buf<<std::endl;
    fout.close();
}