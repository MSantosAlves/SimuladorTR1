#include "CamadaAplicacao.hpp"
#include "CamadaFisica.cpp"
#include "CamadaEnlaceDados.cpp"
#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <ctime>

using namespace std;

int PORCENTAGEM_DE_ERRO = 0;
int APLICACAO_CODIFICACAO = 0;
int APLICACAO_ENQUADRAMENTO = 0;
int APLICACAO_CONTROLE_ERROS = 2;

void AplicacaoTransmissora(){
    string mensagem;
    int protocolCod, protocoloEnquadramento, protocoloDeteccaoErros = 0, porcentagemDeErro;

    cout << "Digite a mensagem a ser transmitida:" << endl;
    getline(cin, mensagem);

    cout << "Selecione um protocolo de codificacao:" << endl;
    cout << "0 - Binario" << endl;
    cout << "1 - Manchester" << endl;
    cout << "2 - Bipolar" << endl;
    cin >> APLICACAO_CODIFICACAO;

    cout << endl << "Selecione um protocolo de enquadramento:" << endl;
    cout << "0 - Contagem de caracteres" << endl;
    cout << "1 - Insercao de Bytes" << endl;
    cin >> APLICACAO_ENQUADRAMENTO;

    cout << endl << "Selecione um protocolo de deteccao de erros:" << endl;
    cout << "0 - Bit de paridade par" << endl;
    cout << "1 - CRC" << endl;
    cout << "2 - Codigo de Hamming" << endl;
    cin >> APLICACAO_CONTROLE_ERROS;

    cout << endl << "Define um valor base para a probabilidade de erros de transmissao:" << endl;
    cin >> PORCENTAGEM_DE_ERRO;

    // Chamada a camada de aplicacao
    CamadaDeAplicacaoTransmissora(mensagem);
}

void CamadaDeAplicacaoTransmissora(string mensagem){
    vector<int> quadro;
    vector<vector<int>> quadros;

    quadros = CamadaEnlanceDadosTransmissora(mensagem, APLICACAO_ENQUADRAMENTO, APLICACAO_CONTROLE_ERROS);

    for(int i = 0 ; i < quadros.size() ; i++){
        // Chama os protocolos de controle de erro
        quadro = CamadaEnlaceTransmissoraControleDeErro(quadros[i]);
        
        // Chama a camada fisica de transmissao com o quadro de bits
        CamadaFisicaTransmissora(quadro, APLICACAO_CODIFICACAO);
    }
}


// Simula o meio de transmissão (teoricamente adicionaria um delay na comunicacao, porem
// a execucao do codigo praticamente nao adiciona complexidade ao tempo de execucao)
void MeioDeTransmissao(vector<int> fluxoBrutoDeBits){
    int erro, porcentagemDeErros;
    vector<int> fluxoBrutoDeBitsPontoA, fluxoBrutoDeBitsPontoB;
    int counter = 0;
    srand(time(0) * rand() % 1000); // "Seed" da funcao de numeros aleatorios

    fluxoBrutoDeBitsPontoA = fluxoBrutoDeBits;
    porcentagemDeErros = PORCENTAGEM_DE_ERRO;
    int bitInvertido = 0;
    bool quadroTemErro = false;
    int chanceErro = 0;
    int erroIndice = 0;

    while(fluxoBrutoDeBitsPontoB.size() != fluxoBrutoDeBitsPontoA.size()){
        // Forca 1 erro por quadro
        chanceErro = (rand() % 100);
        if(!quadroTemErro && (chanceErro > (100 - porcentagemDeErros))){
            erroIndice = counter;
            bitInvertido = fluxoBrutoDeBitsPontoA[counter] == 0 ? 1 : 0;
            fluxoBrutoDeBitsPontoB.push_back(bitInvertido);
            quadroTemErro = true;
        }else{
            fluxoBrutoDeBitsPontoB.push_back(fluxoBrutoDeBitsPontoA[counter]);
        }
        counter++;
    }

    if(quadroTemErro){
        cout << "---------------------------------------------------------------------------------------------------------------------------------------------" << endl;
        cout << "[MeioDeTansmissao] => Fluxo de Bits original/com erro:" << endl;
        
        for (int i = 0; i < fluxoBrutoDeBitsPontoA.size(); i++)
        {
            cout << fluxoBrutoDeBitsPontoA[i] << " ";
        }

        cout << endl;

        for (int i = 0; i < fluxoBrutoDeBitsPontoB.size(); i++)
        {
            cout << fluxoBrutoDeBitsPontoB[i] << " ";
        }

        cout << endl;

        for(int i = 0; i < fluxoBrutoDeBitsPontoB.size(); i++){
            if(i == erroIndice){
                cout << "^" << " ";
            }else{
                cout << "  ";
            }
        }

        cout << endl << "---------------------------------------------------------------------------------------------------------------------------------------------" << endl;
    }

    CamadaFisicaReceptora(fluxoBrutoDeBitsPontoB);
}

// Recebe os bits decodificados e os transforma em caracteres
void CamadaDeAplicacaoReceptora(vector<int> quadro){
    string mensagem;
    int tamanhoByte = 8;
    int numeroDeCaracteres = quadro.size() / tamanhoByte;
    vector<int> bits;
    int value = 0;

    // Itera sobre o vetor de bits e separa em grupos de 8 bits (caracteres ASCII)
    for(int i = 0 ; i < numeroDeCaracteres ; i++){

        for(int j = 0 ; j < tamanhoByte ; j++){
            bits.push_back(quadro[i * tamanhoByte + j]);
        }

        value = 0;

        // Transforma o vetor de bits (byte) em um inteiro
        for (size_t k = 0; k < tamanhoByte; k++) {
            value = (value << 1) | bits[k];
        }
        
        // Transforma o inteiro em caracter e o concatena a mensagem
        mensagem += (char)value;

        // Limpa o vetor de bits para a proxima iteracao
        bits.clear();
    }

    AplicacaoReceptora(mensagem);
}

// Retorna a mensagem decodificada
void AplicacaoReceptora(string mensagem){
    cout << "Mensagem recebida (quadro desenquadrado/decodificado): '" << mensagem << "'" << endl;
}