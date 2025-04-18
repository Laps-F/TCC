#include <cstdlib>
#include "../PTAPI-main/include/ExecTime.h"
#include "./MOSP.h"
#include "../PTAPI-main/include/PT.h"


using namespace std;

int main(int argc, char* argv[])
{
	//varibles
	float tempIni = 0.01;
	float tempfim = 2.0;
	int tempN = 10;
	int MCL = 0;
	int PTL = 2;	
	int tempUp = 50;
	int tempD = 1;
	int uType = 0;
	// int thN = thread::hardware_concurrency();	
	int thN = 2;
	vector<string> arguments(argv + 1, argv + argc);	
	
	// Instance file name
	string fn = arguments[0];
	
	// Read arguments
	for(unsigned int i=1; i<arguments.size(); i+=2)
	{
		            
        if(arguments[i]== "--TEMP_FIM")
            tempfim = stof(arguments[i+1]);
        else if(arguments[i]== "--TEMP_INIT")
            tempIni = stof(arguments[i+1]);
        else if(arguments[i]== "--N_REPLICAS")
            tempN = stoi(arguments[i+1]);
        else if(arguments[i]== "--MCL")
            MCL  = stoi(arguments[i+1]);
        else if(arguments[i]== "--PTL")
            PTL = stoi(arguments[i+1]);
        else if(arguments[i]== "--TEMP_DIST")
            tempD = stoi(arguments[i+1]);
        else if(arguments[i]== "--TYPE_UPDATE")
            uType = stoi(arguments[i+1]);
        else if(arguments[i]== "--TEMP_UPDATE")
            tempUp = stoi(arguments[i+1]);
        else if(arguments[i]== "--THREAD_USED")
            thN = stoi(arguments[i+1]);
    }
	
	// Create MOSP object
	MOSP* prob = new MOSP(fn);
	
	// Create and start PT 
	PT<solMOSP> algo(tempIni,tempfim,tempN,MCL,PTL,tempD,uType,tempUp);
	ExecTime et;
	solMOSP sol = algo.start(thN, prob);
	int elapsed = et.getTimeMs();

	string basename = fn.substr(fn.find_last_of("/\\") + 1);
    string name_no_ext = basename.substr(0, basename.find_last_of('.'));
    vector<string> parts;
    stringstream ss(name_no_ext);
    string token;
    while (getline(ss, token, '-')) parts.push_back(token);

    string dimensao;
    if (parts.size() >= 3) {
        dimensao = parts[1] + " " + parts[2];
    } else {
        dimensao = "?";
    }

    // Garante existência da pasta "resultados"
    filesystem::create_directories("resultados");

    // Prepara nome do arquivo de saída
    string out_name = "resultados/" + name_no_ext + "_res.txt";
    ofstream ofs(out_name);
    if (!ofs.is_open()) {
        cerr << "Erro ao abrir arquivo de resultados: " << out_name << endl;
        return 1;
    }

    // Grava resultados: Dimensão, Tempo, Valor da solução, Solução (índices)
    ofs << dimensao << '\n'
        << elapsed << '\n'
        << sol.evalSol << '\n';
    for (int i: sol.sol) {
        ofs << i << ' ';
    }
    ofs.close();

	return 0;
}

// g++ mainMOSP.cpp ../include/*.h ./MOSP.cpp -std=c++20 -Wshadow -Wall -o mainMOSP -O3 -Wno-unused-result -lpthread -march=native
// ./mainMOSP ../Frinhani/Instances/Random-1000-1000-2-1.txt --TEMP_INIT 0.1 --TEMP_FIM 1 --N_REPLICAS 2 --MCL 50 --PTL 60 --TEMP_DIST 3 --TYPE_UPDATE 1 --TEMP_UPDATE 20
