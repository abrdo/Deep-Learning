#include "graphics.hpp"
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <cmath>
#include <ctime>
#include <sstream>

using namespace genv;
using namespace std;
const int XX = 1200;
const int YY = 1000;
const int szamkivalasztasi_trashold = 90; //120
const string file_pathway = "/Users/Medvoz/Documents/kepkonv/t10k-images-bmp/t10k-images"; // '/'-el végződik
const int hf = 8; //háttérszínkeresés finomsága (256 osztója jegyen)
const int kepmeret = 10;
const string inputfile_name = "vastag.bmp.kep";

string to_str(int n){
    stringstream ss;
    string sn;
    ss<<n;
    ss>>sn;
    return sn;
}

void tr() {
    gout << move_to(0,0) << color(0,0,0) << box(XX,YY);
}

struct Pixel{
private:
	float x, y;
	int r;
	int g;
	int b;
	bool mark;

public:
	Pixel() : r(255), g(255), b(255) {mark = false;}
	//Pixel(int _r, int _g, int _b) : r(_r), g(_g), b(_b)/*, mark(false)*/{}
	Pixel(int _r, int _g, int _b, float _x, float _y) : r(_r), g(_g), b(_b), x(_x), y(_y) {mark = false;}

	float getX() {return x;}
	float getY() {return y;}
	void setX(float _x) {x = _x;}
	void setY(float _y) {y = _y;}

	int getR() {return r;}
	int getG() {return g;}
	int getB() {return b;}
    void setR(int _r) {r = _r;}
	void setG(int _g) {g = _g;}
	void setB(int _b) {b = _b;}

	bool isMarked() {return mark;}
	void setMark(bool _mark) {mark = _mark;}

	void kirajzol(canvas& c, float _X, float _Y) {
        c.transparent( r == 255 && g == 255 && b == 255 );
        if ((int)x+(int)_X < XX && (int)y+(int)_Y < YY){
            c << move_to((int)x+(int)_X,(int)y+(int)_Y) << color(r,g,b) << dot;
        }
	}
};



void beolvaso(vector<vector<Pixel>> &v, string filename) {
    ifstream fin(filename); if(!fin.good()) cerr<<"No such file! (a beolvaso fvben)"<<endl;
    unsigned int XX, YY;
    fin >> XX >> YY;
    v.resize(YY);
    for(size_t i = 0; i < YY; i++) {
        for(size_t j = 0; j < XX; j++) {
            int r,g,b;
            fin >> r >> g >> b;
            v[i].push_back(Pixel(r,g,b,j,i));
        }
    }
}


struct Kep {
private:
    vector<vector<Pixel>> kep;
    float x, y;
public:
    Kep(){}
    Kep(string _filename, int _x = 50, int _y = 50) {
        beolvaso(kep, _filename); //kiemelt_hatter.kep
        x = _x;
        y = _y;
    }

    float getX() const {return x;}
    float getY() const {return y;}
    vector<vector<Pixel>> getKep() const {return kep;}

