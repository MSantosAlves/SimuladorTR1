#include "CamadaEnlaceDados.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <math.h>

using namespace std;

int TAMANHO_QUADRO = 4;
int TIPO_ENQUADRAMENTO = 1;
int TIPO_CONTROLE_ERRO = 2;
vector<int> BYTE_FLAG = {0, 0, 1, 1, 1, 1, 0, 0};// caracter '<'
vector<int> BYTE_ESC  = {0, 0, 1, 1, 1, 1, 1, 0};// caracter '>'
vector<int> CRC_32_802 = {0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1}; //0x04C11DB7
vector<int> CRC_32_802_TIMES_0 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Utilizado no Codigo de Hamming(63, 57)
vector<int> P1 = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62};
vector<int> P2 = { 1, 2, 5, 6, 9, 10, 13, 14, 17, 18, 21, 22, 25, 26, 29, 30, 33, 34, 37, 38, 41, 42, 45, 46, 49, 50, 53, 54, 57, 58, 61, 62};
vector<int> P3 = { 3, 4, 5, 6, 11, 12, 13, 14, 19, 20, 21, 22, 27, 28, 29, 30, 35, 36, 37, 38, 43, 44, 45, 46, 51, 52, 53, 54, 59, 60, 61, 62};
vector<int> P4 = { 7, 8, 9, 10, 11, 12, 13, 14, 23, 24, 25, 26, 27, 28, 29, 30, 39, 40, 41, 42, 43, 44, 45, 46, 55, 56, 57, 58, 59, 60, 61, 62};
vector<int> P5 = { 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62};
vector<int> P6 = { 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62};
vector<int> INDICES_PARIDADE = {0, 1, 3, 7, 15, 31};


