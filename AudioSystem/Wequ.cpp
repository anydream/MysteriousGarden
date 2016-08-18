/************************************************************************/
// 创建时间: 081123
// 最后确定: 081123

#include "../AnyPlay.h"
#include "wequ.h"
#pragma warning(disable: 4305)
#pragma warning(disable: 4244)

namespace AnyPlay
{
	paramlistelm::paramlistelm(void)
	{
		left = right = 1;
		lower = upper = gain = 0;
		next = NULL;
	}

	paramlistelm::~paramlistelm()
	{
		delete next;
		next = NULL;
	}

	paramlist::paramlist(void)
	{
		elm = NULL;
	}

	paramlist::~paramlist()
	{
		delete elm;
		elm = NULL;
	}



	void rfft(int n,int isign,REAL x[]);

#define M 15

#define PI 3.1415926535897932384626433832795

#define RINT(x) ((x) >= 0 ? ((int)((x) + 0.5)) : ((int)((x) - 0.5)))

#define DITHERLEN 65536


	static REAL fact[M+1];
	static REAL aa = 96;
	static REAL iza;
	static REAL *lires,*lires1,*lires2,*rires,*rires1,*rires2,*irest;
	static REAL *fsamples;


	static volatile int chg_ires,cur_ires;
	static int winlen,winlenbit,tabsize,nbufsamples;
	static short *inbuf;
	static REAL *outbuf;
	static int maxamp;


	int weq_enable = 1;


#define NCH 2

#define NBANDS 17

	static REAL bands[] =
	{
		65.406392,92.498606,130.81278,184.99721,261.62557,369.99442,523.25113,
		739.9884 ,1046.5023,1479.9768,2093.0045,2959.9536,4186.0091,5919.9072,
		8372.0181,11839.814,16744.036
	};


	void bitrv2(int n, int *ip, REAL *a);
	void bitrv216(REAL *a);
	void bitrv208(REAL *a);
	void cftf1st(int n, REAL *a, REAL *w);
	void cftrec1(int n, REAL *a, int nw, REAL *w);
	void cftrec2(int n, REAL *a, int nw, REAL *w);
	void cftexp1(int n, REAL *a, int nw, REAL *w);
	void cftexp2(int n, REAL *a, int nw, REAL *w);
	void cftfx41(int n, REAL *a, int nw, REAL *w);
	void cftfx42(int n, REAL *a, int nw, REAL *w);
	void cftmdl1(int n, REAL *a, REAL *w);
	void cftmdl2(int n, REAL *a, REAL *w);
	void cftf161(REAL *a, REAL *w);
	void cftf162(REAL *a, REAL *w);
	void cftf081(REAL *a, REAL *w);
	void cftf082(REAL *a, REAL *w);
	void cftf040(REAL *a);
	void cftx020(REAL *a);


	void bitrv2conj(int n, int *ip, REAL *a);
	void bitrv216neg(REAL *a);
	void bitrv208neg(REAL *a);
	void cftb1st(int n, REAL *a, REAL *w);
	void cftb040(REAL *a);
	void makewt(int nw, int *ip, REAL *w);
	void makect(int nc, int *ip, REAL *c);
	void cftfsub(int n, REAL *a, int *ip, int nw, REAL *w);
	void cftbsub(int n, REAL *a, int *ip, int nw, REAL *w);
	void rftfsub(int n, REAL *a, int nc, REAL *c);
	void rftbsub(int n, REAL *a, int nc, REAL *c);


	static REAL alpha(REAL a)
	{
		if (a <= 21) return 0;
		if (a <= 50) return (REAL)  ( 0.5842*pow((REAL)a-21,(REAL)0.4)+0.07886*(a-21));
		return (REAL) (0.1102*(a-8.7));
	}


	static REAL izero(REAL x)
	{
		REAL ret = 1;
		int m;

		for(m=1;m<=M;m++)
		{
			REAL t;
			t = (REAL)  pow(x/2,m)/fact[m];
			ret += t*t;
		}

		return ret;
	}

	void equ_init(int wb)
	{
		int i,j;

		if (lires1 != NULL)   LocalFree(lires1);
		if (lires2 != NULL)   LocalFree(lires2);
		if (rires1 != NULL)   LocalFree(rires1);
		if (rires2 != NULL)   LocalFree(rires2);
		if (irest != NULL)    LocalFree(irest);
		if (fsamples != NULL) LocalFree(fsamples);
		if (inbuf != NULL)    LocalFree(inbuf);
		if (outbuf != NULL)   LocalFree(outbuf);


		winlen = (1 << (wb-1))-1;
		winlenbit = wb;
		tabsize  = 1 << wb;

		lires1   = (REAL *)  LocalAlloc(0, sizeof(REAL)*tabsize);
		lires2   = (REAL *)  LocalAlloc(0, sizeof(REAL)*tabsize);
		rires1   = (REAL *)  LocalAlloc(0, sizeof(REAL)*tabsize);
		rires2   = (REAL *)  LocalAlloc(0, sizeof(REAL)*tabsize);
		irest    = (REAL *)  LocalAlloc(0, sizeof(REAL)*tabsize);
		fsamples = (REAL *) LocalAlloc(0, sizeof(REAL)*tabsize);
		inbuf    = (short *) LocalAlloc(LPTR, winlen*NCH * sizeof(int));
		outbuf   = (REAL *) LocalAlloc(LPTR, tabsize*NCH * sizeof(REAL));


		lires = lires1;
		rires = rires1;
		cur_ires = 1;
		chg_ires = 1;



		for(i=0;i<=M;i++)
		{
			fact[i] = 1;
			for(j=1;j<=i;j++) fact[i] *= j;
		}

		iza = izero(alpha(aa));
	}

	static REAL win(REAL n,int N)
	{
		return izero(alpha(aa)*sqrt(1-4*n*n/((N-1)*(N-1))))/iza;
	}

	static REAL sinc(REAL x)
	{
		return x == 0 ? 1 : sin(x)/x;
	}

	static REAL hn_lpf(int n,REAL f,REAL fs)
	{
		REAL t = 1/fs;
		REAL omega = 2*PI*f;
		return 2*f*t*sinc(n*omega*t);
	}

	static REAL hn_imp(int n)
	{
		return n == 0 ? 1.0 : 0.0;
	}

	static REAL hn(int n,paramlist &param2,REAL fs)
	{
		paramlistelm *e;
		REAL ret,lhn;

		lhn = hn_lpf(n,param2.elm->upper,fs);
		ret = param2.elm->gain*lhn;

		for(e=param2.elm->next;e->next != NULL && e->upper < fs/2;e = e->next)
		{
			REAL lhn2 = hn_lpf(n,e->upper,fs);
			ret += e->gain*(lhn2-lhn);
			lhn = lhn2;
		}

		ret += e->gain*(hn_imp(n)-lhn);

		return ret;
	}

	void process_param(REAL *bc,paramlist *param,paramlist &param2,REAL fs,int ch)
	{
		paramlistelm **pp,*p,*e,*e2;
		int i;

		delete param2.elm;
		param2.elm = NULL;

		for(i=0,pp=&param2.elm;i<=NBANDS;i++,pp = &(*pp)->next)
		{
			(*pp) = new paramlistelm;
			(*pp)->lower = i == 0        ?  0 : bands[i-1];
			(*pp)->upper = i == NBANDS-1 ? fs : bands[i  ];
			(*pp)->gain  = bc[i];
		}

		for(e = param->elm;e != NULL;e = e->next)
		{
			if ((ch == 0 && !e->left) || (ch == 1 && !e->right)) continue;
			if (e->lower >= e->upper) continue;

			for(p=param2.elm;p != NULL;p = p->next)
				if (p->upper > e->lower) break;

			while(p != NULL && p->lower < e->upper)
			{
				if (e->lower <= p->lower && p->upper <= e->upper)
				{
					p->gain *= pow(10,e->gain/20);
					p = p->next;
					continue;
				}
				if (p->lower < e->lower && e->upper < p->upper)
				{
					e2 = new paramlistelm;
					e2->lower = e->upper;
					e2->upper = p->upper;
					e2->gain  = p->gain;
					e2->next  = p->next;
					p->next   = e2;

					e2 = new paramlistelm;
					e2->lower = e->lower;
					e2->upper = e->upper;
					e2->gain  = p->gain * pow(10,e->gain/20);
					e2->next  = p->next;
					p->next   = e2;

					p->upper  = e->lower;

					p = p->next->next->next;
					continue;
				}
				if (p->lower < e->lower)
				{
					e2 = new paramlistelm;
					e2->lower = e->lower;
					e2->upper = p->upper;
					e2->gain  = p->gain * pow(10,e->gain/20);
					e2->next  = p->next;
					p->next   = e2;

					p->upper  = e->lower;
					p = p->next->next;
					continue;
				}
				if (e->upper < p->upper)
				{
					e2 = new paramlistelm;
					e2->lower = e->upper;
					e2->upper = p->upper;
					e2->gain  = p->gain;
					e2->next  = p->next;
					p->next   = e2;

					p->upper  = e->upper;
					p->gain   = p->gain * pow(10,e->gain/20);
					p = p->next->next;
					continue;
				}
				abort();
			}
		}
	}

	void equ_makeTable(REAL *lbc,REAL *rbc,paramlist *param,REAL fs)
	{
		int i,cires = cur_ires;
		REAL *nires;


		if (fs <= 0) return;

		paramlist param2;


		process_param(lbc,param,param2,fs,0);




		for(i=0;i<winlen;i++)
			irest[i] = hn(i-winlen/2,param2,fs)*win(i-winlen/2,winlen);



		for(;i<tabsize;i++)
			irest[i] = 0;


		rfft(tabsize,1,irest);

		nires = cires == 1 ? lires2 : lires1;


		for(i=0;i<tabsize;i++)
			nires[i] = irest[i];





		process_param(rbc,param,param2,fs,1);


		for(i=0;i<winlen;i++)
			irest[i] = hn(i-winlen/2,param2,fs)*win(i-winlen/2,winlen);

		for(;i<tabsize;i++)
			irest[i] = 0;

		rfft(tabsize,1,irest);


		nires = cires == 1 ? rires2 : rires1;


		for(i=0;i<tabsize;i++)
			nires[i] = irest[i];



		chg_ires = cires == 1 ? 2 : 1;
	}

	void equ_quit(void)
	{
		LocalFree(lires1);
		LocalFree(lires2);
		LocalFree(rires1);
		LocalFree(rires2);
		LocalFree(irest);
		LocalFree(fsamples);
		LocalFree(inbuf);
		LocalFree(outbuf);

		lires1   = NULL;
		lires2   = NULL;
		rires1   = NULL;
		rires2   = NULL;
		irest    = NULL;
		fsamples = NULL;
		inbuf    = NULL;
		outbuf   = NULL;

		rfft(0,0,NULL);
	}

	unsigned int equ_latency()
	{
		return tabsize - 4098;

	}

	void equ_clearbuf(int srate)
	{
		int i;

		nbufsamples = 0;
		for(i=0;i<tabsize*NCH;i++) outbuf[i] = 0;
	}

