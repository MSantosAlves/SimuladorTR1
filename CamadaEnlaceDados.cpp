#include "CamadaEnlaceDados.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <math.h>

using namespace std;

int TAMANHO_QUADRO = 4 ;
int TIPO_ENQUADRAMENTO = 0;
int TIPO_CONTROLE_ERRO = 1;
vector<int> BYTE_FLAG = {0, 0, 1, 1, 1, 1, 0, 0};// caracter '<'
vector<int> BYTE_ESC  = {0, 0, 1, 1, 1, 1, 1, 0};// caracter '>'
vector<int> CRC_32_802 = {0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1}; //0x04C11DB7
vector<int> CRC_32_802_TIMES_0 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

vector<vector<int>> CamadaEnlanceDadosTransmissora(string mensagem){   
    return CamadaEnlanceDadosTransmissoraEnquadramento(mensagem);
}

vector<vector<int>> CamadaEnlanceDadosTransmissoraEnquadramento(string mensagem){
    vector<vector<int>> quadrosEnquadrados;

    switch(TIPO_ENQUADRAMENTO){
        case 0:
            quadrosEnquadrados = CamadaEnlanceTransmissoraEnquadramentoContagemDeCaracteres(mensagem);
            break;
        case 1:
            quadrosEnquadrados = CamadaEnlanceTransmissoraEnquadramentoInsercaoDeBytes(mensagem);
            break;
    }

    return quadrosEnquadrados;
}

vector<vector<int>> CamadaEnlanceTransmissoraEnquadramentoContagemDeCaracteres(string mensagem){
    vector<vector<int>> quadrosEnquadrados;
    vector<int> quadro;
    vector<string> quadrosStrings;
    string quadroString = "";
    int counter = 0;
    int tamanhoMensagem = mensagem.size();

    while (counter < tamanhoMensagem) {
        TAMANHO_QUADRO = min(TAMANHO_QUADRO, tamanhoMensagem - counter);
        quadroString = mensagem.substr(counter, TAMANHO_QUADRO);
        quadroString = to_string(quadroString.size() + 1) + quadroString;
        quadrosStrings.push_back(quadroString);
        counter += TAMANHO_QUADRO;
    }

    // Transforma cada quadro em um vetor de bits
    for(int i = 0; i < quadrosStrings.size(); i++){

        for (int j = 0 ; j < quadrosStrings[i].size() ; j++) {
            char c  = quadrosStrings[i][j];
            
            // Primeiro caracter: Valor numerico que representa o numero de caracteres do quadro
            // Para cada caracter, transforma seu valor ASCII em um vetor de bits
            bitset<8> bits(j == 0 ? c - '0' : c);
            
            for (int i = 7; i >= 0; --i) {
                // Adiciona os bits do caracter ao quadro
                quadro.push_back(bits[i]);
            }
        }

        quadrosEnquadrados.push_back(quadro);
        quadro.clear();

    }

    return quadrosEnquadrados;
}

vector<vector<int>> CamadaEnlanceTransmissoraEnquadramentoInsercaoDeBytes(string mensagem){
    vector<vector<int>> quadrosEnquadrados;
    vector<int> quadro;
    vector<string> quadrosStrings;
    string quadroString = "";
    vector<int> charByte;

    int tamanhoMensagem = mensagem.size();
    bool isFlag = true;
    bool isEsc = true;
    int counter = 0;

    while (counter < tamanhoMensagem) {
        TAMANHO_QUADRO = min(TAMANHO_QUADRO, tamanhoMensagem - counter);
        quadroString = mensagem.substr(counter, TAMANHO_QUADRO);
        quadrosStrings.push_back(quadroString);
        counter += TAMANHO_QUADRO;
    }

    // Transforma cada quadro em um vetor de bits
    for(int i = 0; i < quadrosStrings.size(); i++){

        quadro.insert(quadro.end(), BYTE_FLAG.begin(), BYTE_FLAG.end());

        for (int j = 0 ; j < quadrosStrings[i].size() ; j++) {
            char c  = quadrosStrings[i][j];
            bitset<8> bits(c);

            for (int i = 7; i >= 0; --i) {
                charByte.push_back(bits[i]);
            }

            for(int i = 0 ; i < 8 ; i++){

                if(BYTE_FLAG[i] != charByte[i]){
                    isFlag = false;
                }

                if(BYTE_ESC[i] != charByte[i]){
                    isEsc = false;
                }
            }

            if(isFlag || isEsc){
                quadro.insert(quadro.end(), BYTE_ESC.begin(), BYTE_ESC.end());
            }

            isFlag = true;
            isEsc = true;

            quadro.insert(quadro.end(), charByte.begin(), charByte.end());
            charByte.clear();
        }

        quadro.insert(quadro.end(), BYTE_FLAG.begin(), BYTE_FLAG.end());
        quadrosEnquadrados.push_back(quadro);
        quadro.clear();

    }

    return quadrosEnquadrados;
}

