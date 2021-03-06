/*******************************************************************************
 * SIMPLEX - A simplex algorithm implementation.
 * Copyright (C) 2013 Daniele Pallastrelli
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

#include <iostream>
#include <valarray>
#include <limits>
#include <cassert>

template < typename T >
class Table
{
public:
    Table( size_t rows, size_t cols ) : 
        row_num( rows ),
        col_num( cols ),
        data( rows * cols )
    {}
    T& operator()( size_t row, size_t column )
    {
        assert( row < row_num );
        assert( column < col_num );
        // column major
        return data[ column + row * col_num ];
    }
    T operator()( size_t row, size_t column ) const
    {
        assert( row < row_num );
        assert( column < col_num );
        // column major
        return data[ column + row * col_num ];
    }
    T& Value( size_t row, size_t col ) { return operator()( row, col ); }
    T Value( size_t row, size_t col ) const { return operator()( row, col ); }
    size_t Columns() const { return col_num; }
    size_t Rows() const { return row_num; }
    void Print() const
    {
        for ( size_t i = 0; i < data.size(); ++i )
        {
            if ( i % col_num == 0 && col_num != 0 ) std::cout << "\n";
            std::cout << data[ i ] << "\t";
        }
    }
private:
    const size_t row_num;
    const size_t col_num;
    typedef std::valarray< T > Data;
    Data data;
};

template < typename T >
class Matrix : public Table< T >
{
public:
    Matrix( size_t rows, size_t cols ) : Table< T >( rows, cols ) {}
    void DivideRow( size_t row, T d )
    {
        assert( row < Rows() );
        for ( size_t c = 0; c < Columns(); ++c )
            Value( row, c ) /= d;
    }
    void Linear( size_t r, size_t pivRow, T coeff )
    {
        assert( r < Rows() );
        assert( pivRow  < Rows() );
        for ( size_t c = 0; c < Columns(); ++c )
            Value( r, c ) += Value( pivRow, c )* coeff;
    }
};

/*
Minimize
    c x
Subject to
    A x = b, x_i >= 0
    
Canonical Tableau:
    1   -tr(c)  0
    0   A       b
    
Algorithm:
    Prendere un valore della prima riga positivo (se non � positivo: fine)
    Trovare la riga per cui � minimo il rapporto tra il valore dell'ultima colonna e il valore della colonna scelta
    Il pivot � l'incrocio tra riga e colonna scelta
    Dividere la riga scelta per il pivot
    Far diventare 0 ogni valore della colonna scelta tranne il pivot (con combinazioni lineari)
    Ripetere finch� c'� almeno un valore negativo nella prima riga
*/
template < typename T >
class SimplexTableau : public Matrix< T >
{
public:
    SimplexTableau( size_t rows, size_t cols ) : Matrix< T >( rows, cols ) {}
    void SetRow( size_t row_index, const T row[] )
    {
        assert( row_index < Rows() );
        for ( size_t i = 0; i < Columns(); ++i )
            Value( row_index, i ) = row[ i ];
    }
    bool IsValid() const
    {
        return true;
    }
    void Solve( size_t maxIter )
    {
        for ( size_t count = 0; count < maxIter; ++count )
        {
            // find pivot column:
            int pivCol = FindPivotColumn();

            if ( pivCol == -1 ) return;
            assert( pivCol >= 0 && pivCol < Columns() );

            size_t pivRow = 0;
            
            // find minimum ratio
            double minRatio = std::numeric_limits< double >::max();
            for ( size_t r = 1; r < Rows(); ++r )
            {
                if ( Value( r, pivCol ) == 0 ) continue;
                double ratio = Value( r, Columns() - 1 ) / Value( r, pivCol );
                if ( ratio < minRatio )
                {
                    minRatio = ratio;
                    pivRow = r;
                }
            }
            assert( pivRow < Rows() );

            // divide pivot row for pivot value
            DivideRow( pivRow, Value( pivRow, pivCol ) );
            
            // scale other rows
            for ( size_t r = 0; r < Rows(); ++r )
            {
                if ( r != pivRow )
                {
                    const T coeff = - Value( r, pivCol );
                    Linear( r, pivRow, coeff );
                }
            }
            
            Print();
            std::cout << std::endl;
        }
    }
private:
    // returns -1 if no negative value found
    int FindPivotColumn() const
    {
        for ( size_t i = 1; i < Columns(); ++i )
            if ( Value( 0, i ) > 0 ) return i;
        return -1;
    }
};

int main()
{
    using namespace std;

    /*
    example
    minimize:
        Z = -2x -3y -4z
    subject to:
        3x + 2y + z <= 10
        2x + 5y + 3z <= 15
        x,y,z >= 0
    
    canonical tableau:
        1 2 3 4 0 0 0
        0 3 2 1 1 0 10
        0 2 5 3 0 1 15
    */
    
    SimplexTableau< double > st( 3, 7 );

    const double row1[] = { 1, 2, 3, 4, 0, 0, 0 };
    st.SetRow( 0, row1 );
    const double row2[] = { 0, 3, 2, 1, 1, 0, 10 };
    st.SetRow( 1, row2 );
    const double row3[] = { 0, 2, 5, 3, 0, 1, 15 };
    st.SetRow( 2, row3 );
    
    assert( st.IsValid() );
    
    st.Solve( 10 );
    
    st.Print();
    
    return 0;
}
