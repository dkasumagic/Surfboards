#ifndef SVDMATRIXHANDLER_HPP
#define MATRIXHANDLER_HPP

class SVDMatrixIterator {

public:
    SVDMatrixIterator(int n, int m, int r, int singularvaluesA, int singularvaluesB);
    bool hasNext() const;
    std::string next_line();

private:
    int n_; // rows1
    int m_; // cols1
    int r_; // cols2
    int s1_;
    int s2_;
    int i_; // current row index
    int j_; // current column index
    int count_; // to keep track of what matrix we are on
};


#endif