	int equ_modifySamples(char *buf,int nsamples,int nch)
	{
		int i,p,ch;
		REAL *ires;

		static float hm1 = 0, hm2 = 0;

		if (chg_ires)
		{
			cur_ires = chg_ires;
			lires = cur_ires == 1 ? lires1 : lires2;
			rires = cur_ires == 1 ? rires1 : rires2;
			chg_ires = 0;
		}

		p = 0;

		while(nbufsamples + nsamples >= winlen)
		{
			for(i = 0; i < (winlen - nbufsamples) * nch; i++)
			{
				inbuf[nbufsamples*nch+i] = ((short *)buf)[i+p*nch];
				float s = outbuf[nbufsamples*nch+i];

				if (s < -32768) s = -32768;
				if (32767 < s) s = 32767;

				((short *)buf)[i+p*nch] = RINT(s);

			}

			for(i=winlen*nch;i<tabsize*nch;i++)
				outbuf[i-winlen*nch] = outbuf[i];


			p += winlen-nbufsamples;
			nsamples -= winlen-nbufsamples;
			nbufsamples = 0;

			for(ch=0;ch<nch;ch++)
			{
				ires = ch == 0 ? lires : rires;


				for(i=0;i<winlen;i++)
					fsamples[i] = inbuf[nch*i+ch];


				for(i=winlen;i<tabsize;i++)
					fsamples[i] = 0;

				if (weq_enable)
				{

					rfft(tabsize,1,fsamples);

					fsamples[0] = ires[0]*fsamples[0];
					fsamples[1] = ires[1]*fsamples[1]; 

					for(i=1;i<tabsize/2;i++)
					{
						REAL re,im;

						re = ires[i*2  ]*fsamples[i*2] - ires[i*2+1]*fsamples[i*2+1];
						im = ires[i*2+1]*fsamples[i*2] + ires[i*2  ]*fsamples[i*2+1];

						fsamples[i*2  ] = re;
						fsamples[i*2+1] = im;
					}

					rfft(tabsize,-1,fsamples);
				}
				else
				{
					for(i=winlen-1+winlen/2;i>=winlen/2;i--) fsamples[i] = fsamples[i-winlen/2]*tabsize/2;
					for(;i>=0;i--) fsamples[i] = 0;
				}

				for(i=0;i<winlen;i++) outbuf[i*nch+ch] += fsamples[i]/tabsize*2;

				for(i=winlen;i<tabsize;i++) outbuf[i*nch+ch] = fsamples[i]/tabsize*2;
			}
		}


		for(i=0;i<nsamples*nch;i++)
		{
			inbuf[nbufsamples*nch+i] = ((short *)buf)[i+p*nch];
			float s = outbuf[nbufsamples*nch+i];


			if (s < -32768) s = -32768;
			if (32767 < s) s = 32767;
			((short *)buf)[i+p*nch] = RINT(s);

		}




		p += nsamples;
		nbufsamples += nsamples;

		return p;
	}





#include <stdlib.h>





	void rdft(int n, int isgn, REAL *a, int *ip, REAL *w)
	{

		int nw, nc;
		REAL xi;

		nw = ip[0];
		if (n > (nw << 2))
		{
			nw = n >> 2;
			makewt(nw, ip, w);
		}
		nc = ip[1];
		if (n > (nc << 2))
		{
			nc = n >> 2;
			makect(nc, ip, w + nw);
		}
		if (isgn >= 0)
		{
			if (n > 4)
			{
				cftfsub(n, a, ip + 2, nw, w);
				rftfsub(n, a, nc, w + nw);
			}
			else if (n == 4)
			{
				cftfsub(n, a, ip + 2, nw, w);
			}
			xi = a[0] - a[1];
			a[0] += a[1];
			a[1] = xi;
		}
		else
		{
			a[1] = 0.5 * (a[0] - a[1]);
			a[0] -= a[1];
			if (n > 4)
			{
				rftbsub(n, a, nc, w + nw);
				cftbsub(n, a, ip + 2, nw, w);
			}
			else if (n == 4)
			{
				cftbsub(n, a, ip + 2, nw, w);
			}
		}
	}





	void makewt(int nw, int *ip, REAL *w)
	{
		int j, nwh, nw0, nw1;
		REAL delta, wn4r, wk1r, wk1i, wk3r, wk3i;

		ip[0] = nw;
		ip[1] = 1;
		if (nw > 2)
		{
			nwh = nw >> 1;
			delta = atan(1.0) / nwh;
			wn4r = cos(delta * nwh);
			w[0] = 1;
			w[1] = wn4r;
			if (nwh >= 4)
			{
				w[2] = 0.5 / cos(delta * 2);
				w[3] = 0.5 / cos(delta * 6);
			}
			for (j = 4; j < nwh; j += 4)
			{
				w[j] = cos(delta * j);
				w[j + 1] = sin(delta * j);
				w[j + 2] = cos(3 * delta * j);
				w[j + 3] = sin(3 * delta * j);
			}
			nw0 = 0;
			while (nwh > 2)
			{
				nw1 = nw0 + nwh;
				nwh >>= 1;
				w[nw1] = 1;
				w[nw1 + 1] = wn4r;
				if (nwh >= 4)
				{
					wk1r = w[nw0 + 4];
					wk3r = w[nw0 + 6];
					w[nw1 + 2] = 0.5 / wk1r;
					w[nw1 + 3] = 0.5 / wk3r;
				}
				for (j = 4; j < nwh; j += 4)
				{
					wk1r = w[nw0 + 2 * j];
					wk1i = w[nw0 + 2 * j + 1];
					wk3r = w[nw0 + 2 * j + 2];
					wk3i = w[nw0 + 2 * j + 3];
					w[nw1 + j] = wk1r;
					w[nw1 + j + 1] = wk1i;
					w[nw1 + j + 2] = wk3r;
					w[nw1 + j + 3] = wk3i;
				}
				nw0 = nw1;
			}
		}
	}


	void makect(int nc, int *ip, REAL *c)
	{
		int j, nch;
		REAL delta;

		ip[1] = nc;
		if (nc > 1)
		{
			nch = nc >> 1;
			delta = atan(1.0) / nch;
			c[0] = cos(delta * nch);
			c[nch] = 0.5 * c[0];
			for (j = 1; j < nch; j++)
			{
				c[j] = 0.5 * cos(delta * j);
				c[nc - j] = 0.5 * sin(delta * j);
			}
		}
	}





#ifndef CDFT_RECURSIVE_N   
#define CDFT_RECURSIVE_N 512   
#endif


	void cftfsub(int n, REAL *a, int *ip, int nw, REAL *w)
	{

		int m;

		if (n > 32)
		{
			m = n >> 2;
			cftf1st(n, a, &w[nw - m]);
			if (n > CDFT_RECURSIVE_N)
			{
				cftrec1(m, a, nw, w);
				cftrec2(m, &a[m], nw, w);
				cftrec1(m, &a[2 * m], nw, w);
				cftrec1(m, &a[3 * m], nw, w);
			}
			else if (m > 32)
			{
				cftexp1(n, a, nw, w);
			}
			else
			{
				cftfx41(n, a, nw, w);
			}
			bitrv2(n, ip, a);
		}
		else if (n > 8)
		{
			if (n == 32)
			{
				cftf161(a, &w[nw - 8]);
				bitrv216(a);
			}
			else
			{
				cftf081(a, w);
				bitrv208(a);
			}
		}
		else if (n == 8)
		{
			cftf040(a);
		}
		else if (n == 4)
		{
			cftx020(a);
		}
	}


	void cftbsub(int n, REAL *a, int *ip, int nw, REAL *w)
	{

		int m;

		if (n > 32)
		{
			m = n >> 2;
			cftb1st(n, a, &w[nw - m]);
			if (n > CDFT_RECURSIVE_N)
			{
				cftrec1(m, a, nw, w);
				cftrec2(m, &a[m], nw, w);
				cftrec1(m, &a[2 * m], nw, w);
				cftrec1(m, &a[3 * m], nw, w);
			}
			else if (m > 32)
			{
				cftexp1(n, a, nw, w);
			}
			else
			{
				cftfx41(n, a, nw, w);
			}
			bitrv2conj(n, ip, a);
		}
		else if (n > 8)
		{
			if (n == 32)
			{
				cftf161(a, &w[nw - 8]);
				bitrv216neg(a);
			}
			else
			{
				cftf081(a, w);
				bitrv208neg(a);
			}
		}
		else if (n == 8)
		{
			cftb040(a);
		}
		else if (n == 4)
		{
			cftx020(a);
		}
	}


	void bitrv2(int n, int *ip, REAL *a)
	{
		int j, j1, k, k1, l, m, m2;
		REAL xr, xi, yr, yi;

		ip[0] = 0;
		l = n;
		m = 1;
		while ((m << 3) < l)
		{
			l >>= 1;
			for (j = 0; j < m; j++)
			{
				ip[m + j] = ip[j] + l;
			}
			m <<= 1;
		}
		m2 = 2 * m;
		if ((m << 3) == l)
		{
			for (k = 0; k < m; k++)
			{
				for (j = 0; j < k; j++)
				{
					j1 = 2 * j + ip[k];
					k1 = 2 * k + ip[j];
					xr = a[j1];
					xi = a[j1 + 1];
					yr = a[k1];
					yi = a[k1 + 1];
					a[j1] = yr;
					a[j1 + 1] = yi;
					a[k1] = xr;
					a[k1 + 1] = xi;
					j1 += m2;
					k1 += 2 * m2;
					xr = a[j1];
					xi = a[j1 + 1];
					yr = a[k1];
					yi = a[k1 + 1];
					a[j1] = yr;
					a[j1 + 1] = yi;
					a[k1] = xr;
					a[k1 + 1] = xi;
					j1 += m2;
					k1 -= m2;
					xr = a[j1];
					xi = a[j1 + 1];
					yr = a[k1];
					yi = a[k1 + 1];
					a[j1] = yr;
					a[j1 + 1] = yi;
					a[k1] = xr;
					a[k1 + 1] = xi;
					j1 += m2;
					k1 += 2 * m2;
					xr = a[j1];
					xi = a[j1 + 1];
					yr = a[k1];
					yi = a[k1 + 1];
					a[j1] = yr;
					a[j1 + 1] = yi;
					a[k1] = xr;
					a[k1 + 1] = xi;
				}
				j1 = 2 * k + m2 + ip[k];
				k1 = j1 + m2;
				xr = a[j1];
				xi = a[j1 + 1];
				yr = a[k1];
				yi = a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
			}
		}
		else
		{
			for (k = 1; k < m; k++)
			{
				for (j = 0; j < k; j++)
				{
					j1 = 2 * j + ip[k];
					k1 = 2 * k + ip[j];
					xr = a[j1];
					xi = a[j1 + 1];
					yr = a[k1];
					yi = a[k1 + 1];
					a[j1] = yr;
					a[j1 + 1] = yi;
					a[k1] = xr;
					a[k1 + 1] = xi;
					j1 += m2;
					k1 += m2;
					xr = a[j1];
					xi = a[j1 + 1];
					yr = a[k1];
					yi = a[k1 + 1];
					a[j1] = yr;
					a[j1 + 1] = yi;
					a[k1] = xr;
					a[k1 + 1] = xi;
				}
			}
		}
	}


