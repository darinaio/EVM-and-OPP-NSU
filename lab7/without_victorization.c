#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

typedef struct {
    float* elements;
} Matrix;


unsigned int N = 2048;
unsigned int M = 10;


void swap(float* a, float* b);
float findMaxAbsSumByRows(const Matrix* matrix);
float findMaxAbsSumByColumns(const Matrix* matrix);
Matrix* createMatrix();
void fillMatrix(Matrix* matrix);
float dot(const float* a, const float* b);
Matrix* CreateFilledMatrix();
Matrix* CreateIdentityMatrix();
void Copy(Matrix* destination, const Matrix* source);
Matrix* CreateAndCopy(const Matrix* matrix);
void DestroyMatrix(Matrix* matrix);
void Transpose(const Matrix* matrix);
void SubtractMatrix(Matrix* matrix, const Matrix* term);
void AddMatrix(Matrix* matrix, const Matrix* term);
void MultiplyByNumber(Matrix* matrix, float number);
Matrix* MultiplyMatrices(const Matrix* matrix1, const Matrix* matrix2_transposed);
void MultiplyByMatrix(Matrix* matrix, const Matrix* multiplier_transposed);
Matrix* FindInverseMatrix(const Matrix* matrix);
float GetMatricesDifference(const Matrix* matrix1, const Matrix* matrix2);
void PrintMatrix(const Matrix* matrix);


int main(int arc, char** argv) {

    if (arc == 3) {
        N = strtoull(argv[1], NULL, 10);
        M = strtoull(argv[2], NULL, 10);
    }

    Matrix* matrix = CreateFilledMatrix(); // Создаем и заполняем исходную матрицу

    time_t t0;
    time(&t0);

    Matrix* result = FindInverseMatrix(matrix); // Вычисляем обратную матрицу

    time_t t1;
    time(&t1);

    Transpose(result); // Транспонируем результат

    MultiplyByMatrix(matrix, result); // Умножаем исходную матрицу на транспонированную обратную

    Matrix* identityMatrix = CreateIdentityMatrix(); // Создаем единичную матрицу для сравнения

    printf("%ld s\n", t1 - t0); // Выводим время работы
    printf("Error %f\n\n", GetMatricesDifference(identityMatrix, matrix)); // Выводим ошибку (разницу с единичной матрицей)

    DestroyMatrix(matrix); 
    DestroyMatrix(result); 
    DestroyMatrix(identityMatrix); 

    return 0;
}

Matrix* FindInverseMatrix(const Matrix* matrix) {
    Matrix* B = CreateAndCopy(matrix); // Создаем копию исходной матрицы
    Transpose(B); // Транспонируем копию

    const float A1 = findMaxAbsSumByColumns(matrix); // Находим максимальную сумму абсолютных значений по столбцам
    const float A2 = findMaxAbsSumByRows(matrix); // Находим максимальную сумму абсолютных значений по строкам

    MultiplyByNumber(B, 1 / A1); // Нормируем матрицу B
    MultiplyByNumber(B, 1 / A2); // Нормируем матрицу B

    Matrix* R = CreateIdentityMatrix(); // Создаем единичную матрицу R
    Matrix* RSeries = CreateIdentityMatrix(); // Создаем единичную матрицу для суммы степеней R

    Matrix* matrix_transposed = CreateAndCopy(matrix); // Создаем копию и транспонируем исходную матрицу для последующего умножения
    Transpose(matrix_transposed);

    Matrix* BA = MultiplyMatrices(B, matrix_transposed); // Вычисляем произведение B*A^T

    DestroyMatrix(matrix_transposed); // Освобождаем память

    SubtractMatrix(R, BA); // R = I - BA
    Transpose(R); // Транспонируем R

    Matrix* result = CreateIdentityMatrix(); // Создаем единичную матрицу для результата

    for (unsigned int i = 1; i < M; i++) { // Итерируемся по членам ряда Неймана
        Matrix* tmp = MultiplyMatrices(RSeries, R); // Вычисляем R^i

        Copy(RSeries, tmp); // Копируем результат в RSeries

        AddMatrix(result, RSeries); // Прибавляем к результату текущий член ряда

        DestroyMatrix(tmp); // Освобождаем память
    }

    Transpose(B); // Транспонируем матрицу B обратно
    MultiplyByMatrix(result, B); // Умножаем результат на B

    DestroyMatrix(B);
    DestroyMatrix(R); 
    DestroyMatrix(BA); 
    DestroyMatrix(RSeries);

    return result;
}
Matrix* MultiplyMatrices(const Matrix* matrix1, const Matrix* matrix2_transposed) {
    Matrix* result = createMatrix(); // Создаем матрицу для результата

    for (unsigned int i = 0; i < N; i++) { // Итерируемся по строкам первой матрицы
        const float* row1 = matrix1->elements + i * N; // Указатель на начало текущей строки

        for (unsigned int j = 0; j < N; j++) { // Итерируемся по столбцам второй матрицы
            const float* row2 = matrix2_transposed->elements + j * N; // Указатель на начало текущего столбца (строки транспонированной матрицы)

            result->elements[i * N + j] = dot(row1, row2); // Вычисляем скалярное произведение строки и столбца
        }
    }
    return result;
}

