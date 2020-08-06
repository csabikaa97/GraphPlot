//
//  RGBclass.h
//  GraphPlot
//
//  Created by Csaba on 2020. 06. 04..
//  Copyright © 2020. Csaba. All rights reserved.
//

#ifndef RGBclass_h
#define RGBclass_h

class RGB {
public:
    int R, G, B;
    RGB(int R, int G, int B) {
        this->R = R;
        this->G = G;
        this->B = B;
    }
    RGB(int RGB) {
        R = RGB; G = RGB; B = RGB;
    }
    RGB() { R=0; G=0; B=0; }
    RGB real() {
        return *this;
    }
    friend RGB operator/(RGB szin, int oszto);
    friend bool operator==(RGB szin1, RGB szin2);
    friend bool operator!=(RGB szin1, RGB szin2);
};

RGB operator/(RGB szin, int oszto) {
    RGB a(szin.R/oszto, szin.G/oszto, szin.B/oszto);
    return a;
}

RGB operator+(RGB szin, RGB hozzaad) {
    RGB a(szin.R + hozzaad.R, szin.G + hozzaad.G, szin.B + hozzaad.B);
    return a;
}

bool operator==(RGB szin1, RGB szin2) {
    return szin1.R == szin2.R && szin1.G == szin2.G && szin1.B == szin2.B;
}

bool operator!=(RGB szin1, RGB szin2) {
    return szin1.R != szin2.R || szin1.G != szin2.G || szin1.B != szin2.B;
}

#endif /* RGBclass_h */
