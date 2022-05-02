#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>

// Am folosit typedef pentru a prescurta denumirea tipului de date.
typedef unsigned long long ull;

typedef struct QuadtreeNode {

    unsigned char blue, green, red;
    uint32_t area;
    int32_t top_left, top_right;
    int32_t bottom_left, bottom_right;
} __attribute__((packed)) QuadtreeNode;

// Structura pentru a retine culoarea unui pixel / matricea pixelilor.
typedef struct rgb {

    unsigned char red, green, blue;
} rgb;


// Structura pentru nodurile arborelui.
typedef struct QNode {

    rgb colour;
    uint32_t area;
    struct QNode *top_left, *top_right;
    struct QNode *bottom_left, *bottom_right;
} QNode;

// Alocarea spatiului pentru matrice, cu dimensiunea ppm_size.
rgb** create_Matrix(int ppm_size){ 

    rgb **Matrix;

    Matrix = (rgb**)malloc(ppm_size * sizeof(rgb*));

    int i;

    for (i = 0; i < ppm_size; i++) {
        
        Matrix[i] = (rgb*)malloc(ppm_size * sizeof(rgb));
    }

    return Matrix;
}

// Functie pentru afisarea matricei, cu rol in verificarea codului.
void print_Matrix(int ppm_size, rgb **Matrix) {

    int i, j;

    FILE *out;

    out = fopen("Matrix.out", "w");

    for (i = 0; i < ppm_size; i++) {

        for (j = 0; j < ppm_size; j++) {

            fprintf(out, "%d %d %d| ", Matrix[i][j].red, Matrix[i][j].green, Matrix[i][j].blue);
        }

        fprintf(out, "\n");
    }
}

// Functie ce calculeaza media culorilor pixelilor.
rgb average(int size, int x, int y, rgb **Matrix) {

    // Cum valorile care se aduna vor depasi, cel mai probabil, unsigned char, am preferat folosirea ull.
    ull red = 0, green = 0, blue = 0; 
    int i, j;

    for (i = x; i < x + size; i++) {

        for (j = y; j < y + size; j++) {

            red += Matrix[i][j].red;
            green += Matrix[i][j].green;
            blue += Matrix[i][j].blue;
        }
    }

    red /= size * size;
    green /= size * size;
    blue /= size * size;

    rgb aux;

    aux.red = red;
    aux.green = green;
    aux.blue = blue;
    
    // Am preferat formarea lui aux pentru a-l putea returna din functia de average.
    
    return aux;
}

// Functie ce calculeaza mean-ul, dupa cum este definit in cerinta.
ull det_mean(int size, int x, int y, rgb **Matrix) {

    // Am apelat functia anterior mentionata.
    rgb aux = average(size, x, y, Matrix);

    ull mean_aux = 0;

    int i, j;

    for (i = x; i < x + size; i++) {

        for (j = y; j < y + size; j++) {

            mean_aux += pow(aux.red - Matrix[i][j].red, 2);
            mean_aux += pow(aux.green - Matrix[i][j].green, 2);
            mean_aux += pow(aux.blue - Matrix[i][j].blue, 2);
        }
    }

    mean_aux /= 3 * size * size;

    return mean_aux;
}

// Functie care verifica daca un nod este frunza.
unsigned char is_leaf(QNode *Node) {

    if (Node->top_left == NULL && Node->top_right == NULL && Node->bottom_left == NULL && Node->bottom_right == NULL) {

        return 1;
    }

    return 0;
}

// Functie ce creeaza un nod.
QNode* create_Node(int size, rgb colour) {

    QNode *new_Node = (QNode *)malloc(sizeof(QNode));

    // Egaleaza campul colour cu parametrii primiti in antet.
    new_Node->colour = colour;

    // Egalieaza campul area cu aria patratului respectiv.
    new_Node->area = size * size;

    // Campurile nodurilor fii sunt egalate cu NULL.
    new_Node->top_left = NULL;
    new_Node->top_right = NULL;
    new_Node->bottom_left = NULL;
    new_Node->bottom_right = NULL;

    return new_Node;
}

