#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
tokencentinela,
tokennondigit,                                   
tokenconstanteCaracter,                            
tokenliteralesCadena,                           
tokendigit,                                        
tokendecimal,                                      
tokenoctalDigit,                                 
tokenoctal,                                     
tokenhexaDigit,                                
tokenhexadecimal,                                 
tokenrealPuntoInicial,                           
tokenrealConPunto,                          
tokenrealSoloExponencial,                         
tokenreal,                                       
tokenoperators,                                    
tokenpunctuators,                                  
tokenreservadaEspecificadorAlmacenamiento,        
tokenreservadaEspecificadorTipo,                   
tokenreservadaDeCalificadoresDeTipo,               
tokenreservadaDeStructUnion,                     
tokenreservadaDeEnumeracion,                       
tokenreservadaEtiqueta,                            
tokenreservadaSeleccion,                           
tokenreservadaDeIteracion,                         
tokenreservadaSalto,                              
tokenreservadaUnario,                              
tokenidentifier,                                   
tokennoReconocido                                 
} TipoToken;

typedef struct NodoToken {
    TipoToken tipo;                         
    char* lexema;                         // Puntero al texto del token
    int contador;
    int linea;
    int columna;

    struct NodoToken* siguiente;
} NodoToken;

#endif // TOKENS_H 