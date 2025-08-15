#ifndef MATRIXHANDLER_HPP
#define MATRIXHANDLER_HPP

void writetofile(int rows1, int cols1, int rows2, int cols2);
class MatrixIterator {
public:
    MatrixIterator(int n, int m, int r);
    bool hasNext() const;
    std::string next_line();
    
private:
    int n_; // rows1
    int m_; // cols1
    int r_; // cols2
    int i_; // current row index
    int j_; // current column index
};

#endif