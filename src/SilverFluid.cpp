#include "Silver.hpp"
#include "SilverFluid.hpp"
#include <atomic>
#include <cmath>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

using namespace std;

mutex WorkspaceMutex;
atomic<bool> stopGlobal{false};
unordered_map<int, thread> cullingThreads;
unordered_map<int, thread> flowThreads;

// Function Definitions
void globalCullingThread(int rootID) {
 /* int lastProcessedDepth = 0;
  while (!stopGlobal) {
    lock_guard<mutex> lock(WorkspaceMutex);

    auto it = Workspace.begin();
    while (it != Workspace.end()) {
      Actor m = *((*it).second);
      Actor* f = (*it).second;

      if (m.GetComponent<Fluid>()->fluidRoot != rootID && f->GetInstanceID() != rootID) {
        ++it;
        continue;
      }

      int parentID = m.GetComponent<Fluid>()->fluidParent;
      queue<int> ancestorsQueue;
      ancestorsQueue.push(parentID);
      bool hasDeadAncestor = false;

      while (!ancestorsQueue.empty()) {
        int currentParentID = ancestorsQueue.front();
        ancestorsQueue.pop();

        if (currentParentID == -1)
          break;

        Actor* parentIt = InstanceIDToActor(currentParentID);
        if (parentIt == nullptr || parentIt->GetComponent<Fluid>()->isDead) {
          hasDeadAncestor = true;
          break;
        }

        ancestorsQueue.push(parentIt->GetComponent<Fluid>()->fluidParent);
      }

      if (hasDeadAncestor && !m.GetComponent<Fluid>()->isDead) {
        m.GetComponent<Fluid>()->isDead = true;
      }

      int depth = m.GetComponent<Fluid>()->fluidDepth;

      for (auto [key, entry] : Workspace) {
        if (entry->GetInstanceID() == f->GetInstanceID())
          continue;
        if (entry->GetComponent<Transform>()->position.x ==
                m.GetComponent<Transform>()->position.x &&
            entry->GetComponent<Transform>()->position.y ==
                m.GetComponent<Transform>()->position.y &&
            entry->GetComponent<Fluid>()->preventFlowing) {
          if (!m.GetComponent<Fluid>()->isDead) {
            m.GetComponent<Fluid>()->isDead = true;
          }
          break;
        }
      }

      if (m.GetComponent<Fluid>()->isDead) {
        if (lastProcessedDepth != depth)
          Wait(m.GetComponent<Fluid>()->drySpeed);
        it = Workspace.erase(it);
        lastProcessedDepth = depth;
        continue;
      }

      ++it;
    }
  } */
}



void ThreadedFlow(int rootID) {
  /* queue<pair<int, int>> flowQueue;
  {
    lock_guard<mutex> lock(WorkspaceMutex);
    auto actor = InstanceIDToActor(rootID);
    if (actor == nullptr || !actor->GetComponent<Fluid>()->isFluid)
      return;
    flowQueue.push({rootID, 0});
  }

  const double waitTimeReductionFactor = 0.9;
  int lastGen = -1;

  while (!flowQueue.empty() && !stopGlobal) {
    auto [currentID, generationCount] = flowQueue.front();
    flowQueue.pop();

    {
      lock_guard<mutex> lock(WorkspaceMutex);
      auto actor = InstanceIDToActor(currentID);
      if (actor == nullptr || actor->GetComponent<Fluid>()->isDead)
        continue;
    }

    if (generationCount >= Workspace[currentID]->GetComponent<Fluid>()->maximumDistance)
      break;

    double waitTime = Workspace[currentID]->GetComponent<Fluid>()->diffusionSpeed *
                      pow(waitTimeReductionFactor, generationCount);
    if (generationCount != lastGen) {
      Wait(waitTime);
      lastGen = generationCount;
    }

    bool flowed = false;

    for (int dir = 0; dir < 4; ++dir) {
      int newX = Workspace[currentID]->GetComponent<Transform>()->position.x;
      int newY = Workspace[currentID]->GetComponent<Transform>()->position.y;

      switch (dir) {
      case 0:
        newY++;
        break;
      case 1:
        newY--;
        break;
      case 2:
        newX--;
        break;
      case 3:
        newX++;
        break;
      }

      bool canFlow = true;
      {
        lock_guard<mutex> lock(WorkspaceMutex);
        for (auto [key, entry] : Workspace) {
          if (entry->GetComponent<Transform>()->position.x == newX &&
              entry->GetComponent<Transform>()->position.y == newY) {
            if (entry->GetComponent<Fluid>()->preventFlowing || entry->GetComponent<Fluid>()->isFluid) {
              canFlow = false;
              break;
            }
          }
        }
      }

      if (canFlow) {
        int newDupID;
        {
          lock_guard<mutex> lock(WorkspaceMutex);
          newDupID = Duplicate(currentID)[0];
          Workspace[newDupID]->GetComponent<Transform>()->position.x = newX;
          Workspace[newDupID]->GetComponent<Transform>()->position.y = newY;
          Workspace[newDupID]->GetComponent<Fluid>()->fluidParent = rootID;
          Workspace[newDupID]->GetComponent<Fluid>()->fluidRoot = rootID;
          Workspace[newDupID]->GetComponent<Fluid>()->fluidDepth =
              Workspace[currentID]->GetComponent<Fluid>()->fluidDepth + 1;
        }

        flowQueue.push({newDupID, generationCount + 1});
        flowed = true;
      }
    }

    if (!flowed)
      break;
  } */
}


void stopAllFlowAndCulling() {
/*
  stopGlobal = true;

  for (auto &[id, thread] : flowThreads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  flowThreads.clear();

  for (auto &[id, thread] : cullingThreads) {
    if (thread.joinable()) {
      thread.join();
    }
  }
  cullingThreads.clear(); */
}





void setAsFluidBlocker(const variant<int, vector<int>> &IDs) {
  /* auto solidifyMesh = [&](int id) {
    if (InstanceIDToActor(id) != nullptr) {
      Actor *m = InstanceIDToActor(id);
      m->GetComponent<Fluid>()->preventFlowing = true;
    }
  };

  if (holds_alternative<int>(IDs)) {
    int id = get<int>(IDs);
    solidifyMesh(id);
  } else if (holds_alternative<vector<int>>(IDs)) {
    for (int id : get<vector<int>>(IDs)) {
      solidifyMesh(id);
    }
  } */
}
