#include <iostream>
#include <fstream>
#include "math.h"

using namespace std;

void wypisz(double** A, double* b, unsigned matrix_size)
{
    cout<<endl;
    for(int i=0; i<matrix_size; i++)
    {
        for(int j=0; j<matrix_size; j++)
        {
            cout<<A[i][j]<<" ";
        }
        cout << "|" << b[i];
        cout<<endl;
    }
    cout<<endl;
}

void sortujWyniki(double* wyniki, int* historiaZmian, unsigned matrix_size)
{
    for( int i = 0; i < matrix_size; i++ )
    {
        for( int j = 0; j < matrix_size -i - 1; j++ )
        {
            if( historiaZmian[j] > historiaZmian[j+1] )
            {
                swap(historiaZmian[j], historiaZmian[j+1] );
                swap(wyniki[j], wyniki[j+1] );
            }
        }
    }
}

void zamienMax(double** A, double* b, unsigned matrix_size,int start, int* historiaZmian)
{
    int maxX = start;
    int maxY = start;
    double maks=abs(A[start][start]);
    for(int y=start; y<matrix_size; y++)
    {
        for(int x=start; x<matrix_size; x++)
        {
            if(maks<abs(A[y][x]))
            {
                maxY = y;
                maxX = x;
                maks=abs(A[y][x]);
            }
        }
    }

    for(int i=0; i<matrix_size; i++)
    {
        swap(A[start][i],A[maxY][i]);
    }
    swap(b[start],b[maxY]);

    for(int i=0; i<matrix_size; i++)
    {
        swap(A[i][start],A[i][maxX]);
    }
    swap(historiaZmian[start],historiaZmian[maxX]);

}

void zamienMaxWWierszu(double** A, double* b, unsigned matrix_size,int start, int* historiaZmian)
{
    int maxX = start;
    double maks=abs(A[start][start]);

    for(int x=start; x<matrix_size; x++)
    {
        if(maks<abs(A[start][x]))
        {

            maxX = x;
            maks=abs(A[start][x]);
        }
    }

    for(int i=0; i<matrix_size; i++)
    {
        swap(A[i][start],A[i][maxX]);
    }
    swap(historiaZmian[start],historiaZmian[maxX]);
}

void zamienMaxWKolumnie(double** A, double* b, unsigned matrix_size,int start)
{
    int maxY = start;
    double maks=abs(A[start][start]);
    for(int y=start; y<matrix_size; y++)
    {
        if(maks<abs(A[y][start]))
        {
            maxY = y;

            maks=abs(A[y][start]);
        }
    }
    // cout << "max =  " << maks << endl;
    for(int i=0; i<matrix_size; i++)
    {
        swap(A[start][i],A[maxY][i]);
    }
    swap(b[start],b[maxY]);
}

double* metodaEleiminacjiGausaBezWyboru(double** A, double* b, unsigned matrix_size)
{
    double* wyniki = new double[matrix_size];
    double elementRef;
    double epsilon = 0.0000001;
    for(int i=0; i<matrix_size-1; i++)
    {
        elementRef = A[i][i];
        if(abs(elementRef) < epsilon)
        {
            cout << "element referencyjny = 0 - nie mozna dokonac dalszych obliczen"<<endl;

            delete[] wyniki;
            return NULL;
        }
        ///wyzeruwujemy wszystko pod elementem referencyjnym
        for(int y=i+1; y<matrix_size; y++)
        {
            double p = A[y][i]/elementRef;
            for(int x=0; x<matrix_size; x++)
            {
                A[y][x]=A[y][x]-(p*A[i][x]);

            }
            b[y]=b[y]-(p*b[i]);
            //wypisz(A, b, matrix_size);
        }

    }

    // wypisz(A, b, matrix_size);

    for(int i=matrix_size-1; i>=0; i--)
    {
        double suma = 0;
        for(int j = i+1; j<matrix_size; j++)
        {
            suma += A[i][j]*wyniki[j];
        }

        wyniki[i] = (b[i]-suma)/A[i][i];
    }

    return wyniki;
}

