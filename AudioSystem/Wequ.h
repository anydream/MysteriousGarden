/************************************************************************/
// 创建时间: 081123
// 最后确定: 081123

#ifndef _Wequ_h_
#define _Wequ_h_

namespace AnyPlay
{
	class paramlistelm
	{
	public:
		class paramlistelm *next;

		char left,right;
		float lower,upper,gain,gain2;
		int sortindex;

		paramlistelm(void);

		~paramlistelm();
	};

	class paramlist
	{
	public:
		class paramlistelm *elm;

		paramlist(void);

		~paramlist();

	};

	typedef float REAL;

	extern int weq_enable;

	void equ_init(int wb);
	void equ_makeTable(REAL *lbc,REAL *rbc,paramlist *param,REAL fs);
	void equ_quit(void);
	void equ_clearbuf(int srate);
	int equ_modifySamples(char *buf,int nsamples,int nch);
	unsigned int equ_latency();

}

#endif
