//
// Created by Hyewon Song on 2023/11/23.
//
// Collapse : ctrl-K-0 ctrl-K-J
// Indent : Alt-Shift-F

#include <limits.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <random>
#include <algorithm>
#include <ctime>
#include <iomanip>

#include <utils/config.h>
#include <utils/etc.h>
#include "tinyxml.h"
#include "json/json.h"
#include "Router/Router.h"

#include "Controller/SimulationController.h"
#include "Object/Network/Node.h"
#include "Object/Network/Link.h"
#include "Object/Output/OutputWriter.h"
#include "Object/VehicleManager.h"
#include "Object/Agent/VehicleInterface.h"
#include "Object/NetworkManager.h"

using namespace std;
using namespace SALT;

vector<string> sample(vector<VehicleInterface*> vehs, int samples)
{
    vector<string> ret_value;
    std::mt19937 gen(1);
    std::uniform_int_distribution<> dis(0, vehs.size() - 1);

    for (int i = 0; i < samples; i++)
    {
        auto first = vehs.begin();
        auto offset = dis(gen);

        std::advance(first, offset);
        auto selected = *first;
        ret_value.push_back(selected->getMyName());
    }

    return ret_value;
}

int main(int argc, char **argv)
{
    int rtn = 0;

    SALT::SimulationController *SC = new SALT::SimulationController();
    SALT::Router *UR = new SALT::Router();

    cout << "#########################" << endl;
    cout << "SALT Simulator with MPDUA" << endl;
    cout << "#########################" << endl << endl;

    UR->runSetUp(argc, argv, SC);
    cout << endl; cout << endl;

    UR->runData(SC);
    cout << endl; cout << endl;

    UR->runDijkstra(SC);
    cout << endl; cout << endl;

    UR->runOD2Trips();
    cout << endl; cout << endl;

    UR->runRouter(0);
    cout << endl; cout << endl;

    auto timeStart = Clock::now();
    int NUMBER_PRINT = 20; // simulation progress states will be printed (NUMBER_PRINT) times // 20 -> each 5%

    cout << endl;
    cout << endl;
    cout << "###################################" << endl;
    cout << "SALT Simulator with MPDUA : Start !" << endl;
    cout << "###################################" << endl;
    cout << endl;
    cout << endl;

    while (!SC->checkEnd())
    {
        SALTTime currentStep = SC->getCurrentStep();
        SC->doSimulationStep(currentStep);
        SC->printStep(currentStep, NUMBER_PRINT);

        int check_assign = currentStep % UR->routing_config.dua_sec_period;

        if (check_assign == 0 && currentStep > (int)UR->routing_config.time_begin && currentStep < (int)UR->routing_config.time_end)
        {
            cout << endl;
            cout << endl;
            cout << "########## SC->doSimulationStep(" << currentStep << ")" << endl;
            cout << endl;
            cout << endl;

            UR->runDijkstra(SC);
            cout << endl; cout << endl;

            UR->MPduaRouter(currentStep, SC, false); //*.rou.xml write = false
            cout << endl; cout << endl;
        }
    }
    UR->write_file_route_xml_completed(SC);

    UR->time_now();
    cout << endl;
    cout << endl;
    cout << "######################################" << endl;
    cout << "SALT Simulator with MPDUA : Complete !" << endl;
    cout << "######################################" << endl;
    cout << endl;
    cout << endl;


    delete SC;
    SC = nullptr;
    delete UR;
    UR = nullptr;

    return 0;
}
