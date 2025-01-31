echo "Compilation de B4lang"
./bin/B4lang   # Génère `test.s`

echo "Compilation de test.s"
nasm -f elf64 test.s -o test.o

echo "Compilation de test.o"
ld test.o -o test -lc --dynamic-linker /lib64/ld-linux-x86-64.so.2

echo "Fichier compilé: test"