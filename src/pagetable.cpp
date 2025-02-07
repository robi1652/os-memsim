#include "pagetable.h"

PageTable::PageTable(int page_size)
{
    _page_size = page_size;
}

PageTable::~PageTable()
{
}

std::vector<std::string> PageTable::sortedKeys()
{
    std::vector<std::string> keys;

    std::map<std::string, int>::iterator it;
    for (it = _table.begin(); it != _table.end(); it++)
    {
        keys.push_back(it->first);
    }

    std::sort(keys.begin(), keys.end(), PageTableKeyComparator());

    return keys;
}

void PageTable::addEntry(uint32_t pid, int page_number)
{
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);

    // Changed to -1 for the loop, frame0 can be a thing
    int frame = -1; 
    // Find free frame
    // TODO: implement this!
    // done
    int mapIter = 0;
    while (frame == -1) {
        if (_table.find(entry) == _table.end()) {
            frame = mapIter;
            std::cout << "gets in if " << frame << std::endl;
            break;
        }
        
        mapIter++;
    }
    std::cout << "escapes" << std::endl;
    _table[entry] = frame;
}

int PageTable::getPhysicalAddress(uint32_t pid, uint32_t virtual_address)
{
    // Convert virtual address to page_number and page_offset
    // TODO: implement this!
    // done
    std::cout << "test 2";
    int page_number = _page_size / virtual_address;

    int page_offset = _page_size % virtual_address;
    
    // Combination of pid and page number act as the key to look up frame number
    std::string entry = std::to_string(pid) + "|" + std::to_string(page_number);
    
    // If entry exists, look up frame number and convert virtual to physical address
    int address = -1;
    if (_table.count(entry) > 0)
    {
        // TODO: implement this!
        // Done, should I use find or at? or does it not matter
        address = _table.at(entry) * _page_size + page_offset;
    }

    return address;
}

void PageTable::print()
{
    int i;

    std::cout << " PID  | Page Number | Frame Number" << std::endl;
    std::cout << "------+-------------+--------------" << std::endl;

    std::vector<std::string> keys = sortedKeys();

    for (i = 0; i < keys.size(); i++)
    {
        std::string currentKey = keys[i];
        std::string delimiter = "|";
        std::string currentPid = currentKey.substr(0, currentKey.find(delimiter));
        std::string pageNum = currentKey.substr(1, currentKey.find(delimiter));
        int currentFrame = _table[currentKey];
        // TODO: print all pages
        printf(" %4s | %11s | %12i \n", currentPid, pageNum, currentFrame);
    }
}

int PageTable::pageCount(int pid) {
    int pageNum = 0;
    std::string currEntry = std::to_string(pid) + "|" + std::to_string(pageNum);
    while (_table.find(currEntry) != _table.end()) {
        pageNum++;
        currEntry = std::to_string(pid) + "|" + std::to_string(pageNum);
    }
    return pageNum;
}

uint32_t PageTable::getPageSize() {
    return _page_size;
}

void PageTable::deleteEntry(uint32_t pid, uint32_t page_to_delete) 
{
    std::string entry_to_delete = std::to_string(pid) + "|" + std::to_string(page_to_delete);
    _table.erase(entry_to_delete);
}

void PageTable::deletePage(std::string entry) {
    _table.erase(entry);
}

void PageTable::deletePagesLoop(int pid) {
    int pageNum = 0;
    std::string currEntry = std::to_string(pid) + "|" + std::to_string(pageNum);
    while (_table.find(currEntry) != _table.end()) {
        _table.erase(currEntry);
        pageNum++;
        currEntry = std::to_string(pid) + "|" + std::to_string(pageNum);
    }

}