	void bitrv2conj(int n, int *ip, REAL *a)
	{
		int j, j1, k, k1, l, m, m2;
		REAL xr, xi, yr, yi;

		ip[0] = 0;
		l = n;
		m = 1;
		while ((m << 3) < l)
		{
			l >>= 1;
			for (j = 0; j < m; j++)
			{
				ip[m + j] = ip[j] + l;
			}
			m <<= 1;
		}
		m2 = 2 * m;
		if ((m << 3) == l)
		{
			for (k = 0; k < m; k++)
			{
				for (j = 0; j < k; j++)
				{
					j1 = 2 * j + ip[k];
					k1 = 2 * k + ip[j];
					xr = a[j1];
					xi = -a[j1 + 1];
					yr = a[k1];
					yi = -a[k1 + 1];
					a[j1] = yr;
					a[j1 + 1] = yi;
					a[k1] = xr;
					a[k1 + 1] = xi;
					j1 += m2;
					k1 += 2 * m2;
					xr = a[j1];
					xi = -a[j1 + 1];
					yr = a[k1];
					yi = -a[k1 + 1];
					a[j1] = yr;
					a[j1 + 1] = yi;
					a[k1] = xr;
					a[k1 + 1] = xi;
					j1 += m2;
					k1 -= m2;
					xr = a[j1];
					xi = -a[j1 + 1];
					yr = a[k1];
					yi = -a[k1 + 1];
					a[j1] = yr;
					a[j1 + 1] = yi;
					a[k1] = xr;
					a[k1 + 1] = xi;
					j1 += m2;
					k1 += 2 * m2;
					xr = a[j1];
					xi = -a[j1 + 1];
					yr = a[k1];
					yi = -a[k1 + 1];
					a[j1] = yr;
					a[j1 + 1] = yi;
					a[k1] = xr;
					a[k1 + 1] = xi;
				}
				k1 = 2 * k + ip[k];
				a[k1 + 1] = -a[k1 + 1];
				j1 = k1 + m2;
				k1 = j1 + m2;
				xr = a[j1];
				xi = -a[j1 + 1];
				yr = a[k1];
				yi = -a[k1 + 1];
				a[j1] = yr;
				a[j1 + 1] = yi;
				a[k1] = xr;
				a[k1 + 1] = xi;
				k1 += m2;
				a[k1 + 1] = -a[k1 + 1];
			}
		}
		else
		{
			a[1] = -a[1];
			a[m2 + 1] = -a[m2 + 1];
			for (k = 1; k < m; k++)
			{
				for (j = 0; j < k; j++)
				{
					j1 = 2 * j + ip[k];
					k1 = 2 * k + ip[j];
					xr = a[j1];
					xi = -a[j1 + 1];
					yr = a[k1];
					yi = -a[k1 + 1];
					a[j1] = yr;
					a[j1 + 1] = yi;
					a[k1] = xr;
					a[k1 + 1] = xi;
					j1 += m2;
					k1 += m2;
					xr = a[j1];
					xi = -a[j1 + 1];
					yr = a[k1];
					yi = -a[k1 + 1];
					a[j1] = yr;
					a[j1 + 1] = yi;
					a[k1] = xr;
					a[k1 + 1] = xi;
				}
				k1 = 2 * k + ip[k];
				a[k1 + 1] = -a[k1 + 1];
				a[k1 + m2 + 1] = -a[k1 + m2 + 1];
			}
		}
	}


	void bitrv216(REAL *a)
	{
		REAL x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, 
			x5r, x5i, x7r, x7i, x8r, x8i, x10r, x10i, 
			x11r, x11i, x12r, x12i, x13r, x13i, x14r, x14i;

		x1r = a[2];
		x1i = a[3];
		x2r = a[4];
		x2i = a[5];
		x3r = a[6];
		x3i = a[7];
		x4r = a[8];
		x4i = a[9];
		x5r = a[10];
		x5i = a[11];
		x7r = a[14];
		x7i = a[15];
		x8r = a[16];
		x8i = a[17];
		x10r = a[20];
		x10i = a[21];
		x11r = a[22];
		x11i = a[23];
		x12r = a[24];
		x12i = a[25];
		x13r = a[26];
		x13i = a[27];
		x14r = a[28];
		x14i = a[29];
		a[2] = x8r;
		a[3] = x8i;
		a[4] = x4r;
		a[5] = x4i;
		a[6] = x12r;
		a[7] = x12i;
		a[8] = x2r;
		a[9] = x2i;
		a[10] = x10r;
		a[11] = x10i;
		a[14] = x14r;
		a[15] = x14i;
		a[16] = x1r;
		a[17] = x1i;
		a[20] = x5r;
		a[21] = x5i;
		a[22] = x13r;
		a[23] = x13i;
		a[24] = x3r;
		a[25] = x3i;
		a[26] = x11r;
		a[27] = x11i;
		a[28] = x7r;
		a[29] = x7i;
	}


	void bitrv216neg(REAL *a)
	{
		REAL x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, 
			x5r, x5i, x6r, x6i, x7r, x7i, x8r, x8i, 
			x9r, x9i, x10r, x10i, x11r, x11i, x12r, x12i, 
			x13r, x13i, x14r, x14i, x15r, x15i;

		x1r = a[2];
		x1i = a[3];
		x2r = a[4];
		x2i = a[5];
		x3r = a[6];
		x3i = a[7];
		x4r = a[8];
		x4i = a[9];
		x5r = a[10];
		x5i = a[11];
		x6r = a[12];
		x6i = a[13];
		x7r = a[14];
		x7i = a[15];
		x8r = a[16];
		x8i = a[17];
		x9r = a[18];
		x9i = a[19];
		x10r = a[20];
		x10i = a[21];
		x11r = a[22];
		x11i = a[23];
		x12r = a[24];
		x12i = a[25];
		x13r = a[26];
		x13i = a[27];
		x14r = a[28];
		x14i = a[29];
		x15r = a[30];
		x15i = a[31];
		a[2] = x15r;
		a[3] = x15i;
		a[4] = x7r;
		a[5] = x7i;
		a[6] = x11r;
		a[7] = x11i;
		a[8] = x3r;
		a[9] = x3i;
		a[10] = x13r;
		a[11] = x13i;
		a[12] = x5r;
		a[13] = x5i;
		a[14] = x9r;
		a[15] = x9i;
		a[16] = x1r;
		a[17] = x1i;
		a[18] = x14r;
		a[19] = x14i;
		a[20] = x6r;
		a[21] = x6i;
		a[22] = x10r;
		a[23] = x10i;
		a[24] = x2r;
		a[25] = x2i;
		a[26] = x12r;
		a[27] = x12i;
		a[28] = x4r;
		a[29] = x4i;
		a[30] = x8r;
		a[31] = x8i;
	}


	void bitrv208(REAL *a)
	{
		REAL x1r, x1i, x3r, x3i, x4r, x4i, x6r, x6i;

		x1r = a[2];
		x1i = a[3];
		x3r = a[6];
		x3i = a[7];
		x4r = a[8];
		x4i = a[9];
		x6r = a[12];
		x6i = a[13];
		a[2] = x4r;
		a[3] = x4i;
		a[6] = x6r;
		a[7] = x6i;
		a[8] = x1r;
		a[9] = x1i;
		a[12] = x3r;
		a[13] = x3i;
	}


	void bitrv208neg(REAL *a)
	{
		REAL x1r, x1i, x2r, x2i, x3r, x3i, x4r, x4i, 
			x5r, x5i, x6r, x6i, x7r, x7i;

		x1r = a[2];
		x1i = a[3];
		x2r = a[4];
		x2i = a[5];
		x3r = a[6];
		x3i = a[7];
		x4r = a[8];
		x4i = a[9];
		x5r = a[10];
		x5i = a[11];
		x6r = a[12];
		x6i = a[13];
		x7r = a[14];
		x7i = a[15];
		a[2] = x7r;
		a[3] = x7i;
		a[4] = x3r;
		a[5] = x3i;
		a[6] = x5r;
		a[7] = x5i;
		a[8] = x1r;
		a[9] = x1i;
		a[10] = x6r;
		a[11] = x6i;
		a[12] = x2r;
		a[13] = x2i;
		a[14] = x4r;
		a[15] = x4i;
	}