// Functie ce realizeaza recursiv compresia.
void compression(int size, int x, int y, rgb **Matrix, ull threshold, QNode **Node) {

    // Se calculeaza mean-ul cu functia anterior definita.
    ull mean = det_mean(size, x, y, Matrix);

    // Se calculeaza media culorilor.
    rgb colour = average(size, x, y, Matrix);

    // Se creeaza un nod nou.
    (*Node) = create_Node(size, colour);
    
    // Daca mean-ul este mai mare decat threshold-ul, se creeaza noduri noi.
    if (mean > threshold) {

        compression(size / 2, x, y, Matrix, threshold, &(*Node)->top_left);
        compression(size / 2, x, y + size / 2, Matrix, threshold, &(*Node)->top_right);
        compression(size / 2, x + size / 2, y, Matrix, threshold, &(*Node)->bottom_left);
        compression(size / 2, x + size / 2, y + size / 2, Matrix, threshold, &(*Node)->bottom_right);
    }
}

// Functia ce trece arborele in vector.
void QNode_to_array(QNode *Node, QuadtreeNode **array[], int *no_leaves, int *index, int father, int type) {

    // Realocarea vectorului pentru fiecare nod nou adaugat.
    (*array) = realloc((*array), ((*index) + 1) * sizeof(QuadtreeNode*));
    (*array)[(*index)] = malloc(sizeof(QuadtreeNode));
    
    // Popularea campurilor pentru culori.
    ((*array)[(*index)])->blue = Node->colour.blue;
    ((*array)[(*index)])->green = Node->colour.green;
    ((*array)[(*index)])->red = Node->colour.red;

    // Idem pentru arie.
    ((*array)[(*index)])->area = Node->area;

    // Popularea indicilor pentru nodurile fii.
    ((*array)[(*index)])->top_left = -1;
    ((*array)[(*index)])->top_right = -1;
    ((*array)[(*index)])->bottom_right = -1;
    ((*array)[(*index)])->bottom_left = -1;

    // Daca nu este primul pas (plecarea din radacina), vom reveni in pozitia tatalui din vector, pentru a-i adauga indicele fiului.
    if (father != -1) {

        if(type == 1)
            ((*array)[father])->top_left = (*index);
        else
            if (type == 2)
                ((*array)[father])->top_right = (*index);
        else
            if (type == 3)
                ((*array)[father])->bottom_right = (*index);
        else
            if (type == 4)
                ((*array)[father])->bottom_left = (*index); 
    }

    father = (*index);
    (*index) ++;

    if (is_leaf(Node) == 1) {

        (*no_leaves) ++;
    }

    // Continuarea adaugarii nodurilor in vector.
    else {

        if (Node->top_left != NULL)
            QNode_to_array(Node->top_left, array, no_leaves, index, father, 1);

        if (Node->top_right != NULL)
            QNode_to_array(Node->top_right, array, no_leaves, index, father, 2);

        if (Node->bottom_right != NULL)
            QNode_to_array(Node->bottom_right, array, no_leaves, index, father, 3);

        if (Node->bottom_left != NULL)
            QNode_to_array(Node->bottom_left, array, no_leaves, index, father, 4);
    }
}

// Functie pentru elibararea memoriei utilizata pentru arbore.
void free_tree(QNode **Node) {

    if ((*Node) == NULL)
        return;

    free_tree(&(*Node)->top_left);
    free_tree(&(*Node)->top_right);
    free_tree(&(*Node)->bottom_right);
    free_tree(&(*Node)->bottom_left);

    free((*Node));
}

// Functie ce trece vectorul in arbore, in mod recursiv.
void array_to_QNode(QNode **Node, QuadtreeNode **array, int i) {

    rgb aux;

    aux.red = array[i]->red;
    aux.green = array[i]->green;
    aux.blue = array[i]->blue;

    (*Node) = create_Node(sqrt(array[i]->area), aux);

    if (array[i]->top_left != -1 && array[i]->top_right != -1 && array[i]->bottom_right != -1 && array[i]->bottom_left != -1) {

        array_to_QNode(&(*Node)->top_left, array, array[i]->top_left);
        array_to_QNode(&(*Node)->top_right, array, array[i]->top_right);
        array_to_QNode(&(*Node)->bottom_right, array, array[i]->bottom_right);
        array_to_QNode(&(*Node)->bottom_left, array, array[i]->bottom_left);
    }
}

// Functie ce realizeaza decompresia (trecerea din arbore in matrice).
void decompression(int size, int x, int y, rgb ***Matrix, QNode *Node) {

    // Daca nodul curent este frunza, se vor popula pixelii aferenti.
    if (is_leaf(Node) == 1) {
        
        int i, j;

        for (i = x; i < x + size; i++) {
            for (j = y; j < y + size; j++) 
                (*Matrix)[i][j] = Node->colour;
        }
    }
    // Daca nodul curent nu este frunza, se va continua parcurgerea pana la frunze (size-ul va scadea).
    else {

        decompression(size / 2, x, y, Matrix, Node->top_left);
        decompression(size / 2, x, y + size / 2, Matrix, Node->top_right);
        decompression(size / 2, x + size / 2, y + size / 2, Matrix, Node->bottom_right);
        decompression(size / 2, x + size /2, y, Matrix, Node->bottom_left);
    }
}

