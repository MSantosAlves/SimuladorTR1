#ifndef __CAMADA_ENALCE_DADOS_H__
#define __CAMADA_ENALCE_DADOS_H__

#include <string>
#include <vector>

using namespace std;


vector<vector<int>> CamadaEnlanceDadosTransmissora(string mensagem);

vector<vector<int>> CamadaEnlanceDadosTransmissoraEnquadramento(string mensagem);

vector<vector<int>> CamadaEnlanceTransmissoraEnquadramentoContagemDeCaracteres(string mensagem);

vector<vector<int>> CamadaEnlanceTransmissoraEnquadramentoInsercaoDeBytes(string mensagem);

void CamadaEnlanceDadosReceptora(vector<int> quadro);

vector<int> CamadaEnlanceDadosReceptoraEnquadramento(vector<int> quadro);

vector<int> CamadaEnlanceReceptoraEnquadramentoContagemDeCaracteres(vector<int> quadro);

vector<int> CamadaEnlanceReceptoraEnquadramentoInsercaoDeBytes(vector<int> quadro);

vector<int> CamadaEnlaceTransmissoraControleDeErro(vector<int> quadro);

vector<int> CamadaEnlaceReceptoraControleDeErro(vector<int> quadro);

vector<int> CamadaEnlaceDadosTransmissoraControleDeErroBitParidadePar(vector<int> quadro);

vector<int> CamadaEnlaceDadosTransmissoraControleDeErroCRC(vector<int> quadro);

vector<int> CalculaCRC(vector<int> quadro);

vector<int> CamadaEnlaceDadosReceptoraControleDeErroBitParidadePar(vector<int> quadro);

vector<int> CamadaEnlaceDadosReceptoraControleDeErroCRC(vector<int> quadro);

#endif