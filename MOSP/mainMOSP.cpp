#include <cstdlib>
#include "../PTAPI-main/include/ExecTime.h"
#include "./MOSP.h"
#include "../PTAPI-main/include/PT.h"

using namespace std;

void saveResults(const string& fn, const solMOSP& sol, int elapsed, int trocas, int readForm) {
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

    string dir = (readForm) ? "resultados" : "resultados-challenge";
    filesystem::create_directories(dir);
    string out_name = dir + "/" + name_no_ext + "_res.txt";

    ofstream ofs(out_name);
    if (!ofs.is_open()) {
        cerr << "Erro ao abrir arquivo de resultados: " << out_name << endl;
        return;
    }


    ofs << dimensao << '\n'
        << elapsed << '\n'
        << sol.evalSol << '\n'
        // << trocas << '\n'
        << sol.maxNumberPiecesPerPatern << '\n';

    for (int i : sol.sol) {
        ofs << i << ' ';
    }
    ofs.close();
}

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
    int read = 0;
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
        else if(arguments[i]== "--READ")
            read = stoi(arguments[i+1]);
    }
	
	// Create MOSP object
	MOSP* prob = new MOSP(fn, read);
	
	// Create and start PT 
	PT<solMOSP> algo(tempIni,tempfim,tempN,MCL,PTL,tempD,uType,tempUp);
	ExecTime et;
    ResultPT<solMOSP> resultado = algo.start(thN, prob);
    solMOSP sol = resultado.best;             // pega a melhor solução
	int elapsed = et.getTimeMs();
    int trocas = resultado.numTrocas;   

	saveResults(fn, sol, elapsed, trocas, read);

    // cout << dimensao << '\n'
    //     << elapsed << '\n'
    //     << sol.evalSol << '\n'
    //     << trocas << '\n';
    // for (int i: sol.sol) {
    //     cout << i << ' ';
    // }

	return 0;
}