// Functie care reaizeaza oglindirea pe verticala, in mod recursiv, pe arbore.
void vertical_mirroring(QNode **Node) {

    if ((*Node)->top_left != NULL && (*Node)->top_right != NULL && (*Node)->bottom_right != NULL && (*Node)->bottom_left != NULL) {

        vertical_mirroring(&(*Node)->top_left);
        vertical_mirroring(&(*Node)->top_right);
        vertical_mirroring(&(*Node)->bottom_right);
        vertical_mirroring(&(*Node)->bottom_left);

        // Swap-ul corespunzator oglindirii.
        QNode *aux = (*Node)->top_left;
        (*Node)->top_left = (*Node)->bottom_left;
        (*Node)->bottom_left = aux;

        aux = (*Node)->top_right;
        (*Node)->top_right = (*Node)->bottom_right;
        (*Node)->bottom_right = aux;
    }
}

// Functie care reaizeaza oglindirea pe orizontala, in mod recursiv, pe arbore.
void horizontal_mirroring(QNode **Node) {

    if ((*Node)->top_left != NULL && (*Node)->top_right != NULL && (*Node)->bottom_right != NULL && (*Node)->bottom_left != NULL) {

        horizontal_mirroring(&(*Node)->top_left);
        horizontal_mirroring(&(*Node)->top_right);
        horizontal_mirroring(&(*Node)->bottom_right);
        horizontal_mirroring(&(*Node)->bottom_left);

        // Swap-ul corespunzator oglindirii.
        QNode *aux = (*Node)->top_left;
        (*Node)->top_left = (*Node)->top_right;
        (*Node)->top_right = aux;

        aux = (*Node)->bottom_left;
        (*Node)->bottom_left = (*Node)->bottom_right;
        (*Node)->bottom_right = aux;
    }
}

