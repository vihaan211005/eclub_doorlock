#include "member_management.h"

std::vector<Member> members;
const char* filename = "/data.csv";

Member::Member(const String& n, int i, bool inside, const String& pass, const String& mail)
  : name(n), id(i), inside_status(inside), password(pass), mailID(mail) {}


void saveToCSVFile() {
  File file = SD.open(filename, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  for (const auto& entry : members) {
    file.print(entry.name);
    file.print(",");
    file.print(entry.id);
    file.print(",");
    file.print(entry.password);  // Add password to CSV file
    file.print(",");
    file.print(entry.mailID);  // Add this line for email ID
    file.print(",");
    file.println(entry.inside_status);
  }

  Serial.println("CSV file saved successfully");

  file.close();
}

void appendToCSVFile(const Member& member) {
  File file = SD.open(filename, FILE_APPEND);  // Open file in append mode
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }

  file.print(member.name);
  file.print(",");
  file.print(member.id);
  file.print(",");
  file.print(member.password);  // Add password to CSV file
  file.print(",");
  file.print(member.mailID);  // Add this line for email ID
  file.print(",");
  file.println(member.inside_status);

  Serial.println("Member appended to CSV file successfully");

  file.close();
}

void loadDataFromCSVFile() {
  File file = SD.open(filename);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  while (file.available()) {
    String line = file.readStringUntil('\n');
    if (line.length() > 0) {
      int firstCommaIndex = line.indexOf(',');
      int secondCommaIndex = line.indexOf(',', firstCommaIndex + 1);
      int thirdCommaIndex = line.indexOf(',', secondCommaIndex + 1);
      int fourthCommaIndex = line.indexOf(',', thirdCommaIndex + 1);

      if (firstCommaIndex != -1 && secondCommaIndex != -1 && thirdCommaIndex != -1) {
        String key = line.substring(0, firstCommaIndex);
        String valueStr = line.substring(firstCommaIndex + 1, secondCommaIndex);
        int value = valueStr.toInt();
        String password = line.substring(secondCommaIndex + 1, thirdCommaIndex);
        String statusStr = line.substring(thirdCommaIndex + 1, fourthCommaIndex);
        bool status = statusStr.toInt();
        String mailiD = line.substring(fourthCommaIndex + 1);  // Add this line for email ID

        members.push_back(Member(key, value, status, password, mailiD));
      }
    }
  }

  Serial.println("Data loaded from CSV file");
  file.close();
}

void saveMember(const Member& member) {
  members.push_back(member);
  appendToCSVFile(member);
}

int memberLocation(uint8_t iD) {
  Member* foundMember = getMemberByID(iD);

  int i = 0;

  while (members[i].id != foundMember->id) {
    i++;
  }
  return i;
}

Member* getMemberByID(int id) {
  for (auto& member : members) {
    if (member.id == id) {
      return &member;
    }
  }
  return nullptr;
}