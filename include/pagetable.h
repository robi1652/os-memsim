#ifndef __PAGETABLE_H_
#define __PAGETABLE_H_

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

struct PageTableKeyComparator
{
    inline bool operator() (const std::string& str1, const std::string& str2)
    {
        size_t sep1 = str1.find("|");
        uint32_t pid1 = std::stoi(str1.substr(0, sep1));
        int page1 = std::stoi(str1.substr(sep1 + 1));
        size_t sep2 = str2.find("|");
        uint32_t pid2 = std::stoi(str2.substr(0, sep2));
        int page2 = std::stoi(str2.substr(sep2 + 1));

        return (pid1 < pid2 || (pid1 == pid2 && page1 < page2));
    }
};

class PageTable {
private:
    int _page_size;
    std::map<std::string, int> _table;

    std::vector<std::string> sortedKeys();

public:
    PageTable(int page_size);
    ~PageTable();

    void addEntry(uint32_t pid, int page_number);
    int getPhysicalAddress(uint32_t pid, uint32_t virtual_address);
    void print();
    uint32_t getPageSize();
    int pageCount(int pid);
    void deleteEntry(uint32_t pid, uint32_t page_to_delete);
    void deletePage(std::string entry);
    void deletePagesLoop(int pid);
};

#endif // __PAGETABLE_H_
