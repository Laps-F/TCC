//rodar pro primeiro conjunto a nova versao com 2 soluções mcnh e 3 aleatoria e mais ptl
//pra tentar voltar a ter 7% de gap
//3x seq peças e 3x seq padroes

#include "MOSP.h"
#include "MCNH/HNCM.h"

MOSP::MOSP(std::string filename, int readForm, int neighborStrategy){
    neighborStrt = neighborStrategy;
	fn = filename;
    std::ifstream file(filename);
	std::string line; 
	
    if(!file.is_open()){
        std::cout << "Could not open file! \n";
        return;
    }	

    if (filename.find("/Challenge/") != std::string::npos) {
        std::string ignoredLine;
        getline(file, ignoredLine);
    }

    file >> numberPatterns >> numberPieces;

    patternPieces.resize(numberPatterns);
    stackSizeEvaluation.resize(numberPieces, 0);
    maxNumberPiecesPerPatern = 0;
    constructSolutionTime = 0;
    
    if(readForm){ //leitura arquivos do rafael
        for (int j = 0; j < numberPatterns; j++) {
            for (int i = 0; i < numberPieces; i++) {
                int value;
                file >> value;
                if (value == 1) {
                    patternPieces[j].push_back(i); // armazena o índice da peça (j) associada ao padrão (i)
                    stackSizeEvaluation[i]++; //armazenam a quantidade de vezes que cada peça aparece nos padrões
                }
            }
        }
    } else { // leitura dos outros arquivos
        for (int i = 0; i < numberPieces; i++) {
            for (int j = 0; j < numberPatterns; j++) {
                int value;
                file >> value;
                if (value == 1) {
                    patternPieces[j].push_back(i); // armazena o índice da peça (j) associada ao padrão (i)
                    stackSizeEvaluation[i]++; //armazenam a quantidade de vezes que cada peça aparece nos padrões
                }
            }
        }
    }

    for (int j = 0; j < numberPatterns; j++)
        if(patternPieces[j].size() > maxNumberPiecesPerPatern){
            maxNumberPiecesPerPatern = patternPieces[j].size();
        }

    patternBitsets.resize(numberPatterns);

    // Pré-processa os padrões para bitsets
    for (int i = 0; i < numberPatterns; i++) {
        for (int p : patternPieces[i]) {
            patternBitsets[i].set(p);
        }
    }

    patternsPorPeca.resize(numberPieces);
    for (int pattern = 0; pattern < numberPatterns; ++pattern) {
        for (int piece = 0; piece < numberPieces; ++piece) {
            if (patternBitsets[pattern].test(piece)) {
                patternsPorPeca[piece].push_back(pattern);
            }
        }
    }

    file.close();
}

MOSP::~MOSP(){	
}

// solMOSP MOSP::construction(){
// 	solMOSP ss;
// 	std::random_device rnd_device;
// 	std::mt19937 mersenne_engine {rnd_device()};

// 	for (int i = 0; i < numberPatterns; i++) {
// 		ss.sol.push_back(i);
// 	}

// 	std::shuffle(begin(ss.sol), end(ss.sol), mersenne_engine);

// 	ss.evalSol = evaluate(ss);
//     ss.maxNumberPiecesPerPatern = maxNumberPiecesPerPatern;
// 	ss.Nup = false;
// 	ss.Ndown = false;

// 	return ss;
// }

// solMOSP MOSP::construction() {

//     solMOSP ss;
//     std::random_device rnd_device;
//     std::mt19937 mersenne_engine {rnd_device()};

    
// 	for (int i = 0; i < numberPieces; i++) {
// 		ss.sol.push_back(i);
// 	}

//     std::shuffle(begin(ss.sol), end(ss.sol), mersenne_engine);

//     ss.evalSol = evaluate(ss);
//     ss.maxNumberPiecesPerPatern = maxNumberPiecesPerPatern;
// 	ss.Nup = false;
// 	ss.Ndown = false;

// 	return ss;
// }

