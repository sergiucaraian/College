#include <iostream>
#include <fstream>
#include <stdexcept>
using namespace std;

ifstream fin ("input.in");
ofstream fout ("output.out");

////////////////////////
/// Complex Number ////
//////////////////////

class ComplexNumber
{
    double realPart;
    double imaginaryPart;

public:
    ComplexNumber(double realPartArg, double imaginaryPartArg) { realPart = realPartArg; imaginaryPart = imaginaryPartArg; }
    ComplexNumber(const ComplexNumber& originalNumber) { realPart = originalNumber.realPart; imaginaryPart = originalNumber.imaginaryPart; }
    ComplexNumber() { realPart = 0; imaginaryPart = 0;}
    double getReal() { return realPart; }
    double getImaginary() { return imaginaryPart; }
    void setReal(double realPartArg) { realPart = realPartArg; }
    void setImaginary(double imaginaryPartArg) { imaginaryPart = imaginaryPartArg; }

    ComplexNumber operator*(double);
    ComplexNumber operator*(ComplexNumber);
    ComplexNumber operator+(ComplexNumber);
    ComplexNumber operator/(ComplexNumber);

    friend istream& operator>> (istream&, ComplexNumber&);
    friend ostream& operator<< (ostream&, const ComplexNumber);
    friend class Matrix;
};

istream& operator>> (istream& inputStream, ComplexNumber& complexNumberArg)
{
    inputStream>>complexNumberArg.realPart>>complexNumberArg.imaginaryPart;
    return inputStream;
}

ostream& operator<< (ostream& outputStream, const ComplexNumber complexNumberArg)
{
    outputStream<<complexNumberArg.realPart<<" "<<complexNumberArg.imaginaryPart<<"i";
    return outputStream;
}

ComplexNumber ComplexNumber::operator* (double numberArg)
{
    ComplexNumber C (realPart*numberArg, imaginaryPart*numberArg);
    return C;
}

ComplexNumber ComplexNumber::operator* (ComplexNumber B)
{
    double newRealPart = realPart*B.getReal()-imaginaryPart*B.getImaginary();
    double newImaginaryPart = imaginaryPart*B.getReal()+realPart*B.getImaginary();
    if (newImaginaryPart == -0)
        newImaginaryPart = 0;
    ComplexNumber C (newRealPart,newImaginaryPart);
    return C;
}

ComplexNumber ComplexNumber::operator+ (ComplexNumber B)
{
    ComplexNumber C (realPart+B.getReal(),imaginaryPart+B.getImaginary());
    return C;
}

ComplexNumber ComplexNumber::operator/ (ComplexNumber B)
{
    ComplexNumber B_Conjugated(B.realPart,B.imaginaryPart*(-1));
    ComplexNumber A(realPart,imaginaryPart);
    ComplexNumber C = A*B_Conjugated;
    ComplexNumber D = B*B_Conjugated;

    ComplexNumber Result;
    Result.realPart = C.realPart/D.realPart;
    if (D.imaginaryPart!=0)
        Result.imaginaryPart = C.imaginaryPart/D.imaginaryPart;
    else Result.imaginaryPart = 0;
    return Result;
}

///////////////
/// Node /////
/////////////

class Matrix;

class Node
{
    ComplexNumber value;
    int row, col;
    Node *nextRow;
    Node *nextCol;

public:
    Node(ComplexNumber,int,int,Node*,Node*);

    friend class Matrix;
    friend ostream& operator<< (ostream&, Matrix);
};

Node::Node(ComplexNumber valueArg, int rowArg, int colArg, Node* nextRowArg, Node* nextColArg)
{
    value = valueArg;
    row = rowArg;
    col = colArg;
    nextRow = nextRowArg;
    nextCol = nextColArg;
}

/////////////////
/// Matrix /////
///////////////

class Matrix
{
    int rows;
    int cols;
    Node **topRow;
    Node **topCol;

public:
    Matrix(const Matrix&);
    Matrix() { rows = 0; cols = 0; topRow = NULL; topCol = NULL; }
    Matrix(int,int);

    friend istream& operator>> (istream&,Matrix&);
    friend ostream& operator<< (ostream&,Matrix);
    Matrix& operator= (Matrix);
    void insertion(ComplexNumber,int,int);