double* metodaEleiminacjiGausaPelnyWybor(double** A, double* b, unsigned matrix_size)
{
    double* wyniki = new double[matrix_size];
    int* historiaZmian = new int[matrix_size];
    for(int i=0; i<matrix_size; i++)
    {
        historiaZmian[i] = i;
    }

    double elementRef;
    double epsilon = 0.0000001;
    for(int i=0; i<matrix_size-1; i++)
    {
        ///dokonujemy zamiany - maksymalny element na poczatek
        int start = i;
        zamienMax(A, b, matrix_size,start, historiaZmian);

        elementRef = A[i][i];
        if(abs(elementRef) < epsilon)
        {
            cout << "element referencyjny = 0 - nie mozna dokonac dalszych obliczen";

            delete[] wyniki;
            return NULL;
        }
        ///wyzerowujemy wszystko pod elementem referencyjnym
        for(int y=i+1; y<matrix_size; y++)
        {
            double p = A[y][i]/elementRef;
            for(int x=0; x<matrix_size; x++)
            {
                A[y][x]=A[y][x]-(p*A[i][x]);

            }
            b[y]=b[y]-(p*b[i]);
            //wypisz(A, b, matrix_size);
        }
        //  cout << "po zerowaniu pod " << i << endl;
        // wypisz(A, b, matrix_size);

    }

    //wypisz(A, b, matrix_size);

    for(int i=matrix_size-1; i>=0; i--)
    {
        double suma = 0;
        for(int j = i+1; j<matrix_size; j++)
        {
            suma += A[i][j]*wyniki[j];
        }

        wyniki[i] = (b[i]-suma)/A[i][i];
    }

    ///zamieniamy wyniki na dobra kolejnosc
    sortujWyniki(wyniki, historiaZmian, matrix_size);

    return wyniki;
}

double* metodaEleiminacjiGausaWyborWWierszu(double** A, double* b, unsigned matrix_size)
{
    double* wyniki = new double[matrix_size];
    int* historiaZmian = new int[matrix_size];
    for(int i=0; i<matrix_size; i++)
    {
        historiaZmian[i] = i;
    }

    double elementRef;
    double epsilon = 0.0000001;
    for(int i=0; i<matrix_size-1; i++)
    {
        ///dokonujemy zamiany - maksymalny element na poczatek
        int start = i;
        zamienMaxWWierszu(A, b, matrix_size,start, historiaZmian);

        elementRef = A[i][i];
        if(abs(elementRef) < epsilon)
        {
            cout << "element referencyjny = 0 - nie mozna dokonac dalszych obliczen";

            delete[] wyniki;
            return NULL;
        }
        ///zerujemy wszystko pod elementem referencyjnym
        for(int y=i+1; y<matrix_size; y++)
        {
            double p = A[y][i]/elementRef;
            for(int x=0; x<matrix_size; x++)
            {
                A[y][x]=A[y][x]-(p*A[i][x]);

            }
            b[y]=b[y]-(p*b[i]);
            //wypisz(A, b, matrix_size);
        }
        // cout << "po zerowaniu pod " << i << endl;
        // wypisz(A, b, matrix_size);

    }

    // wypisz(A, b, matrix_size);

    for(int i=matrix_size-1; i>=0; i--)
    {
        double suma = 0;
        for(int j = i+1; j<matrix_size; j++)
        {
            suma += A[i][j]*wyniki[j];
        }

        wyniki[i] = (b[i]-suma)/A[i][i];
    }

    ///zamieniamy wyniki na dobra kolejnosc
    sortujWyniki(wyniki, historiaZmian, matrix_size);
    return wyniki;
}

double* metodaEleiminacjiGausaWyborWKolumnie(double** A, double* b, unsigned matrix_size)
{
    double* wyniki = new double[matrix_size];
    //wypisz(A, b, matrix_size);
    double elementRef;
    double epsilon = 0.0000001;
    for(int i=0; i<matrix_size-1; i++)
    {
        ///dokonujemy zamiany - maksymalny element na poczatek
        int start = i;
        //cout << "start: " << start;
        zamienMaxWKolumnie(A, b, matrix_size,start);

        // cout << "po zamianie" << endl;
        // wypisz(A, b, matrix_size);
        elementRef = A[i][i];
        if(abs(elementRef) < epsilon)
        {
            cout << "element referencyjny = 0 - nie mozna dokonac dalszych obliczen";

            delete[] wyniki;
            return NULL;
        }
        ///wyzeruwujemy wszystko pod elementem referencyjnym
        for(int y=i+1; y<matrix_size; y++)
        {
            double p = A[y][i]/elementRef;
            for(int x=0; x<matrix_size; x++)
            {
                A[y][x]=A[y][x]-(p*A[i][x]);

            }
            b[y]=b[y]-(p*b[i]);
            //wypisz(A, b, matrix_size);
        }
        // cout << "po wyzerowaniu:" << endl;
        //  wypisz(A, b, matrix_size);
        //cout << "po zerowaniu pod " << i << endl;


    }

    //wypisz(A, b, matrix_size);

    for(int i=matrix_size-1; i>=0; i--)
    {
        double suma = 0;
        for(int j = i+1; j<matrix_size; j++)
        {
            suma += A[i][j]*wyniki[j];
        }

        wyniki[i] = (b[i]-suma)/A[i][i];
    }

    return wyniki;
}