	void cftf1st(int n, REAL *a, REAL *w)
	{
		int j, j0, j1, j2, j3, k, m, mh;
		REAL wn4r, csc1, csc3, wk1r, wk1i, wk3r, wk3i, 
			wd1r, wd1i, wd3r, wd3i;
		REAL x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
			y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i;

		mh = n >> 3;
		m = 2 * mh;
		j1 = m;
		j2 = j1 + m;
		j3 = j2 + m;
		x0r = a[0] + a[j2];
		x0i = a[1] + a[j2 + 1];
		x1r = a[0] - a[j2];
		x1i = a[1] - a[j2 + 1];
		x2r = a[j1] + a[j3];
		x2i = a[j1 + 1] + a[j3 + 1];
		x3r = a[j1] - a[j3];
		x3i = a[j1 + 1] - a[j3 + 1];
		a[0] = x0r + x2r;
		a[1] = x0i + x2i;
		a[j1] = x0r - x2r;
		a[j1 + 1] = x0i - x2i;
		a[j2] = x1r - x3i;
		a[j2 + 1] = x1i + x3r;
		a[j3] = x1r + x3i;
		a[j3 + 1] = x1i - x3r;
		wn4r = w[1];
		csc1 = w[2];
		csc3 = w[3];
		wd1r = 1;
		wd1i = 0;
		wd3r = 1;
		wd3i = 0;
		k = 0;
		for (j = 2; j < mh - 2; j += 4)
		{
			k += 4;
			wk1r = csc1 * (wd1r + w[k]);
			wk1i = csc1 * (wd1i + w[k + 1]);
			wk3r = csc3 * (wd3r + w[k + 2]);
			wk3i = csc3 * (wd3i - w[k + 3]);
			wd1r = w[k];
			wd1i = w[k + 1];
			wd3r = w[k + 2];
			wd3i = -w[k + 3];
			j1 = j + m;
			j2 = j1 + m;
			j3 = j2 + m;
			x0r = a[j] + a[j2];
			x0i = a[j + 1] + a[j2 + 1];
			x1r = a[j] - a[j2];
			x1i = a[j + 1] - a[j2 + 1];
			y0r = a[j + 2] + a[j2 + 2];
			y0i = a[j + 3] + a[j2 + 3];
			y1r = a[j + 2] - a[j2 + 2];
			y1i = a[j + 3] - a[j2 + 3];
			x2r = a[j1] + a[j3];
			x2i = a[j1 + 1] + a[j3 + 1];
			x3r = a[j1] - a[j3];
			x3i = a[j1 + 1] - a[j3 + 1];
			y2r = a[j1 + 2] + a[j3 + 2];
			y2i = a[j1 + 3] + a[j3 + 3];
			y3r = a[j1 + 2] - a[j3 + 2];
			y3i = a[j1 + 3] - a[j3 + 3];
			a[j] = x0r + x2r;
			a[j + 1] = x0i + x2i;
			a[j + 2] = y0r + y2r;
			a[j + 3] = y0i + y2i;
			a[j1] = x0r - x2r;
			a[j1 + 1] = x0i - x2i;
			a[j1 + 2] = y0r - y2r;
			a[j1 + 3] = y0i - y2i;
			x0r = x1r - x3i;
			x0i = x1i + x3r;
			a[j2] = wk1r * x0r - wk1i * x0i;
			a[j2 + 1] = wk1r * x0i + wk1i * x0r;
			x0r = y1r - y3i;
			x0i = y1i + y3r;
			a[j2 + 2] = wd1r * x0r - wd1i * x0i;
			a[j2 + 3] = wd1r * x0i + wd1i * x0r;
			x0r = x1r + x3i;
			x0i = x1i - x3r;
			a[j3] = wk3r * x0r + wk3i * x0i;
			a[j3 + 1] = wk3r * x0i - wk3i * x0r;
			x0r = y1r + y3i;
			x0i = y1i - y3r;
			a[j3 + 2] = wd3r * x0r + wd3i * x0i;
			a[j3 + 3] = wd3r * x0i - wd3i * x0r;
			j0 = m - j;
			j1 = j0 + m;
			j2 = j1 + m;
			j3 = j2 + m;
			x0r = a[j0] + a[j2];
			x0i = a[j0 + 1] + a[j2 + 1];
			x1r = a[j0] - a[j2];
			x1i = a[j0 + 1] - a[j2 + 1];
			y0r = a[j0 - 2] + a[j2 - 2];
			y0i = a[j0 - 1] + a[j2 - 1];
			y1r = a[j0 - 2] - a[j2 - 2];
			y1i = a[j0 - 1] - a[j2 - 1];
			x2r = a[j1] + a[j3];
			x2i = a[j1 + 1] + a[j3 + 1];
			x3r = a[j1] - a[j3];
			x3i = a[j1 + 1] - a[j3 + 1];
			y2r = a[j1 - 2] + a[j3 - 2];
			y2i = a[j1 - 1] + a[j3 - 1];
			y3r = a[j1 - 2] - a[j3 - 2];
			y3i = a[j1 - 1] - a[j3 - 1];
			a[j0] = x0r + x2r;
			a[j0 + 1] = x0i + x2i;
			a[j0 - 2] = y0r + y2r;
			a[j0 - 1] = y0i + y2i;
			a[j1] = x0r - x2r;
			a[j1 + 1] = x0i - x2i;
			a[j1 - 2] = y0r - y2r;
			a[j1 - 1] = y0i - y2i;
			x0r = x1r - x3i;
			x0i = x1i + x3r;
			a[j2] = wk1i * x0r - wk1r * x0i;
			a[j2 + 1] = wk1i * x0i + wk1r * x0r;
			x0r = y1r - y3i;
			x0i = y1i + y3r;
			a[j2 - 2] = wd1i * x0r - wd1r * x0i;
			a[j2 - 1] = wd1i * x0i + wd1r * x0r;
			x0r = x1r + x3i;
			x0i = x1i - x3r;
			a[j3] = wk3i * x0r + wk3r * x0i;
			a[j3 + 1] = wk3i * x0i - wk3r * x0r;
			x0r = y1r + y3i;
			x0i = y1i - y3r;
			a[j3 - 2] = wd3i * x0r + wd3r * x0i;
			a[j3 - 1] = wd3i * x0i - wd3r * x0r;
		}
		wk1r = csc1 * (wd1r + wn4r);
		wk1i = csc1 * (wd1i + wn4r);
		wk3r = csc3 * (wd3r - wn4r);
		wk3i = csc3 * (wd3i - wn4r);
		j0 = mh;
		j1 = j0 + m;
		j2 = j1 + m;
		j3 = j2 + m;
		x0r = a[j0 - 2] + a[j2 - 2];
		x0i = a[j0 - 1] + a[j2 - 1];
		x1r = a[j0 - 2] - a[j2 - 2];
		x1i = a[j0 - 1] - a[j2 - 1];
		x2r = a[j1 - 2] + a[j3 - 2];
		x2i = a[j1 - 1] + a[j3 - 1];
		x3r = a[j1 - 2] - a[j3 - 2];
		x3i = a[j1 - 1] - a[j3 - 1];
		a[j0 - 2] = x0r + x2r;
		a[j0 - 1] = x0i + x2i;
		a[j1 - 2] = x0r - x2r;
		a[j1 - 1] = x0i - x2i;
		x0r = x1r - x3i;
		x0i = x1i + x3r;
		a[j2 - 2] = wk1r * x0r - wk1i * x0i;
		a[j2 - 1] = wk1r * x0i + wk1i * x0r;
		x0r = x1r + x3i;
		x0i = x1i - x3r;
		a[j3 - 2] = wk3r * x0r + wk3i * x0i;
		a[j3 - 1] = wk3r * x0i - wk3i * x0r;
		x0r = a[j0] + a[j2];
		x0i = a[j0 + 1] + a[j2 + 1];
		x1r = a[j0] - a[j2];
		x1i = a[j0 + 1] - a[j2 + 1];
		x2r = a[j1] + a[j3];
		x2i = a[j1 + 1] + a[j3 + 1];
		x3r = a[j1] - a[j3];
		x3i = a[j1 + 1] - a[j3 + 1];
		a[j0] = x0r + x2r;
		a[j0 + 1] = x0i + x2i;
		a[j1] = x0r - x2r;
		a[j1 + 1] = x0i - x2i;
		x0r = x1r - x3i;
		x0i = x1i + x3r;
		a[j2] = wn4r * (x0r - x0i);
		a[j2 + 1] = wn4r * (x0i + x0r);
		x0r = x1r + x3i;
		x0i = x1i - x3r;
		a[j3] = -wn4r * (x0r + x0i);
		a[j3 + 1] = -wn4r * (x0i - x0r);
		x0r = a[j0 + 2] + a[j2 + 2];
		x0i = a[j0 + 3] + a[j2 + 3];
		x1r = a[j0 + 2] - a[j2 + 2];
		x1i = a[j0 + 3] - a[j2 + 3];
		x2r = a[j1 + 2] + a[j3 + 2];
		x2i = a[j1 + 3] + a[j3 + 3];
		x3r = a[j1 + 2] - a[j3 + 2];
		x3i = a[j1 + 3] - a[j3 + 3];
		a[j0 + 2] = x0r + x2r;
		a[j0 + 3] = x0i + x2i;
		a[j1 + 2] = x0r - x2r;
		a[j1 + 3] = x0i - x2i;
		x0r = x1r - x3i;
		x0i = x1i + x3r;
		a[j2 + 2] = wk1i * x0r - wk1r * x0i;
		a[j2 + 3] = wk1i * x0i + wk1r * x0r;
		x0r = x1r + x3i;
		x0i = x1i - x3r;
		a[j3 + 2] = wk3i * x0r + wk3r * x0i;
		a[j3 + 3] = wk3i * x0i - wk3r * x0r;
	}