    Matrix operator+ (Matrix);
    Matrix operator* (Matrix);
    Matrix operator* (ComplexNumber);

    ComplexNumber determinant();
    Matrix minor(int,int);
    Matrix transpose();
    Matrix inverse();
};

Matrix::Matrix(int rowsArg, int colsArg)
{
    rows = rowsArg;
    cols = colsArg;

    topRow = new Node* [rows];
    for(int i=0; i<rows; ++i)
        topRow[i]=NULL;

    topCol = new Node* [cols];
    for(int i=0; i<cols; ++i)
        topCol[i]=NULL;
}

Matrix::Matrix(const Matrix& originalMatrix)
{
    rows = originalMatrix.rows;
    cols = originalMatrix.cols;

    topRow = new Node* [rows];
    for(int i=0; i<rows; ++i)
        topRow[i]=NULL;

    topCol = new Node* [cols];
    for(int i=0; i<cols; ++i)
        topCol[i]=NULL;

    for (int i=0; i<rows; ++i)
        if(originalMatrix.topRow[i])
        {
            Node* currentNode = originalMatrix.topRow[i];
            while(currentNode)
            {
                insertion(currentNode->value, currentNode->row, currentNode->col);
                currentNode = currentNode->nextCol;
            }
        }
}

Matrix& Matrix::operator= (Matrix originalMatrix)
{
    rows = originalMatrix.rows;
    cols = originalMatrix.cols;

    topRow = new Node* [rows];
    for(int i=0; i<rows; ++i)
        topRow[i]=NULL;

    topCol = new Node* [cols];
    for(int i=0; i<cols; ++i)
        topCol[i]=NULL;

    for (int i=0; i<rows; ++i)
        if(originalMatrix.topRow[i])
        {
            Node* currentNode = originalMatrix.topRow[i];
            while(currentNode)
            {
                insertion(currentNode->value, currentNode->row, currentNode->col);
                currentNode = currentNode->nextCol;
            }
        }

    return *this;
}

void Matrix::insertion( ComplexNumber numberArg, int rowArg, int colArg )
{
    Node *myNode = new Node(numberArg,rowArg,colArg,NULL,NULL);

    //Inserting in the proper Row
    if(!topRow[rowArg])
        topRow[rowArg] = myNode;
    else if(topRow[rowArg]->col == colArg)
        topRow[rowArg] = myNode;
    else if(topRow[rowArg]->col > colArg)
    {
        myNode->nextCol = topRow[rowArg];
        topRow[rowArg] = myNode;
    }
    else
    {
        Node* currentNode = topRow[rowArg];
        while(currentNode->nextCol!=NULL && currentNode->nextCol->col < colArg)
            currentNode = currentNode->nextCol;
        if(currentNode->nextCol)
        {
            if (currentNode->nextCol->col == colArg)
            {
                myNode->nextCol = currentNode->nextCol->nextCol;
                myNode->nextRow = currentNode->nextCol->nextRow;
                currentNode->nextCol = myNode;
            }
            else
            {
                myNode->nextCol = currentNode->nextCol;
                currentNode->nextCol = myNode;
            }
        }
        else
        {
            myNode->nextCol = NULL;
            currentNode->nextCol = myNode;
        }
    }

    //Inserting in the proper column
    if(!topCol[colArg])
        topCol[colArg] = myNode;
    else if(topCol[colArg]->row == rowArg)
        topRow[colArg] = myNode;
    else if(topCol[colArg]->row > rowArg)
    {
        myNode->nextRow = topCol[colArg];
        topCol[colArg] = myNode;
    }
    else
    {
        Node* currentNode = topCol[colArg];
        while(currentNode->nextRow != NULL && currentNode->nextRow->row < rowArg)
            currentNode = currentNode->nextRow;
        if(currentNode->nextRow)
        {
            if (currentNode->nextRow->row == rowArg)
            {
                myNode->nextRow = currentNode->nextRow->nextRow;
                myNode->nextCol = currentNode->nextRow->nextCol;
                currentNode->nextRow = myNode;
            }
            else
            {
                myNode->nextRow = currentNode->nextRow;
                currentNode->nextRow = myNode;
            }
        }
        else
        {
            myNode->nextRow = NULL;
            currentNode->nextRow = myNode;
        }
    }
}

