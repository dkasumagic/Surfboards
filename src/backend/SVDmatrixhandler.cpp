#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <algorithm>
#include "SVDmatrixhandler.hpp"

SVDMatrixIterator::SVDMatrixIterator(int n, int m, int r, int singularvaluesA, int singularvaluesB) 
    : n_(n), m_(m), r_(r), s1_(singularvaluesA), s2_(singularvaluesB) , i_(0), j_(0), count_(0)  {}

bool SVDMatrixIterator::hasNext() const {
    return count_ < 5; // There are 5 matrix multiplications to iterate over in every SVD operation
}

std::string SVDMatrixIterator::next_line() {
    if (!hasNext()) {
        return {}; 
    }

    std::ostringstream oss;
    // UADA multiplication
    if (count_ == 0) {
        
        oss << i_ * s1_ + j_ << "*" << j_ ;
        
        oss << ";";

        ++j_;
        if (j_ >= s1_) {
            j_ = 0;
            ++i_;
        }
        if (i_ >= n_) {
            i_ = 0;
            ++count_;
            oss << "\n" << "%" << "\n";
        }
    }

    else if (count_ == 1) {
        for (int k = 0; k < m_; ++k) {
            oss << i_ * m_ + k << "*" << k * s2_ + j_;
            if (k < m_ - 1) {
                oss << "+";
            }
        }
        oss << ";";

        ++j_;
        if (j_ >= s2_) {
            j_ = 0;
            ++i_;
        }
        if (i_ >= s1_) {
            i_ = 0;
            ++count_;
            oss << "\n" << "%" << "\n";
        }
    }

    else if (count_ == 2) {
        oss << j_ << "*" << i_ * s2_ + j_;
        
        oss << ";";

        ++j_;
        if (j_ >= s2_) {
            j_ = 0;
            ++i_;
        }
        if (i_ >= r_) {
            i_ = 0;
            ++count_;
            oss << "\n" << "%" << "\n";
        }
    }

    else if (count_ == 3) {
        for (int k=0; k < std::min(s1_, s2_); ++k) {
            oss << i_ * std::min(s1_, s2_) + k << "*" << k * r_ + j_;
            if (k < std::min(s1_, s2_)-1) {
                oss << "+"; 
            }
        }
        oss << ";";

        ++j_;
        if (j_ >= r_) {
            j_ = 0;
            ++i_;
        }
        if (i_ >= std::max(s1_, s2_)) {
            i_ = 0;
            ++count_;
            oss << "\n" << "%" << "\n";
        }       
    }

    else if (count_ == 4) {
        for (int k = 0; k < s1_; ++k) {
            oss << i_ * s1_ + k << "*" << k * r_ + j_;
            if (k < s1_ - 1) {
                oss << "+";
            }
        }
        oss << ";";

        ++j_;
        if (j_ >= r_) {
            j_ = 0;
            ++i_;
        }
        if (i_ >= n_) {
            i_ = 0;
            ++count_;
        }  
    }
    return oss.str();
}
