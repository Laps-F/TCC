#include <cstdlib>
#include "../PTAPI-main/include/ExecTime.h"
#include "./MOSP.h"
#include "../PTAPI-main/include/PT.h"

using namespace std;

void saveResults(const string& fn, const solMOSP& sol, int elapsed, int trocas, int readForm, int instance) {
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
    string out_name = dir + "/" + name_no_ext + "(" + std::to_string(instance) + ")_res.txt";

    ofstream ofs(out_name);
    if (!ofs.is_open()) {
        cerr << "Erro ao abrir arquivo de resultados: " << out_name << endl;
        return;
    }


    ofs << dimensao << '\n'
        << elapsed << '\n'
        << sol.evalSol << '\n'
        << trocas << '\n'
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
	float tempfim = 10;
	int tempN = 5;
	int MCL = 600;
	int PTL = 2400;	
	int tempUp = 960;
	int tempD = 2;
	int uType = 3;
    int mType = 2;
    int read = 1;
	// int thN = thread::hardware_concurrency();	
	int thN = 5;
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
        else if(arguments[i]== "--MOV_TYPE")
            mType = stoi(arguments[i+1]);
        else if(arguments[i]== "--TYPE_UPDATE")
            uType = stoi(arguments[i+1]);
        else if(arguments[i]== "--TEMP_UPDATE"){
            int proportion = stoi(arguments[i+1]);
            tempUp = PTL / proportion;
        }
        else if(arguments[i]== "--THREAD_USED")
            thN = stoi(arguments[i+1]);
        else if(arguments[i]== "--READ")
            read = stoi(arguments[i+1]);
    }
	
    for(int i=0; i<5; i++) {
        // Create MOSP object

        MOSP* prob = new MOSP(fn, read, mType);
        
        // Create and start PT 
        PT<solMOSP> algo(tempIni, tempfim, tempN, MCL, PTL, 0, tempD, uType, tempUp);
        // descobrir como chama o construction do pt e sobrescrever 2 replicas com o MCNH primeira e do meio
        ExecTime et;
        ResultPT<solMOSP> resultado = algo.start(thN, prob);
        solMOSP sol = resultado.best;             // pega a melhor solução
        int elapsed = et.getTimeMs();
        int trocas = resultado.numTrocas;   
        int construcTime = sol.construcTime;

        saveResults(fn, sol, elapsed + construcTime, sol.ptl, read, i);

        // cout<<construcTime<<endl;
        // cout<<"Trocas: "<<sol.ptl<<endl; 

        // cout<<sol.evalSol<<endl;
    }

	return 0;
}

// ./mainMOSP "../Frinhani/Instances/Random-400-400-4-6.txt" --TEMP_INIT 1 --TEMP_FIM 3 --MCL 500 --TEMP_DIST 4 --MOV_TYPE 1 --TYPE_UPDATE 2 --TEMP_UPDATE 5
