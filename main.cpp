//
//  main.cpp
//  GraphPlot
//
//  Created by Csaba on 2019. 09. 03..
//  Copyright © 2019. Csaba. All rights reserved.
//

#include <cstdlib>
#include <math.h>
#include <unistd.h>
#include <fstream>
#include <thread>
#include <iostream>
#include <sstream>
#include "Timer.h"
#include "RGB.h"
#define Pi 3.1415926535897932384626433832795028841

using namespace std;

// Program beállításai
string AlapertelmezettFajlnev = "fuggveny.ppm";     // lényegtelen
int Meret = 1000;                                   // lehetoleg legyen 10-el oszthato
int Nagyitas = 100;                                 // ez is
int Szinmelyseg = 1000;                             // [1..(+akármennyi)]
RGB RacsSzin(Szinmelyseg/2);                        // [0..(színmélység)]
RGB alapSzin(0);
int FutasiSzalakSzama = 10;                         // [1..(+akármennyi)], lehetoleg ez is 10-el oszthato legyen

// Program által használt változók
int MaxFuggvenyekSzamaInt = -1;
int Xkozep=(Meret/2);
int Ykozep=(Meret/2);
char** SzalBufferek;
int* SzalBufferekKarakterSzama;
bool* SzalStatuszok;
RGB** Pixelek;
RGB* FuggvenyekSzine;


//Függvények definíciója   Pl: y = x / 2
int FuggvenyErtek(int helyix, int fuggvenyazonosito) {
    double y, x = double(helyix) / double(Nagyitas);
    
//    switch (fuggvenyazonosito) {
//        case 0: y = x; break;
//        case 1: y = x*x; break;
//        case 2: y = x*x*x; break;
//        case 3: y = x*x*x*x; break;
//        case 4: y = 1; break;
//        case 5: y = sqrt(x); break;
//        case 6: y = sin(x); break;
//        case 7: y = cos(x); break;
//        case 8: y = tan(x); break;
//        default: throw 1;
//    }
    
    if( fuggvenyazonosito == 0 ) {
        if( x < 0.0 ) {
            y = x / 10;
        } else {
            y = x;
        }
    } else {
        throw 1;
    }
    
    /* SIN szivárvány
    if( fuggvenyazonosito > 200 ) {
        throw 1;
    }
    
    y = sin(x/200)*100 + fuggvenyazonosito; */

    return int(round( ( -1.0 * (y) ) * double(Nagyitas) ));
}


int FuggvenyekSzama() {
    if( MaxFuggvenyekSzamaInt != -1 ) {
        return MaxFuggvenyekSzamaInt;
    }
    
    int i = 0;
    try {
        while( true ) {
            FuggvenyErtek(0, i);
            i = i + 1;
        }
    } catch(int hiba) {
        MaxFuggvenyekSzamaInt = i;
        return i;
    }
}

string ByteokSzamaStringeAlakitas(unsigned long int Byte) {
    stringstream buffer;
    buffer.str("");
    
    if(Byte>1073741824) {
        buffer<<double(double(Byte)/1073741824.0)<<" GiB";
    } else {
        if(Byte>1048576) {
            buffer<<double(double(Byte)/1048576.0)<<" MiB";
        } else {
            if(Byte>1024) {
                buffer<<Byte/1024<<" KiB";
            } else {
                if(Byte<=1024) {
                    buffer<<Byte<<" B";
                }
            }
        }
    }
    
    return buffer.str();
}

RGB GetColor(int fuggvenyazonosito) {
    if( FuggvenyekSzine[fuggvenyazonosito] != RGB(-1) ) {
        return FuggvenyekSzine[fuggvenyazonosito];
    }
    
    RGB visszateresiErtek = RGB(0);
    
    double ratio = double(fuggvenyazonosito) / double(FuggvenyekSzama()-1);
    if( FuggvenyekSzama() == 1 ) {
        ratio = 0.0;
    }
    
    if( ratio >= 0.0 && ratio < 0.25 ) {
        visszateresiErtek.R = Szinmelyseg;
        visszateresiErtek.G = sin( (ratio-0.0) * 2 * Pi) * Szinmelyseg;
    }
    if( ratio >= 0.25 && ratio < 0.5 ) {
        visszateresiErtek.R = cos( (ratio-0.25) * 2 * Pi) * Szinmelyseg;
        visszateresiErtek.G = Szinmelyseg;
    }
    if( ratio >= 0.5 && ratio < 0.75 ) {
        visszateresiErtek.G = Szinmelyseg;
        visszateresiErtek.B = sin( (ratio-0.5) * 2 * Pi) * Szinmelyseg;;
    }
    if( ratio >= 0.75 && ratio <= 1.0 ) {
        visszateresiErtek.G = cos( (ratio-0.75) * 2 * Pi) * Szinmelyseg;
        visszateresiErtek.B = Szinmelyseg;
    }
    
    FuggvenyekSzine[fuggvenyazonosito] = visszateresiErtek;
    return visszateresiErtek;
}

