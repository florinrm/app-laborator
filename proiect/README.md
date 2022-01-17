# Despre proiect
## Indicații
- la primul laborator să aveți cel puțin codul serial al proiectului (ca punct de start)
- să vă folosiți de wiki pe GitLab (nu de README, în README puteți scrie legat de setup-ul proiectului, de exemplu script-uri, comenzi de setup, etc.), unde veți scrie în fiecare săptămână ce ați făcut: la ce a lucrat fiecare, ce a făcut fiecare, ce rezultate ați obținut (timpi, speed-up), să puneți grafice
- la partea de grafice e de recomandat să faceți în felul următor:
    - pentru fiecare versiune (pthreads, OpenMP, MPI, etc.), să faceți grafic unde pe axa OX să aveți numărul de thread-uri / procese, dar pe axa OY să aveți timpii de execuție
    - la final să aveți un grafic comparativ pentru toate versiunile (acest fapt este obligatoriu, încât să se vadă ce versiune este cea mai optimă ca speed-up)
- să puneți screenshots cu analiza performanței făcută cu tool-urile de profiling (Solaris Studio, VTune, etc.) pe wiki pe GitLab și la prezentarea finală
- în fiecare săptămână trebuie să lucreze fiecare din echipă la partea sa (de exemplu cineva face la OpenMP - implementare și grafice), încât să treceți pe wiki pe GitLab ce a făcut fiecare persoană în fiecare săptămână
- e de recomandat să faceți optimizări și / sau chestii bonus - exemple: OpenMP Tasks, CUDA, TBB, C++ threads, hibrid (MPI + pthreads, MPI + OpenMP, MPI + C++ threads, MPI + TBB), pe care puteți lua punctaje bonus (pentru acestea aveți materiale pe repository, merită încercat)

Exemple de proiecte cu documentație bună:
- https://gitlab.cs.pub.ro/APP-2020/OmegaNetwork
- https://gitlab.cs.pub.ro/APP-2020/Team_14-341C1
- https://gitlab.cs.pub.ro/APP-2020/Team_2_LCS
- https://gitlab.cs.pub.ro/APP-2020/image-resize
- https://gitlab.cs.pub.ro/APP-2020/Team10-Image_Enlarger
- https://gitlab.cs.pub.ro/APP-2020/341C3_Edge_Detection
- https://gitlab.cs.pub.ro/APP-2020/343C1_Vigenere_cipher
- https://gitlab.cs.pub.ro/APP-2020/smoothen-images
- https://gitlab.cs.pub.ro/APP-2020/FFT
- https://gitlab.cs.pub.ro/APP-2020/HER-FCNN-ADAM
- https://gitlab.cs.pub.ro/APP-2020/fractals

[Link demo Solaris](https://youtu.be/_8v8YI1-a-0)

## Prezentarea proiectului
Fiecare echipă va prezenta proiectul în aproximativ 10 minute, folosind slides, unde vor expune:
- ideea proiectului și ce presupune proiectul lor (adică ce face)
- ce paralelizări au făcut și cum au făcut paralelizările
- rezultatele obținute la paralelizări și observații pe seama rezultatelor obținute (grafice, profiling)
- compararea paralelizărilor (grafice comparative, observații)
- concluzii

Documentația de pe GitLab (care trebuie să fie pe wiki) trebuie să cuprindă aspectele afișate în cadrul prezentării, în detaliu (rezultate, grafice, profiling, observații).

Aici aveți exemple de prezentări de anul trecut.

După prezentare, fiecare membru din cadrul echipei va primi trei întrebări din materia de laborator.