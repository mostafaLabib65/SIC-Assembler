#include "PassTwoManager.h"
#include "../CommandsAndUtilities/CommandIdentifier.h"
#include "../ConvertersAndEvaluators/HexaConverter.h"

vector<string> PassTwoManager::generateObjectCode(PrimaryData primaryData) {

    vector<Command> commands = primaryData.commands;
    Command cursor ;//= commands[0];
    vector<string> objectCode;

     cursor = commands[1];
     int itr = 1;
     while (cursor.mnemonic != "END") {
         objectCode.push_back(getObjectCode(cursor,primaryData));
         cursor = commands[++itr];
     }

}

//TODO ta7abeesh special cases
//TODO check for errors
//TODO ne7sseb amaken Modification records

string PassTwoManager::getObjectCode(Command cursor,PrimaryData primaryData) {
    CommandIdentifier opTable;
    HexaConverter hexaConverter;
    if (cursor.mnemonic == "BYTE" || cursor.mnemonic == "WORD") {
        //convert constant to object code
    } else if (opTable.isInTable(cursor.mnemonic)) {

        OperationInfo operationInfo = opTable.getInfo(cursor.mnemonic);
        int commandObjCode = hexaConverter.hexToDecimal(operationInfo.code);
        int format = operationInfo.format;

        if(format == 1){
            return hexaConverter.decimalToHex(commandObjCode);
        } else if(format == 2){
            return completeObjCodeFormat2(commandObjCode,cursor.operands);
        } else if(format == 3){
            return completeObjCodeFormat3(commandObjCode,cursor.operands);
        } else{
            return completeObjCodeFormat4(commandObjCode,cursor.operands,primaryData);
        }
//             if(isSymbol(cursor.operands)) {
//                 /**
//                  * if there is a symbol in OPERAND field then
//     *               begin
//     *                 search SYMTAB for OPERAND
//     *                 if found then
//     *                    store symbol value as operand address
//     *                 else
//     *                    begin
//     *                      store 0 as operand address
//     *                      set error flag (undefined symbol)
//     *                    end
//     *               end {if symboll}
//     *              else
//                  */
//             } else {
//                 //store 0 as operand address
//             }
        //assemble the object code instruction
    }
}
string PassTwoManager::completeObjCodeFormat2(int uncompletedObjCode, vector<string> operands){
    HexaConverter hexaConverter;
    int registerCode = getRegisterNumber(operands[0]);
    if(operands.size() != 1) {
        registerCode = registerCode << 4;
        registerCode = registerCode | getRegisterNumber(operands[1]);
        uncompletedObjCode = uncompletedObjCode << 8;
    } else{
        uncompletedObjCode = uncompletedObjCode << 4;
    }
    uncompletedObjCode = uncompletedObjCode | registerCode;
    return hexaConverter.decimalToHex(uncompletedObjCode);
}
string PassTwoManager::completeObjCodeFormat3(int uncompletedObjCode,vector<string> operands){
    HexaConverter hexaConverter;
    vector<int> nixbpe = getFlagsCombination(operands[0],3);// give me ni separated from xbpe
    int displacement = getDisplacement();
    int completedObjCode = ((uncompletedObjCode | nixbpe[0]) << 4) | nixbpe[1];
    completedObjCode = (completedObjCode << 12) | displacement;
    return hexaConverter.decimalToHex(completedObjCode);
}
string PassTwoManager::completeObjCodeFormat4(int uncompletedObjCode,vector<string> operands,PrimaryData primaryData){
    HexaConverter hexaConverter;
    vector<int> nixbpe = getFlagsCombination(operands[0],4);// give me ni separated from xbpe
    string address = primaryData.symbolTable.at(operands[0]).address;
    int completedObjCode = ((uncompletedObjCode | nixbpe[0]) << 4) | nixbpe[1];
    completedObjCode = (completedObjCode << 20) | completedObjCode;
    return hexaConverter.decimalToHex(completedObjCode);
}

vector<int> PassTwoManager::getFlagsCombination(string operand, int format){
    //TODO your part of code 😀😀😀😀
    if(format == 3){

    } else{

    }
}

int PassTwoManager::getRegisterNumber(string registerr){
    if(registerr.compare("A") == 0){
        return 0;
    } else if(registerr.compare("X") == 0){
        return 1;
    }else if(registerr.compare("L") == 0){
        return 2;
    }else if(registerr.compare("B") == 0){
        return 3;
    }else if(registerr.compare("S") == 0){
        return 4;
    }else if(registerr.compare("T") == 0){
        return 5;
    }else if(registerr.compare("F") == 0){
        return 6;
    }else if(registerr.compare("PC") == 0){
        return 8;
    }else if(registerr.compare("SW") == 0){
        return 9;
    } else{
        return 10;
    }
}

int getDisplacement() {
    //TODO implementation
    return 0;
}

