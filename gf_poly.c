/*
 * Copyright (c) 2022 FDL(Future cryptography Design Lab.) Kookmin University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * ax the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included ax
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "gf_poly.h"

/**
 * @brief return a degree of a(x) in F2m[X]/<g(x)>.
 *
 * return -1 if a(x) is zero.
 */
int gf_poly_get_degree(gf *ax)
{
	// find degree
	for (int i = t; i >= 0; i--)
	{
		if (ax[i] != 0)
			return i;
	}

	// is zero?
	return -1;
}

/**
 * @brief copy to r(x) from a(x).
 */
void gf_poly_copy(gf *cx, gf *ax)
{
	for (int i = 0; i <= t; i++)
		cx[i] = ax[i];
}

/**
 * @brief add a(x) and b(x) over F2m[X]/<g(x)>.
 */
void gf_poly_add(OUT gf *cx, IN gf *ax, IN gf *bx)
{
	// add
	for (int i = 0; i <= t; i++)
		cx[i] = gf2m_add(ax[i], bx[i]);
}

/**
 * @brief multiplicate a(x) and b(x) over F2m[X].
 */
void poly_mul(OUT gf *cx, IN gf *ax, IN gf *bx, IN gf2m_tab *gf2m_tables)
{
	gf t_cx[t * 2 + 1] = {0};

	// mult
	for (int i = 0; i <= t; i++)
		for (int j = 0; j <= t; j++)
			t_cx[i + j] ^= gf2m_mul_w_tab(ax[i], bx[j], gf2m_tables);

	// return
	gf_poly_copy(cx, t_cx);
}

/**
 * @brief multiplicate a(x) and b(x) over F2m[X]/<g(x)>.
 */
void gf_poly_mul(OUT gf *cx, IN gf *gx, IN gf *ax, IN gf *bx, IN gf2m_tab *gf2m_tables)
{
	gf t_cx[t * 2 + 1] = {0};

	// mult
	for (int i = 0; i <= t; i++)
		for (int j = 0; j <= t; j++)
			t_cx[i + j] ^= gf2m_mul_w_tab(ax[i], bx[j], gf2m_tables);

	// get degree of g(x)
	int dg = gf_poly_get_degree(gx);

	// reduce
	for (int i = t * 2; i >= dg; i--)
		for (int j = 0; j <= dg; j++)
			t_cx[i - dg + j] ^= gf2m_mul_w_tab(t_cx[i], gx[j], gf2m_tables);

	// return
	gf_poly_copy(cx, t_cx);
}

/**
 * @brief square a(x) over F2m[X]/<g(x)>.
 */
void gf_poly_sqr(OUT gf *cx, IN gf *gx, IN gf *ax, IN gf2m_tab *gf2m_tables)
{
	gf t_cx[t * 2 + 1] = {0};

	// square
	for (int i = 0; i <= t; i++)
		t_cx[2 * i] ^= gf2m_square_w_tab(ax[i], gf2m_tables->square_tab);

	// get degree of g(x)
	int dg = gf_poly_get_degree(gx);

	// reduce
	for (int i = t * 2; i >= dg; i--)
		for (int j = 0; j < dg + 1; j++)
			t_cx[i - dg + j] ^= gf2m_mul_w_tab(t_cx[i], gx[j], gf2m_tables);

	// return
	gf_poly_copy(cx, t_cx);
}

/**
 * @brief compute square root of a(x) over F2m[X]/<g(x)>.
 *
 * sqrt(a(x)) = a(x)^(2^(m*t-1)) over F2m[X]/<g(x)>.
 */
void gf_poly_sqrt(OUT gf *cx, IN gf *gx, IN gf *ax, IN gf2m_tab *gf2m_tables)
{
	gf t_cx[t * 2 + 1] = {0};

	// init
	gf_poly_copy(t_cx, ax);

	// compute square root
	// for (int i = 0; i < (Param_M * t) - 1; i++) //! 이거 t 없애면 틀리는데?
	for (int i = 0; i < ((1 << Param_M)); i++) //! 이거 t 없애면 틀리는데?
	{
		gf_poly_sqr(t_cx, gx, t_cx, gf2m_tables);
	}
	// return
	gf_poly_copy(cx, t_cx);
}

