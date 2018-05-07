#include "PassTwoManager.h"
#include "../CommandsAndUtilities/CommandIdentifier.h"
#include "../ConvertersAndEvaluators/HexaConverter.h"
#include "../ConvertersAndEvaluators/ExpressionEvaluator.h"

string locationCounter;
map<string, labelInfo> symbolTable;
HexaConverter hexaConverter;

vector<string> PassTwoManager::generateObjectCode(PrimaryData primaryData) {

    vector<Command> commands = primaryData.commands;
    Command cursor;//= commands[0];
    vector<string> objectCode;
    cursor = commands[1];
    int itr = 1;
    symbolTable = primaryData.symbolTable;
    while (cursor.mnemonic != "END") {
        objectCode.push_back(getObjectCode(cursor));
        cursor = commands[++itr];
        locationCounter = cursor.address;
    }
}

//TODO EQU, ORG, BASE, NOBASE, RSUB
//TODO check for errors
//TODO ne7sseb amaken Modification records
//TODO handel format 4 just like format 3
string PassTwoManager::getObjectCode(Command cursor) {
    CommandIdentifier opTable;
    if (cursor.mnemonic == "WORD") { //check if it's right
        hexaConverter.decimalToHex(stoi(cursor.mnemonic));
    } else if (cursor.mnemonic == "BYTE") { //check if it's right
        string operand = cursor.mnemonic;
        if (operand.front() == 'X') {
            return operand.substr(2, operand.length() - 2);
        } else if (operand.front() == 'C') {
            return convertCToObjCode(operand.substr(2, operand.length() - 2));
        } else {
            //TODO Error
        }
    } else if (opTable.isInTable(cursor.mnemonic)) {

        OperationInfo operationInfo = opTable.getInfo(cursor.mnemonic);
        int commandObjCode = hexaConverter.hexToDecimal(operationInfo.code);
        int format = operationInfo.format;

        if (format == 1) {
            return hexaConverter.decimalToHex(commandObjCode);
        } else if (format == 2) {
            return completeObjCodeFormat2(commandObjCode, cursor.operands);
        } else if (format == 3 && cursor.operands[0].front() != '+') {
            return completeObjCodeFormat3(commandObjCode, cursor.operands);
        } else {
            return completeObjCodeFormat4(commandObjCode, cursor.operands);
        }
    }
}

string PassTwoManager::completeObjCodeFormat2(int uncompletedObjCode, vector<string> operands) {
    HexaConverter hexaConverter;
    int registerCode = getRegisterNumber(operands[0]);
    if (operands.size() != 1) {
        registerCode = registerCode << 4;
        registerCode = registerCode | getRegisterNumber(operands[1]);
        uncompletedObjCode = uncompletedObjCode << 8;
    } else {
        uncompletedObjCode = uncompletedObjCode << 4;
    }
    uncompletedObjCode = uncompletedObjCode | registerCode;
    return hexaConverter.decimalToHex(uncompletedObjCode);
}

string PassTwoManager::completeObjCodeFormat3(int uncompletedObjCode, vector<string> operands) {
    ExpressionEvaluator expressionEvaluator(symbolTable, hexaConverter);
    OperandHolder operandHolder("", 0);
    labelInfo label;
    int displacement;
//    int isPC;
//    int flag = 0;
    bool isPC;
    bool flag = false;
    vector<int> results;
    if (operands.size() != 0) {
        bool isAnExpression = isExpression(operands[0]);
        string address;
        if (isAnExpression) {
            operandHolder = expressionEvaluator.evaluateExpression(operands[0], locationCounter);
            address = operandHolder.value;
        } else if (operands[0][0] != '#' || operands[0][0] != '@') { //&&
            label = symbolTable.at(operands[0]);
            address = label.address;
        } else if ((operands[0][0] == '#' || operands[0][0] == '@') &&
                   symbolTable.find(operands[0].substr(1, operands[0].length() - 1)) == symbolTable.end()) {
            label = symbolTable.at(operands[0].substr(1, operands[0].length() - 1)); //constant?
            address = label.address;
        } else if (operands[0][0] == '#' && symbolTable.find(operands[0].substr(1, operands[0].length() - 1)) !=
                                            symbolTable.end()) { //leeh mafeeshwa7da zai @?
            displacement = stoi(operands[0].substr(1, operands[0].length() - 1));
            if (displacement <= 2047) {
                isPC = true;
            } else if (displacement > 2047 && displacement <= 4095) {
                isPC = false;
            } else {
                //TODO ERROR
            }

            flag = true; //flag++;

        } else {
            //TODO ERROR ta2reban
        }
        if (!flag) { //if (flag == 0) {
            results = getSimpleDisplacement(address, locationCounter);
            if (results[0] == 1) {  // isPC = results[0];
                isPC = true;
            } else {
                isPC = false;
            }
            displacement = results[1];
        }
        vector<int> nixbpe = getFlagsCombination(operands, 3, isPC); // give me ni separated from xbpe
        int completedObjCode = ((uncompletedObjCode | nixbpe[0]) << 4) | nixbpe[1];
        completedObjCode = (completedObjCode << 12) | ((displacement << 20) >> 20);
        return hexaConverter.decimalToHex(completedObjCode);
    } else {
        //TODO RSUB "ta2reban mesh 7ane3melha" return opcode only ex: 1027 RSUB 4C0000
    }
}