	void cftb1st(int n, REAL *a, REAL *w)
	{
		int j, j0, j1, j2, j3, k, m, mh;
		REAL wn4r, csc1, csc3, wk1r, wk1i, wk3r, wk3i, 
			wd1r, wd1i, wd3r, wd3i;
		REAL x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
			y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i;

		mh = n >> 3;
		m = 2 * mh;
		j1 = m;
		j2 = j1 + m;
		j3 = j2 + m;
		x0r = a[0] + a[j2];
		x0i = -a[1] - a[j2 + 1];
		x1r = a[0] - a[j2];
		x1i = -a[1] + a[j2 + 1];
		x2r = a[j1] + a[j3];
		x2i = a[j1 + 1] + a[j3 + 1];
		x3r = a[j1] - a[j3];
		x3i = a[j1 + 1] - a[j3 + 1];
		a[0] = x0r + x2r;
		a[1] = x0i - x2i;
		a[j1] = x0r - x2r;
		a[j1 + 1] = x0i + x2i;
		a[j2] = x1r + x3i;
		a[j2 + 1] = x1i + x3r;
		a[j3] = x1r - x3i;
		a[j3 + 1] = x1i - x3r;
		wn4r = w[1];
		csc1 = w[2];
		csc3 = w[3];
		wd1r = 1;
		wd1i = 0;
		wd3r = 1;
		wd3i = 0;
		k = 0;
		for (j = 2; j < mh - 2; j += 4)
		{
			k += 4;
			wk1r = csc1 * (wd1r + w[k]);
			wk1i = csc1 * (wd1i + w[k + 1]);
			wk3r = csc3 * (wd3r + w[k + 2]);
			wk3i = csc3 * (wd3i - w[k + 3]);
			wd1r = w[k];
			wd1i = w[k + 1];
			wd3r = w[k + 2];
			wd3i = -w[k + 3];
			j1 = j + m;
			j2 = j1 + m;
			j3 = j2 + m;
			x0r = a[j] + a[j2];
			x0i = -a[j + 1] - a[j2 + 1];
			x1r = a[j] - a[j2];
			x1i = -a[j + 1] + a[j2 + 1];
			y0r = a[j + 2] + a[j2 + 2];
			y0i = -a[j + 3] - a[j2 + 3];
			y1r = a[j + 2] - a[j2 + 2];
			y1i = -a[j + 3] + a[j2 + 3];
			x2r = a[j1] + a[j3];
			x2i = a[j1 + 1] + a[j3 + 1];
			x3r = a[j1] - a[j3];
			x3i = a[j1 + 1] - a[j3 + 1];
			y2r = a[j1 + 2] + a[j3 + 2];
			y2i = a[j1 + 3] + a[j3 + 3];
			y3r = a[j1 + 2] - a[j3 + 2];
			y3i = a[j1 + 3] - a[j3 + 3];
			a[j] = x0r + x2r;
			a[j + 1] = x0i - x2i;
			a[j + 2] = y0r + y2r;
			a[j + 3] = y0i - y2i;
			a[j1] = x0r - x2r;
			a[j1 + 1] = x0i + x2i;
			a[j1 + 2] = y0r - y2r;
			a[j1 + 3] = y0i + y2i;
			x0r = x1r + x3i;
			x0i = x1i + x3r;
			a[j2] = wk1r * x0r - wk1i * x0i;
			a[j2 + 1] = wk1r * x0i + wk1i * x0r;
			x0r = y1r + y3i;
			x0i = y1i + y3r;
			a[j2 + 2] = wd1r * x0r - wd1i * x0i;
			a[j2 + 3] = wd1r * x0i + wd1i * x0r;
			x0r = x1r - x3i;
			x0i = x1i - x3r;
			a[j3] = wk3r * x0r + wk3i * x0i;
			a[j3 + 1] = wk3r * x0i - wk3i * x0r;
			x0r = y1r - y3i;
			x0i = y1i - y3r;
			a[j3 + 2] = wd3r * x0r + wd3i * x0i;
			a[j3 + 3] = wd3r * x0i - wd3i * x0r;
			j0 = m - j;
			j1 = j0 + m;
			j2 = j1 + m;
			j3 = j2 + m;
			x0r = a[j0] + a[j2];
			x0i = -a[j0 + 1] - a[j2 + 1];
			x1r = a[j0] - a[j2];
			x1i = -a[j0 + 1] + a[j2 + 1];
			y0r = a[j0 - 2] + a[j2 - 2];
			y0i = -a[j0 - 1] - a[j2 - 1];
			y1r = a[j0 - 2] - a[j2 - 2];
			y1i = -a[j0 - 1] + a[j2 - 1];
			x2r = a[j1] + a[j3];
			x2i = a[j1 + 1] + a[j3 + 1];
			x3r = a[j1] - a[j3];
			x3i = a[j1 + 1] - a[j3 + 1];
			y2r = a[j1 - 2] + a[j3 - 2];
			y2i = a[j1 - 1] + a[j3 - 1];
			y3r = a[j1 - 2] - a[j3 - 2];
			y3i = a[j1 - 1] - a[j3 - 1];
			a[j0] = x0r + x2r;
			a[j0 + 1] = x0i - x2i;
			a[j0 - 2] = y0r + y2r;
			a[j0 - 1] = y0i - y2i;
			a[j1] = x0r - x2r;
			a[j1 + 1] = x0i + x2i;
			a[j1 - 2] = y0r - y2r;
			a[j1 - 1] = y0i + y2i;
			x0r = x1r + x3i;
			x0i = x1i + x3r;
			a[j2] = wk1i * x0r - wk1r * x0i;
			a[j2 + 1] = wk1i * x0i + wk1r * x0r;
			x0r = y1r + y3i;
			x0i = y1i + y3r;
			a[j2 - 2] = wd1i * x0r - wd1r * x0i;
			a[j2 - 1] = wd1i * x0i + wd1r * x0r;
			x0r = x1r - x3i;
			x0i = x1i - x3r;
			a[j3] = wk3i * x0r + wk3r * x0i;
			a[j3 + 1] = wk3i * x0i - wk3r * x0r;
			x0r = y1r - y3i;
			x0i = y1i - y3r;
			a[j3 - 2] = wd3i * x0r + wd3r * x0i;
			a[j3 - 1] = wd3i * x0i - wd3r * x0r;
		}
		wk1r = csc1 * (wd1r + wn4r);
		wk1i = csc1 * (wd1i + wn4r);
		wk3r = csc3 * (wd3r - wn4r);
		wk3i = csc3 * (wd3i - wn4r);
		j0 = mh;
		j1 = j0 + m;
		j2 = j1 + m;
		j3 = j2 + m;
		x0r = a[j0 - 2] + a[j2 - 2];
		x0i = -a[j0 - 1] - a[j2 - 1];
		x1r = a[j0 - 2] - a[j2 - 2];
		x1i = -a[j0 - 1] + a[j2 - 1];
		x2r = a[j1 - 2] + a[j3 - 2];
		x2i = a[j1 - 1] + a[j3 - 1];
		x3r = a[j1 - 2] - a[j3 - 2];
		x3i = a[j1 - 1] - a[j3 - 1];
		a[j0 - 2] = x0r + x2r;
		a[j0 - 1] = x0i - x2i;
		a[j1 - 2] = x0r - x2r;
		a[j1 - 1] = x0i + x2i;
		x0r = x1r + x3i;
		x0i = x1i + x3r;
		a[j2 - 2] = wk1r * x0r - wk1i * x0i;
		a[j2 - 1] = wk1r * x0i + wk1i * x0r;
		x0r = x1r - x3i;
		x0i = x1i - x3r;
		a[j3 - 2] = wk3r * x0r + wk3i * x0i;
		a[j3 - 1] = wk3r * x0i - wk3i * x0r;
		x0r = a[j0] + a[j2];
		x0i = -a[j0 + 1] - a[j2 + 1];
		x1r = a[j0] - a[j2];
		x1i = -a[j0 + 1] + a[j2 + 1];
		x2r = a[j1] + a[j3];
		x2i = a[j1 + 1] + a[j3 + 1];
		x3r = a[j1] - a[j3];
		x3i = a[j1 + 1] - a[j3 + 1];
		a[j0] = x0r + x2r;
		a[j0 + 1] = x0i - x2i;
		a[j1] = x0r - x2r;
		a[j1 + 1] = x0i + x2i;
		x0r = x1r + x3i;
		x0i = x1i + x3r;
		a[j2] = wn4r * (x0r - x0i);
		a[j2 + 1] = wn4r * (x0i + x0r);
		x0r = x1r - x3i;
		x0i = x1i - x3r;
		a[j3] = -wn4r * (x0r + x0i);
		a[j3 + 1] = -wn4r * (x0i - x0r);
		x0r = a[j0 + 2] + a[j2 + 2];
		x0i = -a[j0 + 3] - a[j2 + 3];
		x1r = a[j0 + 2] - a[j2 + 2];
		x1i = -a[j0 + 3] + a[j2 + 3];
		x2r = a[j1 + 2] + a[j3 + 2];
		x2i = a[j1 + 3] + a[j3 + 3];
		x3r = a[j1 + 2] - a[j3 + 2];
		x3i = a[j1 + 3] - a[j3 + 3];
		a[j0 + 2] = x0r + x2r;
		a[j0 + 3] = x0i - x2i;
		a[j1 + 2] = x0r - x2r;
		a[j1 + 3] = x0i + x2i;
		x0r = x1r + x3i;
		x0i = x1i + x3r;
		a[j2 + 2] = wk1i * x0r - wk1r * x0i;
		a[j2 + 3] = wk1i * x0i + wk1r * x0r;
		x0r = x1r - x3i;
		x0i = x1i - x3r;
		a[j3 + 2] = wk3i * x0r + wk3r * x0i;
		a[j3 + 3] = wk3i * x0i - wk3r * x0r;
	}


	void cftrec1(int n, REAL *a, int nw, REAL *w)
	{
		void cftrec1(int n, REAL *a, int nw, REAL *w);
		void cftrec2(int n, REAL *a, int nw, REAL *w);
		void cftexp1(int n, REAL *a, int nw, REAL *w);
		int m;

		m = n >> 2;
		cftmdl1(n, a, &w[nw - 2 * m]);
		if (n > CDFT_RECURSIVE_N)
		{
			cftrec1(m, a, nw, w);
			cftrec2(m, &a[m], nw, w);
			cftrec1(m, &a[2 * m], nw, w);
			cftrec1(m, &a[3 * m], nw, w);
		}
		else
		{
			cftexp1(n, a, nw, w);
		}
	}


	void cftrec2(int n, REAL *a, int nw, REAL *w)
	{
		void cftrec1(int n, REAL *a, int nw, REAL *w);
		void cftrec2(int n, REAL *a, int nw, REAL *w);
		
		
		int m;

		m = n >> 2;
		cftmdl2(n, a, &w[nw - n]);
		if (n > CDFT_RECURSIVE_N)
		{
			cftrec1(m, a, nw, w);
			cftrec2(m, &a[m], nw, w);
			cftrec1(m, &a[2 * m], nw, w);
			cftrec2(m, &a[3 * m], nw, w);
		}
		else
		{
			cftexp2(n, a, nw, w);
		}
	}


	void cftexp1(int n, REAL *a, int nw, REAL *w)
	{
		void cftmdl1(int n, REAL *a, REAL *w);
		void cftmdl2(int n, REAL *a, REAL *w);
		void cftfx41(int n, REAL *a, int nw, REAL *w);
		
		int j, k, l;

		l = n >> 2;
		while (l > 128)
		{
			for (k = l; k < n; k <<= 2)
			{
				for (j = k - l; j < n; j += 4 * k)
				{
					cftmdl1(l, &a[j], &w[nw - (l >> 1)]);
					cftmdl2(l, &a[k + j], &w[nw - l]);
					cftmdl1(l, &a[2 * k + j], &w[nw - (l >> 1)]);
				}
			}
			cftmdl1(l, &a[n - l], &w[nw - (l >> 1)]);
			l >>= 2;
		}
		for (k = l; k < n; k <<= 2)
		{
			for (j = k - l; j < n; j += 4 * k)
			{
				cftmdl1(l, &a[j], &w[nw - (l >> 1)]);
				cftfx41(l, &a[j], nw, w);
				cftmdl2(l, &a[k + j], &w[nw - l]);
				cftfx42(l, &a[k + j], nw, w);
				cftmdl1(l, &a[2 * k + j], &w[nw - (l >> 1)]);
				cftfx41(l, &a[2 * k + j], nw, w);
			}
		}
		cftmdl1(l, &a[n - l], &w[nw - (l >> 1)]);
		cftfx41(l, &a[n - l], nw, w);
	}


	void cftexp2(int n, REAL *a, int nw, REAL *w)
	{
		void cftmdl1(int n, REAL *a, REAL *w);
		void cftmdl2(int n, REAL *a, REAL *w);
		void cftfx41(int n, REAL *a, int nw, REAL *w);
		void cftfx42(int n, REAL *a, int nw, REAL *w);
		int j, k, l, m;

		m = n >> 1;
		l = n >> 2;
		while (l > 128)
		{
			for (k = l; k < m; k <<= 2)
			{
				for (j = k - l; j < m; j += 2 * k)
				{
					cftmdl1(l, &a[j], &w[nw - (l >> 1)]);
					cftmdl1(l, &a[m + j], &w[nw - (l >> 1)]);
				}
				for (j = 2 * k - l; j < m; j += 4 * k)
				{
					cftmdl2(l, &a[j], &w[nw - l]);
					cftmdl2(l, &a[m + j], &w[nw - l]);
				}
			}
			l >>= 2;
		}
		for (k = l; k < m; k <<= 2)
		{
			for (j = k - l; j < m; j += 2 * k)
			{
				cftmdl1(l, &a[j], &w[nw - (l >> 1)]);
				cftfx41(l, &a[j], nw, w);
				cftmdl1(l, &a[m + j], &w[nw - (l >> 1)]);
				cftfx41(l, &a[m + j], nw, w);
			}
			for (j = 2 * k - l; j < m; j += 4 * k)
			{
				cftmdl2(l, &a[j], &w[nw - l]);
				cftfx42(l, &a[j], nw, w);
				cftmdl2(l, &a[m + j], &w[nw - l]);
				cftfx42(l, &a[m + j], nw, w);
			}
		}
	}