void Szamitas() {
    FuggvenyekSzine = new RGB[FuggvenyekSzama()];
    for(int i=0; i<FuggvenyekSzama(); i++) {
        FuggvenyekSzine[i] = RGB(-1);
    }
    
    //Függvény színének felvetése víszintes irányban ( x tengely ) haladva + interpoláció
    for(int f=0; f<FuggvenyekSzama(); f++) {
        RGB mostaniSzin = GetColor(f);
        int* kezdopontok = new int[Meret];
        int* vegzopontok = new int[Meret];
        
        for(int x=0; x<Meret; x++) {
            kezdopontok[x] = FuggvenyErtek(x-Xkozep, f) + Ykozep;
        }
        for(int x=1; x<Meret; x++) {
            vegzopontok[x-1] = kezdopontok[x];
        }
        vegzopontok[Meret-1] = FuggvenyErtek(Xkozep+1, f) + Ykozep;
        
        for(int x=0; x<Meret; x++) {
            if( abs( vegzopontok[x] - kezdopontok[x] ) < Meret ) {
                if( kezdopontok[x] < vegzopontok[x] ) {
                    for(int y=kezdopontok[x]; y<vegzopontok[x]; y++) {
                        if( y >= 0 && y <= Meret ) {
                            if( Pixelek[x][y] != alapSzin ) {
                                Pixelek[x][y] = (Pixelek[x][y] + mostaniSzin) / 2;
                            } else {
                                Pixelek[x][y] = mostaniSzin;
                            }
                        }
                    }
                }
                if( kezdopontok[x] > vegzopontok[x] ) {
                    for(int y=vegzopontok[x]; y<kezdopontok[x]; y++) {
                        if( y >= 0 && y <= Meret ) {
                            if( Pixelek[x][y] != alapSzin ) {
                                Pixelek[x][y] = (Pixelek[x][y] + mostaniSzin) / 2;
                            } else {
                                Pixelek[x][y] = mostaniSzin;
                            }
                        }
                    }
                }
                if( kezdopontok[x] == vegzopontok[x] ) {
                    if( kezdopontok[x] >= 0 && kezdopontok[x] <= Meret ) {
                        if( Pixelek[x][kezdopontok[x]] != alapSzin ) {
                            Pixelek[x][kezdopontok[x]] = (Pixelek[x][kezdopontok[x]] + mostaniSzin) / 2;
                        } else {
                            Pixelek[x][kezdopontok[x]] = mostaniSzin;
                        }
                    }
                }
            }
        }
        
        delete[] kezdopontok;
        delete[] vegzopontok;
    }
    
    //Halvány rács felvétele ( 0.1 egységenként és 1 egységenként )
    for(int x_y=-(Meret/2); x_y<(Meret/2); x_y++) {
        if(Nagyitas >= 10) {
            if(Nagyitas >= 100) {
                if( x_y % (Nagyitas/10) == 0 ) {
                    for(int x=-(Meret/2)+1; x<(Meret/2); x++) {
                        if( Pixelek[x_y+Meret/2][x+Meret/2] == alapSzin ) {
                            Pixelek[x_y+Meret/2][x+Meret/2] = RacsSzin/4;
                        }
                        if( Pixelek[x+Meret/2][x_y+Meret/2] == alapSzin ) {
                            Pixelek[x+Meret/2][x_y+Meret/2] = RacsSzin/4;
                        }
                    }
                }
            }
        }
    }
    for(int x_y=-(Meret/2); x_y<(Meret/2); x_y++) {
        if(Nagyitas >= 10) {
            if( x_y % (Nagyitas) == 0 ) {
                for(int x=-(Meret/2)+1; x<(Meret/2); x++) {
                    if( Pixelek[x_y+Meret/2][x+Meret/2] == alapSzin ) {
                        Pixelek[x_y+Meret/2][x+Meret/2]=RacsSzin/2;
                    }
                    if( Pixelek[x+Meret/2][x_y+Meret/2] == alapSzin ) {
                        Pixelek[x+Meret/2][x_y+Meret/2]=RacsSzin/2;
                    }
                }
            }
        }
    }
    
    //Alaptengelyek felvetése
    for(int x_y=0; x_y<Meret; x_y++) {
        if( Pixelek[x_y][Xkozep] == alapSzin || Pixelek[x_y][Xkozep] == RacsSzin/2 || Pixelek[x_y][Xkozep] == RacsSzin/4 ) {
            Pixelek[x_y][Xkozep]=RacsSzin;
        }
        if( Pixelek[Ykozep][x_y] == alapSzin || Pixelek[Ykozep][x_y] == RacsSzin/2 || Pixelek[Ykozep][x_y] == RacsSzin/4 ) {
            Pixelek[Ykozep][x_y]=RacsSzin;
        }
    }
    
    delete FuggvenyekSzine;
}

