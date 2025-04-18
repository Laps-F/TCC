#include "MOSP.h"

MOSP::MOSP(std::string filename){
	
	fn = filename;
    std::ifstream file(filename);
	std::string line; 
	
    if(!file.is_open()){
        std::cout << "Could not open file! \n";
        return;
    }	

    if (filename.find("/mosp_instances/") != std::string::npos) {
        std::string ignoredLine;
        getline(file, ignoredLine);
    }
    file >> numberPatterns >> numberPieces;

    patternPieces.resize(numberPatterns);
    stackSizeEvaluation.resize(numberPieces, 0);

    for (int j = 0; j < numberPieces; j++) {
        for (int i = 0; i < numberPatterns; i++) {
            int value;
            file >> value;
            if (value == 1) {
                patternPieces[i].push_back(j); // armazena o índice da peça (j) associada ao padrão (i)
                stackSizeEvaluation[j]++; //armezan a quantidade de vezes que cada peça aparece nos padrões
            }
        }
    }

    file.close();
}

MOSP::~MOSP(){	
}

solMOSP MOSP::construction(){
	solMOSP ss;
	std::random_device rnd_device;
	std::mt19937 mersenne_engine {rnd_device()};

	for (int i = 0; i < numberPatterns; i++) {
		ss.sol.push_back(i);
	}

	std::shuffle(begin(ss.sol), end(ss.sol), mersenne_engine);

	ss.evalSol = evaluate(ss);
	ss.Nup = false;
	ss.Ndown = false;

	return ss;
}

// 2-opt neighbor moviment
// solMOSP MOSP::neighbor(solMOSP sol){
// 	solMOSP s;
//     s.sol = sol.sol; 

//     std::random_device rnd_device;
//     std::mt19937 mersenne_engine {rnd_device()};
//     std::uniform_int_distribution<int> distPattern(0, numberPatterns - 1);

//     int pattern1 = 0, pattern2 = 0;
    
//     do {
//         pattern1 = distPattern(mersenne_engine);
//         pattern2 = distPattern(mersenne_engine);
//     } while (pattern1 == pattern2); 
    
//     if(pattern1 > pattern2) std::swap(pattern1, pattern2);

//     while(pattern1 < pattern2) {
//         std::swap(s.sol[pattern1], s.sol[pattern2]);
//         pattern1++;
//         pattern2--;
//     }

//     s.Nup = sol.Nup;
//     s.Ndown = sol.Ndown;

//     return s;
// }

// swap neighbor moviment
solMOSP MOSP::neighbor(solMOSP sol) {
    solMOSP s;
    s.sol = sol.sol;

    std::random_device rnd_device;
    std::mt19937 mersenne_engine {rnd_device()};
    std::uniform_int_distribution<int> distPattern(0, numberPatterns - 1);

    int pattern1 = 0, pattern2 = 0;

    do {
        pattern1 = distPattern(mersenne_engine);
        pattern2 = distPattern(mersenne_engine);
    } while (pattern1 == pattern2);

    std::swap(s.sol[pattern1], s.sol[pattern2]);

    s.Nup = sol.Nup;
    s.Ndown = sol.Ndown;

    return s;
}

double MOSP::evaluate(solMOSP sol) {
    const int OPEN = 1, CLOSED = 0;

    std::vector<int> stack(numberPieces, CLOSED);

    int openStacks = 0;
    int closedStacks = 0;
    int maxOpenStacks = -1;

    for (int i = 0; i < numberPatterns; i++) {
        int index = sol.sol[i]; // padrão atual da solução

        for (int peca : patternPieces[index]) {
            stackSizeEvaluation[peca]--; // uma unidade da peça foi produzida

            if (stack[peca] == CLOSED) {
                openStacks++;           // nova pilha aberta
                stack[peca] = OPEN;
            }

            if (stackSizeEvaluation[peca] == 0) {
                closedStacks++;         // pilha fechada
                stack[peca] = CLOSED;
            }
        }

        if (openStacks > maxOpenStacks)
            maxOpenStacks = openStacks;

        openStacks -= closedStacks;
        closedStacks = 0;
    }

    return maxOpenStacks;
}