    void kirajzol( canvas& c ) {
        for(size_t i = 0; i < kep.size(); i++) {
            for(size_t j = 0; j < kep.size(); j++) {
                kep[i][j].kirajzol(c,x,y);
            }
        }
    }


//---------------------------------------------------------------------------------------------------------------------------------------------------
    void atmeretez_negyzette(float xo){

        float yo = xo;
        float xi = kep[0].size();
        float yi = kep.size();
        for(auto i : kep) if(i.size()!=xi) cerr<< "Nem egyenlok az atmertezendo kep sorpixelvektorai!";


///===================X MENTEN =================================================================================================================
///-------------------Kicsinyites X menten-----------------------------------------------------------
        if(xo<=xi){
            for(int i = 0; i<yi; i++){///yi!!
                //kep[i].resize(xo);
                for(int j = 0; j<xo; j++){
                    float sumpx = 0;
                    int sumRval = 0;
                    int sumGval = 0;
                    int sumBval = 0;
                    float hanyad = 0;

                    for(int k = j*xi/xo;  k < (j+1)*xi/xo;  k++){
                        if (k < j*xi/xo){ // knagyobb lenne csak vissza lett csökkentve a kerekites miatt
                            hanyad = (j*xi/xo - k) / 1;
                            sumpx -= hanyad;
                            sumRval -= hanyad * kep[i][k].getR();
                            sumGval -= hanyad * kep[i][k].getG();
                            sumBval -= hanyad * kep[i][k].getB();
                        }

                        if(k+1 > (j+1)*xi/xo){
                            hanyad = ((j+1)*xi/xo - k) / 1;

                            sumpx += hanyad;
                            sumRval += hanyad * kep[i][k].getR();
                            sumGval += hanyad * kep[i][k].getG();
                            sumBval += hanyad * kep[i][k].getB();


                        }else{
                            sumpx++;
                            sumRval += kep[i][k].getR();
                            sumGval += kep[i][k].getG();
                            sumBval += kep[i][k].getB();
                        }

                    }

                    if(sumpx==0) cerr<<"sumpx is 0!!"<<endl;
                    int ujR = sumRval/sumpx;
                    int ujG = sumGval/sumpx;
                    int ujB = sumBval/sumpx;

                    kep[i][j].setR(ujR);
                    kep[i][j].setG(ujG);
                    kep[i][j].setB(ujB);

                }
                for(int p = 0; p<xi-xo; p++){
                    kep[i].pop_back();
                }

            }

        }else{
///-------------------------Nagyitas X menten-----------------------------------------
            //x iranyba megnagobbitja a matrixot
            for(int i = 0; i<kep.size(); i++ ){
                for(int j = xi; j<xo; j++){
                    Pixel tpix(0, 0, 0, j, i);
                    kep[i].push_back(tpix);
                }
            }


            for(int y = 0; y<yi; y++){
                vector<Pixel> temp;
                for(int x = 0; x<xi; x++){
                    temp.push_back(kep[y][x]);
                }

                for(int j = 0; j<xi; j++){

                    float hanyad = 0;

                    for(int k = j*xo/xi;  k < (j+1)*xo/xi;  k++){

                        if (k < j*xi/xo){ // knagyobb lenne csak vissza lett csökkentve a kerekites miatt
                            k++;
                        }

                        if(k+1 > (j+1)*xo/xi){
                            hanyad = ((j+1)*xo/xi - k);
                            kep[y][k].setR(hanyad*temp[j].getR() + (1-hanyad)*temp[j+1].getR());
                            kep[y][k].setG(hanyad*temp[j].getG() + (1-hanyad)*temp[j+1].getG());
                            kep[y][k].setB(hanyad*temp[j].getB() + (1-hanyad)*temp[j+1].getB());
                        }else{
                            kep[y][k].setR(temp[j].getR());
                            kep[y][k].setG(temp[j].getG());
                            kep[y][k].setB(temp[j].getB());
                        }

                    }


                }
            }


        }
    ///=========================================Y MENTEN====================================================================================================================


 ///--------------------Kicsinyites Y menten
        if(yo<=yi){
            for(int j = 0; j<xo; j++){
                for(int i = 0; i<yo; i++){ ///itt mar nem yi!
                    float sumpx = 0;
                    int sumRval = 0;
                    int sumGval = 0;
                    int sumBval = 0;
                    float hanyad = 0;

                    for(int k = i*yi/yo;  k < (i+1)*yi/yo;  k++){
                        if (k < i*yi/yo){ // k nagyobb lenne csak vissza lett csökkentve a kerekites miatt
                            hanyad = (i*yi/yo - k) / 1;
                            sumpx -= hanyad;
                            sumRval -= hanyad * kep[k][j].getR();
                            sumGval -= hanyad * kep[k][j].getG();
                            sumBval -= hanyad * kep[k][j].getB();
                        }

                        if(k+1 > (i+1)*yi/yo){
                            hanyad = ((i+1)*yi/yo - k) / 1;

                            sumpx += hanyad;
                            sumRval += hanyad * kep[k][j].getR();
                            sumGval += hanyad * kep[k][j].getG();
                            sumBval += hanyad * kep[k][j].getB();


                        }else{
                            sumpx++;
                            sumRval += kep[k][j].getR();
                            sumGval += kep[k][j].getG();
                            sumBval += kep[k][j].getB();
                        }

                    }

                    if(sumpx==0) cerr<<"sumpx is 0!!"<<endl;
                    int ujR = sumRval/sumpx;
                    int ujG = sumGval/sumpx;
                    int ujB = sumBval/sumpx;

                    kep[i][j].setR(ujR);
                    kep[i][j].setG(ujG);
                    kep[i][j].setB(ujB);
                    kep[i][j].setMark(false);

                }

            }
            for(int p = 0; p<yi-yo; p++){
                kep.pop_back();
            }

        }else{
///:::::::::::::::::::::::::::::::::Nagyitas y menten::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
            //y iranyba megnagobbitja a matrixot
            for(int i = yi; i<yo; i++){

                vector<Pixel> tv;
                for(int j = 0; j<xo; j++){
                    Pixel tpix(0, 0, 0, j, i);
                    tv.push_back(tpix);
                }

                kep.push_back(tv);
            }



            for(int x = 0; x<xo; x++){ //j=x

                vector<Pixel> temp;
                for(int ty = 0; ty<yi; ty++){
                    temp.push_back(kep[ty][x]);
                }

                for(int y = 0; y<yi; y++){

                    float hanyad = 0;

                    for(int k = y*yo/yi;  k < (y+1)*yo/yi;  k++){

                        if (k < y*yi/yo){ // knagyobb lenne csak vissza lett csökkentve a kerekites miatt
                            k++;
                        }
                        if(k+1 > (y+1)*yo/yi){
                            hanyad = ((y+1)*yo/yi - k);
                            kep[k][x].setR(hanyad*temp[y].getR() + (1-hanyad)*temp[y+1].getR());
                            kep[k][x].setG(hanyad*temp[y].getG() + (1-hanyad)*temp[y+1].getG());
                            kep[k][x].setB(hanyad*temp[y].getB() + (1-hanyad)*temp[y+1].getB());
                        }else{
                            kep[k][x].setR(temp[y].getR());
                            kep[k][x].setG(temp[y].getG());
                            kep[k][x].setB(temp[y].getB());
                            kep[k][x].setMark(false);
                        }

                        if(k==1200) cout<<kep[1200][2].getR()<<endl;

                    }


                }
            }

        } //else end

    } //atmeretez_negyzette end


///-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void szamot_kiemel(){