void CamadaEnlanceDadosReceptora(vector<int> quadro){
    vector<int> quadroDesenquadrado = CamadaEnlanceDadosReceptoraEnquadramento(quadro);
    
    CamadaDeAplicacaoReceptora(quadroDesenquadrado);
}

vector<int> CamadaEnlanceDadosReceptoraEnquadramento(vector<int> quadro){
    vector<int> quadroDesenquadrado;
    switch(TIPO_ENQUADRAMENTO){
        case 0:
            quadroDesenquadrado = CamadaEnlanceReceptoraEnquadramentoContagemDeCaracteres(quadro);
            break;
        case 1:
            quadroDesenquadrado = CamadaEnlanceReceptoraEnquadramentoInsercaoDeBytes(quadro);
            break;
    }

    return quadroDesenquadrado;
}

vector<int> CamadaEnlanceReceptoraEnquadramentoContagemDeCaracteres(vector<int> quadro){
    vector<int> quadroDesenquadrado;
    vector<int> tamanhoQuadroEmBits = {quadro.begin(), quadro.begin() + 8};
    vector<int> quadroDados = {quadro.begin() + 8, quadro.end()};
    
    int TAMANHO_QUADRO = 0;
    int qtdeDeBytesRecebida = quadroDados.size() / 8;

    for(int i = 7; i >= 0; i--){
        if(tamanhoQuadroEmBits[7 - i] == 1){
            TAMANHO_QUADRO += int(pow(double(2), double(i)));
        }
    }

    int qtdeBytesEsperada = (TAMANHO_QUADRO - 1);

    if(qtdeBytesEsperada != qtdeDeBytesRecebida){
        cout << "[Enquadramento - Cont. caracteres] => Perda de sinc. - bytes esperados/recebidos: " << qtdeBytesEsperada << "/" << qtdeDeBytesRecebida << endl;
    }

    quadroDesenquadrado = quadroDados;

    return quadroDesenquadrado;
}

vector<int> CamadaEnlanceReceptoraEnquadramentoInsercaoDeBytes(vector<int> quadro){
    vector<int> quadroDesenquadrado;
    vector<int> cargaUtil = {quadro.begin() + 8, quadro.end() - 8};
    vector<vector<int>> quadroBytes;
    vector<int> byte, byteAtual, proxByte;
    int bytesCargaUtil = cargaUtil.size() / 8;
    vector<int> bytesParaRemover;

    //Cria um vetor de vetores, onde cada vetor interno eh um byte
    for(int i = 0 ; i < bytesCargaUtil ; i++){
        byte = {cargaUtil.begin() + i * 8, cargaUtil.begin() + (i + 1) * 8};
        quadroBytes.push_back(byte);
        byte.clear();
    }

    // Compara cada byte com seu proximo, se for um byte de escape e o proximo 
    //for um byte de escape ou flag, remove o byte de escape.
    for(int i = 0 ; i < quadroBytes.size() - 1; i++){
        byteAtual = quadroBytes[i];
        proxByte = quadroBytes[i+1];

        if(compareVectors(byteAtual, BYTE_ESC) && (compareVectors(proxByte, BYTE_FLAG) || compareVectors(proxByte, BYTE_ESC))){
            bytesParaRemover.push_back(i);
        }
    }

    // Apaga os bytes dos indices que foram adicionados no vetor de bytes para remover
    for(int i = 0 ; i < bytesParaRemover.size() ; i++){
        quadroBytes.erase(quadroBytes.begin() + bytesParaRemover[i]);
    }

    //Copia os bytes para o vetor de quadro desenquadrado
    for(int i = 0 ; i < quadroBytes.size() ; i++){
        quadroDesenquadrado.insert(quadroDesenquadrado.end(), quadroBytes[i].begin(), quadroBytes[i].end());
    }

    return quadroDesenquadrado;
}

vector<int> CamadaEnlaceTransmissoraControleDeErro(vector<int> quadro){
    vector<int> quadroComControleDeErro;
    
    switch (TIPO_CONTROLE_ERRO){
        case 0:
            quadroComControleDeErro = CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar(quadro);
            break;

        case 1:
            quadroComControleDeErro = CamadaEnlaceDadosTransmissoraControleDeErroCRC(quadro);
            break;
    }
    return quadroComControleDeErro;
};

