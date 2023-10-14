//
// Created by lukasbt on 10/14/23.
//

#ifndef PROJECTAD3_TREE_H
#define PROJECTAD3_TREE_H

/**
 * Tel hoeveel keer elk karakter voorkomt in een bestand, bereken de optimale orde-bewarende prefix-codering (OPC) en
 * schrijf deze uit naar een bestand, volgens volgend formaat:
 *
 * \<karakter-waarde (decimaal)\> \<aantal voorkomens (decimaal)\> \<orde-bewarende prefix-code (binair)\>
 *
 * @param inputFile Het pad naar het inputbestand.
 * @param outputFile Het pad naar het outputbestand.
 * @param bufferSize De grootte van de buffer die je mag gebruiken om de ingelezen strings in op te slaan, in bytes.
 */
void tree(const char *inputFile, const char *outputFile, int bufferSize);

#endif //PROJECTAD3_TREE_H
