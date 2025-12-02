#ifndef FILEIO_HPP
#define FILEIO_HPP

int loadPoints(const std::string& filepath, PointVector& vec);
int pMatrixFromFile(const std::string& filepath, DoubleVector2D& matrix);

void printMatrix(const DoubleVector2D& matrix);
int readMatrix(const std::string& filepath, DoubleVector2D& matrix);

#endif // FILEIO_HPP
