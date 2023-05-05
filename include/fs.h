#ifndef __fs_h__
#define __fs_h__

#include <string>

class fs{
private:
    static size_t file_size(std::ifstream& fin);
public:
    static std::string read(std::string path);
    static void write(std::string path, char* buf);
};

extern char* read(std::string path);

#endif