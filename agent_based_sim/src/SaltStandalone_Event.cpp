//
// Created by Hyewon Song on 25. 10. 16.
//

#include <utils/config.h>
#include <utils/etc.h>
#include "Controller/SimulationController.h"

typedef std::chrono::high_resolution_clock Clock;

using namespace std;
using namespace SALT;


int main_template(int argc, char **argv){
    // '''Procedure'''
    // 1. locate input file
    // 2. configure simulation setting (load input)
    // 3. do simulation step

    //////////////////////////////////////////////////////////////////////////////
    // @ Locate input file
    //////////////////////////////////////////////////////////////////////////////
    string scenarioFile="";
    string partitionID="";
    if(argc==1){
        scenarioFile = string("data/GD/config.json");
    }else{
        scenarioFile = string(argv[1]);
    }
    cout << "[SALT Simulator] "<<endl;

#ifdef DEBUG
    cout<<"-----Configuration File : "<<scenarioFile << endl;
#endif
    // ------------------------------------------------------------------------
    // @ Configure simulation controller (load input)
    SimulationController* SC = new SimulationController();
    Result configResult = SC->configureSimulation(scenarioFile, partitionID);
    if(configResult==FAILURE){
        cerr<<"-----Configuration Failure-----"<<endl;
        return -1;
    }

#ifdef RELEASE
    cout<<"-----Configuration Done-----"<<endl;
#endif

    SC->printSimulationSetting();

#ifdef RENDER
    // @Road Network Rendering
	auto nodeList = SC->getNodeList();
	for(auto node: nodeList){
		cout<<"Node "<<node->getID()<<" shape: "<<node->getShape()<<endl;
	}

	auto linkList = SC->getLinkList();
	for(auto link: linkList){
		cout<<"Link "<<link->getID()<<" "<<"#section: "<<link->getNumSection()<<" #lane: "<<link->getNumLane()<<" shape: "<< link->getShape()<<endl;
	}

	auto cellList = SC->getCellList();
	for(auto cell: cellList){
		cout<<"Cell "<<cell->getMyLink()->getID()<<"-"<<cell->getSection()<<"-"<<cell->getLane()<<": "<<cell->getMyCurrentVolume()<<endl;
	}
	// @Traffic Signal Rendering
	auto trafficSignalList = SC->getTrafficSignalList();
	for(auto ts : trafficSignalList){
		Node* signalizedNode = ts->getMyNode();
		// FIXME - something wrong...
		string currentSignalState = signalizedNode->getCurrentTrafficSignalState(); //
		// iterate over all the connections on the node
		cout << signalizedNode->getID() << endl;
		auto conList = signalizedNode->getOrderedConnectionList();
		for(unsigned order=0;order<conList.size();order++){
			Connection* targetConnection =conList[order];
			// get connection's info and traffic signal on the connection
			Link* fromLink = targetConnection->getFromLink(); // where to render
			int fromLane = targetConnection->getFromLane(); // where to render
			Vector2D direction = targetConnection->getDirection(); // what to render (vector img)
			std::string rotationDir = targetConnection->getRotationDir(); // "L", "S", "R"
			char& currentSignal = currentSignalState[order]; // one of 'G', 'g', 'r','y'

			// render a colored(signal) arrow(direction vector) at location(fromLink,fromLane)
			cout << "Render a colored("<<string(1,currentSignal)<<") turn info("<<rotationDir<<")\t at ("<<fromLane<<")th lane of Link"<<fromLink->getID() << endl;
		}
		cout << "done\n";;

	}
#endif


    //////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
    cout<<"-----Simulation Loop started-----"<<endl;
#endif
    auto timeStart = Clock::now();

    //cout<<"(node,link,cell,ts, active cell):"<< SC->getNodeList().size()<<" "<<SC->getLinkList().size()<<" "<<SC->getCellList().size()<<" "<<SC->getTrafficSignalList().size()<<" "<<SC->getActiveCellSet().size()<<endl;
    // repeat to call both  SimulationController->doSimulationStep(currentStep)
    std::cout << "[Simulation Start]" << std::endl;

    // SC->assertReady();
    int NUMBER_PRINT = 20;
    while(!SC->checkEnd()) {
        auto timeStepStart = Clock::now();
        SALTTime currentStep = SC->getCurrentStep();
        // update status of vehicles, traffic signal and event
        SC->doSimulationStep(currentStep);

#ifdef DEBUG
        cout <<"(Step "<<currentStep << ")";
#endif

#ifdef RENDER
        // @ how to access only the activated cells at the current step.
		const auto activeCellSet = SC->getActiveCellSet();

		for(auto activeCell: activeCellSet){
			string cellID= activeCell->getID();
			int currentVehNum = activeCell->getMyCurrentVolume();
			float density = (5.0f*currentVehNum)/activeCell->getMyLength();
			cout<<"Cell id: "<<cellID<<" current #(veh): " << currentVehNum<< " density: "<<density<< endl;
		}

#endif

        //		VehicleManager::getInstance()->printStatus(MICRO);

        SC->printStep(currentStep,NUMBER_PRINT);

        auto timeStepEnd = Clock::now();
        auto elapsedTimePerStep = chrono::duration_cast<std::chrono::milliseconds>(timeStepEnd - timeStepStart).count();
        SC->writeProgress(currentStep, std::to_string(elapsedTimePerStep));

#ifdef DUMP_TEST
        if(currentStep==100){
			SC->doDump(currentStep); // dump api usage
		}
#endif
    }


    auto timeEnd = Clock::now();
    cout<<"[Simulation End]"<<endl;
    cout << "Elapsed Time: "
         << chrono::duration_cast<std::chrono::seconds>(timeEnd - timeStart).count()
         << " seconds" << std::endl;


    delete SC;
    SC = nullptr;

    return 0;
}