solMOSP MOSP::construction() {
    int idx = constructionCallIndex.fetch_add(1);

    solMOSP ss;

    if(idx == 0 || idx == 2){
         int* solution = nullptr;
        int resultHNCM, solutionSize = 0;	
        double timeHNCM;

        char* filename = new char[fn.size() + 1];
        strcpy(filename, fn.c_str());

        // mainMethod(&resultHNCM, &timeHNCM, filename, &solution, &solutionSize, 0); //padroes
        mainMethod(&resultHNCM, &timeHNCM, filename, &solution, &solutionSize, 1); //peças
        constructSolutionTime += timeHNCM;

        for(int i = 0; i < numberPieces; i++) {
            ss.sol.push_back(solution[i]);
        }

        delete[] solution;
    } else {
        std::random_device rnd_device;
        std::mt19937 mersenne_engine {rnd_device()};

        for (int i = 0; i < numberPieces; i++) {
            ss.sol.push_back(i);
        }

        std::shuffle(begin(ss.sol), end(ss.sol), mersenne_engine);
    }

    // for (int i = 0; i < numberPatterns; i++) {
    //     std::cout<<ss.sol[i]<< " - ";
    // }
    // std::cout<<"terminou"<<std::endl;

    ss.evalSol = evaluate(ss);
    ss.maxNumberPiecesPerPatern = maxNumberPiecesPerPatern;
	ss.Nup = false;
	ss.Ndown = false;

	return ss;
}

solMOSP MOSP::neighbor(solMOSP sol){
    switch (neighborStrt) {
        case 0:
            return neighborSwap(sol);
        case 1:
            return neighbor2Opt(sol);
        case 2:
            return neighborInsertion(sol);
        default:
            return sol;  // Retorna a mesma solução se tipo inválido
    }
}

// 2-opt neighbor moviment
solMOSP MOSP::neighbor2Opt(solMOSP sol){
	solMOSP s;
    s.sol = sol.sol; 
    s.maxNumberPiecesPerPatern = sol.maxNumberPiecesPerPatern;

    std::random_device rnd_device;
    std::mt19937 mersenne_engine {rnd_device()};
    std::uniform_int_distribution<int> distPattern(0, numberPatterns - 1);

    int pattern1 = 0, pattern2 = 0;
    
    do {
        pattern1 = distPattern(mersenne_engine);
        pattern2 = distPattern(mersenne_engine);
    } while (pattern1 == pattern2); 
    
    if(pattern1 > pattern2) std::swap(pattern1, pattern2);

    while(pattern1 < pattern2) {
        std::swap(s.sol[pattern1], s.sol[pattern2]);
        pattern1++;
        pattern2--;
    }

    s.Nup = sol.Nup;
    s.Ndown = sol.Ndown;

    return s;
}

