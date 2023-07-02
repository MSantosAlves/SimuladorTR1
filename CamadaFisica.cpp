#include "CamadaFisica.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <bitset>

using namespace std;

int TIPO_DE_CODIFICACAO = 0;

void AplicacaoTransmissora(){
    string mensagem;
    // Mensagem e tipo de codificacao sao definidos pelo usuario
    cout << "Digite uma mensagem:" << endl;
    getline(cin, mensagem);

    cout << "Selecione um protocolo de codificacao:" << endl;
    cout << "0 - Binario" << endl;
    cout << "1 - Manchester" << endl;
    cout << "2 - Bipolar" << endl;

    cin >> TIPO_DE_CODIFICACAO;

    // Chamada a camada de aplicacao
    CamadaDeAplicacaoTransmissora(mensagem);
}

void CamadaDeAplicacaoTransmissora(string mensagem){
    vector<int> quadro;
    
    // Transforma a mensagem em um vetor de bits
    for (char c : mensagem) {
        // Para cada caracter, transforma seu valor ASCII em um vetor de bits
        bitset<8> bits(c);
        for (int i = 7; i >= 0; --i) {
            // Adiciona os bits do caracter ao quadro
            quadro.push_back(bits[i]);
        }
    }

    // Imprime o quadro de bits
    cout << "Quadro de bits: ";

    for(int i = 0 ; i < quadro.size() ; i++){
        cout << quadro[i];
    }

    cout << endl;
    
    // Chama a camada fisica de transmissao com o quadro de bits
    CamadaFisicaTransmissora(quadro);
}

// Transmite os bits do quadro de acordo com o protocolo escolhido
void CamadaFisicaTransmissora(vector<int> quadro){
    int tipoDeCodificacao = TIPO_DE_CODIFICACAO;
    vector<int> fluxoBrutoDebits;

    switch(tipoDeCodificacao){
        case 0:
            fluxoBrutoDebits = CamadaFisicaTransmissoraCodBinaria(quadro);
            break;
        case 1:
            fluxoBrutoDebits = CamadaFisicaTransmissoraCodManchester(quadro);
            break;
        case 2:
            fluxoBrutoDebits = CamadaFisicaTransmissoraCodBipolar(quadro);
            break;
    }

    // Imprime o quadro de bits codificado
    cout << "Quadro de bits codificado: ";

    for(int i = 0 ; i < fluxoBrutoDebits.size() ; i++){
        if(TIPO_DE_CODIFICACAO < 2){
            cout << fluxoBrutoDebits[i];
        }else{
            if(fluxoBrutoDebits[i] == 1){
                cout << "+";
            }else if(fluxoBrutoDebits[i] == -1){
                cout << "-";
            }else{
                cout << "0";
            }
        }
    }

    cout << endl;

    // Chama o meio de transmissao com o fluxo bruto de bits
    MeioDeTransmissao(fluxoBrutoDebits);
}

// Codificacao Binaria - quadro ja esta no formato esperado pelo meio de transmissao
vector<int> CamadaFisicaTransmissoraCodBinaria(vector<int> quadro){
    return quadro;
}

// Codificacao Manchester
vector<int> CamadaFisicaTransmissoraCodManchester(vector<int> quadro){
    int TAMANHO_QUADRO = quadro.size();
    vector<int> manchesterCodificado;

    // Equivalente a um XOR entre cada bit do quadro e o clock
    for (int i = 0; i < TAMANHO_QUADRO; i++) {
        if (quadro[i] == 0) {
            manchesterCodificado.push_back(0);
            manchesterCodificado.push_back(1);
        } else {
            manchesterCodificado.push_back(1);
            manchesterCodificado.push_back(0);
        }
    }

    return manchesterCodificado;
}

// Codificacao Bipolar
vector<int> CamadaFisicaTransmissoraCodBipolar(vector<int> quadro){
    // Armazena o nivel logico do ultimo bit 1
    bool ultimoNivelPositivo = false;
    int TAMANHO_QUADRO = quadro.size();
    vector<int> bipolarCodificado;

    // Alterna o nivel do sinal para bits 1 e mantem o bit 0 como 0
    for (int i = 0; i < TAMANHO_QUADRO; i++) {
        if (quadro[i] == 1) {
            if (ultimoNivelPositivo) {
                bipolarCodificado.push_back(-1);
                ultimoNivelPositivo = false;
            } else {
                bipolarCodificado.push_back(1);
                ultimoNivelPositivo = true;
            }
        }else{
            bipolarCodificado.push_back(0);
        }
    }

    return bipolarCodificado;
}

// Simula o meio de transmissão (teoricamente adicionaria um delay na comunicacao, porem
// a execucao do codigo praticamente nao adiciona complexidade ao tempo de execucao)
void MeioDeTransmissao(vector<int> fluxoBrutoDeBits){
    vector<int> fluxoBrutoDeBitsPontoA, fluxoBrutoDeBitsPontoB;
    int counter = 0;

    fluxoBrutoDeBitsPontoA = fluxoBrutoDeBits;

    while(fluxoBrutoDeBitsPontoB.size() != fluxoBrutoDeBitsPontoA.size()){
        fluxoBrutoDeBitsPontoB.push_back(fluxoBrutoDeBitsPontoA[counter]);
        counter++;
    }

    // Cahama a camada receptora com os bits codificados
    CamadaFisicaReceptora(fluxoBrutoDeBitsPontoB);
}

// Decodifica os bits do quadro de acordo com o protocolo escolhido
void CamadaFisicaReceptora(vector<int> quadro){
    int tipoDeCodificacao = TIPO_DE_CODIFICACAO;
    vector<int> fluxoBrutoDebits;

    switch(tipoDeCodificacao){
        case 0:
            fluxoBrutoDebits = CamadaFisicaReceptoraDecodBinaria(quadro);
            break;
        case 1:
            fluxoBrutoDebits = CamadaFisicaReceptoraDecodManchester(quadro);
            break;
        case 2:
            fluxoBrutoDebits = CamadaFisicaReceptoraDecodBipolar(quadro);
            break;
    }

     // Imprime o quadro de bits codificado
    cout << "Quadro de bits decodificado: ";

    for(int i = 0 ; i < fluxoBrutoDebits.size() ; i++){
        cout << fluxoBrutoDebits[i];
    }

    cout << endl;

    // Chama a camada de aplicacao receptora com o fluxo bruto de bits decodificados
    CamadaDeAplicacaoReceptora(fluxoBrutoDebits);
}

// Decofica os bits do quadro de acordo com a codificacao binaria
vector<int> CamadaFisicaReceptoraDecodBinaria(vector<int> quadro){
    return quadro;
}

// Decoficacao Manchester
vector<int> CamadaFisicaReceptoraDecodManchester(vector<int> quadro){
    vector<int> manchesterDecodificado;
    
    for(int i = 0 ; i < quadro.size() ; i +=2){
        manchesterDecodificado.push_back(quadro[i]);
    }

    return manchesterDecodificado;
}

// Decodificacao Bipolar
vector<int> CamadaFisicaReceptoraDecodBipolar(vector<int> quadro){
    int TAMANHO_QUADRO = quadro.size();
    vector<int> bipolarDecodificado;
    int tmp;
    
    // Transforma os niveis positivos/negativos em 1's e mantem os 0's
    for(int i = 0 ; i < TAMANHO_QUADRO ; i ++){
        tmp = quadro[i] == 0 ? 0 : 1;
        bipolarDecodificado.push_back(tmp);
    }

    return bipolarDecodificado;
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
    cout << endl << "A mensagem recebida foi: " << mensagem << endl;
}
