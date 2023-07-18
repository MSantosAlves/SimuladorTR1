
#ifndef __CAMADA_APLICACAO_H__
#define __CAMADA_APLICACAO_H__

#include <string>
#include <vector>

using namespace std;

void AplicacaoTransmissora(void);

void AplicacaoReceptora(string mensagem);

void CamadaDeAplicacaoTransmissora(string mensagem);

void CamadaDeAplicacaoReceptora(vector<int> quadro);

void MeioDeTransmissao(vector<int> fluxoBrutoDeBits);

//Funcoes Auxiliares

bool compareVectors(vector<int> a, vector<int> b);

void printVector(vector<int> a);

bool estaIncluso(int a, vector<int> b);

int calculaParidade(int idx, vector<int> b);

#endif