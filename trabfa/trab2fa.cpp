#include <vector>
#include <iostream>
#include <string>
#include "bscpp.hpp"

using namespace std;


//Exercico 03:
//Análise: Produzir um arranjo dinamico de nomes organizado (curto, mediano, longo), 
//que contém os nomes do arranjo dinamico de entrada. Levando em conta que nome curto até 3 letras, mediano de 4 a 8 letras 
//e longo mais que 8 letras.

//Tipo de dados: Os nomes serão armazenados em um arranjo dinamico do tipo string.

//Especificação: Reorganiza arranjo dinamico de nomes
vector<string> reorganiza_nomes(vector<string> nomes)
{
    vector<string> resultado;
    vector<string> curtos;
    vector<string> medianos;
    vector<string> longos;

    int i;

    if(nomes.size() == 0)
    {
        resultado = {};
    }

    for(i = 0; i < nomes.size(); i++)
    {
        if(nomes[i].length() <= 3){
            curtos.push_back(nomes[i]);
        }
        else if((3 < nomes[i].length()) && (nomes[i].length() <= 8)){
            medianos.push_back(nomes[i]);
        }
        else{
            longos.push_back(nomes[i]);
        }
    }

    for(i = 0; i < curtos.size(); i++)
    {
        resultado.push_back(curtos[i]);
    }

    for(i = 0; i < medianos.size(); i++)
    {
        resultado.push_back(medianos[i]);
    }

    for(i = 0; i < longos.size(); i++)
    {
        resultado.push_back(longos[i]);
    }

    return resultado;
    
}

examples
{
    check_expect(reorganiza_nomes({"Maria", "Joao", "Pedro", "Ash"}), (vector<string>{"Ash", "Maria", "Joao", "Pedro"}));
    check_expect(reorganiza_nomes({"Maria", "Joao", "Pedro", "Joao Vassallo"}), (vector<string>{"Maria", "Joao", "Pedro", "Joao Vassallo"}));
    check_expect(reorganiza_nomes({"Maria", "Joao", "Pedro", "Ash", "Joao Vassallo"}), (vector<string>{"Ash", "Maria", "Joao", "Pedro", "Joao Vassallo"}));
    check_expect(reorganiza_nomes({}), (vector<string>{}));
    
}

//Exercicio 04
//Análise: Contar quantos números de um arranjo dinamicos são únicos;
//Tipos de dados: os número serão representados por inteiros, em um vector de inteiros
//Especificação: Conta quantos números de um arranjo são únicos;

bool num_unicos(vector<int> numeros)
{

    bool resultado = true; 

    
    for(int i = 0; i < numeros.size(); i++)
    {   
        for(int j = 0; j < numeros.size(); j++)
        {
            if(numeros[i] == numeros[j])
            {
                resultado = false;
            }
        }
        
    }

    return resultado;
}

examples
{
    check_expect(num_unicos({1, 2, 3, 4, 5, 6}), true);
    check_expect(num_unicos({1, 2, 5, 4, 5, 6}), false);
    check_expect(num_unicos({1, 2, 3, 3, 3, 6}), false);
}
    

int main()
{
    run_tests();
}