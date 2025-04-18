#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

// Constrói a matriz de pilhas abertas a partir da matriz A e uma permutação
vector<vector<int>> construirMatrizPilhasAbertas(const vector<vector<int>>& A, const vector<int>& permutacao) {
    int M = A.size();    // Número de peças
    int N = A[0].size(); // Número de padrões

    vector<vector<int>> B(M, vector<int>(N, 0));

    // Pré-calcular prefixos e sufixos para cada peça
    for (int i = 0; i < M; ++i) {
        vector<bool> prefix(N, false);
        vector<bool> suffix(N, false);

        // Construindo prefixo
        for (int j = 0; j < N; ++j) {
            prefix[j] = (j > 0 ? prefix[j - 1] : false) || (A[i][permutacao[j]] == 1);
        }

        // Construindo sufixo (de trás pra frente)
        for (int j = N - 1; j >= 0; --j) {
            suffix[j] = (j < N - 1 ? suffix[j + 1] : false) || (A[i][permutacao[j]] == 1);
        }

        // Preenche B com base em prefixo e sufixo
        for (int j = 0; j < N; ++j) {
            if (prefix[j] && suffix[j])
                B[i][j] = 1;
        }
    }

    return B;
}

// Calcula o número máximo de pilhas abertas na matriz B
int calcularMaxPilhasAbertas(const vector<vector<int>>& B) {
    int maxPilhas = 0;
    int M = B.size(), N = B[0].size();

    for (int estagio = 0; estagio < N; ++estagio) {
        int pilhasAbertas = 0;
        for (int peca = 0; peca < M; ++peca) {
            pilhasAbertas += B[peca][estagio];
        }
        maxPilhas = max(maxPilhas, pilhasAbertas);
    }

    return maxPilhas;
}

// Lê a matriz de incidência a partir do arquivo
vector<vector<int>> lerMatrizDeArquivo(const string& nomeArquivo, int& numPecas, int& numPadroes) {
    ifstream file(nomeArquivo);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo.\n";
        exit(1);
    }

    file >> numPecas >> numPadroes;
    vector<vector<int>> A(numPecas, vector<int>(numPadroes));

    for (int i = 0; i < numPecas; ++i) {
        for (int j = 0; j < numPadroes; ++j) {
            file >> A[i][j];
        }
    }

    file.close();
    return A;
}

int main() {
    int numPecas, numPadroes;
    // string nomeArquivo = "../../Frinhani/Instances/Random-400-400-2-1.txt";
    string nomeArquivo = "teste.txt";

    vector<vector<int>> A = lerMatrizDeArquivo(nomeArquivo, numPecas, numPadroes);

    // Permutação padrão dos padrões
    // vector<int> permutacao(numPadroes);
    // for (int i = numPadroes - 1; i >= 0; --i)
    //     permutacao[i] = i;

    vector<int> permutacao = {2,5,1,4,3,0};

    // Constrói a matriz de pilhas abertas
    vector<vector<int>> B = construirMatrizPilhasAbertas(A, permutacao);

    // Calcula e exibe o número máximo de pilhas abertas
    int maxPilhas = calcularMaxPilhasAbertas(B);
    cout << "Número máximo de pilhas abertas: " << maxPilhas << endl;

    return 0;
}