        int th = szamkivalasztasi_trashold;
        vector<vector<int>> V;
        ///Feltoltom 3 sorral (r, g, b) és 32 oszolppal (r/g/b értéke 0-7; 8-15; 16-23... között van) :
        for(int i = 0; i<3; i++){
            vector<int> tv;
            for(int i =0; i<256/hf; i++) tv.push_back(0);
            V.push_back(tv);
        }

        int osszpx=0;
        ///végigmegyek a kép pixelein és feltöltöm az elõbb létrhozott mátrixomat
        for(int i = 0; i<kep.size(); i++){
            for(int j = 0; j<kep[i].size(); j++){

                osszpx++;
                for(int k = 0; k<256/hf; k++){
                    if(k*hf<=kep[i][j].getR() && kep[i][j].getR()<k*hf+hf) V[0][k]++;
                }
                for(int k = 0; k<256/hf; k++){
                    if(k*hf<=kep[i][j].getG() && kep[i][j].getG()<k*hf+hf) V[1][k]++;
                }
                for(int k = 0; k<256/hf; k++){
                    if(k*hf<=kep[i][j].getB() && kep[i][j].getB()<k*hf+hf) V[2][k]++;
                }
            }
        }

        ///maxkeresés
        int maxr = 0; ///a maximális r érték összeget keresem az egyes intervallumokban.
        int maxhr = 0; ///a maximális r érték helye a vektorban.
        for(int i = 0; i<256/hf; i++){
            if(V[0][i]>maxr) {maxr=V[0][i]; maxhr=i;}
        }

