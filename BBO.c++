#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>
using namespace std;

class Habitat
{
    public:
        vector <int> SIV;
        int Smax;
        double HSI;
        double immigration, emigration;
        Habitat(int size)
        {
            Smax = size;
            SIV.resize(Smax);
            RandomInitialize();
            HSI = 0;
        }

        void RandomInitialize()
        {
            for (int i = 0; i < Smax; i++)
                SIV[i] = rand() % 100;
        }

        void CalculateHSI()
        {
            double sum = 0;
            for (int x : SIV)
                sum += x * x;
            HSI = sum;
        }

        void Mutate(double m_rate)
        {
            for (int i = 0; i < Smax; i++) {
                if ((double)rand() / RAND_MAX < m_rate)
                    SIV[i] = rand() % 100;
            }
        }
};

class Ecosystem
{
    public:
        const int habitatCount = 10;
        int Smax;
        double I, E, mmax;
        int elitismCount;
        vector<Habitat> habitats;
        Ecosystem(int nSIV, double I_, double E_, double mmax_, int elite) {
            Smax = nSIV;
            I = I_;
            E = E_;
            mmax = mmax_;
            elitismCount = elite;
            for (int i = 0; i < habitatCount; i++)
                habitats.emplace_back(Smax);
        }

        void EvaluateHSI() {
            for (auto& h : habitats)
                h.CalculateHSI();
        }

        void CalculateMigrationRates() {
            sort(habitats.begin(), habitats.end(), [](Habitat& a, Habitat& b) {
                return a.HSI < b.HSI;
            });
            for (int i = 0; i < habitatCount; i++) {
                habitats[i].emigration = I * (habitatCount - 1 - i) / (habitatCount - 1);
                habitats[i].immigration = E * i / (habitatCount - 1);
            }
        }

        int SelectDonor(int current) {
            double total = 0;
            for (int i = 0; i < habitatCount; i++)
                if (i != current) total += habitats[i].emigration;
            double r = ((double)rand() / RAND_MAX) * total, acc = 0;
            for (int i = 0; i < habitatCount; i++)
            {
                if (i == current) continue;
                acc += habitats[i].emigration;
                if (acc >= r) return i;
            }
            return 0;
        }

        void Migration() {
            int count = 0;
            for (int i = elitismCount; i < habitatCount; i++) {
                for (int d = 0; d < Smax; d++) {
                    if ((double)rand() / RAND_MAX < habitats[i].immigration) {
                        int donor = SelectDonor(i);
                        habitats[i].SIV[d] = habitats[donor].SIV[d];
                        count++;
                    }
                }
            }
            cout << "di cư: "<<count<<", ";
        }

        vector<double> GenerateSymmetricProbabilities()
        {
            vector<double> P(habitatCount);
            vector<double> v(habitatCount);
            double sumv = 0;

            for (int i = 0; i < habitatCount; i++)
            {
                v[i] = factorial(habitatCount - 1) / (factorial(i) * factorial(habitatCount - 1 - i));
                sumv += v[i];
            }
            for (int i = 0; i < habitatCount; i++)
                P[i] = v[i] / sumv;
            return P;
        }

        double factorial(int n)
        {
            double result = 1;
            for (int i = 2; i <= n; ++i)
                result *= i;
            return result;
        }

        void Mutation() {
            int count = 0;
            vector<double> P = GenerateSymmetricProbabilities();
            double Pmax = *max_element(P.begin(), P.end());
            for (int i = elitismCount; i < habitatCount; i++) {
                double mS = mmax * ((1.0 - P[i]) / Pmax);
                //habitats[i].Mutate(mS);
                for (int j = 0; j < Smax; j++) {
                if ((double)rand() / RAND_MAX < mS)
                    {
                        habitats[i].SIV[j] = rand() % 100;
                        count++;
                    }
                }
            }
            cout<<"đột biến: "<<count<<", ";
        }

        bool Termination(int Critic)
        {
            return (habitats[0].HSI <= Critic);
        }

        void TransformPopulation() {
            CalculateMigrationRates();
            Migration();
            EvaluateHSI();
            Mutation();
        }

        void BBO(int critic) {
            EvaluateHSI();
            int lap = 0;
            while (!Termination(critic))
                {
                    cout << "Lặp " << lap++ << ": ";
                    TransformPopulation();
                    cout<< "HSI tốt nhất: " <<habitats[0].HSI<<endl;
                }
            cout << "Giải pháp tốt nhất: "; for (int x : habitats[0].SIV) cout << x;
            cout << "\nHSI tốt nhất = " << habitats[0].HSI << endl;
        }
};

int main() {
    srand(time(0));

    int Smax = 5;             // số lượng vector SIV
    double I = 1.0;           // Tỷ lệ nhập cư tối đa
    double E = 1.0;           // Tỷ lệ di cư tối đa
    double mmax = 0.01;        // Tỷ lệ đột biến tối đa
    int elitism = 1;          // Số lượng habitat được giữ nguyên

    Ecosystem eco(Smax, I, E, mmax, elitism);
    eco.BBO(10);              // tiêu chí finess

    return 0;
}