vector<int> CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar(vector<int> quadro){
    int count = 0;

    for (int i = 0; i < quadro.size(); i++)
    {
        if (quadro[i] == 1)
        {
            count++;
        }
    }

    if (count % 2 == 0)
    {
        quadro.push_back(0);
    }
    else
    {
        quadro.push_back(1);
    }
    
    return quadro;
};

vector<int> CamadaEnlaceDadosTransmissoraControleDeErroCRC(vector<int> quadro){
    vector<int> CRC = CalculaCRC(quadro);

    quadro.insert(quadro.end(), CRC.begin(), CRC.end());

    return quadro;
};

vector<int> CamadaEnlaceReceptoraControleDeErro(vector<int> quadro){
    vector<int> quadroSemControleDeErro;
    
    switch (TIPO_CONTROLE_ERRO){
        case 0:
            quadroSemControleDeErro = CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar(quadro);
            break;

        case 1:
            quadroSemControleDeErro = CamadaEnlaceDadosReceptoraControleDeErroCRC(quadro);
            break;
    }
    return quadroSemControleDeErro;
}

vector<int> CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar(vector<int> quadro){
    int paridadeRecebida = quadro[quadro.size() - 1];
    int count = 0;
    vector<int> quadroSemParidade = {quadro.begin(), quadro.end() - 1};

    for(int i = 0; i < quadro.size() - 1; i++){
        if(quadro[i] == 1){
            count++;
        }
    }

    int paridadeCalculada = count % 2;

     if (paridadeRecebida != paridadeCalculada){
        cout << "[DeteccaoDeErros] => Erro de paridade." << endl;
     }

    return quadroSemParidade;
};

vector<int> CamadaEnlaceDadosReceptoraControleDeErroCRC(vector<int> quadro){
    bool crcError = false;
    vector<int> quadroSemCRC;
    quadroSemCRC.assign(quadro.begin(), quadro.end() - CRC_32_802.size() + 1);

    vector<int> CRCEsperado = CalculaCRC(quadroSemCRC);
    vector<int> CRCRecebido = {quadro.end() - CRC_32_802.size() + 1, quadro.end()};

    for(int i = 0 ; i < CRCRecebido.size() ; i++){
        if(CRCRecebido[i] != CRCEsperado[i]){
            crcError = true;
        }
    }

    if(crcError){
        cout << "[DeteccaoDeErros] => Erro de CRC." << endl;
    }
    
    return quadroSemCRC;
};


//Funcoes auxiliares

bool compareVectors(vector<int> v1, vector<int> v2){
    bool isEqual = true;
    for(int i = 0 ; i < v1.size() ; i++){
        if(v1[i] != v2[i]){
            isEqual = false;
        }
    }

    return isEqual;
}

void printVector(vector<int> v){
    for(int i = 0 ; i < v.size() ; i++){
        cout << v[i];
    }
    cout << endl;
}

vector<int> operacaoXOR(vector<int> v1, vector<int> v2){
    vector<int> resultadoXOR;
    int xorBit = 0;

    for(int i = 0 ; i < v1.size() ; i++){
        xorBit = v1[i] ^ v2[i];
        resultadoXOR.push_back(xorBit);
    }

    return resultadoXOR;
}

vector<int> CalculaCRC(vector<int> quadro){
    vector<int> quadroComCRCComplemento;
    vector<int> dividendo;
    vector<int> divisor;
    vector<int> resultadoParcial;

    quadroComCRCComplemento.assign(quadro.begin(), quadro.end());

    // Adiciona n - 1 (31 bits) 0's ao final do quadro 
    for(int i = 0 ; i < CRC_32_802.size() -1 ; i++){
        quadroComCRCComplemento.push_back(0);
    }

    for(int i = CRC_32_802.size() ; i < quadroComCRCComplemento.size() + 1; i++){

        if(i == CRC_32_802.size()){
            dividendo.assign(quadroComCRCComplemento.begin(), quadroComCRCComplemento.begin() + CRC_32_802.size());
        }else{
            dividendo.assign(resultadoParcial.begin(), resultadoParcial.end());
            dividendo.push_back(0); // Adiciona um bit 0 ao fim do resultado parcial
        }

        if(resultadoParcial.empty() || resultadoParcial[0] == 1){
            divisor = CRC_32_802;
        }else{
            divisor = CRC_32_802_TIMES_0;
        }
        resultadoParcial = operacaoXOR(dividendo, divisor);
        resultadoParcial.assign(resultadoParcial.begin() + 1, resultadoParcial.end());
    }

    return resultadoParcial;
}