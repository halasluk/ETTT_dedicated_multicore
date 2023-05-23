#ifndef ETTT_DEDICATED_MULTICORE_INSTANCEPARSER_H
#define ETTT_DEDICATED_MULTICORE_INSTANCEPARSER_H

#include <fstream>
#include <string>
#include "EtInstance.h"
#include "EtttInstance.h"

void saveEtInstances(vector<EtInstance> &instances, const string& outputFile);
void saveEtttInstances(vector<EtttInstance> &instances, const string& outputFile);
vector<EtInstance> loadEtInstances(const string& inputFile);
vector<EtttInstance> loadEtttInstances(const string& inputFile);

#endif