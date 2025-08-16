#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cstdint>

int svd_verilog_writer(std::string& inputFile, std::string& outputFile, int numBits, 
                       int n, int m, int r, int s1, int s2) { 

    std::ifstream readFile { inputFile };
    std::ofstream writeFile { outputFile };
    //SVDMatGen gen = SVDMatGen(n,m,r,s1,s2);
    std::string line = " a"; // = std::gen::nextline();
    int i = 0;

    if (readFile.is_open() && writeFile.is_open()) {

        int finMatSize = n*r;
        int uaMatSize = n*n;
        int daMatSize = n*m;
        int vaMatSize = m*m;
        int ubMatSize = m*m;
        int dbMatSize = m*r;
        int vbMatSize = r*r;

        bool sign = true; // TODO: generalise this please. Talk to Howard.


        writeFile 
            << "`timescale 1ns/1ps\n\n"
            << "module surfboard #(\n"
            << "\tparameter W = " << numBits << ",\n"
            << "\tparameter sign = " << sign << "\n"
            << ") (\n"
            << "\tinput logic [W-1:0] UA [0: " << uaMatSize << " - 1],\n"	// need to implement signed.	
            << "\tinput logic [W-1:0] DA [0: " << daMatSize << " - 1],\n"		
            << "\tinput logic [W-1:0] VA [0: " << vaMatSize << " - 1],\n"		
            << "\tinput logic [W-1:0] UB [0: " << ubMatSize << " - 1],\n"		
            << "\tinput logic [W-1:0] DB [0: " << dbMatSize << " - 1],\n"		
            << "\tinput logic [W-1:0] VB [0: " << vbMatSize << " - 1],\n"		
            << "\toutput logic [W-1:0] C [0: " << finMatSize << " - 1]\n"		
            << ");\n\n"
            << "\tlocalparam PROD_W_LEN = 2*W;\n\n"
            << "\tlogic [W-1:0] UADA [0: " << daMatSize << " - 1];\n"
            << "\tlogic [W-1:0] VAUB [0: " << ubMatSize << " - 1];\n"
            << "\tlogic [W-1:0] DBVB [0: " << dbMatSize << " - 1];\n"
            << "\tlogic [W-1:0] VUDV [0: " << dbMatSize << " - 1];\n\n"
            << "\tfunction automatic logic [W-1:0] mul(\n"
            << "\t\tinput int i,\n"
            << "\t\tinput int j,\n"
            << "\t\tinput logic [W-1:0] A[],\n"
            << "\t\tinput logic [W-1:0] B[]\n"
            << "\t);\n"
            << "\t\tlogic [2*W-1] temp;\n"
            << "\t\ttemp = A[i] * B[j];\n"
            << "\t\tmul = temp[2*W-2:W-1];\n"
            << "\tendfunction\n\n";


       while (line.length() > 0) {

        std::string logicNet = "UADA";
        std::string mat1 = "UA";
        std::string mat2 = "DA";

        int matMult { 0 }, a { 0 }, b { 0 };
        char operand;
        bool first = true;
        
        std::istringstream iss(line);

        if (line[0] == ' ') {
            matMult++;
            writeFile << "\n\n";
            switch (matMult) {
                case 1:
                    logicNet = "VAUB";
                    mat1 = "VA";
                    mat2 = "UB";
                    break;
                case 2:
                    logicNet = "DBVB";
                    mat1 = "DB";
                    mat2 = "VB";
                    break;
                case 3:
                    logicNet = "VUDV";
                    mat1 = "UADA";
                    mat2 = "VAUB";
                    break;
                case 4:
                    logicNet = "C";
                    mat1 = "DBVB";
                    mat2 = "VUDV";
                    break;    
            }
        }
        else {
            writeFile << "assign " << logicNet << "[ " << i << " ] = ";
            while (iss >> a >> operand >> b) {
                if (first) {first = false;}
                else {
                    writeFile << " + ";
                    writeFile << "mul(" << a << "," << b << "," << mat1 << "," << mat2 << ")";
                }
            }
            writeFile << "\n";
        }
        i++;
    }
       writeFile << "\nendmodule\n";
    }
    else {
        std::cerr << "Error: could not open file/s: "
            << (!readFile.is_open() ? inputFile : "")
            << (!writeFile.is_open() ? outputFile : "")
            << std::endl;
    }
    return 0;
}