void wypiszISkasujWyniki(double* wyniki, int matrix_size)
{
    if(wyniki!=NULL)
    {
        for(int i=0; i<matrix_size; i++)
        {
            cout << "x"<<(i+1)<< " = " << wyniki[i] << endl;
        }
        delete wyniki;
    }
}

struct Dane
{
    double** A;
    double* b;
    unsigned matrix_size;
};

bool Wczytaj(struct Dane* dane, string nazwaPliku)
{
    ifstream source_file(nazwaPliku);
    if (!source_file.is_open())
    {
        cout <<"The file has not been open!"<<endl;
        return false;
    }
    source_file >> dane->matrix_size;

    dane->A = new double*[dane->matrix_size];
    dane->A[0] = new double[dane->matrix_size*dane->matrix_size];
    for(unsigned i = 1; i< dane->matrix_size; i++)
        dane->A[i] = dane->A[i-1] + dane->matrix_size;

    dane->b = new double[dane->matrix_size];

    char semicolumn;
    for (unsigned i = 0; i < dane->matrix_size+1; i++)
        source_file >> semicolumn;

    for (unsigned i = 0; i < dane->matrix_size; i++)
    {
        for (unsigned j = 0; j < dane->matrix_size; j++)
        {
            source_file >> dane->A[i][j];
            source_file >> semicolumn;
        }
        source_file >> semicolumn;
        source_file >> dane->b[i];
    }
    source_file.close();
}

void zadanie1(string nazwaPliku)
{
    cout << "metodaEleiminacjiGausaBezWyboru" << endl;
    struct Dane dane;
    Wczytaj(&dane, nazwaPliku);

    double* wyniki = metodaEleiminacjiGausaBezWyboru( dane.A,dane.b,dane.matrix_size);
    wypiszISkasujWyniki(wyniki, dane.matrix_size);
    delete [] dane.b;
    delete [] dane.A[0];
    delete [] dane.A;
}

void zadanie2(string nazwaPliku)
{
    cout << "metodaEleiminacjiGausaWyborWWierszu" << endl;
    struct Dane dane;
    Wczytaj(&dane, nazwaPliku);

    double* wyniki = metodaEleiminacjiGausaWyborWWierszu( dane.A,dane.b,dane.matrix_size);
    wypiszISkasujWyniki(wyniki, dane.matrix_size);
    delete [] dane.b;
    delete [] dane.A[0];
    delete [] dane.A;
}

void zadanie3(string nazwaPliku)
{
    cout << "metodaEleiminacjiGausaWyborWKolumnie" << endl;
    struct Dane dane;
    Wczytaj(&dane, nazwaPliku);

    double* wyniki = metodaEleiminacjiGausaWyborWKolumnie( dane.A,dane.b,dane.matrix_size);
    wypiszISkasujWyniki(wyniki, dane.matrix_size);
    delete [] dane.b;
    delete [] dane.A[0];
    delete [] dane.A;
}

void zadanie4(string nazwaPliku)
{
    cout << "metodaEleiminacjiGausaPelnyWybor" << endl;
    struct Dane dane;
    Wczytaj(&dane, nazwaPliku);

    double* wyniki = metodaEleiminacjiGausaPelnyWybor( dane.A,dane.b,dane.matrix_size);
    wypiszISkasujWyniki(wyniki, dane.matrix_size);
    delete [] dane.b;
    delete [] dane.A[0];
    delete [] dane.A;
}

int main()
{

    zadanie1("plik1.csv");
    zadanie2("plik2.csv");
    zadanie3("plik3.csv");
    zadanie4("plik4.csv");


}