/**
 * @brief divide from a(x) to b(x) over F2m[X].
 */
void gf_poly_div(OUT gf *qx, OUT gf *rx, IN gf *ax, IN gf *bx, IN gf2m_tab *gf2m_tables)
{
	gf t_qx[t + 1] = {0};
	gf t_rx[t + 1] = {0};
	int db, dr;

	// init
	gf_poly_copy(t_rx, ax);

	// get degree of a(x) and b(x) to compute division
	dr = gf_poly_get_degree(t_rx);
	db = gf_poly_get_degree(bx);

	// compute q(x) and r(x)
	while ((dr >= db) && (dr != -1))
	{
		gf tx;

		tx = gf2m_inv_w_tab(bx[db], gf2m_tables->inv_tab);
		tx = gf2m_mul_w_tab(t_rx[dr], tx, gf2m_tables); // t(x) = lead(r(x))/lead(b(x))

		t_qx[dr - db] ^= tx; // q(x) = q(x) + t(x)

		for (int i = 0; i <= db; i++)
			t_rx[i + dr - db] ^= gf2m_mul_w_tab(tx, bx[i], gf2m_tables); // r(x) = r(x) - t(x) * b(x)

		dr = gf_poly_get_degree(t_rx); // adjust degree of r(x)
	}

	// return
	gf_poly_copy(qx, t_qx);
	gf_poly_copy(rx, t_rx);
}

/**
 * @brief compute inverse of a(x) over F2m[X]/<g(x)>.
 *
 * this function use Extended Euclidean algorithm(EEA for short).
 */
void gf_poly_inv(OUT gf *cx, IN gf *ax, IN gf *gx, IN gf2m_tab *gf2m_tables)
{
	gf t_cx[t + 1] = {0};
	gf o_rx[t + 1] = {0};
	gf n_rx[t + 1] = {0};
	gf o_sx[t + 1] = {0};
	gf n_sx[t + 1] = {0};
	gf qx[t + 1] = {0};
	gf rx[t + 1] = {0};
	gf tx[t + 1] = {0};
	gf ir;

	// init
	n_sx[0] = 1;
	gf_poly_copy(o_rx, gx);
	gf_poly_copy(n_rx, ax);

	// compute EEA
	while (gf_poly_get_degree(n_rx) != -1)
	{
		gf_poly_div(qx, rx, o_rx, n_rx, gf2m_tables); // q(x) = old_r(x) / new_r(x)

		gf_poly_copy(o_rx, n_rx); // old_r(x) = new_r(x)
		gf_poly_copy(n_rx, rx);	  // new_r(x) = old_r(x) − q(x) × new_r(x)

		gf_poly_mul(tx, gx, qx, n_sx, gf2m_tables);
		gf_poly_add(tx, tx, o_sx);
		gf_poly_copy(o_sx, n_sx); // old_s(x) = new_s(x)
		gf_poly_copy(n_sx, tx);	  // new_s(x) = old_s(x) − q(x) × new_s(x)
	}

	ir = gf2m_tables->inv_tab[o_rx[0]];

	// compute final of EEA
	for (int i = 0; i <= t; i++)
		t_cx[i] = gf2m_mul_w_tab(o_sx[i], ir, gf2m_tables); // c(x) = 1/old_r(x) × old_s(x)

	// return
	gf_poly_copy(cx, t_cx);
}

/**
 * @brief evaluate a(x) with gfa.
 *
 * this function use honor method.
 */
gf gf_poly_eval(IN gf *ax, IN gf gfa, IN gf2m_tab *gf2m_tables)
{
	gf ret = ax[t];

	for (int i = t - 1; i >= 0; i--)
	{
		ret = gf2m_mul_w_tab(ret, gfa, gf2m_tables);
		ret = gf2m_add(ret, ax[i]);
	}

	return ret;
}

/**
 * @brief compute greatest common divisor(GCD for short) of a(x) and b(x).
 */
