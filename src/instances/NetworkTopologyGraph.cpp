#include "../../include/NetworkTopologyGraph.h"

NetworkTopologyGraph::NetworkTopologyGraph(int numVerts, bool random) {
    if (numVerts < 2) {
        cerr << "WARNING: wrong number of vertices in network topology graph, number of vertices on input: " << numVerts << "; must be at least equal to 2; changing number of vertices to 2" << endl;
        this->numVerts = 2;
    } else {
        this->numVerts = numVerts;
    }
    this->totalNumProcessors = 0;
    this->numUsedProcessors = 0;
    this->random = random;
    vector<pair<int, int>> temp;
    vector<vector<pair<int, int>>> adjInit(this->numVerts, temp);
    this->adjacencyList = adjInit;
}

void NetworkTopologyGraph::generateRandomTreeTopologyGraph() {
    random_device seed;
    int processorId = 0;
    int connectToVert;
    for (int toBeConnectedVert = 1; toBeConnectedVert < numVerts; toBeConnectedVert++) {
        connectToVert = (int)(seed() % toBeConnectedVert);
        adjacencyList[connectToVert].push_back(make_pair(toBeConnectedVert, processorId));
        processorId++;
        adjacencyList[toBeConnectedVert].push_back(make_pair(connectToVert, processorId));
        processorId++;
    }
    totalNumProcessors = processorId;
}

vector<vector<int>> NetworkTopologyGraph::generateJobSequences(int numTasks) {
    vector<vector<int>> jobSequences;
    vector<int> jobSequence;
    vector<int> setOfEndNodesToChooseFrom;
    random_device seed;
    int sourceVertIdx;
    int targetVertIdx;
    int sourceVert;
    int targetVert;
    for (int i = 0; i < numVerts; i++) {
        if (!random) {
            if (adjacencyList[i].size() == 1) {
                setOfEndNodesToChooseFrom.push_back(i);
            }
        } else {
            setOfEndNodesToChooseFrom.push_back(i);
        }
    }
    for (int i = 0; i < numTasks; i++) {
        sourceVertIdx = (int)(seed() % (int)setOfEndNodesToChooseFrom.size());
        targetVertIdx = (int)(seed() % (int)setOfEndNodesToChooseFrom.size());
        while (sourceVertIdx == targetVertIdx) {
            targetVertIdx = (int)(seed() % (int)setOfEndNodesToChooseFrom.size());
        }
        sourceVert = setOfEndNodesToChooseFrom[sourceVertIdx];
        targetVert = setOfEndNodesToChooseFrom[targetVertIdx];
        jobSequence = findShortestPath(sourceVert, targetVert);
        if (!jobSequence.empty()) {
            jobSequences.push_back(jobSequence);
        } else {
            cerr << "WARNING: no connection between vertices: " << sourceVert << ", " << targetVert << "; generating new job sequence" << endl;
            i--;
        }
    }
    return jobSequences;
}

vector<int> NetworkTopologyGraph::findShortestPath(int sourceVert, int targetVert) {
    vector<int> path;
    list<int> queue;
    vector<bool> visited(numVerts, false);
    vector<int> predecessors(numVerts, NIL);
    vector<int> paths(numVerts, NIL);
    int currentVert;
    visited[sourceVert] = true;
    queue.push_back(sourceVert);
    while (!queue.empty()) {
        currentVert = queue.front();
        queue.pop_front();
        for (int i = 0; i < adjacencyList[currentVert].size(); i++) {
            if (!visited[adjacencyList[currentVert][i].first]) {
                visited[adjacencyList[currentVert][i].first] = true;
                predecessors[adjacencyList[currentVert][i].first] = currentVert;
                paths[adjacencyList[currentVert][i].first] = adjacencyList[currentVert][i].second;
                queue.push_back(adjacencyList[currentVert][i].first);
                if (adjacencyList[currentVert][i].first == targetVert) {
                    while (targetVert != sourceVert) {
                        path.insert(path.begin(), paths[targetVert]);
                        targetVert = predecessors[targetVert];
                    }
                    return path;
                }
            }
        }
    }
    return path;
}

void NetworkTopologyGraph::getRelaxedJobSequences(vector<vector<int>> &jobSequences) {
    set<int> setOfProcessorIds;
    for (auto & jobSequence : jobSequences) {
        for (auto & oldProcessorId : jobSequence) {
            setOfProcessorIds.insert(oldProcessorId);
        }
    }
    vector<int> newProcessorIds(totalNumProcessors, NIL);
    int newProcessorId = 0;
    for (auto & oldId : setOfProcessorIds) {
        newProcessorIds[oldId] = newProcessorId;
        newProcessorId++;
    }
    for (auto & jobSequence : jobSequences) {
        for (auto & oldProcessorId : jobSequence) {
            oldProcessorId = newProcessorIds[oldProcessorId];
        }
    }
    numUsedProcessors = newProcessorId;
}