// Driver code-ul.
int main(int argc, char *argv[]) { 

    FILE *in, *out;
    int i, j;

    // Deschiderea fisierelor este diferita in functie de tipul cerintei (difera argumentele primite).
    if (argc == 5) {

        in = fopen(argv[3], "rb");
        out = fopen(argv[4], "wb");
    }
    else {
        if (argc == 4) {
            in = fopen(argv[2], "rb");
            out = fopen(argv[3], "wb");
        }
        else {
            in = fopen(argv[4], "rb");
            out = fopen(argv[5], "wb");
        }
    }

    QNode *root = NULL;
    QuadtreeNode **array = malloc(sizeof(QuadtreeNode*));
    
    if (strcmp(argv[1], "-c") == 0) {

        // PPM -> Matrice -> Arbore -> Vector -> Fisier binar.
        int no_leaves = 0, index = 0;
        char ppm_type[3];

        fscanf(in, "%s", ppm_type);

        int ppm_size;

        fscanf(in, "%d", &ppm_size);
        fscanf(in, "%d", &ppm_size);

        int ppm_max;

        fscanf(in, "%d", &ppm_max);

        // Este necesara citirea unui caracter in plus, pentru a trece de newline.
        char extra;
        fread(&extra, sizeof(char), 1, in);

        rgb **Matrix;

        Matrix = create_Matrix(ppm_size);

        for (i = 0; i < ppm_size; i++) {

            for (j = 0; j < ppm_size; j++) {

                fread(&Matrix[i][j].red, sizeof(unsigned char), 1, in);
                fread(&Matrix[i][j].green, sizeof(unsigned char), 1, in);
                fread(&Matrix[i][j].blue, sizeof(unsigned char), 1, in);
            }
        }

        ull threshold = atoi(argv[2]);

        compression(ppm_size, 0, 0, Matrix, threshold, &root);  // Matricea este trecuta in arbore.
        QNode_to_array(root, &array, &no_leaves, &index, -1, 0);    // Arborele este trecut in vector.

        // Printarea in fisierul binar a numarului de noduri si frunze.
        fwrite(&no_leaves, sizeof(int), 1, out);
        fwrite(&index, sizeof(int), 1, out);

        //### Eliberarea memoriei ###
        for (i = 0; i < index; i++) {
            
            fwrite(array[i], sizeof(QuadtreeNode), 1, out);
            free(array[i]);
        }

        free(array);

        free_tree(&root);

        for (i = 0; i < ppm_size; i++) {
            
            free(Matrix[i]);
        }

        free(Matrix);
        fclose(in);
        fclose(out);
    }
    else {
        if (strcmp(argv[1], "-d") == 0) {
            
            // Fisier binar -> Vector -> Arbore -> Matrice -> PPM.
            int *no_leaves_ptr = (int *)malloc(sizeof(int)), *index_ptr = (int *)malloc(sizeof(int));
            fread(no_leaves_ptr, sizeof(int), 1, in);
            fread(index_ptr, sizeof(int), 1, in);

            int no_leaves = *no_leaves_ptr, index = *index_ptr;

            free(no_leaves_ptr);
            free(index_ptr);
            
            array = realloc(array, index * sizeof(QuadtreeNode*));

            for (i = 0; i < index; i++) {
                
                array[i] = malloc(sizeof(QuadtreeNode));
                fread(array[i], sizeof(QuadtreeNode), 1, in);
            }

            rgb **Matrix;
            int ppm_size = sqrt(array[0]->area);
            Matrix = create_Matrix(ppm_size);

            array_to_QNode(&root, array, 0);    // Vectorul este trecut in arbore.

            decompression(ppm_size, 0, 0, &Matrix, root);   // Arborele este trecut in matrice.

            fprintf(out, "P6\n");
            fprintf(out, "%d %d\n", ppm_size, ppm_size);
            fprintf(out, "255\n");

            for (i = 0; i < ppm_size; i++) {
                for (j = 0; j < ppm_size; j++) {
                    
                    fwrite(&Matrix[i][j], sizeof(rgb), 1, out);
                }   
            }

            //### Eliberarea memoriei ###
            for (i = 0; i < index; i++) {
            
                free(array[i]);
            }

            free(array);

            free_tree(&root);

            for (i = 0; i < ppm_size; i++) {
            
                free(Matrix[i]);
            }

            free(Matrix);
            fclose(in);
            fclose(out);
        }
        else {
            if (strcmp(argv[1], "-m") == 0) {
                
                // PPM -> Matrice -> Arbore (+ procesul de oglindire) -> Matrice -> PPM
                int no_leaves = 0, index = 0;
                char ppm_type[3];

                fscanf(in, "%s", ppm_type);

                int ppm_size;

                fscanf(in, "%d", &ppm_size);
                fscanf(in, "%d", &ppm_size);


                int ppm_max;

                fscanf(in, "%d", &ppm_max);

                char extra;
                fread(&extra, sizeof(char), 1, in);

                rgb **Matrix;

                Matrix = create_Matrix(ppm_size);

                for (i = 0; i < ppm_size; i++) {

                    for (j = 0; j < ppm_size; j++) {

                        fread(&Matrix[i][j].red, sizeof(unsigned char), 1, in);
                        fread(&Matrix[i][j].green, sizeof(unsigned char), 1, in);
                        fread(&Matrix[i][j].blue, sizeof(unsigned char), 1, in);
                    }
                }

                char type = argv[2][0];
                ull threshold = atoi(argv[3]);

                compression(ppm_size, 0, 0, Matrix, threshold, &root);  // Matricea este trecuta in arbore.

                if (type == 'h')
                    horizontal_mirroring(&root);
                else {
                    if (type == 'v')
                        vertical_mirroring(&root);
                }

                decompression(ppm_size, 0, 0, &Matrix, root);   // Arborele este trecut in matrice.

                fprintf(out, "P6\n");
                fprintf(out, "%d %d\n", ppm_size, ppm_size);
                fprintf(out, "255\n");

                for (i = 0; i < ppm_size; i++) {
                    for (j = 0; j < ppm_size; j++) {
                        
                        fwrite(&Matrix[i][j], sizeof(rgb), 1, out);
                    }   
                }

                //### Eliberarea memoriei ###
                for (i = 0; i < index; i++) {
                    
                    fwrite(array[i], sizeof(QuadtreeNode), 1, out);
                    free(array[i]);
                }

                free(array);

                free_tree(&root);

                for (i = 0; i < ppm_size; i++) {
                    
                    free(Matrix[i]);
                }

                free(Matrix);
                fclose(in);
                fclose(out);
            }
        }
    }

    return 0;
}