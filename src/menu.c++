#include <iostream>
#include <vector>
#include <string>
#include "main.h"

class MenuItem {
private:
    
    std::string content;
public:
    MenuItem(const std::string& n) : content(n) {}
    std::string getContent() const { return content; }
};

class Menu {
private:
    std::vector<MenuItem> items;
    int32_t selectedItem;
    int32_t itemCount;
public:

    
    Menu() {
        selectedItem = 0;
        itemCount = 0;
    }

    VOID addItem(const std::string& content) {
        items.push_back(MenuItem(content));
        itemCount++;
    }
    
    int32_t getSelectedItem() {
        return selectedItem;
    }

    int32_t GetCount(){
        return itemCount;
    }

    std::string GetItemContent(int32_t index){
    if (index >= 0 && index < items.size()) {
        return items[index].getContent();
    } else {
        return "Invalid Index";
    }
}


    VOID IncrementSelectedItem() {
        if(selectedItem < items.size()-1){selectedItem++;}
    }
     VOID DecrementSelectedItem() {
        if(selectedItem > 0){selectedItem--;}
    }
};
