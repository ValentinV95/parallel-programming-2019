#include <time.h>

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

std::vector<int> kernel = {1, 2, 1, 2, 4, 2, 1, 2, 1};
int kernelSum = 16;

std::vector<int> randomPicture(int row, int col) {
  if (col <= 0) {
    throw "error";
  }
  if (row <= 0) {
    throw "error";
  }
  std::mt19937 gen;
  gen.seed(static_cast<unsigned int>(time(0)));
  std::vector<int> pic(row * col);
  for (int i = 0; i < row * col; ++i) {
    pic[i] = gen() % 255;
  }
  return pic;
}

std::vector<int> borderPicture(std::vector<int> pic, int row, int col) {
  if (col <= 0) {
    throw "error";
  }
  if (row <= 0) {
    throw "error";
  }

  int tmpRow = row + 2;
  int tmpCol = col + 2;
  std::vector<int> borderedPicture(tmpRow * tmpCol);

  //borderedPicture corners
  borderedPicture[0] = pic[0];
  borderedPicture[tmpCol - 1] = pic[col - 1];
  borderedPicture[tmpRow * tmpCol - tmpCol] = pic[row * col - col];
  borderedPicture[tmpRow * tmpCol - 1] = pic[row * col - 1];

  //horizontal edges
  for (int columnIter = 1; columnIter < col + 1; columnIter++) {
    borderedPicture[columnIter] = pic[columnIter - 1];
    borderedPicture[tmpRow * tmpCol - tmpCol + columnIter] =
        pic[row * col - col - 1 + columnIter];
  }

  //vertical edges
  for (int rowsIter = 1; rowsIter < row + 1; rowsIter++) {
    borderedPicture[rowsIter * tmpCol] = pic[(rowsIter - 1) * col];
    borderedPicture[(rowsIter + 1) * tmpCol - 1] = pic[rowsIter * col - 1];
  }

  //borderedPicture middle by pic
  for (int rowsIter = 1; rowsIter < row + 1; rowsIter++) {
    for (int columnIter = 1; columnIter < col + 1; columnIter++) {
      borderedPicture[rowsIter * tmpCol + columnIter] =
          pic[(rowsIter - 1) * col + (columnIter - 1)];
    }
  }

  return borderedPicture;
}

std::vector<int> filterPicture(std::vector<int> borderedPicture,
                               std::vector<int> kernel, int row, int col) {
  std::vector<int> picture(row * col);

  for (int columnIter = 1; columnIter < col + 1; columnIter++) {
    for (int rowsIter = 1; rowsIter < row + 1; rowsIter++) {
      int result = 0, kernelIterator = 0;

      for (int i = rowsIter - 1; i < rowsIter + 2; i++) {
        for (int j = columnIter - 1; j < columnIter + 2; j++) {
          result +=
              borderedPicture[i * (col + 2) + j] * kernel[kernelIterator++];
        }
      }
      picture[(rowsIter - 1) * col + (columnIter - 1)] = result / kernelSum;
    }
  }
  return picture;
}

int main(int argc, char** argv) {
  int col = atoi(argv[1});
  int row = atoi(argv[2});
  std::vector<int> sourceImage = randomPicture(row, col);
  std::vector<int> borderImage = borderPicture(sourceImage, row, col);
  std::vector<int> result = filterPicture(borderImage, kernel, row, col);

  std::cout << "Source image:" << std::endl;
  for (int r = 0; r < row; r++) {
    for (int c = 0; c < col; c++) {
      std::cout << sourceImage[r * col + c] << " ";
    }
    std::cout << std::endl;
  }

  std::cout << "Actual result:" << std::endl;
  for (int r = 0; r < row; r++) {
    for (int c = 0; c < col; c++) {
      std::cout << result[r * col + c] << " ";
    }
    std::cout << std::endl;
  }
  return 0;
}