	void cftmdl1(int n, REAL *a, REAL *w)
	{
		int j, j0, j1, j2, j3, k, m, mh;
		REAL wn4r, wk1r, wk1i, wk3r, wk3i;
		REAL x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

		mh = n >> 3;
		m = 2 * mh;
		j1 = m;
		j2 = j1 + m;
		j3 = j2 + m;
		x0r = a[0] + a[j2];
		x0i = a[1] + a[j2 + 1];
		x1r = a[0] - a[j2];
		x1i = a[1] - a[j2 + 1];
		x2r = a[j1] + a[j3];
		x2i = a[j1 + 1] + a[j3 + 1];
		x3r = a[j1] - a[j3];
		x3i = a[j1 + 1] - a[j3 + 1];
		a[0] = x0r + x2r;
		a[1] = x0i + x2i;
		a[j1] = x0r - x2r;
		a[j1 + 1] = x0i - x2i;
		a[j2] = x1r - x3i;
		a[j2 + 1] = x1i + x3r;
		a[j3] = x1r + x3i;
		a[j3 + 1] = x1i - x3r;
		wn4r = w[1];
		k = 0;
		for (j = 2; j < mh; j += 2)
		{
			k += 4;
			wk1r = w[k];
			wk1i = w[k + 1];
			wk3r = w[k + 2];
			wk3i = -w[k + 3];
			j1 = j + m;
			j2 = j1 + m;
			j3 = j2 + m;
			x0r = a[j] + a[j2];
			x0i = a[j + 1] + a[j2 + 1];
			x1r = a[j] - a[j2];
			x1i = a[j + 1] - a[j2 + 1];
			x2r = a[j1] + a[j3];
			x2i = a[j1 + 1] + a[j3 + 1];
			x3r = a[j1] - a[j3];
			x3i = a[j1 + 1] - a[j3 + 1];
			a[j] = x0r + x2r;
			a[j + 1] = x0i + x2i;
			a[j1] = x0r - x2r;
			a[j1 + 1] = x0i - x2i;
			x0r = x1r - x3i;
			x0i = x1i + x3r;
			a[j2] = wk1r * x0r - wk1i * x0i;
			a[j2 + 1] = wk1r * x0i + wk1i * x0r;
			x0r = x1r + x3i;
			x0i = x1i - x3r;
			a[j3] = wk3r * x0r + wk3i * x0i;
			a[j3 + 1] = wk3r * x0i - wk3i * x0r;
			j0 = m - j;
			j1 = j0 + m;
			j2 = j1 + m;
			j3 = j2 + m;
			x0r = a[j0] + a[j2];
			x0i = a[j0 + 1] + a[j2 + 1];
			x1r = a[j0] - a[j2];
			x1i = a[j0 + 1] - a[j2 + 1];
			x2r = a[j1] + a[j3];
			x2i = a[j1 + 1] + a[j3 + 1];
			x3r = a[j1] - a[j3];
			x3i = a[j1 + 1] - a[j3 + 1];
			a[j0] = x0r + x2r;
			a[j0 + 1] = x0i + x2i;
			a[j1] = x0r - x2r;
			a[j1 + 1] = x0i - x2i;
			x0r = x1r - x3i;
			x0i = x1i + x3r;
			a[j2] = wk1i * x0r - wk1r * x0i;
			a[j2 + 1] = wk1i * x0i + wk1r * x0r;
			x0r = x1r + x3i;
			x0i = x1i - x3r;
			a[j3] = wk3i * x0r + wk3r * x0i;
			a[j3 + 1] = wk3i * x0i - wk3r * x0r;
		}
		j0 = mh;
		j1 = j0 + m;
		j2 = j1 + m;
		j3 = j2 + m;
		x0r = a[j0] + a[j2];
		x0i = a[j0 + 1] + a[j2 + 1];
		x1r = a[j0] - a[j2];
		x1i = a[j0 + 1] - a[j2 + 1];
		x2r = a[j1] + a[j3];
		x2i = a[j1 + 1] + a[j3 + 1];
		x3r = a[j1] - a[j3];
		x3i = a[j1 + 1] - a[j3 + 1];
		a[j0] = x0r + x2r;
		a[j0 + 1] = x0i + x2i;
		a[j1] = x0r - x2r;
		a[j1 + 1] = x0i - x2i;
		x0r = x1r - x3i;
		x0i = x1i + x3r;
		a[j2] = wn4r * (x0r - x0i);
		a[j2 + 1] = wn4r * (x0i + x0r);
		x0r = x1r + x3i;
		x0i = x1i - x3r;
		a[j3] = -wn4r * (x0r + x0i);
		a[j3 + 1] = -wn4r * (x0i - x0r);
	}


	void cftmdl2(int n, REAL *a, REAL *w)
	{
		int j, j0, j1, j2, j3, k, kr, m, mh;
		REAL wn4r, wk1r, wk1i, wk3r, wk3i, wd1r, wd1i, wd3r, wd3i;
		REAL x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, y0r, y0i, y2r, y2i;

		mh = n >> 3;
		m = 2 * mh;
		wn4r = w[1];
		j1 = m;
		j2 = j1 + m;
		j3 = j2 + m;
		x0r = a[0] - a[j2 + 1];
		x0i = a[1] + a[j2];
		x1r = a[0] + a[j2 + 1];
		x1i = a[1] - a[j2];
		x2r = a[j1] - a[j3 + 1];
		x2i = a[j1 + 1] + a[j3];
		x3r = a[j1] + a[j3 + 1];
		x3i = a[j1 + 1] - a[j3];
		y0r = wn4r * (x2r - x2i);
		y0i = wn4r * (x2i + x2r);
		a[0] = x0r + y0r;
		a[1] = x0i + y0i;
		a[j1] = x0r - y0r;
		a[j1 + 1] = x0i - y0i;
		y0r = wn4r * (x3r - x3i);
		y0i = wn4r * (x3i + x3r);
		a[j2] = x1r - y0i;
		a[j2 + 1] = x1i + y0r;
		a[j3] = x1r + y0i;
		a[j3 + 1] = x1i - y0r;
		k = 0;
		kr = 2 * m;
		for (j = 2; j < mh; j += 2)
		{
			k += 4;
			wk1r = w[k];
			wk1i = w[k + 1];
			wk3r = w[k + 2];
			wk3i = -w[k + 3];
			kr -= 4;
			wd1i = w[kr];
			wd1r = w[kr + 1];
			wd3i = w[kr + 2];
			wd3r = -w[kr + 3];
			j1 = j + m;
			j2 = j1 + m;
			j3 = j2 + m;
			x0r = a[j] - a[j2 + 1];
			x0i = a[j + 1] + a[j2];
			x1r = a[j] + a[j2 + 1];
			x1i = a[j + 1] - a[j2];
			x2r = a[j1] - a[j3 + 1];
			x2i = a[j1 + 1] + a[j3];
			x3r = a[j1] + a[j3 + 1];
			x3i = a[j1 + 1] - a[j3];
			y0r = wk1r * x0r - wk1i * x0i;
			y0i = wk1r * x0i + wk1i * x0r;
			y2r = wd1r * x2r - wd1i * x2i;
			y2i = wd1r * x2i + wd1i * x2r;
			a[j] = y0r + y2r;
			a[j + 1] = y0i + y2i;
			a[j1] = y0r - y2r;
			a[j1 + 1] = y0i - y2i;
			y0r = wk3r * x1r + wk3i * x1i;
			y0i = wk3r * x1i - wk3i * x1r;
			y2r = wd3r * x3r + wd3i * x3i;
			y2i = wd3r * x3i - wd3i * x3r;
			a[j2] = y0r + y2r;
			a[j2 + 1] = y0i + y2i;
			a[j3] = y0r - y2r;
			a[j3 + 1] = y0i - y2i;
			j0 = m - j;
			j1 = j0 + m;
			j2 = j1 + m;
			j3 = j2 + m;
			x0r = a[j0] - a[j2 + 1];
			x0i = a[j0 + 1] + a[j2];
			x1r = a[j0] + a[j2 + 1];
			x1i = a[j0 + 1] - a[j2];
			x2r = a[j1] - a[j3 + 1];
			x2i = a[j1 + 1] + a[j3];
			x3r = a[j1] + a[j3 + 1];
			x3i = a[j1 + 1] - a[j3];
			y0r = wd1i * x0r - wd1r * x0i;
			y0i = wd1i * x0i + wd1r * x0r;
			y2r = wk1i * x2r - wk1r * x2i;
			y2i = wk1i * x2i + wk1r * x2r;
			a[j0] = y0r + y2r;
			a[j0 + 1] = y0i + y2i;
			a[j1] = y0r - y2r;
			a[j1 + 1] = y0i - y2i;
			y0r = wd3i * x1r + wd3r * x1i;
			y0i = wd3i * x1i - wd3r * x1r;
			y2r = wk3i * x3r + wk3r * x3i;
			y2i = wk3i * x3i - wk3r * x3r;
			a[j2] = y0r + y2r;
			a[j2 + 1] = y0i + y2i;
			a[j3] = y0r - y2r;
			a[j3 + 1] = y0i - y2i;
		}
		wk1r = w[m];
		wk1i = w[m + 1];
		j0 = mh;
		j1 = j0 + m;
		j2 = j1 + m;
		j3 = j2 + m;
		x0r = a[j0] - a[j2 + 1];
		x0i = a[j0 + 1] + a[j2];
		x1r = a[j0] + a[j2 + 1];
		x1i = a[j0 + 1] - a[j2];
		x2r = a[j1] - a[j3 + 1];
		x2i = a[j1 + 1] + a[j3];
		x3r = a[j1] + a[j3 + 1];
		x3i = a[j1 + 1] - a[j3];
		y0r = wk1r * x0r - wk1i * x0i;
		y0i = wk1r * x0i + wk1i * x0r;
		y2r = wk1i * x2r - wk1r * x2i;
		y2i = wk1i * x2i + wk1r * x2r;
		a[j0] = y0r + y2r;
		a[j0 + 1] = y0i + y2i;
		a[j1] = y0r - y2r;
		a[j1 + 1] = y0i - y2i;
		y0r = wk1i * x1r - wk1r * x1i;
		y0i = wk1i * x1i + wk1r * x1r;
		y2r = wk1r * x3r - wk1i * x3i;
		y2i = wk1r * x3i + wk1i * x3r;
		a[j2] = y0r - y2r;
		a[j2 + 1] = y0i - y2i;
		a[j3] = y0r + y2r;
		a[j3 + 1] = y0i + y2i;
	}


	void cftfx41(int n, REAL *a, int nw, REAL *w)
	{
		void cftf161(REAL *a, REAL *w);
		
		void cftf081(REAL *a, REAL *w);
		

		if (n == 128)
		{
			cftf161(a, &w[nw - 8]);
			cftf162(&a[32], &w[nw - 32]);
			cftf161(&a[64], &w[nw - 8]);
			cftf161(&a[96], &w[nw - 8]);
		}
		else
		{
			cftf081(a, &w[nw - 16]);
			cftf082(&a[16], &w[nw - 16]);
			cftf081(&a[32], &w[nw - 16]);
			cftf081(&a[48], &w[nw - 16]);
		}
	}


	void cftfx42(int n, REAL *a, int nw, REAL *w)
	{
		void cftf161(REAL *a, REAL *w);
		void cftf162(REAL *a, REAL *w);
		void cftf081(REAL *a, REAL *w);
		void cftf082(REAL *a, REAL *w);

		if (n == 128)
		{
			cftf161(a, &w[nw - 8]);
			cftf162(&a[32], &w[nw - 32]);
			cftf161(&a[64], &w[nw - 8]);
			cftf162(&a[96], &w[nw - 32]);
		}
		else
		{
			cftf081(a, &w[nw - 16]);
			cftf082(&a[16], &w[nw - 16]);
			cftf081(&a[32], &w[nw - 16]);
			cftf082(&a[48], &w[nw - 16]);
		}
	}


