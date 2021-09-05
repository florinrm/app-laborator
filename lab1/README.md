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

De asemenea, putem avea regiuni paralele imbricate:
```c
```c
#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv) {
    #pragma omp parallel 
    {
        printf("Parallel region thread %d\n", omp_get_thread_num());
        
        #pragma omp parallel 
        {
            printf("Nested parallel region thread %d\n", omp_get_thread_num());
        }
    }
    return 0;
}
```

## Funcții utile

## Vizibilitatea variabilelor

## Paralelizarea buclelor
În OpenMP putem paraleliza buclele de tip for folosind directiva `#pragma omp for` în cadrul unei zone paralele. În acest fel, iterațiile din for sunt împărțite egal thread-urilor, fiecare thread având iterațiile sale din cadrul buclei for.

Paralelizarea buclelor poate fi eficientizată folosind directiva `SCHEDULE`, despre care vom discuta în laboratorul 2.

Exemplu de folosire:
```c
#include <stdio.h>
#include <omp.h>

int main(int argc, char** argv) {
    int i, x[20];
    #pragma omp parallel private(i) shared(x) 
    {
        #pragma omp for 
        for (i = 0; i < 20; i++) {
            x[i] = i;
            printf("iteration no. %d | thread no. %d\n", i, omp_get_thread_num());
        }
    }

    printf("\n");

    // o alta forma, aceeasi functionalitate
    #pragma omp parallel for private(i) shared(x) 
    for (i = 0; i < 20; i++) {
        x[i] = i;
        printf("iteration no. %d | thread no. %d\n", i, omp_get_thread_num());
    }

    return 0;
}
```

## Elemente de sincronizare
În OpenMP avem la dispoziție elemente de sincronizare, prin care putem să ne asigurăm faptul că soluția paralelizată funcționează corect, fără probleme în ceea ce privește rezultatele incorecte sau deadlocks.

### Mutex
Pentru zonele critice, unde avem operații de read-write, folosim directiva `#pragma omp critical`, care reprezintă un mutex, echivalentul lui `pthread_mutex_t` din pthreads, care asigură faptul că un singur thread accesează zona critică la un moment dat, thread-ul deținând lock-ul pe zona critică în momentul respectiv, și că celelalte thread-uri care nu au intrat încă în zona critică așteaptă eliberarea lock-ului de către thread-ul aflat în zona critică în acel moment.

Exemplu de folosire:
```c
#include <stdio.h>
#include <omp.h>

int main (int argc, char** argv) {
    int thread_id, sum = 0;
    #pragma omp parallel private(thread_id) shared(sum)
    {
        thread_id = omp_get_thread_num();
        #pragma omp critical
        sum += thread_id;
    }
    printf("%d",sum);
    
    return 0;
}
```

### Barieră
Un alt element de sincronizare reprezintă bariera, care asigură faptul că niciun thread gestionat de barieră nu trece mai departe de aceasta decât atunci cand toate thread-urile gestionate de barieră au ajuns la punctul unde se află bariera.

În OpenMP, pentru barieră avem directiva `#pragma omp barrier`, echivalent cu `pthread_barrier_t` din pthreads.

Exemplu de folosire:
```c
#include <stdio.h>
#include <omp.h>

int main (int argc, char** argv) {
    #pragma omp parallel 
    {
        printf("First print by %d\n", omp_get_thread_num());
        #pragma omp barrier
        printf("Second print by %d\n", omp_get_thread_num());
    }
    
    return 0;
}
```

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

## Resurse utile
- http://jakascorner.com/blog/
- https://ppc.cs.aalto.fi/