/************************************************************************/
// 创建时间: 081123
// 最后确定: 081123


#ifndef _Wmp3Decoder_h_
#define _Wmp3Decoder_h_

namespace AnyPlay
{
#define MAD_BUFFER_GUARD	8
#define MAD_BUFFER_MDLEN	(511 + 2048 + MAD_BUFFER_GUARD)

#define MAD_F(x)		((mad_fixed_t) (x##L))
#define MAD_F_ONE		MAD_F(0x10000000)

#define MAD_RECOVERABLE(error)	((error) & 0xff00)


# define mad_f_add(x, y)	((x) + (y))
# define mad_f_sub(x, y)	((x) - (y))


	enum mad_decoder_mode
	{
		MAD_DECODER_MODE_SYNC  = 0,
		MAD_DECODER_MODE_ASYNC
	};

	enum mad_flow
	{
		MAD_FLOW_CONTINUE = 0x0000,	 
		MAD_FLOW_STOP     = 0x0010,	 
		MAD_FLOW_BREAK    = 0x0011,	 
		MAD_FLOW_IGNORE   = 0x0020	 
	};


	struct mad_bitptr
	{
		unsigned char const *byte;
		unsigned short cache;
		unsigned short left;
	};


	enum mad_error
	{
		MAD_ERROR_NONE	   = 0x0000,	 

		MAD_ERROR_BUFLEN	   = 0x0001,	 
		MAD_ERROR_BUFPTR	   = 0x0002,	 

		MAD_ERROR_NOMEM	   = 0x0031,	 

		MAD_ERROR_LOSTSYNC	   = 0x0101,	 
		MAD_ERROR_BADLAYER	   = 0x0102,	 
		MAD_ERROR_BADBITRATE	   = 0x0103,	 
		MAD_ERROR_BADSAMPLERATE  = 0x0104,	 
		MAD_ERROR_BADEMPHASIS	   = 0x0105,	 

		MAD_ERROR_BADCRC	   = 0x0201,	 
		MAD_ERROR_BADBITALLOC	   = 0x0211,	 
		MAD_ERROR_BADSCALEFACTOR = 0x0221,	 
		MAD_ERROR_BADMODE        = 0x0222,	 
		MAD_ERROR_BADFRAMELEN	   = 0x0231,	 
		MAD_ERROR_BADBIGVALUES   = 0x0232,	 
		MAD_ERROR_BADBLOCKTYPE   = 0x0233,	 
		MAD_ERROR_BADSCFSI	   = 0x0234,	 
		MAD_ERROR_BADDATAPTR	   = 0x0235,	 
		MAD_ERROR_BADPART3LEN	   = 0x0236,	 
		MAD_ERROR_BADHUFFTABLE   = 0x0237,	 
		MAD_ERROR_BADHUFFDATA	   = 0x0238,	 
		MAD_ERROR_BADSTEREO	   = 0x0239	 
	};



	enum
	{
		MAD_FLAG_NPRIVATE_III	= 0x0007,	 
		MAD_FLAG_INCOMPLETE	= 0x0008,	 

		MAD_FLAG_PROTECTION	= 0x0010,	 
		MAD_FLAG_COPYRIGHT	= 0x0020,	 
		MAD_FLAG_ORIGINAL	= 0x0040,	 
		MAD_FLAG_PADDING	= 0x0080,	 

		MAD_FLAG_I_STEREO	= 0x0100,	 
		MAD_FLAG_MS_STEREO	= 0x0200,	 
		MAD_FLAG_FREEFORMAT	= 0x0400,	 

		MAD_FLAG_LSF_EXT	= 0x1000,	 
		MAD_FLAG_MC_EXT	= 0x2000,	 
		MAD_FLAG_MPEG_2_5_EXT	= 0x4000	 
	};



	enum mad_mode
	{
		MAD_MODE_SINGLE_CHANNEL = 0,		 
		MAD_MODE_DUAL_CHANNEL	  = 1,		 
		MAD_MODE_JOINT_STEREO	  = 2,		 
		MAD_MODE_STEREO	  = 3		 
	};


	enum
	{
		MAD_OPTION_IGNORECRC      = 0x0001,	 
		MAD_OPTION_HALFSAMPLERATE = 0x0002	 
	};


	enum mad_layer
	{
		MAD_LAYER_I   = 1,			 
		MAD_LAYER_II  = 2,			 
		MAD_LAYER_III = 3			 
	};



