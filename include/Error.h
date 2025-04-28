#pragma once

#include <string>

using std::string;

enum class ERROR_CODE {
    FILE_PATH_EMPTY = 1,
    FILE_CANNOT_OPEN,

    DIRECTORY_EMPTY,
    DIRECTORY_CREATE,
    DIRECTORY_WHERE,
    
    TABLE_EMPTY,
    TABLE_CREATE,
    TABLE_WHERE,
    TABLE_INSERT,

    SYNTAX_TYPE,
    SYNTAX_CREATE,
    SYNTAX_FROM,
    SYNTAX_INSERT,
    SYNTAX_VALUE,
    SYNTAX_SELECT,
    SYNTAX_WHERE,
    SYNTAX_OPERATOR,
    SYNTAX_UPDATE,
    SYNTAX_SET,
    SYNTAX_UNKNOWN
};

inline string GET_ERROR_MESSAGE(ERROR_CODE e) {
    switch (e) {
        case ERROR_CODE::FILE_PATH_EMPTY:  return "[File] : Path is empty";                    break;
        case ERROR_CODE::FILE_CANNOT_OPEN: return "[File] : File cannot be opened";            break;
        
        case ERROR_CODE::TABLE_CREATE:     return "[Table] : Table cannot be created";         break;
        case ERROR_CODE::TABLE_EMPTY:      return "[Table] : Name is empty";                   break;
        case ERROR_CODE::TABLE_WHERE:      return "[Table] : Table cannot be found";           break;
        case ERROR_CODE::TABLE_INSERT:     return "[Table] : Record Insertion failed";         break;
        
        case ERROR_CODE::SYNTAX_TYPE:      return "[Syntax] : Undentified Type";                                             break;
        case ERROR_CODE::SYNTAX_CREATE:    return "[Syntax] : Expected format : CREATE TABLE / DATABASe <name>";             break;
        case ERROR_CODE::SYNTAX_FROM:      return "[Syntax] : Expected format : CREATE TABLE <> FROM <>";                    break;
        case ERROR_CODE::SYNTAX_INSERT:    return "[Syntax] : Expected format : INSERT INTO TABLE <> VALUES <>";             break;
        case ERROR_CODE::SYNTAX_VALUE:     return "[Syntax] : Expected format : VALUES <key1>=<val1> <key2>=<val2> ...";     break;
        case ERROR_CODE::SYNTAX_SELECT:    return "[Syntax] : Expected format : SELECT * FROM <table_name> WHERE ...";       break;
        case ERROR_CODE::SYNTAX_WHERE:     return "[Syntax] : Expected format : WHERE <key> = <val> / <key> > <>val";        break;
        case ERROR_CODE::SYNTAX_OPERATOR:  return "[Syntax] : Expected format : =, !=, <, >, <=, >=";                        break;
        case ERROR_CODE::SYNTAX_UPDATE:    return "[Syntax] : Expected format : UPDATE <table> SET key1=val1, .. WHERE ..."; break;
        case ERROR_CODE::SYNTAX_SET:       return "[Syntax] : Expected format : SET key1=val1, key2=val2, ...";              break;
        case ERROR_CODE::SYNTAX_UNKNOWN:   return "[Syntax] : Unknown Operation type encounetered !";                        break;

        case ERROR_CODE::DIRECTORY_EMPTY:  return "[Directory] : Name is empty";               break;
        case ERROR_CODE::DIRECTORY_CREATE: return "[Directory] : Directory cannot be created"; break;
        case ERROR_CODE::DIRECTORY_WHERE:  return "[DIrectory] : Directory cannot be found";   break;
        default:                           return "Unknown Error !";
    }
}