void MultiplyByMatrix(Matrix* matrix, const Matrix* multiplier_transposed) {
    Matrix* tmp = MultiplyMatrices(matrix, multiplier_transposed); // Вычисляем произведение
    Copy(matrix, tmp); // Копируем результат во входную матрицу
    DestroyMatrix(tmp); // Освобождаем память от временной матрицы
}

void swap(float* a, float* b) {
    float tmp = *a;
    *a = *b;
    *b = tmp;
}

float findMaxAbsSumByRows(const Matrix* matrix) { //строки
    float result = 0;

    for (unsigned int i = 0; i < N; i++) { // Итерируемся по строкам
        const float * current_row = matrix->elements + i * N; // Указатель на начало текущей строки

        float current_sum = 0;

        for (unsigned int j = 0; j < N; j++) { // Итерируемся по элементам строки
            current_sum += fabsf(current_row[j]); // Суммируем абсолютные значения элементов
        }

        if (current_sum > result) { // Обновляем максимальную сумму, если текущая больше
            result = current_sum;
        }
    }

    return result;
}

float findMaxAbsSumByColumns(const Matrix* matrix) { //столбцы
    float result = 0;

    for (unsigned int i = 0; i < N; i++) { //выбираем столбец по которому будем суммировать
        float current_sum = 0;

        for (unsigned int j = 0; j < N; j++) { // Итерируемся по элементам столбца
            current_sum += fabsf(matrix->elements[j * N + i]); // Суммируем абсолютные значения элементов столбца
        }

        if (current_sum > result) { // Обновляем максимальную сумму, если текущая больше
            result = current_sum;
        }
    }

    return result;
}

Matrix* createMatrix() {
    Matrix* new_matrix = malloc(sizeof(Matrix)); // Выделяем память для структуры Matrix

    new_matrix->elements = calloc(N * N, sizeof(float)); // Выделяем память для элементов матрицы, инициализируем нулями

    if (!new_matrix->elements) { // Проверка на успешность выделения памяти
        free(new_matrix);
        return NULL;
    }

    return new_matrix;
}

void fillMatrix(Matrix* matrix) {
    for (unsigned int i = 0; i < N * N; i++) {
        matrix->elements[i] = (float)rand() / (float)RAND_MAX; // Генерируем случайное число и преобразуем его в float
    }
}

float dot(const float* a, const float* b) {
    float result = 0;

    for (unsigned int k = 0; k < N; k++) {
        result += a[k] * b[k];
    }
    return result;
}

Matrix* CreateFilledMatrix() {
    Matrix* result = createMatrix();
    fillMatrix(result);
    return result;
}

Matrix* CreateIdentityMatrix() {
    Matrix* new_matrix = createMatrix();

    for (unsigned int i = 0; i < N; i++) {
        new_matrix->elements[i * N + i] = 1; // Устанавливаем единицы на главной диагонали
    }

    return new_matrix;
}

void Copy(Matrix* destination, const Matrix* source) {
    for (unsigned int i = 0; i < N * N; i++) {
        destination->elements[i] = source->elements[i];
    }
}

Matrix* CreateAndCopy(const Matrix* matrix) {
    Matrix* result = createMatrix();
    Copy(result, matrix);
    return result;
}

void DestroyMatrix(Matrix* matrix) {
    if (matrix) {
        free(matrix->elements);
        free(matrix);
    }
}

void Transpose(const Matrix* matrix) {
    for (unsigned int i = 0; i < N; i++) {
        for (unsigned int j = i; j < N; j++) {
            swap(&(matrix->elements[i * N + j]), &(matrix->elements[j * N + i])); // Меняем местами элементы симметрично относительно главной диагонали
        }
    }
}

void SubtractMatrix(Matrix* matrix, const Matrix* term) {
    for (unsigned int i = 0; i < N * N; i++) {
        matrix->elements[i] -= term->elements[i];
    }
}

void AddMatrix(Matrix* matrix, const Matrix* term) {
    for (unsigned int i = 0; i < N * N; i++) {
        matrix->elements[i] += term->elements[i];
    }
}

void MultiplyByNumber(Matrix* matrix, float number) {
    for (unsigned int i = 0; i < N * N; i++) {
        matrix->elements[i] *= number;
    }
}

float GetMatricesDifference(const Matrix* matrix1, const Matrix* matrix2) {
    float result = 0;

    for (unsigned int i = 0; i < N * N; i++) {
        result += fabsf(matrix1->elements[i] - matrix2->elements[i]);
    }
    return result;
}

void PrintMatrix(const Matrix* matrix) {
    for (unsigned int i = 0; i < N; i++) {
        for (unsigned int j = 0; j < N; j++) {
            printf("%f ", matrix->elements[N * i + j]);
        }
        printf("\n");
    }
    printf("\n");
}