void FajlbairasSzalMunka(int SzalSzama) {
    for(int y=0 + SzalSzama*(Meret / FutasiSzalakSzama); y<(Meret / FutasiSzalakSzama) + SzalSzama*(Meret / FutasiSzalakSzama); y++) {
        for(int x=0; x<Meret; x++) {
            char* atmenetiTomb = new char[20];
            sprintf(atmenetiTomb, "%d %d %d\n", Pixelek[x][y].R, Pixelek[x][y].G, Pixelek[x][y].B);
            for(int i=0; i<20; i++) {
                if( atmenetiTomb[i] == 0 ) {
                    break;
                } else {
                    SzalBufferek[SzalSzama][SzalBufferekKarakterSzama[SzalSzama]++] = atmenetiTomb[i];
                }
            }
            delete[] atmenetiTomb;
        }
    }
    
    SzalStatuszok[SzalSzama] = true;
}

void Fajlbairas() {
    printf("Adatok írása \"%s\" nevű fájlba...\n", AlapertelmezettFajlnev.c_str());
    
    SzalStatuszok = new bool[FutasiSzalakSzama];
    SzalBufferek = new char*[FutasiSzalakSzama];
    for(int i=0; i<FutasiSzalakSzama; i++) {
        SzalBufferek[i] = new char[Meret*Meret*12];
    }
    
    SzalBufferekKarakterSzama = new int[FutasiSzalakSzama];
    for(int i=0; i<FutasiSzalakSzama; i++) {
        SzalBufferekKarakterSzama[i] = 0;
    }
    
    for(int t=0; t<FutasiSzalakSzama; t++) {
        SzalStatuszok[t] = false;
        thread Szal(FajlbairasSzalMunka, t);
        Szal.detach();
    }
    
    while(true) {
        usleep(20);
        int szamlalo = 0;
        for(int i=0; i<FutasiSzalakSzama; i++) {
            szamlalo = szamlalo + SzalStatuszok[i];
        }
        if( szamlalo == FutasiSzalakSzama ) {
            delete SzalStatuszok;
            break;
        }
    }

    ofstream *FileFolyam = new ofstream;
    FileFolyam->open(AlapertelmezettFajlnev.c_str());
    *FileFolyam<<"P3\n"<<Meret<<" "<<Meret<<"\n";
    *FileFolyam<<Szinmelyseg<<"\n";
    for(int t=0; t<FutasiSzalakSzama; t++) {
        *FileFolyam << SzalBufferek[t];
    }
    
    for(int i=0; i<FutasiSzalakSzama; i++) {
        delete SzalBufferek[i];
    }
    delete SzalBufferek;
    
    FileFolyam->close();
    delete FileFolyam;
    
    int meret = 0;
    for(int t=0; t<FutasiSzalakSzama; t++) {
        meret = meret + SzalBufferekKarakterSzama[t];
    }
    delete SzalBufferekKarakterSzama;
    
    printf("Adatok írása sikeres: \"%s\" - %s\n", AlapertelmezettFajlnev.c_str(), (ByteokSzamaStringeAlakitas(meret)).c_str());
    string Parancs = "open " + AlapertelmezettFajlnev;
    printf("Fájl megnyitása az \"open\" paranccsal...\n");
    system(Parancs.c_str());
}

int main() {
    Pixelek = new RGB*[Meret];
    for(int x=0; x<Meret; x++) {
        Pixelek[x] = new RGB[Meret];
        for(int y=0; y<Meret; y++) {
            Pixelek[x][y] = alapSzin;
        }
    }
    
    Szamitas();
    Fajlbairas();
    
    for(int i=0; i<Meret; i++) {
        delete Pixelek[i];
    }
    delete Pixelek;
    return 0;
}

