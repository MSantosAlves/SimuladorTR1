#include "CamadaFisica.hpp"
#include "CamadaEnlaceDados.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <bitset>

using namespace std;

int CAMADA_FISICA_TIPO_CODIFICACAO = 0;

// Transmite os bits do quadro de acordo com o protocolo escolhido
void CamadaFisicaTransmissora(vector<int> quadro, int protocoloDeCodificacao){
    CAMADA_FISICA_TIPO_CODIFICACAO = protocoloDeCodificacao;
    
    vector<int> fluxoBrutoDebits;

    switch(CAMADA_FISICA_TIPO_CODIFICACAO){
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

// Decodifica os bits do quadro de acordo com o protocolo escolhido
void CamadaFisicaReceptora(vector<int> quadro){
    vector<int> fluxoBrutoDeBits;

    switch(CAMADA_FISICA_TIPO_CODIFICACAO){
        case 0:
            fluxoBrutoDeBits = CamadaFisicaReceptoraDecodBinaria(quadro);
            break;
        case 1:
            fluxoBrutoDeBits = CamadaFisicaReceptoraDecodManchester(quadro);
            break;
        case 2:
            fluxoBrutoDeBits = CamadaFisicaReceptoraDecodBipolar(quadro);
            break;
    }
    fluxoBrutoDeBits = CamadaEnlaceReceptoraControleDeErro(fluxoBrutoDeBits);
    CamadaEnlanceDadosReceptora(fluxoBrutoDeBits);
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