	void cftf161(REAL *a, REAL *w)
	{
		REAL wn4r, wk1r, wk1i, 
			x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
			y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
			y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i, 
			y8r, y8i, y9r, y9i, y10r, y10i, y11r, y11i, 
			y12r, y12i, y13r, y13i, y14r, y14i, y15r, y15i;

		wn4r = w[1];
		wk1i = wn4r * w[2];
		wk1r = wk1i + w[2];
		x0r = a[0] + a[16];
		x0i = a[1] + a[17];
		x1r = a[0] - a[16];
		x1i = a[1] - a[17];
		x2r = a[8] + a[24];
		x2i = a[9] + a[25];
		x3r = a[8] - a[24];
		x3i = a[9] - a[25];
		y0r = x0r + x2r;
		y0i = x0i + x2i;
		y4r = x0r - x2r;
		y4i = x0i - x2i;
		y8r = x1r - x3i;
		y8i = x1i + x3r;
		y12r = x1r + x3i;
		y12i = x1i - x3r;
		x0r = a[2] + a[18];
		x0i = a[3] + a[19];
		x1r = a[2] - a[18];
		x1i = a[3] - a[19];
		x2r = a[10] + a[26];
		x2i = a[11] + a[27];
		x3r = a[10] - a[26];
		x3i = a[11] - a[27];
		y1r = x0r + x2r;
		y1i = x0i + x2i;
		y5r = x0r - x2r;
		y5i = x0i - x2i;
		x0r = x1r - x3i;
		x0i = x1i + x3r;
		y9r = wk1r * x0r - wk1i * x0i;
		y9i = wk1r * x0i + wk1i * x0r;
		x0r = x1r + x3i;
		x0i = x1i - x3r;
		y13r = wk1i * x0r - wk1r * x0i;
		y13i = wk1i * x0i + wk1r * x0r;
		x0r = a[4] + a[20];
		x0i = a[5] + a[21];
		x1r = a[4] - a[20];
		x1i = a[5] - a[21];
		x2r = a[12] + a[28];
		x2i = a[13] + a[29];
		x3r = a[12] - a[28];
		x3i = a[13] - a[29];
		y2r = x0r + x2r;
		y2i = x0i + x2i;
		y6r = x0r - x2r;
		y6i = x0i - x2i;
		x0r = x1r - x3i;
		x0i = x1i + x3r;
		y10r = wn4r * (x0r - x0i);
		y10i = wn4r * (x0i + x0r);
		x0r = x1r + x3i;
		x0i = x1i - x3r;
		y14r = wn4r * (x0r + x0i);
		y14i = wn4r * (x0i - x0r);
		x0r = a[6] + a[22];
		x0i = a[7] + a[23];
		x1r = a[6] - a[22];
		x1i = a[7] - a[23];
		x2r = a[14] + a[30];
		x2i = a[15] + a[31];
		x3r = a[14] - a[30];
		x3i = a[15] - a[31];
		y3r = x0r + x2r;
		y3i = x0i + x2i;
		y7r = x0r - x2r;
		y7i = x0i - x2i;
		x0r = x1r - x3i;
		x0i = x1i + x3r;
		y11r = wk1i * x0r - wk1r * x0i;
		y11i = wk1i * x0i + wk1r * x0r;
		x0r = x1r + x3i;
		x0i = x1i - x3r;
		y15r = wk1r * x0r - wk1i * x0i;
		y15i = wk1r * x0i + wk1i * x0r;
		x0r = y12r - y14r;
		x0i = y12i - y14i;
		x1r = y12r + y14r;
		x1i = y12i + y14i;
		x2r = y13r - y15r;
		x2i = y13i - y15i;
		x3r = y13r + y15r;
		x3i = y13i + y15i;
		a[24] = x0r + x2r;
		a[25] = x0i + x2i;
		a[26] = x0r - x2r;
		a[27] = x0i - x2i;
		a[28] = x1r - x3i;
		a[29] = x1i + x3r;
		a[30] = x1r + x3i;
		a[31] = x1i - x3r;
		x0r = y8r + y10r;
		x0i = y8i + y10i;
		x1r = y8r - y10r;
		x1i = y8i - y10i;
		x2r = y9r + y11r;
		x2i = y9i + y11i;
		x3r = y9r - y11r;
		x3i = y9i - y11i;
		a[16] = x0r + x2r;
		a[17] = x0i + x2i;
		a[18] = x0r - x2r;
		a[19] = x0i - x2i;
		a[20] = x1r - x3i;
		a[21] = x1i + x3r;
		a[22] = x1r + x3i;
		a[23] = x1i - x3r;
		x0r = y5r - y7i;
		x0i = y5i + y7r;
		x2r = wn4r * (x0r - x0i);
		x2i = wn4r * (x0i + x0r);
		x0r = y5r + y7i;
		x0i = y5i - y7r;
		x3r = wn4r * (x0r - x0i);
		x3i = wn4r * (x0i + x0r);
		x0r = y4r - y6i;
		x0i = y4i + y6r;
		x1r = y4r + y6i;
		x1i = y4i - y6r;
		a[8] = x0r + x2r;
		a[9] = x0i + x2i;
		a[10] = x0r - x2r;
		a[11] = x0i - x2i;
		a[12] = x1r - x3i;
		a[13] = x1i + x3r;
		a[14] = x1r + x3i;
		a[15] = x1i - x3r;
		x0r = y0r + y2r;
		x0i = y0i + y2i;
		x1r = y0r - y2r;
		x1i = y0i - y2i;
		x2r = y1r + y3r;
		x2i = y1i + y3i;
		x3r = y1r - y3r;
		x3i = y1i - y3i;
		a[0] = x0r + x2r;
		a[1] = x0i + x2i;
		a[2] = x0r - x2r;
		a[3] = x0i - x2i;
		a[4] = x1r - x3i;
		a[5] = x1i + x3r;
		a[6] = x1r + x3i;
		a[7] = x1i - x3r;
	}


	void cftf162(REAL *a, REAL *w)
	{
		REAL wn4r, wk1r, wk1i, wk2r, wk2i, wk3r, wk3i, 
			x0r, x0i, x1r, x1i, x2r, x2i, 
			y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
			y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i, 
			y8r, y8i, y9r, y9i, y10r, y10i, y11r, y11i, 
			y12r, y12i, y13r, y13i, y14r, y14i, y15r, y15i;

		wn4r = w[1];
		wk1r = w[4];
		wk1i = w[5];
		wk3r = w[6];
		wk3i = w[7];
		wk2r = w[8];
		wk2i = w[9];
		x1r = a[0] - a[17];
		x1i = a[1] + a[16];
		x0r = a[8] - a[25];
		x0i = a[9] + a[24];
		x2r = wn4r * (x0r - x0i);
		x2i = wn4r * (x0i + x0r);
		y0r = x1r + x2r;
		y0i = x1i + x2i;
		y4r = x1r - x2r;
		y4i = x1i - x2i;
		x1r = a[0] + a[17];
		x1i = a[1] - a[16];
		x0r = a[8] + a[25];
		x0i = a[9] - a[24];
		x2r = wn4r * (x0r - x0i);
		x2i = wn4r * (x0i + x0r);
		y8r = x1r - x2i;
		y8i = x1i + x2r;
		y12r = x1r + x2i;
		y12i = x1i - x2r;
		x0r = a[2] - a[19];
		x0i = a[3] + a[18];
		x1r = wk1r * x0r - wk1i * x0i;
		x1i = wk1r * x0i + wk1i * x0r;
		x0r = a[10] - a[27];
		x0i = a[11] + a[26];
		x2r = wk3i * x0r - wk3r * x0i;
		x2i = wk3i * x0i + wk3r * x0r;
		y1r = x1r + x2r;
		y1i = x1i + x2i;
		y5r = x1r - x2r;
		y5i = x1i - x2i;
		x0r = a[2] + a[19];
		x0i = a[3] - a[18];
		x1r = wk3r * x0r - wk3i * x0i;
		x1i = wk3r * x0i + wk3i * x0r;
		x0r = a[10] + a[27];
		x0i = a[11] - a[26];
		x2r = wk1r * x0r + wk1i * x0i;
		x2i = wk1r * x0i - wk1i * x0r;
		y9r = x1r - x2r;
		y9i = x1i - x2i;
		y13r = x1r + x2r;
		y13i = x1i + x2i;
		x0r = a[4] - a[21];
		x0i = a[5] + a[20];
		x1r = wk2r * x0r - wk2i * x0i;
		x1i = wk2r * x0i + wk2i * x0r;
		x0r = a[12] - a[29];
		x0i = a[13] + a[28];
		x2r = wk2i * x0r - wk2r * x0i;
		x2i = wk2i * x0i + wk2r * x0r;
		y2r = x1r + x2r;
		y2i = x1i + x2i;
		y6r = x1r - x2r;
		y6i = x1i - x2i;
		x0r = a[4] + a[21];
		x0i = a[5] - a[20];
		x1r = wk2i * x0r - wk2r * x0i;
		x1i = wk2i * x0i + wk2r * x0r;
		x0r = a[12] + a[29];
		x0i = a[13] - a[28];
		x2r = wk2r * x0r - wk2i * x0i;
		x2i = wk2r * x0i + wk2i * x0r;
		y10r = x1r - x2r;
		y10i = x1i - x2i;
		y14r = x1r + x2r;
		y14i = x1i + x2i;
		x0r = a[6] - a[23];
		x0i = a[7] + a[22];
		x1r = wk3r * x0r - wk3i * x0i;
		x1i = wk3r * x0i + wk3i * x0r;
		x0r = a[14] - a[31];
		x0i = a[15] + a[30];
		x2r = wk1i * x0r - wk1r * x0i;
		x2i = wk1i * x0i + wk1r * x0r;
		y3r = x1r + x2r;
		y3i = x1i + x2i;
		y7r = x1r - x2r;
		y7i = x1i - x2i;
		x0r = a[6] + a[23];
		x0i = a[7] - a[22];
		x1r = wk1i * x0r + wk1r * x0i;
		x1i = wk1i * x0i - wk1r * x0r;
		x0r = a[14] + a[31];
		x0i = a[15] - a[30];
		x2r = wk3i * x0r - wk3r * x0i;
		x2i = wk3i * x0i + wk3r * x0r;
		y11r = x1r + x2r;
		y11i = x1i + x2i;
		y15r = x1r - x2r;
		y15i = x1i - x2i;
		x1r = y0r + y2r;
		x1i = y0i + y2i;
		x2r = y1r + y3r;
		x2i = y1i + y3i;
		a[0] = x1r + x2r;
		a[1] = x1i + x2i;
		a[2] = x1r - x2r;
		a[3] = x1i - x2i;
		x1r = y0r - y2r;
		x1i = y0i - y2i;
		x2r = y1r - y3r;
		x2i = y1i - y3i;
		a[4] = x1r - x2i;
		a[5] = x1i + x2r;
		a[6] = x1r + x2i;
		a[7] = x1i - x2r;
		x1r = y4r - y6i;
		x1i = y4i + y6r;
		x0r = y5r - y7i;
		x0i = y5i + y7r;
		x2r = wn4r * (x0r - x0i);
		x2i = wn4r * (x0i + x0r);
		a[8] = x1r + x2r;
		a[9] = x1i + x2i;
		a[10] = x1r - x2r;
		a[11] = x1i - x2i;
		x1r = y4r + y6i;
		x1i = y4i - y6r;
		x0r = y5r + y7i;
		x0i = y5i - y7r;
		x2r = wn4r * (x0r - x0i);
		x2i = wn4r * (x0i + x0r);
		a[12] = x1r - x2i;
		a[13] = x1i + x2r;
		a[14] = x1r + x2i;
		a[15] = x1i - x2r;
		x1r = y8r + y10r;
		x1i = y8i + y10i;
		x2r = y9r - y11r;
		x2i = y9i - y11i;
		a[16] = x1r + x2r;
		a[17] = x1i + x2i;
		a[18] = x1r - x2r;
		a[19] = x1i - x2i;
		x1r = y8r - y10r;
		x1i = y8i - y10i;
		x2r = y9r + y11r;
		x2i = y9i + y11i;
		a[20] = x1r - x2i;
		a[21] = x1i + x2r;
		a[22] = x1r + x2i;
		a[23] = x1i - x2r;
		x1r = y12r - y14i;
		x1i = y12i + y14r;
		x0r = y13r + y15i;
		x0i = y13i - y15r;
		x2r = wn4r * (x0r - x0i);
		x2i = wn4r * (x0i + x0r);
		a[24] = x1r + x2r;
		a[25] = x1i + x2i;
		a[26] = x1r - x2r;
		a[27] = x1i - x2i;
		x1r = y12r + y14i;
		x1i = y12i - y14r;
		x0r = y13r - y15i;
		x0i = y13i + y15r;
		x2r = wn4r * (x0r - x0i);
		x2i = wn4r * (x0i + x0r);
		a[28] = x1r - x2i;
		a[29] = x1i + x2r;
		a[30] = x1r + x2i;
		a[31] = x1i - x2r;
	}