        int maxg = 0;
        int maxhg = 0;
        for(int i = 0; i<256/hf; i++){
            if(V[1][i]>maxg) {maxg=V[1][i]; maxhg=i;}
        }
        int maxb = 0;
        int maxhb = 0;
        for(int i = 0; i<256/hf; i++){
            if(V[2][i]>maxb) {maxb=V[2][i]; maxhb=i;}
        }


        ///max tartomanyban átlagkeresés
        int sumR = 0, sumG = 0, sumB = 0;
        int sumRval = 0, sumGval = 0, sumBval = 0;


        for(int i = 0; i<kep.size(); i++){
            for(int j = 0; j<kep[i].size(); j++){
                if(maxhr*hf<=kep[i][j].getR() && kep[i][j].getR()<maxhr*hf+hf) {sumRval+=kep[i][j].getR(); sumR+=1;}
                if(maxhg*hf<=kep[i][j].getG() && kep[i][j].getG()<maxhg*hf+hf) {sumGval+=kep[i][j].getG(); sumG+=1;}
                if(maxhb*hf<=kep[i][j].getB() && kep[i][j].getB()<maxhb*hf+hf) {sumBval+=kep[i][j].getB(); sumB+=1;}
            }
        }

        int avgr=0, avgg=0, avgb=0;
        if(sumR!=0) avgr = sumRval/sumR;
        if(sumG!=0) avgg = sumGval/sumG;
        if(sumB!=0) avgb = sumBval/sumB;

        if(sumR==0 && sumG==0 && sumB==0) cerr<<"A sumR sumG sumB is nulla! (szamot_kiemel fv-ben)"<<endl;


        for(int i = 0; i<kep.size(); i++){
            for(int j = 0; j<kep[i].size(); j++){
                if (abs(kep[i][j].getR()-avgr)>th ||
                    abs(kep[i][j].getG()-avgg)>th ||
                    abs(kep[i][j].getB()-avgb)>th){

                        kep[i][j].setMark(true);

                }
            }
        }

    } //szamot_kiemel end
