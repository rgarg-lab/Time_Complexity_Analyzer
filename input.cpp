#include <bits/stdc++.h>
using namespace std;

// Merge two sorted halves
void merge(vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    vector<int> L(n1), R(n2);

    // Copy data
    for(int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for(int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;

    // Merge
    while(i < n1 && j < n2) {
        if(L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Remaining elements
    while(i < n1) {
        arr[k++] = L[i++];
    }

    while(j < n2) {
        arr[k++] = R[j++];
    }
}

// Merge Sort function
void mergeSort(vector<int>& arr, int left, int right) {
    if(left >= right) return;

    int mid = (left + right) / 2;

    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);

    merge(arr, left, mid, right);
}

int main() {
    vector<int> arr = {5, 2, 4, 6, 1, 3};

    mergeSort(arr, 0, arr.size() - 1);

    for(int x : arr) cout << x << " ";
}