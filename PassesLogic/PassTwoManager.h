#ifndef ASSEMBLER_PASSTWOMANAGER_H
#define ASSEMBLER_PASSTWOMANAGER_H

#include <iostream>
#include <vector>
#include "../DTOs/PrimaryData.h"
#include "../DTOs/ModificationRecord.h"
#include "../ConvertersAndEvaluators/HexaConverter.h"
#include "../CommandsAndUtilities/CommandIdentifier.h"


using namespace std;

class PassTwoManager {

private:
    string nextInstructionAddress;
    vector<vector<string>> DefRecord;
    vector<ModificationRecord> modificationRecords;
    vector<string> definitions;
    vector<string> references;
    map<string, string> defRecordUnsorted;
    map<string, string> extDefinitions;
    vector<string> litrals;
    vector<string> textRecord;
    HexaConverter hexaConverter;
    bool baseAvailable = false;
    void checkForErrors(Command cursor);
    void update(Command cursor,map<string,  Literal> literalTable);
    void calculateLitrals(map<string,  Literal> literalTable);
    string convertCToObjCode(string str);
    bool noObjCode(string mnemonic);
public:
    void generateObjectCode(PrimaryData primaryData);
    vector<ModificationRecord> getModifiactionRecords();
    vector<vector<string>> getDefRecord();
    vector<string> getTextRecord();
};


#endif