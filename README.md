Graphplot - Függvényábrázoló program

A "main.cpp" fájl elején található "FuggvenyErtek" nevű függvényben található switch helyes használatával lehet használni a programot. A switch-ben használható az 'x' változó amely a vízszintes tengely értékére utal. 

Helyes használat példa:

//    switch (fuggvenyazonosito) {
//        case 0: y = x; break;
//        case 1: y = x*x; break;
//        case 2: y = x*x; break;
//        default: throw 1;
//    }

A switchben a 0. esettől kell definiálni a függvényeket. A különböző esetek között egy lépésnek kell lennie, különben onnanstól kezdve ahol nem egy a lépés, onnanstól nem ábrázolja a többi függvényt.

A következő esetnek mindenféleképpen benne kell maradnia a switchben: "default: throw 1;"