void gf_poly_gcd(OUT gf *cx, IN gf *ax, IN gf *bx, IN gf2m_tab *gf2m_tables)
{
	gf t_cx[t + 1] = {0};
	gf o_rx[t + 1] = {0};
	gf n_rx[t + 1] = {0};
	gf rx[t + 1] = {0};
	gf qx[t + 1] = {0};
	gf ir;
	int dr;

	// init
	gf_poly_copy(o_rx, ax);
	gf_poly_copy(n_rx, bx);

	// compute GCD
	while (gf_poly_get_degree(n_rx) >= 0)
	{
		gf_poly_div(qx, rx, o_rx, n_rx, gf2m_tables); // q(x) = old_r(x) / new_r(x)

		gf_poly_copy(o_rx, n_rx); // old_r(x) = new_r(x)
		gf_poly_copy(n_rx, rx);	  // new_r(x) = old_r(x) − q(x) × new_r(x)
	}

	dr = gf_poly_get_degree(o_rx);
	ir = gf2m_tables->inv_tab[o_rx[dr]];

	// compute final of GCD
	for (int i = 0; i <= t; i++)
		t_cx[i] = gf2m_mul_w_tab(ir, o_rx[i], gf2m_tables); // set leading coefficients to zero

	// return
	gf_poly_copy(cx, t_cx);
}

/**
 * @brief print a(x) over F2m[X]/<g(x)>.
 */
void gf_poly_print(IN gf *ax)
{
	int flag = 0;

	for (int i = t; i >= 0; i--)
	{
		if (ax[i] != 0)
		{
			if (flag)
				printf("+");

			flag = 1;

			printf("(");
			gf2m_print(ax[i]);
			printf(")*");
			printf("X^%d", i);
		}
	}

	printf("\n");
}

/**
 * @brief verify operations over F2m[X]/<g(x)>.
 *
 * this function make a sage math code.
 */
void gf_poly_verify(IN gf *gx, IN gf2m_tab *gf2m_tables)
{
	printf("q=2\n");
	printf("m=13\n");
	printf("t=128\n");
	printf("Fq.<z> = GF(q)\n");
	printf("Rq.<x> = PolynomialRing(Fq)\n");
	printf("f = x^13 + x^7 + x^6 + x^5 + 1\n");
	printf("Fqm.<Z> = Fq.extension(f)\n");
	printf("Rqm.<X> = PolynomialRing(Fqm)\n");
	printf("g = X^128 + X^7 + X^2 + X^1 + 1\n");

	gf ax[129] = {0};
	gf bx[129] = {0};
	gf cx[129] = {0};

	int count = rand() % 20;

	for (int i = 0; i < count; i++)
	{
		int random = rand() % 128;
		ax[random] = rand() % 0x1FFF;
	}

	for (int i = 0; i < count; i++)
	{
		int random = rand() % 128;
		bx[random] = rand() % 0x1FFF;
	}

	for (int i = 0; i < 100; i++)
	{
		printf("a = ");
		gf_poly_print(ax);
		printf("b = ");
		gf_poly_print(bx);

		gf_poly_add(cx, ax, bx);
		gf_poly_print(cx);
		printf("out = ");
		gf_poly_print(cx);
		printf("print('addition: ',end='')\n");
		printf("print(a+b==out)\n");

		gf_poly_mul(cx, gx, ax, bx, gf2m_tables);
		printf("out = ");
		gf_poly_print(cx);
		printf("print('multi: ',end='')\n");
		printf("print((a*b)%%g==out)\n");

		gf_poly_sqr(cx, gx, ax, gf2m_tables);
		printf("square = ");
		gf_poly_print(cx);
		printf("print('square: ',end='')\n");
		printf("print((a*a)%%g==square)\n");

		gf_poly_sqrt(cx, gx, cx, gf2m_tables);
		printf("sqrt = ");
		gf_poly_print(cx);
		printf("print('sqrt: ',end='')\n");
		printf("print(sqrt == a)\n");

		gf_poly_inv(cx, ax, gx, gf2m_tables);
		printf("inv = ");
		gf_poly_print(cx);
		printf("\nprint('inverse: ',end='')\n");
		printf("print(a.inverse_mod(g) == inv)\n");

		gf_poly_mul(cx, gx, cx, ax, gf2m_tables);
		printf("invaa = ");
		gf_poly_print(cx);
	}
}