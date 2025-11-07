#include "../src/languages/GrammarBicol.h"
#include <iostream>

int main()
{
        BicolGrammar a;
        std::cout << a.getLanguageName() << std::endl;

        std::cout << "Grammatical aspects:\n";
        a.printAspects();
        std::cout << "\nGrammatical Voices:\n";
        a.printVoices();
        std::cout << "\nGrammatical Cases:\n";
        a.printCases();
        std::cout << "\nNominal Markers:\n";
        a.printNomMarkers();

        return 0;
}