	enum mad_emphasis
	{
		MAD_EMPHASIS_NONE	  = 0,		 
		MAD_EMPHASIS_50_15_US	  = 1,		 
		MAD_EMPHASIS_CCITT_J_17 = 3,		 
		MAD_EMPHASIS_RESERVED   = 2		 
	};


# define MAD_TIMER_RESOLUTION	352800000UL

	enum mad_units
	{
		MAD_UNITS_HOURS	 =    -2,
		MAD_UNITS_MINUTES	 =    -1,
		MAD_UNITS_SECONDS	 =     0,



		MAD_UNITS_DECISECONDS	 =    10,
		MAD_UNITS_CENTISECONDS =   100,
		MAD_UNITS_MILLISECONDS =  1000,



		MAD_UNITS_8000_HZ	 =  8000,
		MAD_UNITS_11025_HZ	 = 11025,
		MAD_UNITS_12000_HZ	 = 12000,

		MAD_UNITS_16000_HZ	 = 16000,
		MAD_UNITS_22050_HZ	 = 22050,
		MAD_UNITS_24000_HZ	 = 24000,

		MAD_UNITS_32000_HZ	 = 32000,
		MAD_UNITS_44100_HZ	 = 44100,
		MAD_UNITS_48000_HZ	 = 48000,



		MAD_UNITS_24_FPS	 =    24,
		MAD_UNITS_25_FPS	 =    25,
		MAD_UNITS_30_FPS	 =    30,
		MAD_UNITS_48_FPS	 =    48,
		MAD_UNITS_50_FPS	 =    50,
		MAD_UNITS_60_FPS	 =    60,



		MAD_UNITS_75_FPS	 =    75,



		MAD_UNITS_23_976_FPS	 =   -24,
		MAD_UNITS_24_975_FPS	 =   -25,
		MAD_UNITS_29_97_FPS	 =   -30,
		MAD_UNITS_47_952_FPS	 =   -48,
		MAD_UNITS_49_95_FPS	 =   -50,
		MAD_UNITS_59_94_FPS	 =   -60
	};





# define MAD_F_FRACBITS		28

# define MAD_F_MIN		((mad_fixed_t) -0x80000000L)
# define MAD_F_MAX		((mad_fixed_t) +0x7fffffffL)

# define mad_f_todouble(x)	((double)  \
	((x) / (double) (1L << MAD_F_FRACBITS)))

# define mad_f_tofixed(x)	((mad_fixed_t)  \
	((x) * (double) (1L << MAD_F_FRACBITS) + 0.5))


	struct mad_stream
	{
		unsigned char const *buffer;		 
		unsigned char const *bufend;		 
		unsigned long skiplen;		 

		int sync;				 
		unsigned long freerate;		 

		unsigned char const *this_frame;	 
		unsigned char const *next_frame;	 
		struct mad_bitptr ptr;		 

		struct mad_bitptr anc_ptr;		 
		unsigned int anc_bitlen;		 

		unsigned char (*main_data) [MAD_BUFFER_MDLEN];

		unsigned int md_len;			 

		int options;				 
		enum mad_error error;			 

	};

	typedef struct
	{
		signed long seconds;		 
		unsigned long fraction;	 
	} mad_timer_t;



	struct mad_header
	{
		int layer;			 
		int mode;			 
		int mode_extension;			 
		int emphasis;		 

		unsigned long bitrate;		 
		unsigned int samplerate;		 

		unsigned short crc_check;		 
		unsigned short crc_target;		 

		int flags;				 
		int private_bits;			 

		mad_timer_t duration;			 
		unsigned int size;		 
	};

	typedef   signed int mad_fixed_t;

	struct mad_frame
	{
		struct mad_header header;		 

		int options;				 

		mad_fixed_t sbsample[2][36][32];	 
		mad_fixed_t (*overlap)[2][32][18];	 
	};


	struct mad_pcm
	{
		unsigned int samplerate;		 
		unsigned short channels;		 
		unsigned short length;		 
		mad_fixed_t samples[2][1152];		 
	};

	struct mad_synth
	{
		mad_fixed_t filter[2][2][2][16][8];	 


		unsigned int phase;			 

		struct mad_pcm pcm;			 
	};


	typedef struct
	{
		struct mad_stream stream;
		struct mad_frame frame;
		struct mad_synth synth;
	} SYNC;

	struct mad_decoder
	{
		int mode;

		int options;

		struct
		{
			long pid;
			int in;
			int out;
		} async;

		SYNC* sync;
		void *cb_data;


		enum mad_flow (*input_func)(void *, struct mad_stream *);
		enum mad_flow (*header_func)(void *, struct mad_header const *);
		enum mad_flow (*filter_func)(void *,
		struct mad_stream const *, struct mad_frame *);
		enum mad_flow (*output_func)(void *,
		struct mad_header const *, struct mad_pcm *);
		enum mad_flow (*error_func)(void *, struct mad_stream *, struct mad_frame *);
		enum mad_flow (*message_func)(void *, void *, unsigned int *);
	};


	void mad_header_init(struct mad_header *);
