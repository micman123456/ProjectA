#include <iostream>
#include <vector>
#include <string>

class Dialogue {
private:
    int NumberOfPages;
    int CurrentPage;
    BOOL set = false;
    
    std::vector<std::vector<std::string>> dialogueData; 

public:
    Dialogue() : NumberOfPages(6) {
       
        dialogueData.resize(6, std::vector<std::string>(4, std::string(42, ' ')));
        CurrentPage = 0;
    }
public:
    Dialogue(int numPages) : NumberOfPages(numPages) {
       
        dialogueData.resize(numPages, std::vector<std::string>(4, std::string(42, ' ')));
        CurrentPage = 0;
    }

    
    void setDialogue(int page, int column, const std::string& text) {
        if (page >= 0 && page < NumberOfPages && column >= 0 && column < 4) {
            dialogueData[page][column] = text.substr(0, 42);
        } else {
            
        }
    }

    int getCurrentPage(){
        return CurrentPage;
    }

    void incrementPage(){
        CurrentPage++;
    }

    VOID Set(){
        set = TRUE;
    }
    
    BOOL IsSet(){
        return set;
    }

    std::string getDialogue(int page, int column) const {
        if (page >= 0 && page < NumberOfPages && column >= 0 && column < 4 ) {
            return dialogueData[page][column];
        } else {
           
            return "";
        }
    }

    std::string getDialogueInOrder(int CurrentPage, int column) const {
        if (CurrentPage >= 0 && CurrentPage < NumberOfPages && column >= 0 && column < 4) {
            return dialogueData[CurrentPage][column];
        } else {
           
            return "";
        }
    }

    void setNumberOfPages(int32_t number){
        NumberOfPages = number;
    }
    
    int getNumberOfPages() const {
        return NumberOfPages;
    }

    void ResetDialogue(){
        CurrentPage = 0;
    }

};


