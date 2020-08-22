# Script para a compilação de código em C-
# Desenvolvido por Maria Luisa Santos Moreno Sanches
# e por André Lucas Maegina na disciplina de Compiladores
# do ICT_UNIFESP 2019

#!/bin/sh
out_file="scanner.cpp"
rm compilador
echo gerando scanner.c ...
flex -o scanner.cpp scanner.l
cat scanner.cpp > lex.yy.cpp
sed 's/extern int yylex (void);/\/\/caso queira usar c++\
#ifdef __cplusplus\
    extern "C" int yylex (void);\
#else\
    extern int yylex (void);\
#endif/g' lex.yy.cpp > scanner.cpp
rm lex.yy.cpp
echo gerando parser ...
bison -d parser.y
echo gerando objetos para compilacao ...
#rm parser.tab.o
g++ -c main.cpp scanner.cpp parser.tab.c tiny/util.c tiny/analyze.c tiny/symtab.c tiny/code.c tiny/cgen.c
echo compilando objetos ...
g++ -o compilador main.o scanner.o parser.tab.o util.o analyze.o symtab.o cgen.o code.o -ly -lfl
rm *.o 

./compilador sort.txt > saida.txt
#ls -la | grep 'compilador'
#echo executando scanner.out com \"sort.txt\" como entrada ...
#./scanner.out sort.txt
#echo imprimindo saida.txt ...
#sleep 1
#cat saida.txt