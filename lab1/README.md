# Laboratorul 1 - Introducere în OpenMP

## Ce învățăm la APP?
La APP vom învăța cum să analizăm o problemă și o soluție a acesteia (mai precis, un program secvențial) și cum am putea să îmbunătățim soluțiile la problema respectivă (adică cum am putea să eficientizăm prin paralelizare soluțiile problemei).

În cadrul laboratoarelor de APP vom studia despre programarea paralelă (OpenMP, pthreads), despre programarea distribuită (MPI) și despre analiza performanțelor unui program (profiling).

## Ce este OpenMP?
OpenMP reprezintă un API prin care putem paraleliza programe secvențiale scrise în C/C++. Acesta este un API high-level, în sensul că programatorul are o varietate de tool-uri și de opțiuni la dispoziția sa, ele putând fi folosite cu mare ușurință.

OpenMP este creat pe baza modelului fork-join, unde avem un thread principal (master), din care se creează alte thread-uri (fork, echivalent cu `pthread_create` din pthreads), care, împreună cu thread-ul master, execută task-uri în paralel, în cadrul unor zone numite regiuni paralele. După ce task-urile respective sunt terminate, thread-urile forked "revin" în thread-ul principal (join).

![fork join](../media/lab1/fork_join.png)

## Includere și compilare
Pentru a putea folosi OpenMP în cod, trebuie inclusă biblioteca `omp.h` în cod: `#include <omp.h>`.

Pentru compilare, este necesar un flag, care diferă în funcție de compilator:
- gcc: `gcc helloworld.c -o helloworld -fopenmp`
- SunStudio (pe fep): `cc -xopenmp helloworld.c -o helloworld`

## Sintaxa OpenMP
Pentru OpenMP, se folosesc directive de compilare de tip pragma pentru a marca blocuri de cod paralelizate și pentru a folosi elemente de sincronizare.

Tipar: `#pragma omp numele_directivei [clause, ...]`

Exemplu: `#pragma omp parallel default(shared) private(beta, pi)`

### Paralelizarea secvențelor de cod
Pentru ca o bucată de cod să fie executată de mai multe thread-uri, folosim directiva `#pragma omp parallel` prin care marcăm faptul că acea zonă de cod este executată în paralel de mai multe thread-uri.

```c
#pragma omp parallel
{
	// cod paralelizat
}
```

Exemplu de folosire:
```c
#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv) {
    #pragma omp parallel 
    {
        int tid = omp_get_thread_num();
        printf("Hello world from thread number %d\n", tid);
    }

    #pragma omp parallel 
    {
        int tid = omp_get_thread_num();
        printf("Frumos in anul 4, zice thread-ul %d\n", tid);
    }
    return 0;
}
```

## Funcții utile

## Vizibilitatea variabilelor

## Paralelizarea buclelor

## Elemente de sincronizare

## Reduction
`reduction` este o directivă folosită pentru operații de tip reduce / fold pe arrays / colecții sau simple însumări / înmulțiri în cadrul unui loop. Mai precis, elementele dintr-un array sau indecșii unui loop sunt "acumulați" într-o singură variabilă, cu ajutorul unei operații, al cărui semn este precizat.

Tipar: `reduction(operator_operatie:variabila_in_care_se_acumuleaza)`

Exemplu de reduction: `reduction(+:sum)`, unde se însumează elementele unui array în variabila `sum`

Exemplu de folosire de reduction:
```c
int sum = 0;

#pragma omp parallel for reduction(+:sum) private(i)
for (i = 1; i <= num_steps; i++) {
    sum += i;
}
```

## Exerciții
1) **(2 puncte)** Descărcați [scheletul de laborator](https://github.com/florinrm/app-laborator/tree/main/lab1/skel) și [demo-ul pentru laborator](https://github.com/florinrm/app-laborator/tree/main/lab1/demo). Rulați exemplele din demo.
 
2) **(8 puncte)** Paralelizați însumarea elementelor unui array, încât suma să fie corectă, folosind fișierul `array_sum.c` din schelet, unde este implementată suma serială a elementelor dintr-un array.