vector<vector<int>> CamadaEnlanceDadosTransmissora(string mensagem, int tipoDeEnquadramento, int tipoDeControleDeErro){   
    TIPO_ENQUADRAMENTO = tipoDeEnquadramento;
    TIPO_CONTROLE_ERRO = tipoDeControleDeErro;
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
    int tamanhoQuadro = 0;

    while (counter < tamanhoMensagem) {
        tamanhoQuadro = min(TAMANHO_QUADRO, tamanhoMensagem - counter);
        quadroString = mensagem.substr(counter, tamanhoQuadro);
        quadroString = to_string(quadroString.size() + 1) + quadroString;
        quadrosStrings.push_back(quadroString);
        counter += tamanhoQuadro;
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
    int tamanhoQuadro = 0;

    int tamanhoMensagem = mensagem.size();
    bool isFlag = true;
    bool isEsc = true;
    int counter = 0;

    while (counter < tamanhoMensagem) {
        tamanhoQuadro = min(TAMANHO_QUADRO, tamanhoMensagem - counter);
        quadroString = mensagem.substr(counter, tamanhoQuadro);
        quadrosStrings.push_back(quadroString);
        counter += tamanhoQuadro;
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
    vector<int> header = {quadro.begin(), quadro.begin() + 8};
    vector<int> quadroDados = {quadro.begin() + 8, quadro.end()};
    int tamanhoQuadro = 0;
    string headerBitString = "";
    
    for(int i = 7; i >= 0; i--){
        headerBitString += to_string(header[7 - i]);
    }

    tamanhoQuadro = 0;
    tamanhoQuadro = headerBitString == "00000001" ? 1 : tamanhoQuadro;
    tamanhoQuadro = headerBitString == "00000010" ? 2 : tamanhoQuadro;
    tamanhoQuadro = headerBitString == "00000011" ? 3 : tamanhoQuadro;
    tamanhoQuadro = headerBitString == "00000100" ? 4 : tamanhoQuadro;
    tamanhoQuadro = headerBitString == "00000101" ? 5 : tamanhoQuadro;
    tamanhoQuadro = headerBitString == "00000110" ? 6 : tamanhoQuadro;
    tamanhoQuadro = headerBitString == "00000111" ? 7 : tamanhoQuadro;
    tamanhoQuadro = headerBitString == "00001000" ? 8 : tamanhoQuadro;

    if((tamanhoQuadro - 1) * 8 != quadroDados.size()){
        cout << "[Enquadramento - Cont. caracteres] => Perda de sinc." << endl;
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
        case 2:
            quadroComControleDeErro = CamadaEnlaceDadosTransmissoraControleDeErroCodigoHamming(quadro);
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

// Implementacao para Hamming(63, 57) (6 bits de paridade)
// Como os quadros possuem menos de 57 bits, adiciona-se 0's ao fim de cada quadro para completar o blocos de 57 bits
vector<int> CamadaEnlaceDadosTransmissoraControleDeErroCodigoHamming(vector<int> quadro){
    vector<int> codigoHamming;
    int countQuadroPositions = 0 ;

    for(int i = 0; i < 63 ; i ++){
        if(estaIncluso(i, INDICES_PARIDADE)){
            codigoHamming.push_back(-1);
        }else if(countQuadroPositions < quadro.size()){
            codigoHamming.push_back(quadro[countQuadroPositions]);
            countQuadroPositions++;
        }else{
            codigoHamming.push_back(0);
        }
    }

    for(int i = codigoHamming.size() ; i >= 0 ; i--){
        if(codigoHamming[i] == -1){
            codigoHamming[i] = calculaParidade(i, codigoHamming);
        }
    }

    return codigoHamming;
}

vector<int> CamadaEnlaceReceptoraControleDeErro(vector<int> quadro){
    vector<int> quadroSemControleDeErro;
    
    switch (TIPO_CONTROLE_ERRO){
        case 0:
            quadroSemControleDeErro = CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar(quadro);
            break;

        case 1:
            quadroSemControleDeErro = CamadaEnlaceDadosReceptoraControleDeErroCRC(quadro);
            break;
        case 2:
            quadroSemControleDeErro = CamadaEnlaceDadosReceptoraControleDeErroCodigoHamming(quadro);
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

vector<int> CamadaEnlaceDadosReceptoraControleDeErroCodigoHamming(vector<int> quadro){
    vector<int> quadroSemHamming;
    vector<int> posicoesComErroHamming;
    int paridadeEsperada = 0;

    for(int i = 0 ; i < quadro.size() ; i++){
        if(estaIncluso(i, INDICES_PARIDADE)){
            paridadeEsperada = calculaParidade(i, quadro);

            if(paridadeEsperada != quadro[i]){
                posicoesComErroHamming.push_back(i);
            }
        }
    }

    vector<vector<int>> posicoesCandidatas;
    vector<int> intersec;

    for(int i = 0 ; i < posicoesComErroHamming.size() ; i++){
        posicoesCandidatas.push_back(retornaPosicoesPorIndice(posicoesComErroHamming[i]));
    }

    if(posicoesCandidatas.size() > 0){
        intersec = posicoesCandidatas[0];
        for(int i = 0 ; i < posicoesCandidatas.size(); i++){
            intersec = interseccao(intersec, posicoesCandidatas[i]);
        }
    }

    if(intersec.size() > 0){
        cout << "[Correcao de Erros] => Codigo de Hamming: Ha um erro na posicao [" << intersec[0] << "] do quadro recebido. Invertendo bit..." << endl;
        quadro[intersec[0]] = !quadro[intersec[0]];
    }

    for(int i = 0 ; i < quadro.size() ; i++){
        if(!estaIncluso(i, INDICES_PARIDADE)){
            quadroSemHamming.push_back(quadro[i]);
        }
    }

    return quadroSemHamming;
}


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
        cout << v[i] << " ";
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

bool estaIncluso(int a, vector<int> b){
    bool isIncluded = false;
    for(int i = 0 ; i < b.size() ; i++){
        if(a == b[i]){
            isIncluded = true;
        }
    }

    return isIncluded;
}

int calculaParidade(int idx, vector<int> codigoHamming){
    int paridade = 0;
    vector<int> posicoes = retornaPosicoesPorIndice(idx);

    for(int i = codigoHamming.size() - 1 ; i > idx ; i--){
        if(estaIncluso(i, posicoes)){
            paridade += codigoHamming[i];
        }
    }

    return paridade % 2 == 0 ? 0 : 1;
}

vector<int> retornaPosicoesPorIndice(int idx){
    vector<int> posicoes;

    posicoes = P1;
    posicoes = idx == 1  ? P2 : posicoes;
    posicoes = idx == 3  ? P3 : posicoes;
    posicoes = idx == 7  ? P4 : posicoes;
    posicoes = idx == 15 ? P5 : posicoes;
    posicoes = idx == 31 ? P6 : posicoes;

    return posicoes;
}

vector<int> interseccao(vector<int> v1, vector<int> v2){
    vector<int> interseccao;
    for(int i = 0 ; i < v1.size() ; i++){
        for(int j = 0 ; j < v2.size() ; j++){
            if(v1[i] == v2[j]){
                interseccao.push_back(v1[i]);
            }
        }
    }

    return interseccao;
}