///----------------------------------------------------------------------------------------------------------------------


    void kivag(){

        int maxx=0, maxy=0, minx=kep[0].size(), miny=kep.size();
        for(auto &s : kep){
            for(auto &i : s){
                if(i.isMarked() && i.getX() < minx)  minx = i.getX();
                if(i.isMarked() && i.getY() < miny)  miny = i.getY();
                if(i.isMarked() && i.getX() > maxx)  maxx = i.getX();
                if(i.isMarked() && i.getY() > maxy)  maxy = i.getY();
            }
        }
        maxx+=1, maxy+=1;

        for(int i = 0; i<kep.size(); i++){
            for(int j = 0; j<kep[i].size()-minx; j++){

                kep[i][j].setR(kep[i][j+minx].getR());
                kep[i][j].setG(kep[i][j+minx].getG());
                kep[i][j].setB(kep[i][j+minx].getB());
                kep[i][j].setMark(kep[i][j+minx].isMarked());

            }
            int sx = kep[i].size();
            for(int j = 0; j<sx-(maxx-minx); j++){
                kep[i].pop_back();
            }
        }
        ///.............................................................
        for(int j = 0; j<kep[0].size(); j++){
            for(int i = 0; i<kep.size()-miny; i++){

                kep[i][j].setR(kep[i+miny][j].getR());
                kep[i][j].setG(kep[i+miny][j].getG());
                kep[i][j].setB(kep[i+miny][j].getB());
                kep[i][j].setMark(kep[i+miny][j].isMarked());
            }
        }

        int sy = kep.size();
        for(int j = 0; j<sy-(maxy-miny+1); j++){
            kep.pop_back();
        }



        if(kep.size()==0) cerr<<"Minden pixel ki lett szűrve!"<<endl;

    }

    void marked_megjelenit(){
        for(auto &sor : kep){
            for(auto &i : sor){
                if(i.isMarked()){
                    i.setR(255);
                    i.setG(255);
                    i.setB(0);
                }
            }
        }
    }
    int kisebb_kepmeret(){
        if(kep.size()==0) cerr<<"Minden pixel ki lett szűrve! (2)"<<endl;
        if(kep.size()<kep[0].size()) return kep.size();
        else return kep[0].size();
    }


    void sulyokat_beir(vector<vector<int>> &v, int n){
        ofstream of(to_str(n) + "_SULYMATRIX.txt");

        if(v.size()!=kep.size() || v[0].size()!=kep[0].size()) cerr<<"Nem egyenlo a ket vektor! (sulyokat_beir fvben)"<<endl;
        for(int i = 0; i<kep.size(); i++){
            for(int j = 0; j<kep[i].size(); j++){
                if(kep[i][j].isMarked())  v[i][j]+=1;

                if(100<=v[i][j]) of<<" "<<v[i][j];
                if(10<=v[i][j] && v[i][j]<100) of<<"  "<<v[i][j];
                if(v[i][j]<10) of<<"   "<<v[i][j];
            }
            of<<endl;
        }
    }


    int felismer(string fveg = "_SULYMATRIX.txt"){

        vector<int> sulyosszegek(10, 0);
        for(int n = 0; n<10; n++){
            ifstream inf(to_str(n) + fveg); if(!inf.good()) cerr<<"No file!(feismer fv-ben)"<<endl;
            for(int i = 0; i<kep.size(); i++){
                for(int j = 0; j<kep[i].size(); j++){
                    int suly;
                    inf>>suly;
                    if(kep[i][j].isMarked()) sulyosszegek[n]+=suly;
                    else sulyosszegek[n]-=suly;
                }
            }
        }

        int maxsuly = sulyosszegek[0];
        int a_szam = 0;
        for(int i = 0; i<10; i++){

            if(sulyosszegek[i]>maxsuly){
                a_szam = i;
                maxsuly=sulyosszegek[i];
            }
        }
        return a_szam;
    }

};

void matrixot_vizualizal(vector<vector<int>>m, int x = 50, int y = 50, int bsize = 10){

    int maxi = 0;
    for(int i = 0; i<m.size(); i++){
        for(int j = 0; j<m[i].size(); j++){
            if(m[i][j]>maxi) maxi = m[i][j];
        }
    }

    for(int i = 0; i<m.size(); i++){
        for(int j = 0; j<m[i].size(); j++){
            if(x+j*bsize<XX && y+i*bsize<YY){
                gout<<color(m[i][j]*255/maxi,0,0)<<move_to(x+j*bsize, y+i*bsize)<<box(bsize, bsize);
            }
        }
    }

}


void matrixot_vizualizal(string filename, int x = 50, int y = 50, int bsize = 10){

    ifstream inf (filename); if(!inf.good()) cerr<<"No file!(matrixot_vizualizal fvben 1)"<<endl;
    int maxi = 0;
    string sor;
    stringstream ss;
    int tmp;
    while (inf.good()){
            inf>>tmp;
            if(tmp>maxi) maxi = tmp;
    }
    inf.close();
    inf.open(filename); if(!inf.good()) cerr<<"No file!(matrixot_vizualizal fvben 2)"<<endl;
    int i=0;
    while(inf.good()){
        getline(inf, sor);
        inf>>ws;
        ss<<sor;
        int j=0;
        while(ss.good()){
            ss>>tmp; ss>>ws;
            gout<<color(tmp*255/maxi,0,0)<<move_to(x+j*bsize, y+i*bsize)<<box(bsize, bsize);
            j++;
        }
        ss.clear();
        i++;
    }

}

