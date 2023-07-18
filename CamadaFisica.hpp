
#ifndef __CAMADA_FISICA_H__
#define __CAMADA_FISICA_H__

#include <string>
#include <vector>

using namespace std;

void CamadaFisicaTransmissora(vector<int> quadro);

vector<int> CamadaFisicaTransmissoraCodBinaria(vector<int> quadro);

vector<int> CamadaFisicaTransmissoraCodManchester(vector<int> quadro);

vector<int> CamadaFisicaTransmissoraCodBipolar(vector<int> quadro);

void CamadaFisicaReceptora(vector<int> quadro);

vector<int> CamadaFisicaReceptoraDecodBinaria(vector<int> quadro);

vector<int> CamadaFisicaReceptoraDecodManchester(vector<int> quadro);

vector<int> CamadaFisicaReceptoraDecodBipolar(vector<int> quadro);

#endif