#define mad_header_finish(header)   

	int mad_header_decode(struct mad_header *header, struct mad_stream *stream);



	void mad_stream_init(struct mad_stream* stream);
	void mad_stream_finish(struct mad_stream* stream);
	void mad_stream_skip(struct mad_stream*, unsigned long length);
	int mad_stream_sync(struct mad_stream* stream);
	void mad_stream_buffer(struct mad_stream* stream,
		unsigned char const *buffer, unsigned long length);

	char const *mad_stream_errorstr(struct mad_stream const *);
#define mad_stream_options(stream, opts) ((void) ((stream)->options = (opts)))






	void mad_frame_init(struct mad_frame *);

	int mad_frame_decode(struct mad_frame *frame, struct mad_stream *stream);

	void mad_frame_finish(struct mad_frame *);
	void mad_frame_mute(struct mad_frame *);


# define MAD_NCHANNELS(header)		((header)->mode ? 2 : 1)
# define MAD_NSBSAMPLES(header)  \
	((header)->layer == MAD_LAYER_I ? 12 :  \
	(((header)->layer == MAD_LAYER_III &&  \
	((header)->flags & MAD_FLAG_LSF_EXT)) ? 18 : 36))




	void mad_synth_init(struct mad_synth *synth);
#define mad_synth_finish(synth)   
	void mad_synth_mute(struct mad_synth *synth);
	void mad_synth_frame(struct mad_synth *synth, struct mad_frame const *frame);




	extern mad_timer_t const mad_timer_zero;
# define mad_timer_reset(timer)	((void) (*(timer) = mad_timer_zero))
	int mad_timer_compare(mad_timer_t, mad_timer_t);
#define mad_timer_sign(timer)	mad_timer_compare((timer), mad_timer_zero)
	void mad_timer_negate(mad_timer_t *);
	mad_timer_t mad_timer_abs(mad_timer_t);
	void mad_timer_set(mad_timer_t *, unsigned long, unsigned long, unsigned long);
	void mad_timer_add(mad_timer_t *, mad_timer_t);
	void mad_timer_multiply(mad_timer_t *, signed long);
	signed long mad_timer_count(mad_timer_t, enum mad_units);
	unsigned long mad_timer_fraction(mad_timer_t, unsigned long);
	void mad_timer_string(mad_timer_t, char *, char const *,
		enum mad_units, enum mad_units, unsigned long);





	void mad_bit_init(struct mad_bitptr *, unsigned char const *);
# define mad_bit_finish(bitptr)		 
	unsigned int mad_bit_length(struct mad_bitptr const *,
	struct mad_bitptr const *);

# define mad_bit_bitsleft(bitptr)  ((bitptr)->left)
	unsigned char const *mad_bit_nextbyte(struct mad_bitptr const *);
	void mad_bit_skip(struct mad_bitptr *, unsigned int);
	unsigned long mad_bit_read(struct mad_bitptr *, unsigned int);
	void mad_bit_write(struct mad_bitptr *, unsigned int, unsigned long);
	unsigned short mad_bit_crc(struct mad_bitptr, unsigned int, unsigned short);





	void mad_decoder_init(struct mad_decoder *, void *,
		enum mad_flow (*)(void *, struct mad_stream *),
		enum mad_flow (*)(void *, struct mad_header const *),
		enum mad_flow (*)(void *,
	struct mad_stream const *,
	struct mad_frame *),
		enum mad_flow (*)(void *,
	struct mad_header const *,
	struct mad_pcm *),
		enum mad_flow (*)(void *,
	struct mad_stream *,
	struct mad_frame *),
		enum mad_flow (*)(void *, void *, unsigned int *));


	int mad_decoder_run(struct mad_decoder *, enum mad_decoder_mode);



	int mad_decoder_finish(struct mad_decoder *decoder);

	char const *mad_stream_errorstr(struct mad_stream const *);

}
#endif