	void cftf081(REAL *a, REAL *w)
	{
		REAL wn4r, x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i, 
			y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
			y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i;

		wn4r = w[1];
		x0r = a[0] + a[8];
		x0i = a[1] + a[9];
		x1r = a[0] - a[8];
		x1i = a[1] - a[9];
		x2r = a[4] + a[12];
		x2i = a[5] + a[13];
		x3r = a[4] - a[12];
		x3i = a[5] - a[13];
		y0r = x0r + x2r;
		y0i = x0i + x2i;
		y2r = x0r - x2r;
		y2i = x0i - x2i;
		y1r = x1r - x3i;
		y1i = x1i + x3r;
		y3r = x1r + x3i;
		y3i = x1i - x3r;
		x0r = a[2] + a[10];
		x0i = a[3] + a[11];
		x1r = a[2] - a[10];
		x1i = a[3] - a[11];
		x2r = a[6] + a[14];
		x2i = a[7] + a[15];
		x3r = a[6] - a[14];
		x3i = a[7] - a[15];
		y4r = x0r + x2r;
		y4i = x0i + x2i;
		y6r = x0r - x2r;
		y6i = x0i - x2i;
		x0r = x1r - x3i;
		x0i = x1i + x3r;
		x2r = x1r + x3i;
		x2i = x1i - x3r;
		y5r = wn4r * (x0r - x0i);
		y5i = wn4r * (x0r + x0i);
		y7r = wn4r * (x2r - x2i);
		y7i = wn4r * (x2r + x2i);
		a[8] = y1r + y5r;
		a[9] = y1i + y5i;
		a[10] = y1r - y5r;
		a[11] = y1i - y5i;
		a[12] = y3r - y7i;
		a[13] = y3i + y7r;
		a[14] = y3r + y7i;
		a[15] = y3i - y7r;
		a[0] = y0r + y4r;
		a[1] = y0i + y4i;
		a[2] = y0r - y4r;
		a[3] = y0i - y4i;
		a[4] = y2r - y6i;
		a[5] = y2i + y6r;
		a[6] = y2r + y6i;
		a[7] = y2i - y6r;
	}


	void cftf082(REAL *a, REAL *w)
	{
		REAL wn4r, wk1r, wk1i, x0r, x0i, x1r, x1i, 
			y0r, y0i, y1r, y1i, y2r, y2i, y3r, y3i, 
			y4r, y4i, y5r, y5i, y6r, y6i, y7r, y7i;

		wn4r = w[1];
		wk1r = w[4];
		wk1i = w[5];
		y0r = a[0] - a[9];
		y0i = a[1] + a[8];
		y1r = a[0] + a[9];
		y1i = a[1] - a[8];
		x0r = a[4] - a[13];
		x0i = a[5] + a[12];
		y2r = wn4r * (x0r - x0i);
		y2i = wn4r * (x0i + x0r);
		x0r = a[4] + a[13];
		x0i = a[5] - a[12];
		y3r = wn4r * (x0r - x0i);
		y3i = wn4r * (x0i + x0r);
		x0r = a[2] - a[11];
		x0i = a[3] + a[10];
		y4r = wk1r * x0r - wk1i * x0i;
		y4i = wk1r * x0i + wk1i * x0r;
		x0r = a[2] + a[11];
		x0i = a[3] - a[10];
		y5r = wk1i * x0r - wk1r * x0i;
		y5i = wk1i * x0i + wk1r * x0r;
		x0r = a[6] - a[15];
		x0i = a[7] + a[14];
		y6r = wk1i * x0r - wk1r * x0i;
		y6i = wk1i * x0i + wk1r * x0r;
		x0r = a[6] + a[15];
		x0i = a[7] - a[14];
		y7r = wk1r * x0r - wk1i * x0i;
		y7i = wk1r * x0i + wk1i * x0r;
		x0r = y0r + y2r;
		x0i = y0i + y2i;
		x1r = y4r + y6r;
		x1i = y4i + y6i;
		a[0] = x0r + x1r;
		a[1] = x0i + x1i;
		a[2] = x0r - x1r;
		a[3] = x0i - x1i;
		x0r = y0r - y2r;
		x0i = y0i - y2i;
		x1r = y4r - y6r;
		x1i = y4i - y6i;
		a[4] = x0r - x1i;
		a[5] = x0i + x1r;
		a[6] = x0r + x1i;
		a[7] = x0i - x1r;
		x0r = y1r - y3i;
		x0i = y1i + y3r;
		x1r = y5r - y7r;
		x1i = y5i - y7i;
		a[8] = x0r + x1r;
		a[9] = x0i + x1i;
		a[10] = x0r - x1r;
		a[11] = x0i - x1i;
		x0r = y1r + y3i;
		x0i = y1i - y3r;
		x1r = y5r + y7r;
		x1i = y5i + y7i;
		a[12] = x0r - x1i;
		a[13] = x0i + x1r;
		a[14] = x0r + x1i;
		a[15] = x0i - x1r;
	}


	void cftf040(REAL *a)
	{
		REAL x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

		x0r = a[0] + a[4];
		x0i = a[1] + a[5];
		x1r = a[0] - a[4];
		x1i = a[1] - a[5];
		x2r = a[2] + a[6];
		x2i = a[3] + a[7];
		x3r = a[2] - a[6];
		x3i = a[3] - a[7];
		a[0] = x0r + x2r;
		a[1] = x0i + x2i;
		a[4] = x0r - x2r;
		a[5] = x0i - x2i;
		a[2] = x1r - x3i;
		a[3] = x1i + x3r;
		a[6] = x1r + x3i;
		a[7] = x1i - x3r;
	}


	void cftb040(REAL *a)
	{
		REAL x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;

		x0r = a[0] + a[4];
		x0i = a[1] + a[5];
		x1r = a[0] - a[4];
		x1i = a[1] - a[5];
		x2r = a[2] + a[6];
		x2i = a[3] + a[7];
		x3r = a[2] - a[6];
		x3i = a[3] - a[7];
		a[0] = x0r + x2r;
		a[1] = x0i + x2i;
		a[4] = x0r - x2r;
		a[5] = x0i - x2i;
		a[2] = x1r + x3i;
		a[3] = x1i - x3r;
		a[6] = x1r - x3i;
		a[7] = x1i + x3r;
	}


	void cftx020(REAL *a)
	{
		REAL x0r, x0i;

		x0r = a[0] - a[2];
		x0i = a[1] - a[3];
		a[0] += a[2];
		a[1] += a[3];
		a[2] = x0r;
		a[3] = x0i;
	}


	void rftfsub(int n, REAL *a, int nc, REAL *c)
	{
		int j, k, kk, ks, m;
		REAL wkr, wki, xr, xi, yr, yi;

		m = n >> 1;
		ks = 2 * nc / m;
		kk = 0;
		for (j = 2; j < m; j += 2)
		{
			k = n - j;
			kk += ks;
			wkr = 0.5 - c[nc - kk];
			wki = c[kk];
			xr = a[j] - a[k];
			xi = a[j + 1] + a[k + 1];
			yr = wkr * xr - wki * xi;
			yi = wkr * xi + wki * xr;
			a[j] -= yr;
			a[j + 1] -= yi;
			a[k] += yr;
			a[k + 1] -= yi;
		}
	}



	void rftbsub(int n, REAL *a, int nc, REAL *c)
	{
		int j, k, kk, ks, m;
		REAL wkr, wki, xr, xi, yr, yi;

		m = n >> 1;
		ks = 2 * nc / m;
		kk = 0;
		for (j = 2; j < m; j += 2)
		{
			k = n - j;
			kk += ks;
			wkr = 0.5 - c[nc - kk];
			wki = c[kk];
			xr = a[j] - a[k];
			xi = a[j + 1] + a[k + 1];
			yr = wkr * xr + wki * xi;
			yi = wkr * xi - wki * xr;
			a[j] -= yr;
			a[j + 1] -= yi;
			a[k] += yr;
			a[k + 1] -= yi;
		}
	}




	void rfft(int n,int isign,REAL x[])
	{
		static int ipsize = 0,wsize=0;
		static int *ip = NULL;
		static REAL *w = NULL;
		int newipsize,newwsize;



		if (n == 0)
		{
			LocalFree(ip); ip = NULL; ipsize = 0;
			LocalFree(w);  w  = NULL; wsize  = 0;
			return;
		}



		newipsize = 2+sqrt((REAL)n/2);
		if (newipsize > ipsize)
		{
			if(ip != NULL)
				LocalFree(ip);

			ip = NULL;

			ipsize = newipsize;
			ip = (int *) LocalAlloc(0, sizeof(int)*ipsize);

			ip[0] = 0;
		}

		newwsize = n/2;
		if (newwsize > wsize)
		{

			if(w)
			{
				REAL *w1 = w;
				w = (REAL *) LocalAlloc(0, sizeof(REAL) * newwsize);
				CopyMemory(w, w1, sizeof(REAL)*wsize);
				LocalFree(w1);

			}
			else
			{
				w = (REAL *) LocalAlloc(0, sizeof(REAL) * newwsize);

			}


			wsize = newwsize;
		}

		rdft(n,isign,x,ip,w);
	}

}