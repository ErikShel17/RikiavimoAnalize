#include <iostream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <random>
#include <iomanip>

using namespace std;
using namespace std::chrono;

// --- 1. ĮTERPIMO RIKIAVIMAS (Insertion Sort) ---
// Algoritmas modifikuotas taip, kad skaičiuotų palyginimus ir poslinkius (sukeitimus)
void insertionSort(vector<int>& arr, long long& comp, long long& swaps) {
    int n = arr.size();
    for (int i = 1; i < n; ++i) {
        int key = arr[i];
        int j = i - 1;

        // Ciklas vykdomas, kol randama tinkama vieta elementui
        while (j >= 0) {
            comp++; // Skaičiuojame palyginimą
            if (arr[j] > key) {
                arr[j + 1] = arr[j]; // Elemento poslinkis (traktuojama kaip sukeitimas)
                swaps++;
                j--;
            }
            else {
                break; // Jei ne didesnis, nutraukiame ciklą
            }
        }
        arr[j + 1] = key;
    }
}

// --- 2. SĄLAJINIS RIKIAVIMAS (Merge Sort) ---
// Pagalbinė funkcija dviejų surikiuotų masyvo dalių sujungimui
void merge(vector<int>& arr, int left, int mid, int right, long long& comp) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<int> L(n1), R(n2);

    // Duomenų kopijavimas į laikinus masyvus
    for (int i = 0; i < n1; ++i) L[i] = arr[left + i];
    for (int j = 0; j < n2; ++j) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    // Suliejimas atgal į pagrindinį masyvą
    while (i < n1 && j < n2) {
        comp++; // Skaičiuojame elementų palyginimą
        if (L[i] <= R[j]) {
            arr[k++] = L[i++];
        }
        else {
            arr[k++] = R[j++];
        }
    }

    // Likusių elementų perkėlimas
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

// Pagrindinė rekursinė sąlajinio rikiavimo funkcija
void mergeSort(vector<int>& arr, int left, int right, long long& comp) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid, comp);
        mergeSort(arr, mid + 1, right, comp);
        merge(arr, left, mid, right, comp);
    }
}
// Sąlajiniam rikiavimi sukeitimų neskaičiuojame (pagal užduoties išimtį),
// nes jis naudoja elementų perrašymą į laikiną masyvą, o ne tiesioginius sukeitimus.


// --- PAGALBINĖS FUNKCIJOS EKSPERIMENTUI ---
enum DataType { RANDOM, REVERSE_SORTED, SORTED };

// Funkcija masyvo generavimui
vector<int> generateData(int size, DataType type) {
    vector<int> arr(size);
    for (int i = 0; i < size; ++i) arr[i] = i + 1;

    if (type == RANDOM) {
        random_device rd;
        mt19937 g(rd());
        shuffle(arr.begin(), arr.end(), g);
    }
    else if (type == REVERSE_SORTED) {
        reverse(arr.begin(), arr.end());
    }
    return arr;
}

// Funkcija vienam eksperimento ciklui atlikti
void runExperiment(int size, DataType type, string typeName) {
    const int RUNS = 5;

    // Kintamieji vidurkiams kaupti
    long long insTotalTime = 0, insTotalComp = 0, insTotalSwaps = 0;
    long long merTotalTime = 0, merTotalComp = 0;

    for (int r = 0; r < RUNS; ++r) {
        // Sugeneruojame pradinius duomenis (generavimo laikas į matavimus neįtraukiamas)
        vector<int> originalData = generateData(size, type);

        // Kopijuojame duomenis, kad abu algoritmai gautų identiškus masyvus
        vector<int> dataForInsertion = originalData;
        vector<int> dataForMerge = originalData;

        // --- Įterpimo rikiavimo testavimas ---
        long long insComp = 0, insSwaps = 0;
        auto startIns = high_resolution_clock::now();
        insertionSort(dataForInsertion, insComp, insSwaps);
        auto stopIns = high_resolution_clock::now();
        insTotalTime += duration_cast<microseconds>(stopIns - startIns).count();
        insTotalComp += insComp;
        insTotalSwaps += insSwaps;

        // --- Sąlajinio rikiavimo testavimas ---
        long long merComp = 0;
        auto startMer = high_resolution_clock::now();
        mergeSort(dataForMerge, 0, dataForMerge.size() - 1, merComp);
        auto stopMer = high_resolution_clock::now();
        merTotalTime += duration_cast<microseconds>(stopMer - startMer).count();
        merTotalComp += merComp;
    }

    // Rezultatų išvedimas (vidurkiai)
    cout << "Duomenu dydis: " << size << " | Tipas: " << typeName << "\n";
    cout << "--------------------------------------------------------\n";
    cout << "Insertion Sort -> Laikas: " << insTotalTime / RUNS << " us | Palyginimai: "
        << insTotalComp / RUNS << " | Sukeitimai: " << insTotalSwaps / RUNS << "\n";
    cout << "Merge Sort     -> Laikas: " << merTotalTime / RUNS << " us | Palyginimai: "
        << merTotalComp / RUNS << " | Sukeitimai: N/A\n";
    cout << "--------------------------------------------------------\n\n";
}

int main() {
    int sizes[] = { 5000, 10000, 50000 };

    cout << "=== RIKIAVIMO ALGORITMU LYGINAMOJI ANALIZE ===\n\n";

    for (int size : sizes) {
        runExperiment(size, RANDOM, "Nesurikiuoti (Atsitiktiniai)");
        runExperiment(size, REVERSE_SORTED, "Atvirksciai surikiuoti");
        runExperiment(size, SORTED, "Jau surikiuoti");
    }

    return 0;
}