int main(int argc, char **argv){
    // '''Procedure'''
    // 1. locate input file
    // 2. configure simulation setting (load input)
    // 3. do simulation step

    // ------------------------------------------------------------------------
    // @ Locate input file
    string scenarioFile="";
    int option=0;
    if(argc>=3){
        scenarioFile = string(argv[1]);
        option = stoi(argv[2]);
    }else{
        cerr << "Specify scenario file" << endl;
        return -1;
    }

    cout << "[SALT Simulator] "<<endl;

    // ------------------------------------------------------------------------
    // @ Configure simulation controller (load input)
    cout<<"-----Configuration File : "<<scenarioFile << endl;
    cout<<"-----Event Option : "<<option << endl;
    SimulationController* SC = new SimulationController();
    Result configResult = SC->configureSimulation(scenarioFile, "", option);
    if(configResult==FAILURE){
        cerr<<"-----Configuration Failure-----"<<endl;
        return -1;
    }else{
        cout<<"-----Configuration Done-----"<<endl;
    }

    SC->printSimulationSetting();

    // ------------------------------------------------------------------------
    // @ do simulation loop
    std::cout << "[Simulation Start]" << std::endl;
    auto timeStart = Clock::now();
    int NUMBER_PRINT = 20; // simulation progress states will be printed (NUMBER_PRINT) times // 20 -> each 5%

    // repeat to call SimulationController->doSimulationStep(currentStep)
    while(!SC->checkEnd()) {
        // get current step + update status of vehicles, traffic signal and event
        SALTTime currentStep = SC->getCurrentStep();
        SC->doSimulationStep(currentStep); // doSimulationStep will increase SC::currentStep

        // (optional) print
        SC->printStep(currentStep,NUMBER_PRINT);

//        // (optional) write trajectory log
//        if(SC->checkRecord()){
//            SC->writeTrajectory();
//        }
    }

//    SC->printRouteInfo();

    auto timeEnd = Clock::now();
    auto totalTime = chrono::duration_cast<std::chrono::seconds>(timeEnd - timeStart).count();
    cout<<"[Simulation End]"<<endl;
    cout << "Elapsed Time: "<< totalTime << " seconds" << std::endl;

    delete SC;
    SC = nullptr;

    return 0;
}