int main()
{

    string inpf;
    cout<<"Testing of one picture"<<endl;
    while(1){
    cout<<"Please type here the filename of the picture! (to skip this part type 'q')"<<endl;
    cin>>inpf;
    ifstream inf(inpf);
    if(inf.good() || inpf=="q") break;
    else cout<<"This file is not exist. Please try again!"<<endl;
    }

    if(inpf!="q"){

        Kep k(inpf);
        k.atmeretez_negyzette(k.kisebb_kepmeret()); ///mark-okat törli!
        k.szamot_kiemel();
        k.kivag();

        k.atmeretez_negyzette(kepmeret);
        k.szamot_kiemel();
        k.marked_megjelenit();

        int A_szam = k.felismer();
        cout<<"The number: "<<A_szam<<endl;
    }


    string path;

    cout<<endl<<"Testing of more picture"<<endl
        <<"    The format of the pictures name have to be like 'n_k.bmp.kep'," <<endl
        <<"    where n is the number what it presents and k is the its serial number indexed from 0"<<endl;

    cout<<endl<<"Please type here the pathway of the folder, where the pictures are!"<<endl
        <<"By Windows like this: '/Users/Anonymous/Documents/images' (to skip this part please type 'q')"<<endl;
    cin>>path;

    int lst, hst, mmax;
    if(path!="q"){

    cout<<endl<<"Type the lowest presented number in the folder"<<endl;
    cin>>lst;
    cout<<"Type the highest presented number in the folder"<<endl;
    cin>>hst;
    cout<<"Type how many pictures are there about the several numbers"<<endl;
    cin>>mmax;

    float correct=0, sum=0;
    for(int n = lst; n<=hst; n++){
        for(int m = 0; m<mmax; m++){

            string filename = path + "/" +to_str(n) + "_" + to_str(m) + ".bmp.kep";
            Kep k(filename);

            k.atmeretez_negyzette(k.kisebb_kepmeret()); ///mark-okat törli!
            k.szamot_kiemel();
            k.kivag();

            k.atmeretez_negyzette(kepmeret);
            k.szamot_kiemel();
            k.marked_megjelenit();

            int A_szam = k.felismer();
            cout<<"The number: "<<A_szam<<" (picture: "<<n<<'_'<<m<<")"<<endl;

            if(A_szam == n) correct++;
            sum++;
        }
    }

    cout<<"CORRECTNESS: "<< 100*correct/sum << " %" <<endl;


    }


///"Tanitas" (ezzel hoztam letre sulymatrixokat)
/*
    vector<vector<vector<int>>>sulymx;

    for(int n = 0; n<10; n++){
        vector<vector<int>> tvv;
        for(int i = 0; i<kepmeret; i++){
            vector<int> tv(kepmeret, 0);
            tvv.push_back(tv);
        }
        sulymx.push_back(tvv);
    }

    for(int n = 0; n<10; n++){
            cout<<n<<endl;
        for(int m =0; m<800; m++){
            string filename = file_pathway + "/" + to_str(n) + '_' + to_str(m) + ".bmp.kep";
            Kep k(filename,(m+1)*50);

            k.atmeretez_negyzette(k.kisebb_kepmeret()); ///mark-okat törli!
            k.szamot_kiemel();
            k.kivag();

            k.atmeretez_negyzette(kepmeret); ///mark-okat törli!
            k.szamot_kiemel();
            k.marked_megjelenit();

            k.sulyokat_beir(sulymx[n], n);

        }
    }
*/

///Vizualizalja a sulymatrixokat:
/*
    srand(time(0));
    gout.open(XX,YY);
    event ev;
    gin.timer(40);
    while(gin >> ev && ev.keycode!=key_escape) {

        if(ev.type == ev_timer) {
            tr();


            for(int n = 0; n<10; n++){
                string filename = to_str(n) + "_SULYMATRIX.txt";
                matrixot_vizualizal(filename, n%3*150+50, n/3*150+50);
            }

            //k.kirajzol(gout);
        }
        gout << refresh;
    }
*/
    return 0;
}