string PassTwoManager::completeObjCodeFormat4(int uncompletedObjCode, vector<string> operands) {
    HexaConverter hexaConverter;
    vector<int> nixbpe = getFlagsCombination(operands, 4, false);// give me ni separated from xbpe

    ExpressionEvaluator expressionEvaluator(symbolTable, hexaConverter);
    OperandHolder operandHolder("", 0);
    string address;
    if (operands.size() != 0) { // mafeesh #,@
        bool isAnExpression = isExpression(operands[0]);
        string address;
        if (isAnExpression) {
            operandHolder = expressionEvaluator.evaluateExpression(operands[0], locationCounter);
            address = operandHolder.value;
        } else {
            address = symbolTable.at(operands[0]).address;
        }
//    unsigned int completedObjCode = ((uncompletedObjCode | nixbpe[0]) << 4) | nixbpe[1];
//    completedObjCode = (completedObjCode << 20) | ((completedObjCode << 12) >> 12);//TODO its not completedObjCode after or its TA in its all cases must be handeled
        unsigned int completedObjCode = ((((uncompletedObjCode >> 2) << 2) | nixbpe[0]) << 4) |
                                        nixbpe[1]; //deleted first two bits from the right (enta sa7 :D)
        completedObjCode = (completedObjCode << 20) | ((stoi(address) << 12) >> 12);
        return hexaConverter.decimalToHex(completedObjCode);
    } else {
        //TODO RSUB
    }
}

vector<int> PassTwoManager::getFlagsCombination(vector<string> operands, int format, bool PCRelative) {
    int ni = 0;
    int xbpe = 0;
    if (operands.size() > 2) {
        //TODO ERROR too many operands //mesh 3arfa handelled wala la2?!!
    }
    if (operands.size() == 2) {
        if (operands[1] == "X") {
            xbpe = 8;       //indexing
        } else {
            //TODO ERROR wrong second operand
        }
    }
    if (format == 3) {
        if (operands[0].front() == '#') {
            if (xbpe = 8) {
                //TODO ERROR can't have immediate with X
            }
            ni = 1;             //01 immediate
        } else if (operands[0].front() == '@') {
            if (xbpe = 8) {
                //TODO ERROR can't have indirect with X
            }
            ni = 2;  //10 indirect
        } else {
            ni = 3; //11 simple addressing
        }
        if (PCRelative) {
            xbpe = xbpe | 2; //pc relative
        } else {
            xbpe = xbpe | 4; //base relative
        }

    } else { //format 4
        ni = 3;
        xbpe = xbpe | 1;
    }
    vector<int> returnedValue;
    returnedValue.push_back(ni);
    returnedValue.push_back(xbpe);
}

int PassTwoManager::getRegisterNumber(string registerr) {
    if (registerr.compare("A") == 0) {
        return 0;
    } else if (registerr.compare("X") == 0) {
        return 1;
    } else if (registerr.compare("L") == 0) {
        return 2;
    } else if (registerr.compare("B") == 0) {
        return 3;
    } else if (registerr.compare("S") == 0) {
        return 4;
    } else if (registerr.compare("T") == 0) {
        return 5;
    } else if (registerr.compare("F") == 0) {
        return 6;
    } else if (registerr.compare("PC") == 0) {
        return 8;
    } else if (registerr.compare("SW") == 0) {
        return 9;
    } else {
        return 10;
    }
}

vector<int> PassTwoManager::getSimpleDisplacement(string TA, string progCounter) {
    HexaConverter hexaConverter;
    vector<int> results;
    int displacement = 0;
    int isPC = 1;
    int targetAdd = hexaConverter.hexToDecimal(TA);
    int programCount = hexaConverter.hexToDecimal(progCounter);
    displacement = targetAdd - programCount;
    if (displacement > 2047) {
        isPC = 0;
    } else if (displacement < -2048) {
        //TODO Error
    } else if (displacement > 4095) {
        //TODO Error
    }
    results.push_back(isPC);
    results.push_back(displacement);
    return results;
}

bool PassTwoManager::isExpression(string operand) {
    if (operand.find('+') != std::string::npos || operand.find('-') != std::string::npos ||
        operand.find('*') != std::string::npos || operand.find('/') != std::string::npos) {
        return true;
    }
    return false;
}

string PassTwoManager::convertCToObjCode(string str) {
    //TODO implementation
    return "";
}