istream& operator>> (istream& inputStream, Matrix& matrixArg)
{
    int nrOfElements;
    inputStream>>matrixArg.rows>>matrixArg.cols>>nrOfElements;

    matrixArg.topRow = new Node* [matrixArg.rows];
    for(int i=0; i<matrixArg.rows; ++i)
        matrixArg.topRow[i]=NULL;

    matrixArg.topCol = new Node* [matrixArg.cols];
    for(int i=0; i<matrixArg.cols; ++i)
        matrixArg.topCol[i]=NULL;

    for(int i=0; i<nrOfElements; ++i)
    {
        int elementRow, elementCol;
        ComplexNumber elementValue;
        inputStream>>elementRow>>elementCol>>elementValue;
        matrixArg.insertion(elementValue,elementRow,elementCol);
    }
    return inputStream;
}

ostream& operator<< (ostream& outputStream, Matrix matrixArg)
{
    for(int i=0; i<matrixArg.rows; ++i)
    {
        if(!matrixArg.topRow[i])
        {
            for(int j=0; j<matrixArg.cols; ++j)
                outputStream<<0<<" "<<0<<"i"<<" ";
            outputStream<<'\n';
        }
        else
        {
            Node *currentNode = matrixArg.topRow[i];
            for(int j=0; j<matrixArg.cols; ++j)
            {
                if(currentNode && currentNode->col == j)
                {
                    outputStream<<currentNode->value<<" ";
                    currentNode = currentNode->nextCol;
                }
                else outputStream<<0<<" "<<0<<"i"<<" ";
            }
            outputStream<<'\n';
        }
    }
    return outputStream;
}

Matrix Matrix::operator+(Matrix B)
{
    Matrix C (max(rows,B.rows),max(cols,B.cols));

    for(int i=0; i < min(rows,B.rows); ++i)
    {
        Node *currentNodeA = topRow[i];
        Node *currentNodeB = B.topRow[i];

        while(currentNodeA && currentNodeB)
        {
            if(currentNodeA->col < currentNodeB->col)
            {
                C.insertion(currentNodeA->value,currentNodeA->row,currentNodeA->col);
                currentNodeA = currentNodeA->nextCol;
            }
            else if (currentNodeA->col > currentNodeB->col)
            {
                C.insertion(currentNodeB->value,currentNodeB->row,currentNodeB->col);
                currentNodeB = currentNodeB->nextCol;
            }
            else if (currentNodeA->col == currentNodeB->col)
            {
                C.insertion(currentNodeA->value + currentNodeB->value, currentNodeA->row, currentNodeA->col);
                currentNodeA = currentNodeA->nextCol;
                currentNodeB = currentNodeB->nextCol;
            }
        }
        while(currentNodeA)
        {
            C.insertion(currentNodeA->value, currentNodeA->row, currentNodeA->col);
            currentNodeA = currentNodeA->nextCol;
        }
        while(currentNodeB)
        {
            C.insertion(currentNodeB->value, currentNodeB->row, currentNodeB->col);
            currentNodeB = currentNodeB->nextCol;
        }
    }
    return C;
}

Matrix Matrix::operator*(Matrix B)
{
    if(cols!=B.rows)
    {
        throw std::runtime_error("Can't multiply two matrices of different width and height");
    }

    Matrix C(rows,B.cols);
    for(int i=0; i<rows; ++i)
    {
        if(topRow[i])
        {
            for(int j=0; j<B.cols; ++j)
            {
                if(B.topCol[j])
                {
                    ComplexNumber newValue;
                    Node *currentNodeA = topRow[i];
                    Node *currentNodeB = B.topCol[j];

                    while(currentNodeA && currentNodeB)
                    {
                        if(currentNodeA->col == currentNodeB->row)
                        {
                            newValue = currentNodeA->value * currentNodeB->value;
                            currentNodeA = currentNodeA->nextCol;
                            currentNodeB = currentNodeB->nextRow;
                        }
                        else if(currentNodeA->col < currentNodeB->row)
                            currentNodeA = currentNodeA->nextCol;
                        else if(currentNodeB->row < currentNodeB->col)
                            currentNodeB = currentNodeB->nextRow;
                    }

                    C.insertion(newValue,i,j);
                }
            }
        }
    }
    return C;
}