// swap neighbor moviment
solMOSP MOSP::neighborSwap(solMOSP sol) {
    solMOSP s;
    s.sol = sol.sol;
    s.maxNumberPiecesPerPatern = sol.maxNumberPiecesPerPatern;

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

// random insertion neighbor movement
solMOSP MOSP::neighborInsertion(solMOSP sol) {
    solMOSP s;
    s.sol = sol.sol;
    s.maxNumberPiecesPerPatern = sol.maxNumberPiecesPerPatern;

    std::random_device rnd_device;
    std::mt19937 mersenne_engine {rnd_device()};
    std::uniform_int_distribution<int> distPattern(0, numberPatterns - 1);

    int patternFrom = 0, patternTo = 0;

    do {
        patternFrom = distPattern(mersenne_engine);
        patternTo = distPattern(mersenne_engine);
    } while (patternFrom == patternTo);

    int element = s.sol[patternFrom];
    s.sol.erase(s.sol.begin() + patternFrom);

    if (patternTo > patternFrom) {
        patternTo--;  // ajuste porque o vetor ficou menor após remoção
    }

    s.sol.insert(s.sol.begin() + patternTo, element);

    s.Nup = sol.Nup;
    s.Ndown = sol.Ndown;

    return s;
}

double MOSP::evaluate(solMOSP& sol) {
    std::bitset<1000> piecesRead;
    std::bitset<1000>  patternUsed;
    std::vector<int> patternSequence;
    sol.construcTime = constructSolutionTime;

    const int OPEN = 1, CLOSED = 0;

    std::vector<int> stack(numberPieces, CLOSED);

    std::vector<int> vet(numberPieces);
    vet = stackSizeEvaluation;

    int openStacks = 0;
    int closedStacks = 0;
    int maxOpenStacks = -1;
    // int freqMaxOpenStacks = 0;

    // usar um set de padrões, ou seja, um conjunto de padroes que va retirando padroes conforme eles sao sequenciados
    // for (int piece : sol.sol) {
    //     piecesRead.set(piece);

    //     for (int pattern = 0; pattern < numberPatterns; pattern++) {
    //         if (patternUsed[pattern]) continue;

    //         // if ((patternBitsets[pattern] & piecesRead) == patternBitsets[pattern]) {
    //         if ((patternBitsets[pattern] & ~piecesRead).none()) {
    //             patternSequence.push_back(pattern);
    //             patternUsed[pattern] = true;
    //         }
    //     }
    // }

    // std::vector<int> disponiveis(numberPatterns);
    // std::iota(disponiveis.begin(), disponiveis.end(), 0);

    // for(int piece : sol.sol) {
    //     piecesRead.set(piece);
    //     disponiveis.erase(
    //         std::remove_if(disponiveis.begin(), disponiveis.end(),
    //             [&](int pattern){
    //                 if ((patternBitsets[pattern] & ~piecesRead).none()) {
    //                     patternSequence.push_back(pattern);
    //                     patternUsed[pattern] = true;
    //                     return true;  // remove do vetor
    //                 }
    //                 return false;
    //             }),
    //         disponiveis.end());
    // }

    // std::list<int> disponiveis(numberPatterns);
    // std::iota(disponiveis.begin(), disponiveis.end(), 0);
    // for(int piece : sol.sol) {
    //     piecesRead.set(piece);
    //     disponiveis.remove_if([&](int pattern) {
    //         if ((patternBitsets[pattern] & ~piecesRead).none()) {
    //             patternSequence.push_back(pattern);
    //             patternUsed[pattern] = true;
    //             return true;  // remove da lista
    //         }
    //         return false;
    //     });
    // }


    // std::vector<int> ativos;
    // ativos.reserve(numberPatterns);
    // for (int i = 0; i < numberPatterns; ++i) ativos.push_back(i);

    // for (int piece : sol.sol) {
    //     piecesRead.set(piece);

    //     // Refiltra os ativos de forma eficiente
    //     std::vector<int> novosAtivos;
    //     for (int pattern : ativos) {
    //         if ((patternBitsets[pattern] & ~piecesRead).none()) {
    //             patternSequence.push_back(pattern);
    //             patternUsed[pattern] = true;
    //         } else {
    //             novosAtivos.push_back(pattern); // mantém ativo
    //         }
    //     }

    //     ativos.swap(novosAtivos); // atualiza
    // }

    // std::bitset<1000> patternUsed;

    // for (int piece : sol.sol) {
    //     piecesRead.set(piece);

    //     for (int pattern = 0; pattern < numberPatterns; ++pattern) {
    //         if (patternUsed.test(pattern)) continue;

    //         if ((patternBitsets[pattern] & ~piecesRead).none()) {
    //             patternSequence.push_back(pattern);
    //             patternUsed.set(pattern);
    //         }
    //     }
    // }

    for (int piece : sol.sol) {
        piecesRead.set(piece);
        const auto& candidatos = patternsPorPeca[piece];
        auto notRead = ~piecesRead;

        for (int pattern : candidatos) {
            if (!patternUsed.test(pattern) &&
                (patternBitsets[pattern] & notRead).none()) {
                patternSequence.push_back(pattern);
                patternUsed.set(pattern);
            }
        }
    }

    // std::cout << "patternSequence.size(): " << patternSequence.size() << std::endl;

    for (int i = 0; i < numberPatterns; i++) {
        // int index = sol.sol[i]; // padrão atual da solução
        int index = patternSequence[i]; // padrão atual da solução


        for (int peca : patternPieces[index]) {
            vet[peca]--; // uma unidade da peça foi produzida

            if (stack[peca] == CLOSED) {
                openStacks++;           // nova pilha aberta
                stack[peca] = OPEN;
            }

            if (vet[peca] == 0) {
                closedStacks++;         // pilha fechada
                stack[peca] = CLOSED;
            }
        }

        // if (openStacks > maxOpenStacks) {
        //     maxOpenStacks = openStacks;
        //     freqMaxOpenStacks = 1;
        // } else if(openStacks == maxOpenStacks){
        //     freqMaxOpenStacks++;
        // }

        if (openStacks > maxOpenStacks) {
            maxOpenStacks = openStacks;
        }

        openStacks -= closedStacks;
        closedStacks = 0;
    }

    //soma em decimal o valor final de todos os openstacks
    return maxOpenStacks;
}

//valor*1000/1000
//valor*800
