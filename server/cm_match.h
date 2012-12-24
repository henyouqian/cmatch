#ifndef __CM_MATCH_H__
#define __CM_MATCH_H__

#include <string>
#include <stdint.h>

typedef uint64_t DBID;

void initCmatch();
DBID genDBID();

class Game{
private:
    static Game* create();
    
private:
    DBID _id;
    std::string _name;
};

class Match{
public:
    Match();
    ~Match();
    
private:
    

};

#endif // __CM_MATCH_H__
