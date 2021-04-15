#include "erl_nif.h"
#include <stdio.h>

static ERL_NIF_TERM
make_atom(ErlNifEnv* env, const char* atom) {
    ERL_NIF_TERM ret;
    if (!enif_make_existing_atom(env, atom, &ret, ERL_NIF_LATIN1)) {
        return enif_make_atom(env, atom);
    }
    return ret;
}

static ERL_NIF_TERM
make_error(ErlNifEnv* env, const char* mesg) {
    return enif_make_tuple2(env, make_atom(env, "error"), make_atom(env, mesg));
}

static ERL_NIF_TERM
make_ok(ErlNifEnv* env, ERL_NIF_TERM value) {
    return enif_make_tuple2(env, make_atom(env, "ok"), value);
}

/* Replace nkf's getchar/putchar for variable modification */
/* we never use getc, ungetc */
#undef getc
#undef ungetc
#define getc(f) \
    (input_ctr>=i_len?-1:input[input_ctr++])
#define ungetc(c,f) \
    input_ctr--

#define INCSIZE     32
#undef putchar
#undef TRUE
#undef FALSE
#define putchar(c)  erl_nkf_putchar(c)

#define MAX_BUF_LEN 255

/* Input/Output pointers */
static unsigned char *output;
static unsigned char *input;
static int input_ctr;
static int i_len;
static int output_ctr;
static int o_len;
static int incsize;

static int
erl_nkf_putchar(unsigned int c) {
    if (output_ctr >= o_len) {
        o_len += incsize;
        output = realloc(output, o_len);
        incsize *= 2;
    }
    return output[output_ctr++] = c;
}

#define PERL_XS 1
#include "nkf/config.h"
#include "nkf/utf8tbl.c"
#include "nkf/nkf.c"

static ERL_NIF_TERM
nkf(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    ErlNifBinary orig_bin, conv_bin, opts_bin;

    if (argc != 2
        || !enif_inspect_binary(env, argv[1], &orig_bin)) {
        return make_error(env, "bad_argument");
    }

    incsize = INCSIZE;

    input_ctr = 0;
    input = orig_bin.data;
    i_len = orig_bin.size;

    output_ctr = 0;
    o_len = i_len*3 + 10;
    output = malloc(o_len);
    *output = '\0';

    reinit();

    if (!enif_is_list(env, argv[0])
        || !enif_inspect_iolist_as_binary(env, argv[0], &opts_bin)) {
        return make_error(env, "bad_augument");
    }

    options((unsigned char*)opts_bin.data);
    kanji_convert( NULL );
    erl_nkf_putchar(0);

    if (!enif_alloc_binary(output_ctr, &conv_bin)){
        return make_error(env, "no_memory");
    }
    memcpy(conv_bin.data, output, conv_bin.size);

    return make_ok(env, enif_make_binary(env, &conv_bin));
}

static ERL_NIF_TERM
guess(ErlNifEnv *env, int argc, const ERL_NIF_TERM argv[]) {
    const char *code;
    ErlNifBinary orig_bin, conv_bin;

    if (argc != 1
        || !enif_inspect_binary(env, argv[0], &orig_bin)) {
        return make_error(env, "bad_argument");
    }

    reinit();

    input_ctr = 0;
    input = orig_bin.data;
    i_len = orig_bin.size;

    guess_f = TRUE;
    kanji_convert( NULL );
    guess_f = FALSE;

    code = get_guessed_code();
    if (!enif_alloc_binary(strlen(code), &conv_bin)){
        return make_error(env, "no_memory");
    }

    memcpy(conv_bin.data, code, conv_bin.size);

    return make_ok(env, enif_make_binary(env, &conv_bin));
}

static ErlNifFunc funcs[] = {
    { "nkf", 2, nkf, 0 },
    { "guess", 1, guess, 0 }
};

ERL_NIF_INIT(nkferl, funcs, NULL, NULL, NULL, NULL)
