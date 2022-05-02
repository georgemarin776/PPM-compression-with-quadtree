-c (compresia):

Se citeste fisierul PPM. Din PPM (fisier binar), este creata o matrice a pixelilor.
Din matricea pixelilor, este realizat arborele, pana cand fiii sunt zone uniforme
de culoare (pana cand mean-ul < threshold). Arborele este trecut in vector, iar
vectorul este printat intr-un fisier binar.


-d (decompresie):

Se citeste fisierul binar, ce contine un vector identic (ca format) cu cel de
la compresie. Vectorul va fi trecut intr-un arbore, arborele in matrice, iar matricea
este scrisa intr-un fisier binar (PPM).


-m (oglindire):

Aici vom imbina functiile create pana acum, singurele care se adauga fiind cele
de oglindire (verticala si orizontala). PPM-ul se citeste in matrice, matricea este
trecuta in arbore, arborele este prelucrat cu functiile de oglindire. Arborele
trece, apoi, inapoi in matrice, iar matricea este printata intr-un fisier binar,
sub format PPM.


Functia QNode_to_array

Functia QNode_to_array realizeaza trecerea arborelui in vector, in maniera recursiva.
Totusi, o problema ce se ridica este adaugarea indicilor fiilor, caci "index"-ul se
modifica la fiecare apelara a functiei. Astlel, am adaugat "father" si "type". "father"
pastreaza indicele nodului din care s-a apelat functia pentru nodul curent (exceptie
face radacina, care are "father"-ul -1) si, in functie de tipul de nod in care ne aflam
(colt stanga-sus, dreapta-jos etc - cu type o valoare de la 1 la 4, conform schemei
din pdf-ul enuntului), se adauga tatalui indicele noului fiu.

# PPM-compression-with-quadtree
