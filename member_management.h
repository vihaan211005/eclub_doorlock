#ifndef MEMBER_MANAGEMENT_H
#define MEMBER_MANAGEMENT_H

#include <vector>
#include <SD.h>
#include <Arduino.h>

class Member {
public:
    String name;
    int id;
    bool inside_status;
    String password;
    String mailID;

    Member(const String& n, int i, bool inside, const String& pass, const String& mail);
};

extern std::vector<Member> members;
extern const char *filename;


void saveToCSVFile();
void appendToCSVFile(const Member& member);
void loadDataFromCSVFile();
void saveMember(const Member& member);
int memberLocation(uint8_t iD);
Member* getMemberByID(int id);

#endif