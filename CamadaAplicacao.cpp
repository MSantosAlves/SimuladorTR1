#include "CamadaAplicacao.hpp"
#include "CamadaFisica.cpp"
#include "CamadaEnlaceDados.cpp"
#include <iostream>
#include <string>
#include <vector>
#include <bitset>

using namespace std;

void AplicacaoTransmissora(){
    string mensagem;
    // Mensagem e tipo de codificacao sao definidos pelo usuario
    cout << "Digite uma mensagem:" << endl;
    getline(cin, mensagem);

    // cout << "Selecione um protocolo de codificacao:" << endl;
    // cout << "0 - Binario" << endl;
    // cout << "1 - Manchester" << endl;
    // cout << "2 - Bipolar" << endl;

    // cin >> 0;

    // Chamada a camada de aplicacao
    CamadaDeAplicacaoTransmissora(mensagem);
}

void CamadaDeAplicacaoTransmissora(string mensagem){
    vector<int> quadro;
    vector<vector<int>> quadros;

    quadros = CamadaEnlanceDadosTransmissora(mensagem);

    for(int i = 0 ; i < quadros.size() ; i++){
        // Chama a camada fisica de transmissao com o quadro de bits
        CamadaFisicaTransmissora(quadros[i]);
    }
}


// Simula o meio de transmissão (teoricamente adicionaria um delay na comunicacao, porem
// a execucao do codigo praticamente nao adiciona complexidade ao tempo de execucao)
void MeioDeTransmissao(vector<int> fluxoBrutoDeBits){
    int erro, porcentagemDeErros;
    vector<int> fluxoBrutoDeBitsPontoA, fluxoBrutoDeBitsPontoB;
    int counter = 0;

    fluxoBrutoDeBitsPontoA = fluxoBrutoDeBits;
    porcentagemDeErros = -2;//2% de chance de erro
    int bitInvertido = 0;
    bool quadroTemErro = false;

    while(fluxoBrutoDeBitsPontoB.size() != fluxoBrutoDeBitsPontoA.size()){
        // Forca 1 erro por quadro
        if(!quadroTemErro && ((rand() % 100) > (100 - porcentagemDeErros))){
            bitInvertido = fluxoBrutoDeBitsPontoA[counter] == 0 ? 1 : 0;
            fluxoBrutoDeBitsPontoB.push_back(bitInvertido);
            quadroTemErro = true;
            cout << "Tem erro" << endl;
        }else{
            fluxoBrutoDeBitsPontoB.push_back(fluxoBrutoDeBitsPontoA[counter]);
            counter++;
        }
    }

    CamadaFisicaReceptora(fluxoBrutoDeBitsPontoB);
}

// Recebe os bits decodificados e os transforma em caracteres
void CamadaDeAplicacaoReceptora(vector<int> quadro){
    for(int i = 0 ; i < quadro.size() ; i++){
        cout << quadro[i];
    }

    cout << endl;

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
    cout << "Quadro decodificado: '" << mensagem << "'" << endl;
}