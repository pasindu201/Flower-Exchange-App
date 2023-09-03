#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>  
#include <stack>

using namespace std;

struct DataItem {
    string Cl_order_id;
    int quantity;
    double price;
    int order_id;
};

//function to insert new buying item in to correct position of the order book(price decreasing order).
void pushSorted1(stack<DataItem>& stk, const DataItem& newItem) {
    stack<DataItem> tempStack;

    while (!stk.empty() && stk.top().price > newItem.price) {
        tempStack.push(stk.top());
        stk.pop();
    }
    stk.push(newItem);

    while (!tempStack.empty()) {
        stk.push(tempStack.top());
        tempStack.pop();
    }
}

//function to insert new selling item in to correct position of the order book(price increasing order).
void pushSorted2(stack<DataItem>& stk, const DataItem& newItem) {
    stack<DataItem> tempStack;

    while (!stk.empty() && stk.top().price < newItem.price) {
        tempStack.push(stk.top());
        stk.pop();
    }

    stk.push(newItem);

    while (!tempStack.empty()) {
        stk.push(tempStack.top());
        tempStack.pop();
    }
}

// push result to the execution report.
void pushResult(ofstream& outputFile, const DataItem& newItem, const string& instrument, int side, const string& status) {
    outputFile << newItem.order_id << ',';
    outputFile << newItem.Cl_order_id << ',';
    outputFile << instrument << ',';
    outputFile << side << ',';
    outputFile << status << ',';
    outputFile << newItem.quantity << ',';
    outputFile << newItem.price << '\n';
}
void pushResult_with_instrument_error(ofstream& outputFile, const DataItem& newItem, int side, const string& status){
    outputFile << newItem.order_id << ',';
    outputFile << newItem.Cl_order_id << ',';
    outputFile << " " << ',';
    outputFile << side << ',';
    outputFile << status << ',';
    outputFile << newItem.quantity << ',';
    outputFile << newItem.quantity << ',';
    outputFile << "Invalid Instrument" << '\n';
}
void pushResult_with_side_error(ofstream& outputFile, const DataItem& newItem, const string& instrument, const string& status){
    outputFile << newItem.order_id << ',';
    outputFile << newItem.Cl_order_id << ',';
    outputFile << instrument << ',';
    outputFile << " " << ',';
    outputFile << status << ',';
    outputFile << newItem.quantity << ',';
    outputFile << "Invalid side" << ',';
    outputFile << newItem.price << '\n';
}
void pushResult_with_price_error(ofstream& outputFile, const DataItem& newItem, const string& instrument, int side, const string& status){
    outputFile << newItem.order_id << ',';
    outputFile << newItem.Cl_order_id << ',';
    outputFile << instrument << ',';
    outputFile << side << ',';
    outputFile << status << ',';
    outputFile << newItem.quantity << ',';
    outputFile << "Invalid price" << ',';
    outputFile << newItem.price << '\n';
}
void pushResult_with_quentity_error(ofstream& outputFile, const DataItem& newItem, const string& instrument, int side, const string& status){
    outputFile << newItem.order_id << ',';
    outputFile << newItem.Cl_order_id << ',';
    outputFile << instrument << ',';
    outputFile << side << ',';
    outputFile << status << ',';
    outputFile << newItem.quantity << ',';
    outputFile << "Invalid quentity" << ',';
    outputFile << newItem.price << '\n';
}
int main() {
    ifstream file("order.csv");  // Replace with your file name

    if (!file.is_open()) {
        cout << "Failed to open the file." << endl;
        return 1;
    }
    
    string filename = "execution_rep1.csv"; // Define the file name for out put CSV file
    ofstream outputFile(filename);  // Create an output file stream

    if (!outputFile.is_open()) {
        cout << "Failed to open the output file." << endl;
        return 1;
    }
    outputFile << "Order ID" << ",";
    outputFile << "Order" << ",";
    outputFile << "Instrument" << ",";
    outputFile << "Side" << ",";
    outputFile << "Exec Status" << ",";
    outputFile << "Quentity" << ",";
    outputFile << "Reason" << ",";
    outputFile << "Price" << "\n";

    stack<DataItem> rose_order_book_buySide, rose_order_book_sellSide;  //order book for rose
    stack<DataItem> lavender_order_book_buySide, lavender_order_book_sellSide;  //order book for lavender
    stack<DataItem> lotus_order_book_buySide, lotus_order_book_sellSide;  //order book for lotus
    stack<DataItem> tulip_order_book_buySide, tulip_order_book_sellSide;  //order book for tulip
    stack<DataItem> orchid_order_book_buySide, orchid_order_book_sellSide;  //order book for orchid
    
    string line;
    bool isFirstLine = true;
    int k = -1;
    while (getline(file, line)) {
        k++;   
        if (isFirstLine) {
            isFirstLine = false;
            continue; // Skip the first line
        }
        istringstream ss(line);
        string col1, col2, col3, col4, col5;
        if (getline(ss, col1, ',') &&
            getline(ss, col2, ',') &&
            getline(ss, col3, ',') &&
            getline(ss, col4, ',') &&
            getline(ss, col5, ',')) {

            int col4_int = stoi(col4);    // Quantity
            double col5_double = stod(col5); // Price
            int side = stoi(col3);  // Side

            DataItem item = {col1, col4_int, col5_double, k};
            //checking whether price is valid or not
            if (item.price < 0) {
                pushResult_with_price_error(outputFile, item, col2, side, "Reject");
                continue;
            }
            //check for the valid quentity
            else if(10 >= item.quantity || item.quantity >= 1000){
                pushResult_with_quentity_error(outputFile, item, col2, side, "Reject");
                continue;
            }
            else if (col2 == "Rose"){
                // process buying order for rose
                if (side == 1){
                    bool pFill = false;
                    // try to fill with existing suppliers in order book.
                    while (!rose_order_book_sellSide.empty() && item.quantity > 0) {
                        DataItem topItem = rose_order_book_sellSide.top();
                        if (item.price >= topItem.price) {
                            if (item.quantity > topItem.quantity){
                                topItem.price = item.price;
                                int temp = item.quantity - topItem.quantity;
                                item.quantity = topItem.quantity;
                                pushResult(outputFile, item, "Rose", 1, "pFill");
                                pushResult(outputFile, topItem, "Rose", 2, "Fill");
                                item.quantity = temp;
                                rose_order_book_sellSide.pop();
                                pFill = true;         
                            }
                            else if (item.quantity == topItem.quantity) { 
                                topItem.price = item.price;
                                pushResult(outputFile, item, "Rose", 1, "Fill");
                                pushResult(outputFile, topItem, "Rose", 2, "Fill");
                                rose_order_book_sellSide.pop();
                                item.quantity = 0;
                                break;
                            }
                            else {
                                int temp_quentity = topItem.quantity - item.quantity;
                                double temp_price = topItem.price;
                                topItem.price = item.price;
                                topItem.quantity = item.quantity;
                                pushResult(outputFile, item, "Rose", 1, "Fill");
                                pushResult(outputFile, topItem, "Rose", 2, "pFill");
                                topItem.price = temp_price;
                                topItem.quantity = temp_quentity;
                                item.quantity = 0;               
                            }
                           
                        } else {
                            break;
                        }
                    }
                    if (item.quantity > 0) {
                        if (!pFill) pushResult(outputFile, item, "Rose", 1, "New");
                        pushSorted1(rose_order_book_buySide, item);
                    }
                }
                //process selling order for rose
                else if (side == 2) {
                    bool pFill = false;
                    // try to fill with existing buyers in order book.
                    while (!rose_order_book_buySide.empty() && item.quantity > 0) {
                        DataItem topItem = rose_order_book_buySide.top();
                        if (item.price <= topItem.price) {
                            if (item.quantity > topItem.quantity){
                                double temp_price = item.price;
                                item.price = topItem.price;
                                int temp = item.quantity - topItem.quantity;
                                item.quantity = topItem.quantity;
                                pushResult(outputFile, item, "Rose", 2, "pFill");
                                pushResult(outputFile, topItem, "Rose", 1, "Fill");
                                item.quantity = temp;
                                item.price = temp_price;
                                rose_order_book_buySide.pop();
                                pFill = true;
                            }
                            else if (item.quantity == topItem.quantity) { 
                                item.price = topItem.price;
                                pushResult(outputFile, item, "Rose", 2, "Fill");
                                pushResult(outputFile, topItem, "Rose", 1, "Fill");
                                rose_order_book_buySide.pop();
                                item.quantity = 0;
                                break;
                            }
                            else {
                                int temp_quentity = topItem.quantity - item.quantity;
                                item.price = topItem.price;
                                topItem.quantity = item.quantity;
                                pushResult(outputFile, item, "Rose", 2, "Fill");
                                pushResult(outputFile, topItem, "Rose", 1, "pFill");
                                topItem.quantity = temp_quentity; 
                                item.quantity = 0;                          
                            }
                           
                        } else {
                            break;
                        }
                    }
                    if (item.quantity > 0) {
                        if (!pFill) pushResult(outputFile, item, "Rose", 2, "New");
                        pushSorted2(rose_order_book_sellSide, item);
                    }
                }
                else {
                    //error
                }
            } 
            else if (col2 == "Lavender"){
                //process buying order for Lavender
                if (side == 1){
                    bool pFill = false;
                    // try to fill with existing suppliers in order book.
                    while (!lavender_order_book_sellSide.empty() && item.quantity > 0) {
                        DataItem topItem = lavender_order_book_sellSide.top();
                        if (item.price >= topItem.price) {
                            if (item.quantity > topItem.quantity){
                                topItem.price = item.price;
                                int temp = item.quantity - topItem.quantity;
                                item.quantity = topItem.quantity;
                                pushResult(outputFile, item, "Lavender", 1, "pFill");
                                pushResult(outputFile, topItem, "Lavender", 2, "Fill");
                                item.quantity = temp;
                                lavender_order_book_sellSide.pop();
                                pFill = true;                               
                            }
                            else if (item.quantity == topItem.quantity) { 
                                topItem.price = item.price;
                                pushResult(outputFile, item, "Lavender", 1, "Fill");
                                pushResult(outputFile, topItem, "Lavender", 2, "Fill");
                                lavender_order_book_sellSide.pop();
                                item.quantity = 0;
                                break;
                            }
                            else {
                                int temp_quentity = topItem.quantity - item.quantity;
                                double temp_price = topItem.price;
                                topItem.price = item.price;
                                topItem.quantity = item.quantity;
                                pushResult(outputFile, item, "Lavender", 1, "Fill");
                                pushResult(outputFile, topItem, "Lavender", 2, "pFill");
                                topItem.price = temp_price;
                                topItem.quantity = temp_quentity;  
                                item.quantity = 0;                             
                            }
                           
                        } else {
                            break;
                        }
                    }
                    if (item.quantity > 0) {
                        if (!pFill) pushResult(outputFile, item, "Lavender", 1, "New");
                        pushSorted1(lavender_order_book_buySide, item);
                    }
                }
                //process selling order for Lavender
                else if (side == 2) {
                    bool pFill = false;
                    // try to fill with existing buyers in order book.
                    while (!lavender_order_book_buySide.empty() && item.quantity > 0) {
                        DataItem topItem = lavender_order_book_buySide.top();
                        if (item.price <= topItem.price) {
                            if (item.quantity > topItem.quantity){
                                double temp_price = item.price;
                                item.price = topItem.price;
                                int temp = item.quantity - topItem.quantity;
                                item.quantity = topItem.quantity;
                                pushResult(outputFile, item, "Lavender", 2, "pFill");
                                pushResult(outputFile, topItem, "Lavender", 1, "Fill");
                                item.quantity = temp;
                                item.price = temp_price;
                                lavender_order_book_buySide.pop();
                                pFill = true;
                            }
                            else if (item.quantity == topItem.quantity) { 
                                item.price = topItem.price;
                                pushResult(outputFile, item, "Lavender", 2, "Fill");
                                pushResult(outputFile, topItem, "Lavender", 1, "Fill");
                                lavender_order_book_buySide.pop();
                                item.quantity = 0;
                                break;
                            }
                            else {
                                int temp_quentity = topItem.quantity - item.quantity;
                                item.price = topItem.price;
                                topItem.quantity = item.quantity;
                                pushResult(outputFile, item, "Lavender", 2, "Fill");
                                pushResult(outputFile, topItem, "Lavender", 1, "pFill");
                                topItem.quantity = temp_quentity;
                                item.quantity = 0;                                
                            }
                           
                        } else {
                            break;
                        }
                    }
                    if (item.quantity > 0) {
                        if (!pFill) pushResult(outputFile, item, "Lavender", 2, "New");
                        pushSorted2(lavender_order_book_sellSide, item);
                    }
                }
                else {
                    //error
                }
            } 
            else if (col2 == "Lotus"){
                            //process buying order for Lotus
                if (side == 1){
                    bool pFill = false;
                    // try to fill with existing suppliers in order book.
                    while (!lotus_order_book_sellSide.empty() && item.quantity > 0) {
                        DataItem topItem = lotus_order_book_sellSide.top();
                        if (item.price >= topItem.price) {
                            if (item.quantity > topItem.quantity){
                                topItem.price = item.price;
                                int temp = item.quantity - topItem.quantity;
                                item.quantity = topItem.quantity;
                                pushResult(outputFile, item, "Lotus", 1, "pFill");
                                pushResult(outputFile, topItem, "Lotus", 2, "Fill");
                                item.quantity = temp;
                                lotus_order_book_sellSide.pop();
                                pFill = true;                              
                            }
                            else if (item.quantity == topItem.quantity) { 
                                topItem.price = item.price;
                                pushResult(outputFile, item, "Lotus", 1, "Fill");
                                pushResult(outputFile, topItem, "Lotus", 2, "Fill");
                                lotus_order_book_sellSide.pop();
                                item.quantity = 0;
                                break;
                            }
                            else {
                                int temp_quentity = topItem.quantity - item.quantity;
                                double temp_price = topItem.price;
                                topItem.price = item.price;
                                topItem.quantity = item.quantity;
                                pushResult(outputFile, item, "Lotus", 1, "Fill");
                                pushResult(outputFile, topItem, "Lotus", 2, "pFill");
                                topItem.price = temp_price;
                                topItem.quantity = temp_quentity;
                                item.quantity = 0;                              
                            }
                           
                        } else {
                            break;
                        }
                    }
                    if (item.quantity > 0) {
                        if (!pFill) pushResult(outputFile, item, "Lotus", 1, "New");
                        pushSorted1(lotus_order_book_buySide, item);
                    }
                }
                //process selling order for Lotus
                else if (side == 2) {
                    bool pFill = false;
                    // try to fill with existing buyers in order book.
                    while (!lotus_order_book_buySide.empty() && item.quantity > 0) {
                        DataItem topItem = lotus_order_book_buySide.top();
                        if (item.price <= topItem.price) {
                            if (item.quantity > topItem.quantity){
                                double temp_price = item.price;
                                item.price = topItem.price;
                                int temp = item.quantity - topItem.quantity;
                                item.quantity = topItem.quantity;
                                pushResult(outputFile, item, "Lotus", 2, "pFill");
                                pushResult(outputFile, topItem, "Lotus", 1, "Fill");
                                item.quantity = temp;
                                item.price = temp_price;
                                lotus_order_book_buySide.pop();
                                pFill = true;
                            }
                            else if (item.quantity == topItem.quantity) { 
                                item.price = topItem.price;
                                pushResult(outputFile, item, "Lotus", 2, "Fill");
                                pushResult(outputFile, topItem, "Lotus", 1, "Fill");
                                lotus_order_book_buySide.pop();
                                item.quantity = 0;
                                break;
                            }
                            else {
                                int temp_quentity = topItem.quantity - item.quantity;
                                item.price = topItem.price;
                                topItem.quantity = item.quantity;
                                pushResult(outputFile, item, "Lotus", 2, "Fill");
                                pushResult(outputFile, topItem, "Lotus", 1, "pFill");
                                topItem.quantity = temp_quentity; 
                                item.quantity = 0;                              
                            }
                           
                        } else {
                            break;
                        }
                    }
                    if (item.quantity > 0) {
                        if (!pFill) pushResult(outputFile, item, "Lotus", 2, "New");
                        pushSorted2(lotus_order_book_sellSide, item);
                    }
                }
                else {
                    //error
                }
            }  
            else if (col2 == "Tulip"){
                //process buying order for Tulip
                if (side == 1){
                    bool pFill = false;
                    // try to fill with existing suppliers in order book.
                    while (!tulip_order_book_sellSide.empty() && item.quantity > 0) {
                        DataItem topItem = tulip_order_book_sellSide.top();
                        if (item.price >= topItem.price) {
                            if (item.quantity > topItem.quantity){
                                topItem.price = item.price;
                                int temp = item.quantity - topItem.quantity;
                                item.quantity = topItem.quantity;
                                pushResult(outputFile, item, "Tulip", 1, "pFill");
                                pushResult(outputFile, topItem, "Tulip", 2, "Fill");
                                item.quantity = temp;
                                tulip_order_book_sellSide.pop();
                                pFill = true;                               
                            }
                            else if (item.quantity == topItem.quantity) { 
                                topItem.price = item.price;
                                pushResult(outputFile, item, "Tulip", 1, "Fill");
                                pushResult(outputFile, topItem, "Tulip", 2, "Fill");
                                tulip_order_book_sellSide.pop();
                                item.quantity = 0;
                                break;
                            }
                            else {
                                int temp_quentity = topItem.quantity - item.quantity;
                                double temp_price = topItem.price;
                                topItem.price = item.price;
                                topItem.quantity = item.quantity;
                                pushResult(outputFile, item, "Tulip", 1, "Fill");
                                pushResult(outputFile, topItem, "Tulip", 2, "pFill");
                                topItem.price = temp_price;
                                topItem.quantity = temp_quentity;
                                item.quantity = 0;                               
                            }
                           
                        } else {
                            break;
                        }
                    }
                    if (item.quantity > 0) {
                        if (!pFill) pushResult(outputFile, item, "Tulip", 1, "New");
                        pushSorted1(tulip_order_book_buySide, item);
                    }
                }
                //process selling order for Tulip
                else if (side == 2) {
                    bool pFill = false;
                    // try to fill with existing buyers in order book.
                    while (!tulip_order_book_buySide.empty() && item.quantity > 0) {
                        DataItem topItem = tulip_order_book_buySide.top();
                        if (item.price <= topItem.price) {
                            if (item.quantity > topItem.quantity){
                                double temp_price = item.price;
                                item.price = topItem.price;
                                int temp = item.quantity - topItem.quantity;
                                item.quantity = topItem.quantity;
                                pushResult(outputFile, item, "Tulip", 2, "pFill");
                                pushResult(outputFile, topItem, "Tulip", 1, "Fill");
                                item.quantity = temp;
                                item.price = temp_price;
                                tulip_order_book_buySide.pop();
                                pFill = true;
                            }
                            else if (item.quantity == topItem.quantity) { 
                                item.price = topItem.price;
                                pushResult(outputFile, item, "Tulip", 2, "Fill");
                                pushResult(outputFile, topItem, "Tulip", 1, "Fill");
                                tulip_order_book_buySide.pop();
                                item.quantity = 0;
                                break;
                            }
                            else {
                                int temp_quentity = topItem.quantity - item.quantity;
                                item.price = topItem.price;
                                topItem.quantity = item.quantity;
                                pushResult(outputFile, item, "Tulip", 2, "Fill");
                                pushResult(outputFile, topItem, "Tulip", 1, "pFill");
                                topItem.quantity = temp_quentity;
                                item.quantity = 0;                              
                            }
                           
                        } else {
                            break;
                        }
                    }
                    if (item.quantity > 0) {
                        if (!pFill) pushResult(outputFile, item, "Tulip", 2, "New");
                        pushSorted2(tulip_order_book_sellSide, item);
                    }
                }
                else {
                    //error
                }
            }  
            else if (col2 == "Orchid"){
                //process buying order for Tulip
                if (side == 1){
                    bool pFill = false;
                    // try to fill with existing suppliers in order book.
                    while (!orchid_order_book_sellSide.empty() && item.quantity > 0) {
                        DataItem topItem = orchid_order_book_sellSide.top();
                        if (item.price >= topItem.price) {
                            if (item.quantity > topItem.quantity){
                                topItem.price = item.price;
                                int temp = item.quantity - topItem.quantity;
                                item.quantity = topItem.quantity;
                                pushResult(outputFile, item, "Orchid", 1, "pFill");
                                pushResult(outputFile, topItem, "Orchid", 2, "Fill");
                                item.quantity = temp;
                                orchid_order_book_sellSide.pop();
                                pFill = true;                               
                            }
                            else if (item.quantity == topItem.quantity) { 
                                topItem.price = item.price;
                                pushResult(outputFile, item, "Orchid", 1, "Fill");
                                pushResult(outputFile, topItem, "Orchid", 2, "Fill");
                                orchid_order_book_sellSide.pop();
                                item.quantity = 0;
                                break;
                            }
                            else {
                                int temp_quentity = topItem.quantity - item.quantity;
                                double temp_price = topItem.price;
                                topItem.price = item.price;
                                topItem.quantity = item.quantity;
                                pushResult(outputFile, item, "Orchid", 1, "Fill");
                                pushResult(outputFile, topItem, "Orchid", 2, "pFill");
                                topItem.price = temp_price;
                                topItem.quantity = temp_quentity;
                                item.quantity = 0;                               
                            }
                           
                        } else {
                            break;
                        }
                    }
                    if (item.quantity > 0) {
                        if (!pFill) pushResult(outputFile, item, "Orchid", 1, "New");
                        pushSorted1(orchid_order_book_buySide, item);
                    }
                }
                //process selling order for Tulip
                else if (side == 2) {
                    bool pFill = false;
                    // try to fill with existing buyers in order book.
                    while (!orchid_order_book_buySide.empty() && item.quantity > 0) {
                        DataItem topItem = orchid_order_book_buySide.top();
                        if (item.price <= topItem.price) {
                            if (item.quantity > topItem.quantity){
                                double temp_price = item.price;
                                item.price = topItem.price;
                                int temp = item.quantity - topItem.quantity;
                                item.quantity = topItem.quantity;
                                pushResult(outputFile, item, "Orchid", 2, "pFill");
                                pushResult(outputFile, topItem, "Orchid", 1, "Fill");
                                item.quantity = temp;
                                item.price = temp_price;
                                orchid_order_book_buySide.pop();
                                pFill = true;
                            }
                            else if (item.quantity == topItem.quantity) { 
                                item.price = topItem.price;
                                pushResult(outputFile, item, "Orchid", 2, "Fill");
                                pushResult(outputFile, topItem, "Orchid", 1, "Fill");
                                orchid_order_book_buySide.pop();
                                item.quantity = 0;
                                break;
                            }
                            else {
                                int temp_quentity = topItem.quantity - item.quantity;
                                item.price = topItem.price;
                                topItem.quantity = item.quantity;
                                pushResult(outputFile, item, "Orchid", 2, "Fill");
                                pushResult(outputFile, topItem, "Orchid", 1, "pFill");
                                topItem.quantity = temp_quentity;
                                item.quantity = 0;                               
                            }
                           
                        } else {
                            break;
                        }
                    }
                    if (item.quantity > 0) {
                        if (!pFill) pushResult(outputFile, item, "Orchid", 2, "New");
                        pushSorted2(orchid_order_book_sellSide, item);
                    }
                }
                else {
                    //invalid side error
                    pushResult_with_side_error(outputFile, item, "Orchid", "Reject");
                }
            }
            else {
                //invalid instrument error
                pushResult_with_instrument_error(outputFile, item, 2, "Reject");       
            }               
        } 
        else {
            cerr << "Error reading line: " << line << endl;
        }

    }
    file.close();
    outputFile.close();
    
    return 0;

}
