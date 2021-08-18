/***************************************************************************
 *   This file is part of the Lime Report project                          *
 *   Copyright (C) 2021 by Alexander Arin                                  *
 *   arin_a@bk.ru                                                          *
 *                                                                         *
 **                   GNU General Public License Usage                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 **                  GNU Lesser General Public License                    **
 *                                                                         *
 *   This library is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation, either version 3 of the    *
 *   License, or (at your option) any later version.                       *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library.                                      *
 *   If not, see <http://www.gnu.org/licenses/>.                           *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 ****************************************************************************/
#include "lrsimplecrypt.h"

namespace LimeReport {

#if defined(LP64) || defined(_LP64) || defined(__LP64__)
typedef unsigned  int WORD; /* Should be 32-bit = 4 bytes        */
#else
typedef unsigned long int WORD; /* Should be 32-bit = 4 bytes        */
#endif

const int w = 32;             /* word size in bits                 */
const int r = 12;             /* number of rounds                  */
const int b = 16;             /* number of bytes in key            */
const int c = 4;              /* number  words in key = ceil(8*b/w)*/
const int t = 26;             /* size of table S = 2*(r+1) words   */

const char* passPhrase = "HjccbzHjlbyfCkjy";

WORD P = 0xb7e15163, Q = 0x9e3779b9;

#define ROTL(x,y) (((x)<<(y&(w-1))) | ((x)>>(w-(y&(w-1)))))
#define ROTR(x,y) (((x)>>(y&(w-1))) | ((x)<<(w-(y&(w-1)))))

union WTB{
   WORD word[2];
   char bytes[8];
};

void initPt(WTB& pt, QByteArray::Iterator* it, QByteArray::Iterator end){
    for (int i = 0; i<8; i++){
        if (*it != end){
            pt.bytes[i]=**it;
            ++*it;
        } else break;
    }
}

class ChipperPrivate{
    friend class Chipper;
public:
    ChipperPrivate():m_prepared(false){}
    bool isPrepared(){ return m_prepared;}
private:
    void RC5_SETUP(const char *K);
    void RC5_ENCRYPT(WORD *pt, WORD *ct);
    void RC5_DECRYPT(WORD *ct, WORD *pt);
    WORD S[26] = {0};
    bool m_prepared;
};

void ChipperPrivate::RC5_SETUP(const char *K)
{
    WORD i, j, k, u=w/8, A, B, L[c];
    for (i=b,L[c-1]=0; i!=0; i--)
        L[(i-1)/u] = (L[(i-1)/u]<<8)+K[i-1];
    for (S[0]=P,i=1; i<t; i++)
        S[i] = S[i-1]+Q;
    for (A=B=i=j=k=0; k<3*t; k++,i=(i+1)%t,j=(j+1)%c){   /* 3*t > 3*c */
        A = S[i] = ROTL(S[i]+(A+B),3);
        B = L[j] = ROTL(L[j]+(A+B),(A+B));
    }
    m_prepared = true;
}

void ChipperPrivate::RC5_ENCRYPT(WORD *pt, WORD *ct)
{
    WORD i, A=pt[0]+S[0], B=pt[1]+S[1];
    for (i=1; i<=r; i++)
      { A = ROTL(A^B,B)+S[2*i];
        B = ROTL(B^A,A)+S[2*i+1];
      }
    ct[0] = A; ct[1] = B;
}

void ChipperPrivate::RC5_DECRYPT(WORD *ct, WORD *pt)
{
    WORD i, B=ct[1], A=ct[0];
    for (i=r; i>0; i--)
      { B = ROTR(B-S[2*i+1],A)^A;
        A = ROTR(A-S[2*i],B)^B;
      }
    pt[1] = B-S[1]; pt[0] = A-S[0];
}

QByteArray Chipper::cryptString(QString value)
{
    QByteArray buff;
    QByteArray result;
    buff += value.toUtf8();
    WTB pt, ct, prior;

    if (!d->isPrepared())
        d->RC5_SETUP(passPhrase);

    prior.word[0]=0;
    prior.word[1]=0;

    QByteArray::Iterator it = buff.begin();
    while (it!=buff.end()){

        pt.word[0] = 0;
        pt.word[1] = 0;

        initPt(pt,&it,buff.end());

        pt.word[0] = pt.word[0] ^ prior.word[0];
        pt.word[1] = pt.word[1] ^ prior.word[1];

        d->RC5_ENCRYPT(pt.word,ct.word);

        prior.word[0] = pt.word[0];
        prior.word[1] = pt.word[1];

        for (int i=0;i<8;i++){
            result += ct.bytes[i];
        }
    }
    return result;
}

QString Chipper::decryptByteArray(QByteArray value)
{
    QByteArray result;
    WTB pt, ct, prior;

    if (!d->isPrepared())
        d->RC5_SETUP(passPhrase);
    prior.word[0] = 0;
    prior.word[1] = 0;

    QByteArray::Iterator it = value.begin();
    while (it!=value.end()){

        pt.word[0] = 0;
        pt.word[1] = 0;

        initPt(pt,&it,value.end());

        d->RC5_DECRYPT(pt.word,ct.word);

        ct.word[0] = ct.word[0] ^ prior.word[0];
        ct.word[1] = ct.word[1] ^ prior.word[1];

        prior.word[0] = ct.word[0];
        prior.word[1] = ct.word[1];

        for (int i=0;i<8;i++){
            result += ct.bytes[i];
        }

    }
    return QString(result);
}

Chipper::Chipper(): d(new ChipperPrivate()){}

Chipper::Chipper(QString passphrase): d(new ChipperPrivate())
{
    if (!passphrase.trimmed().isEmpty()){
        passphrase = passphrase.leftJustified(16,'#');
        d->RC5_SETUP(passphrase.toLatin1().data());
    }
}

Chipper::~Chipper()
{
    delete d;
}

}