Matrix Matrix::operator*(ComplexNumber numberArg)
{
    Matrix M(rows,cols);
    for (int i=0; i<rows; ++i)
    {
        Node *currentNode = topRow[i];
        while(currentNode)
        {
            M.insertion (currentNode->value * numberArg, currentNode->row, currentNode->col);
            currentNode = currentNode->nextCol;
        }
    }
    return M;
}

ComplexNumber Matrix::determinant()
{
    if(rows!=cols)
    {
        throw std::runtime_error("This function only works on square matrices");
    }
    else
    {
        if(rows==1)
        {
            ComplexNumber C(0,0);
            if(topRow[0])
                C  = topRow[0]->value;
            return C;
        }

        else if(rows==2)
        {
            ComplexNumber A,B,C,D;

            if(topRow[0] && !topRow[0]->nextCol)
            {
                if(topRow[0]->col==0)
                    A = topRow[0]->value;
                else B = topRow[0]->value;
            }
            else if(topRow[0] && topRow[0]->nextCol)
            {
                A = topRow[0]->value;
                B = topRow[0]->nextCol->value;
            }

            if(topRow[1] && !topRow[1]->nextCol)
            {
                if(topRow[1]->col==0)
                    C = topRow[1]->value;
                else D = topRow[1]->value;
            }
            else if(topRow[1] && topRow[1]->nextCol)
            {
                C = topRow[1]->value;
                D = topRow[1]->nextCol->value;
            }

            ComplexNumber Result = A*D + B*C*(-1);
            return Result;
        }

        else
        {
            ComplexNumber Result;
            int putere = -1;
            for(int i=0; i<rows; ++i)
            {
                putere*=-1;
                if(topRow[i] && topRow[i]->col==0)
                    Result = Result + (topRow[i]->value * putere * minor(i,0).determinant());
            }
            return Result;
        }
    }
}

Matrix Matrix::minor(int rowArg, int colArg)
{
    Matrix M(rows-1,cols-1);
    for(int i=0; i<rows; ++i)
    {
        Node* currentNode = topRow[i];
        while(currentNode)
        {
            if(currentNode->row != rowArg && currentNode->col != colArg)
            {
                if(currentNode->row < rowArg && currentNode->col < colArg)
                    M.insertion(currentNode->value, currentNode->row, currentNode->col);
                else if (currentNode->row < rowArg && currentNode->col > colArg)
                    M.insertion(currentNode->value, currentNode->row, currentNode->col-1);
                else if (currentNode->row > rowArg && currentNode->col < colArg)
                    M.insertion(currentNode->value, currentNode->row-1, currentNode->col);
                else if (currentNode->row > rowArg && currentNode->col > colArg)
                    M.insertion(currentNode->value, currentNode->row-1, currentNode->col-1);
            }
            currentNode = currentNode->nextCol;
        }
    }
    return M;
}

Matrix Matrix::transpose()
{
    Matrix M(rows,cols);
    for(int i=0; i<rows; ++i)
    {
        Node *currentNode = topRow[i];
        while(currentNode)
        {
            M.insertion(currentNode->value, currentNode->col, currentNode->row);
            currentNode = currentNode->nextCol;
        }
    }
    return M;
}

Matrix Matrix::inverse()
{
    if(rows!=cols)
    {
        throw std::runtime_error("Inverse function only works on square matrices");
    }
    else if (determinant().realPart==0 && determinant().imaginaryPart==0)
    {
        throw std::runtime_error("This determinant of the matrix must not be 0 in order to have an inverse");
    }
    else
    {
        Matrix M = transpose();
        Matrix Final(rows,cols);

        for(int i=0; i<rows; ++i)
            for(int j=0; j<cols; ++j)
            {
                int putere;
                if((i+j)%2==0)
                    putere = 1;
                else putere = -1;

                ComplexNumber C = M.transpose().minor(i,j).determinant() * putere;

                if(C.realPart!=0 || C.imaginaryPart!=0)
                    Final.insertion(C,i,j);
            }

        ComplexNumber A(1,0);
        ComplexNumber B = A/determinant();

        Final = Final*B;
        return Final;
    }
}

int main()
{
    Matrix A;
    fin>>A;

    fout<<A.inverse();
    return 0;
}
